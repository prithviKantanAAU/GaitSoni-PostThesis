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
	dspFaust.stop();
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
	if (isPlaying)
	{
		// INCREMENT BASIC PLAYBACK COUNTERS
		timeElapsed_SONG += 0.001;
		dynamicTarget_phaseTime += 0.001;
		pulsesElapsed = pulsesElapsed % 100;
		midiTicksElapsed += ticksPerMS;
		tapTempoCounter = min(1, tapTempoCounter + 0.001);		// Limit to 1

		// RUN MUSICAL CLOCK CALLBACK
		clockCallback();

		// STOP MUSIC IF SONG COMPLETE
		if (getSongProgress())
			stopMusic();
	}

	// IF SAMPLE INTERVAL EXCEEDED, RUN SENSOR AND SONIFICATION CALLBACK
	if (pulsesElapsed % pulseInterval_sensorCallback == 0)
		sensorCallback();
}

// MUSIC CLOCK CALLBACK - EVERY 1 MS
void GaitSonificationAudioProcessor::clockCallback()
{
	// INITIALIZE PULSE LENGTH AND MEASURED TIME
	double clockPulseLength = 70 * ticksPerMS;
	double timeMeasure = midiTicksElapsed;

	// SET CLOCK STATE LOW IF HIGH FOR LONGER THAN 70 MS
	if (clockTriggeredLast && (timeMeasure - lastPulseTime >= clockPulseLength))
		triggerClock(false);

	// IF CLOCK PULSE DUE, TRIGGER CLOCK
	if (checkIfPulseDue())
		triggerClock(true);

	// IF NEW MIDI MESSAGE ON ANY TRACK, THEN MAP				//FETCHING FUNCTION TO SEQUENCER ?
	if (fetch_MusicInfo_Mode_MIDI())
		infoMapped_CurrentPulse_MIDI = mapMusicInfo_Mode_MIDI();

	// UPDATE MUSIC PHASE
	sequencer.musicPhase.updatePhase();

	// ADD CUEING MAPPING
	dspFaust.setParamValue(soniAddress_Cue.c_str(), sequencer.musicPhase.emphFunc_Present); // Map Primary Sonification
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
	if (isPlaying)
	{
		// CHECK AP TYPE
		switch (audioParams.audioParam_ObjectArray[audioParams.activeAudioParam].type)
		{
		case 1:	// AUDIO - BASED
			dspFaust.setParamValue(soniAddress_Primary.c_str(), mapVal);
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
	std::string masterClockAddress = faustStrings.baseName + faustStrings.MasterClock;
	if (polarity == true)
	{
		// FAUST SET ON
		sequencer.incrementPulseCounter();
		dspFaust.setParamValue(masterClockAddress.c_str(), 1.0);
		lastPulseTime = nextPulseTime;
		nextPulseTime += midiTickIncrement;
		clockTriggeredLast = true;
	}
	else
	{
		//FAUST SET OFF
		dspFaust.setParamValue(masterClockAddress.c_str(), 0);
		clockTriggeredLast = false;
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
	nextPulseTime += interPulseIntervalMs * 0.001;
}

// FIRST TIME INITIALIZE MUTES, VARIANTS, MASTER GAIN, MAP-----------------MOVE TO SEQUENCER
void GaitSonificationAudioProcessor::initializeTrackGains()
{
	for (int i = 0; i < 8; i++)
	{
		setTrackMutes(i, 1);
		switchInstVariant(i, mixerSettings.currentVariant[sequencer.index_baseBeat][i]);
	}
	applyMasterGain(mixerSettings.masterGain);
	dspFaust.setParamValue(faustStrings.MasterEQ_1_Q.c_str(), 0.7);
	dspFaust.setParamValue(faustStrings.MasterEQ_2_Q.c_str(), 0.7);
}

// MAP CURRENT VARIANT GAIN - SINGLE TRACK---------------------------------MOVE TO SEQUENCER
void GaitSonificationAudioProcessor::applyCurrentVariantGain(int trackIndex)
{
	std::string address = "";
	float gain = 0;
	short variantNum = mixerSettings.currentVariant[sequencer.index_baseBeat][trackIndex] - 1;
	address = faustStrings.getTrackGainString(trackIndex);
	gain = mixerSettings.trackGains[trackIndex][variantNum]
		+ mixerSettings.trackGain_Offsets[sequencer.index_baseBeat][trackIndex];
	dspFaust.setParamValue(address.c_str(), gain);
}

// APPLY AND MAP CURRENT VARIANT GAIN - SINGLE TRACK (on Variant Change)---MOVE TO SEQUENCER
void GaitSonificationAudioProcessor::setTrackGains(int trackIndex, float value)
{
	mixerSettings.trackGain_Offsets[sequencer.index_baseBeat][trackIndex] = value;
	applyCurrentVariantGain(trackIndex);
}

// APPLY AND MAP MUTE VALUE - SINGLE TRACK---------------------------------MOVE TO SEQUENCER
void GaitSonificationAudioProcessor::setTrackMutes(int trackIndex, int value)
{
	std::string address = faustStrings.baseName + faustStrings.trackMutes[trackIndex];
	dspFaust.setParamValue(address.c_str(), value);
}

// APPLY AND MAP VARIANT EQ - SINGLE TRACK (on Variant Change)-------------MOVE TO SEQUENCER
void GaitSonificationAudioProcessor::applyCurrentVariantEQ(int trackIndex)
{
	short variantNum = mixerSettings.currentVariant[sequencer.index_baseBeat][trackIndex] - 1;
	float value = 0;
	bool toMap = true;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
				std::string address = faustStrings.FetchEQ_String(trackIndex, i, j);
				value = mixerSettings.fetchEQValue(variantNum, trackIndex, i, j);
				dspFaust.setParamValue(address.c_str(), value);
		}
	}
}

// APPLY AND MAP VARIANT COMP - SINGLE TRACK (on Variant Change)-----------MOVE TO SEQUENCER
void GaitSonificationAudioProcessor::applyCurrentVariantComp(int trackIndex)
{
	std::string address = "";
	float value = 0;
	int currentRhythm = sequencer.index_baseBeat;
	short variantNum = mixerSettings.currentVariant[currentRhythm][trackIndex] - 1;
	for (int j = 0; j < 4; j++)		//Param ID
	{
		address = faustStrings.FetchComp_String(trackIndex, j);
		value = mixerSettings.fetchCompValue(variantNum, trackIndex, j);
		dspFaust.setParamValue(address.c_str(), value);
	}
}

// SWITCH INSTRUMENT VARIANT - SINGLE TRACK--------------------------------MOVE TO SEQUENCER
void GaitSonificationAudioProcessor::switchInstVariant(int trackIndex, int newVariant)
{
	// UPDATE MIXER SETTINGS WITH NEW SETTING
	int currentRhythm = sequencer.index_baseBeat;
	mixerSettings.currentVariant[currentRhythm][trackIndex] = newVariant;
	//MAP VARIANT TO FAUST
	// CHANNEL SETTINGS
	applyCurrentVariantGain(trackIndex);
	applyCurrentVariantComp(trackIndex);
	applyCurrentVariantEQ(trackIndex);
	// VARIANT NUMBER
	std::string address = faustStrings.baseName + faustStrings.trackVariant_Base + std::to_string(trackIndex + 1);
	dspFaust.setParamValue(address.c_str(), mixerSettings.currentVariant[sequencer.index_baseBeat][trackIndex]);
}

// LOAD MIDI SONG FILE-----------------------------------------------------MOVE TO SEQUENCER
void GaitSonificationAudioProcessor::setFilename(String name)
{
	stopMusic();
	sequencer.loadNewFile_MIDI(name);							// Load MIDI Song File from PATH
	togglePlayPause();
}

// HANDLE TAP TEMPO BUTTON PRESS ------------------------------------------MOVE TO SEQUENCER
void GaitSonificationAudioProcessor::handleTapTempoPress()
{
	float newTempo = 0;
	if (tapTempoCounter >= 1.0)
	{
		tapTempoCounter = 0;
	}
	else
	{
		newTempo = 60 / tapTempoCounter;
		setTempo(newTempo);
		tapTempoCounter = 0;
	}
}

// PAUSE OR PLAY IF FILE HAS BEEN LOADED-----------------------------------MOVE TO SEQUENCER
void GaitSonificationAudioProcessor::togglePlayPause()
{
	if (isPlaying)
	{
		dspFaust.stop();
		isPlaying = false;
	}
	else
	{
		if (sequencer.isFileLoaded)
			{
				onStartMusic();
				isPlaying = true;
			}
	}
}

// STOP MUSIC, RESET COUNTERS AND TIME-------------------------------------MOVE TO SEQUENCER
void GaitSonificationAudioProcessor::stopMusic()
{
	if (isPlaying)
	{
		isPlaying = false;
		dspFaust.stop();
		sequencer.resetCounters();
		sequencer.resetMidiPlayback();
		midiTicksElapsed = 0;
		timeElapsed_SONG = 0;
		nextPulseTime = 0;
		sequencer.musicPhase.resetPhase();
	}
}

// CHECK FOR NEW MIDI EVENTS ON ALL TRACKS, RETURN TRUE IF NEW EVENT-------MOVE TO SEQUENCER
bool GaitSonificationAudioProcessor::fetch_MusicInfo_Mode_MIDI()
{
	// CHECK NEW - Tracks 1, 2, 3 ,8 - PERC
	bool isNew_MIDI_PERC_BASE =
		sequencer.checkMIDIEventsDue(4, 4, false, ticksPerMS, midiTicksElapsed, info_PERC_V_COMMON
			, audioParams.audioParam_ObjectArray[audioParams.activeCueParam].name);

	// CHECK NEW - Track 4 - Chords
	bool isNew_MIDI_C =
		sequencer.checkMIDIEventsDue(2, 4, true, ticksPerMS, midiTicksElapsed, info_C_MIDI
			, audioParams.audioParam_ObjectArray[audioParams.activeCueParam].name);
	if (isNew_MIDI_C) arrangeChordNotes_Asc(info_C_MIDI, 8);					// Arrange in ASC Order

	// CHECK NEW - Track 5 - Bassline
	bool isNew_MIDI_R =
		sequencer.checkMIDIEventsDue(3, 1, true, ticksPerMS, midiTicksElapsed, info_R_MIDI
			, audioParams.audioParam_ObjectArray[audioParams.activeCueParam].name);
	if (isNew_MIDI_R)															// Limit Note Range
		info_R_MIDI[0] = sequencer.midiNoteLimit(info_R_MIDI[0],
			mixerSettings.var_noteMin[4][mixerSettings.currentVariant[sequencer.index_baseBeat][4] - 1],
			mixerSettings.var_noteMax[4][mixerSettings.currentVariant[sequencer.index_baseBeat][4] - 1]);

	// CHECK NEW - Track 6 - Main Melody
	bool isNew_MIDI_M =
		sequencer.checkMIDIEventsDue(1, 1, false, ticksPerMS, midiTicksElapsed, info_M_MIDI
			,audioParams.audioParam_ObjectArray[audioParams.activeCueParam].name);
	
	// SNARE FLURRY - HANDLE IF ACTIVE
	bool isNew_MIDI_PERC_SnareFlurry = false;
	if (audioParams.audioParam_ObjectArray[audioParams.activeCueParam].name == "Snare Flurry")
	{
		info_PERC_V_COMMON[1] = 0;
		isNew_MIDI_PERC_SnareFlurry = sequencer.checkMIDIEventsDue(6, 4, false, ticksPerMS, midiTicksElapsed, info_PERC_V_COMMON
			, audioParams.audioParam_ObjectArray[audioParams.activeCueParam].name);
	}

	return isNew_MIDI_M 
		|| isNew_MIDI_C 
		|| isNew_MIDI_R 
		|| isNew_MIDI_PERC_BASE 
		|| isNew_MIDI_PERC_SnareFlurry;
}

// MAP MIDI INFO - MODIFY--------------------------------------------------MOVE TO SEQUENCER
bool GaitSonificationAudioProcessor::mapMusicInfo_Mode_MIDI()
{
	// Will be rewritten in SEQUENCER
	if (audioParams.audioParam_ObjectArray[audioParams.activeAudioParam].name == "Foot Drum")
	{
		info_PERC_V_COMMON[0] = 9;
		info_PERC_V_COMMON[1] = 9;
	}
	else {
		dspFaust.setParamValue(faustStrings.KickV.c_str(), info_PERC_V_COMMON[0]); //REMOVE
		dspFaust.setParamValue(faustStrings.SnareV.c_str(), info_PERC_V_COMMON[1]); //REMOVE
	}
	dspFaust.setParamValue(faustStrings.hhV.c_str(), info_PERC_V_COMMON[2]); //REMOVE
	dspFaust.setParamValue(faustStrings.crashV.c_str(), info_PERC_V_COMMON[3]); //REMOVE

	dspFaust.setParamValue(faustStrings.CF1.c_str(), sequencer.midiNoteLimit(info_C_MIDI[0], mixerSettings.chordNote_Mins[0], mixerSettings.chordNote_Maxs[0]));
	dspFaust.setParamValue(faustStrings.CF2.c_str(), sequencer.midiNoteLimit(info_C_MIDI[1], mixerSettings.chordNote_Mins[1], mixerSettings.chordNote_Maxs[1]));
	dspFaust.setParamValue(faustStrings.CF3.c_str(), sequencer.midiNoteLimit(info_C_MIDI[2], mixerSettings.chordNote_Mins[2], mixerSettings.chordNote_Maxs[2]));
	dspFaust.setParamValue(faustStrings.CF4.c_str(), sequencer.midiNoteLimit(info_C_MIDI[3], mixerSettings.chordNote_Mins[3], mixerSettings.chordNote_Maxs[3]));

	dspFaust.setParamValue(faustStrings.CSF1.c_str(), sequencer.midiNoteLimit(info_C_MIDI[0], mixerSettings.chordStab_Mins[0], mixerSettings.chordStab_Mins[0]));
	dspFaust.setParamValue(faustStrings.CSF2.c_str(), sequencer.midiNoteLimit(info_C_MIDI[1], mixerSettings.chordStab_Mins[1], mixerSettings.chordStab_Mins[1]));
	dspFaust.setParamValue(faustStrings.CSF3.c_str(), sequencer.midiNoteLimit(info_C_MIDI[2], mixerSettings.chordStab_Mins[2], mixerSettings.chordStab_Mins[2]));
	dspFaust.setParamValue(faustStrings.CSF4.c_str(), sequencer.midiNoteLimit(info_C_MIDI[3], mixerSettings.chordStab_Mins[3], mixerSettings.chordStab_Mins[3]));

	dspFaust.setParamValue(faustStrings.RF.c_str(), info_R_MIDI[0]);
	dspFaust.setParamValue(faustStrings.MV.c_str(), info_M_MIDI[1]);
	dspFaust.setParamValue(faustStrings.MF.c_str(), info_M_MIDI[0]);

	return true;
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
