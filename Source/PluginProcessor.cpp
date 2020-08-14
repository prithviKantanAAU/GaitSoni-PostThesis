#include "PluginProcessor.h"
#include "PluginEditor.h"

// CONSTRUCTOR - INITIALIZATIONS AND START TIMER
GaitSonificationAudioProcessor::GaitSonificationAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
	setTempo(120);
	gaitAnalysis.setupReceivers();
	updateAudioParameter(1, 1);
	soniMappingCompute.setSampleRate(gaitAnalysis.sensorInfo.IMU_SampleRate);
	initializeClocking();
	startTimer(1);	
}

// DESTRUCTOR - STOP DSPFAUST AND TIMER
GaitSonificationAudioProcessor::~GaitSonificationAudioProcessor()
{
	sequencer.dspFaust.stop();
	stopTimer();
}

// HI RES TIMER CALLBACK - EVERY 1 MS
void GaitSonificationAudioProcessor::hiResTimerCallback()
{
	// INCREMENT GLOBAL TIME AND CLOCK PULSES ELAPSED
	gaitAnalysis.timeElapsed = globalTimeMs++ / 1000.0;
	pulsesElapsed += 1;

	// UPDATE SENSOR ONLINE STATUS EVERY 2 SECONDS
	if (globalTimeMs % 2000 == 0)
		gaitAnalysis.updateSensorStatus();

	// INCREMENT IMU RECORDING TIME IF ENABLED
	if (imuRecord.isRecording_Sensor)
		imuRecord.timeElapsed_REC += 0.001;

	// HANDLE SONG PLAYBACK
	if (sequencer.isPlaying)
	{
		// INCREMENT BASIC PLAYBACK COUNTERS
		sequencer.timeElapsed_SONG += 0.001;
		dynamicTarget_phaseTime += 0.001;
		pulsesElapsed = pulsesElapsed % 100;
		sequencer.midiTicksElapsed += sequencer.ticksPerMS;
		sequencer.tapTempoCounter = min(1, sequencer.tapTempoCounter + 0.001);		// Limit to 1

		// RUN MUSICAL CLOCK CALLBACK
		clockCallback();

		// STOP MUSIC IF SONG COMPLETE
		if (sequencer.getSongProgress())
			sequencer.stopMusic();
	}

	// IF SAMPLE INTERVAL EXCEEDED, RUN SENSOR AND SONIFICATION CALLBACK
	if (pulsesElapsed % pulseInterval_sensorCallback == 0)
		sensorCallback();
}

// MUSIC CLOCK CALLBACK - EVERY 1 MS
void GaitSonificationAudioProcessor::clockCallback()
{
	// IF CLOCK PULSE DUE, TRIGGER CLOCK
	if (checkIfPulseDue())
		triggerClock(true);

	// CHECK FOR NEW MIDI EVENTS, HANDLE IF NEEDED
	sequencer.check_Handle_New_MIDIEvents();

	// UPDATE MUSIC PHASE
	sequencer.musicPhase.updatePhase();

	// ADD CUEING MAPPING --------------------- UNCOMMENT WHEN MUSIC PLAYBACK DEBUGGED
	sequencer.dspFaust.setParamValue(soniAddress_Cue.c_str(), sequencer.musicPhase.emphFunc_Present); // Map Primary Sonification
}

// COMPUTE MP, AP, STORE SENSOR RECORDING EVERY 10 MS
void GaitSonificationAudioProcessor::sensorCallback()
{
	// UPDATE IMU BUFFERS FOR ALL ACTIVE SENSORS
	for (int i = 0; i < gaitAnalysis.sensorInfo.numSensorsMax; i++)
		if (gaitAnalysis.sensorInfo.isOnline[i])
			gaitAnalysis.sensors_OSCReceivers[i].updateBuffers();

	// COMPUTE CHOSEN MOVEMENT PARAM
	gaitAnalysis.compute(gaitAnalysis.gaitParams.activeGaitParam, isCalibrating);

	// RECALIBRATE TRUNK SENSOR REST EVERY 3 SECONDS
	if (pulsesElapsed % 3000 == 0)
		gaitAnalysis.calibrateRest(gaitAnalysis.sensors_OSCReceivers[gaitAnalysis.idx_Sensor_Trunk].acc_Buf);

	// COMPUTE PRESENT AP VALUE IF STANDBY DISABLED
	if (isStandby) mapVal = 0;
	else
		mapVal = jlimit(audioParams.audioParam_ObjectArray[audioParams.activeAudioParam].minVal,
			audioParams.audioParam_ObjectArray[audioParams.activeAudioParam].maxVal,
			getCurrentMappingValue());

	// CHECK MUSIC PLAYBACK
	if (sequencer.isPlaying)
	{
		// CHECK AP TYPE
		switch (audioParams.audioParam_ObjectArray[audioParams.activeAudioParam].type)
		{
		case 1:	// AUDIO - BASED
			sequencer.dspFaust.setParamValue(soniAddress_Primary.c_str(), mapVal);
			break;
		case 2: // SEQUENCER - BASED
			applySequencerSonifications();
			break;
		}
	}

	// IF SENSOR RECORDING ENABLED, WRITE PRESENT LINE TO FILE
	if (imuRecord.isRecording_Sensor)
		writeSensorValue_ToFile(gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[gaitAnalysis.gaitParams.activeGaitParam].currentValue);
}

// COMPUTE PRESENT AP VALUE
float GaitSonificationAudioProcessor::getCurrentMappingValue()
{
	// LOAD SONIFICATION-RELATED PARAMS TO LOCAL VARIABLES
	short activeGaitParam = gaitAnalysis.gaitParams.activeGaitParam;
	String mp_Name = gaitAnalysis.gaitParams.gaitParam_ObjectArray[activeGaitParam].name;
	float targetValue = gaitAnalysis.gaitParams.gaitParam_ObjectArray[activeGaitParam].target;
	short desiredBehavior = gaitAnalysis.gaitParams.gaitParam_ObjectArray[activeGaitParam].desiredBehavior;
	float range = gaitAnalysis.gaitParams.gaitParam_ObjectArray[activeGaitParam].maxVal
		- gaitAnalysis.gaitParams.gaitParam_ObjectArray[activeGaitParam].minVal;
	float currentValue = gaitAnalysis.gaitParams.gaitParam_ObjectArray
		[activeGaitParam].currentValue;
	float toleranceBW = audioParams.audioParam_ObjectArray[audioParams.activeAudioParam].toleranceBW;
	float order_MappingFunc = audioParams.audioParam_ObjectArray[audioParams.activeAudioParam].mappingOrder;
	int numQuantLevels = audioParams.audioParam_ObjectArray[audioParams.activeAudioParam].quantLevels;
	float smoothingFc = audioParams.audioParam_ObjectArray[audioParams.activeAudioParam].smoothingFc;

	// SEPARATE FOR 2D PROJECTION ZONE
	if (mp_Name == "Trunk Projection Zone")
	{
		if (!soniMappingCompute.isSoniSource_Slider)
			return gaitAnalysis.staticBalance_ZoneMaps[gaitAnalysis.staticBalance_ZoneMap_Current - 1]
			[(int)gaitAnalysis.gaitParams.gaitParam_ObjectArray[2].currentValue - 1];
		else
			return gaitAnalysis.staticBalance_ZoneMaps[gaitAnalysis.staticBalance_ZoneMap_Current - 1]
			[(int)(soniMappingCompute.soniVal_Slider * 5)];
	}

	// IF NOT 2D PROJECTION, CHECK AND HANDLE DYNAMIC TARGET SCENARIO
	else
		if (isTargetDynamic)
		{
			targetValue = soniMappingCompute.getDynamicTarget(targetValue, soniMappingCompute.movementFunc_Order,
				tempo, dynamicTarget_phaseTime);
			dynamicTarget = targetValue;
			toleranceBW = soniMappingCompute.movement_ErrorTolerance;
		}

	// COMBINE SONI PARAMS INTO ARRAY TO SEND SONIMAPPINGCOMPUTE
	float functionParamSet[6] = { order_MappingFunc, toleranceBW, numQuantLevels, smoothingFc, range, desiredBehavior };

	// CHECK IF SLIDER SONIFICATION ENABLED, ELSE OPERATE ON PRESENT MP VALUE
	if (!soniMappingCompute.isSoniSource_Slider)
		return jlimit((float)0, (float)1, soniMappingCompute.computeParamValue(currentValue, targetValue, functionParamSet));
	else
		return jlimit((float)0, (float)1, soniMappingCompute.computeParamValue_SliderSource(soniMappingCompute.soniVal_Slider, targetValue, functionParamSet));
}

// TRIGGER MUSIC MASTER CLOCK AT 16TH NOTE INTERVAL FOR COUNTER UPDATE
void GaitSonificationAudioProcessor::triggerClock(bool polarity)
{
	if (polarity == true)
	{
		// FAUST SET ON
		sequencer.incrementPulseCounter();
		sequencer.lastPulseTime = sequencer.nextPulseTime;
		sequencer.nextPulseTime += sequencer.midiTickIncrement;
	}
}

// APPLY SEQUENCER SONIFICATIONS ON COMPUTED AP VALUE
void GaitSonificationAudioProcessor::applySequencerSonifications()
{
	switch (audioParams.activeAudioParam)
	{

	}
}

// FIRST TIME SET SENSOR PULSE INTERVALS, NEXT SIXTEENTH MUSIC PULSE TIME
void GaitSonificationAudioProcessor::initializeClocking()
{
	pulseInterval_sensorCallback = 1000 / gaitAnalysis.sensorInfo.IMU_SampleRate;
	sequencer.nextPulseTime += interPulseIntervalMs * 0.001;
}

//=========================================================================================================
const String GaitSonificationAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GaitSonificationAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GaitSonificationAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GaitSonificationAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GaitSonificationAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GaitSonificationAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
}

int GaitSonificationAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GaitSonificationAudioProcessor::setCurrentProgram (int index)
{
}

const String GaitSonificationAudioProcessor::getProgramName (int index)
{
    return {};
}

void GaitSonificationAudioProcessor::changeProgramName (int index, const String& newName)
{
}

void GaitSonificationAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

void GaitSonificationAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GaitSonificationAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GaitSonificationAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
    }
}

bool GaitSonificationAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* GaitSonificationAudioProcessor::createEditor()
{
    return new GaitSonificationAudioProcessorEditor (*this);
}

void GaitSonificationAudioProcessor::getStateInformation (MemoryBlock& destData)
{
}

void GaitSonificationAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GaitSonificationAudioProcessor();
}
