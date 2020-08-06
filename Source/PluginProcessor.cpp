/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
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

//DESTRUCTOR - STOP DSPFAUST AND TIMER
GaitSonificationAudioProcessor::~GaitSonificationAudioProcessor()
{
	dspFaust.stop();
	stopTimer();
}

// BEGIN IMU SENSOR RECORDING ------------------------------------------MOVE TO NEW CLASS
void GaitSonificationAudioProcessor::startRecording_Sensor()
{
		// CREATE NEW FILEPATH FOR RECORDINGS
		filePath_SensorRec = forRootDirectory.getSpecialLocation(File::currentApplicationFile).getFullPathName();
		filePath_SensorRec = filePath_SensorRec.upToLastOccurrenceOf("\\", true, false);
		filePath_SensorRec += String(exerciseMode_Present) + " - " 
			+ String(gaitAnalysis.gaitParams.gaitParam_ObjectArray
				[gaitAnalysis.gaitParams.activeGaitParam].name) + " - REC - " + getCurrentTime();
		// FULL LOG
		String sensorFileName = filePath_SensorRec + "\\Full Log.csv";
		String sensorFileNameRaw = "";
		CreateDirectory(filePath_SensorRec.toStdString().c_str(), NULL);
		sensorRecording = fopen(sensorFileName.toStdString().c_str(), "w");

		short bodyPartIndex = 0;

		// RAW LOGS FOR ALL SENSORS
		for (int i = 0; i < gaitAnalysis.sensorInfo.numSensorsMax; i++)
		{
			if (gaitAnalysis.sensorInfo.isOnline[i])
			{
				bodyPartIndex = gaitAnalysis.sensorInfo.bodyLocation[i];
				sensorFileNameRaw = filePath_SensorRec + "\\Raw IMU Data - " + String(bodyPartIndex) + ".csv";
				rawSensorData[i] = fopen(sensorFileNameRaw.toStdString().c_str(), "w");
			}
		}
		
		// RESET TIME, ENABLE FLAG
		timeElapsed_REC = 0;
		isRecording_Sensor = true;
}

void GaitSonificationAudioProcessor::stopRecording_Sensor()
{
	//DISABLE FLAG, CLOSE FILES
	isRecording_Sensor = false;
	//MAIN LOG
	fclose(sensorRecording);
	//RAW LOGS
	for (int i = 0; i < gaitAnalysis.sensorInfo.numSensorsMax; i++)
	{
		if (gaitAnalysis.sensorInfo.isOnline[i])
		{
			fclose(rawSensorData[i]);
		}
	}
}

void GaitSonificationAudioProcessor::writeSensorValue_ToFile(float value)
{
	std::string format_GP = "%f,%f,%f,%f,%s,%f,%s,%f,%s,%f,%f,%f,\n";
	std::string format_RAW = "%f,%f,%f,%f,%f,%f,%f,\n";

	//FULL LOG
	switch (exerciseMode_Present)
	{
	case 2:
		fprintf(sensorRecording,						//FID
			format_GP.c_str(),							//Format Specifier
			(float)exerciseMode_Present,						//Exercise Mode			// %f
			timeElapsed_REC,							//Recording Timestamp	// %f
			isPlaying ? 1.0 : 0.0,						//Is Playing?			// %f
			isStandby ? 1.0 : 0.0,						//Is Standby?			// %f
			gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[gaitAnalysis.gaitParams.activeGaitParam].name,	//Active MP Name		// %s
			value,										//MP Value				// %f
			audioParams.audioParam_ObjectArray[audioParams.activeAudioParam].name,		//AP Name				// %s
			isTargetDynamic ? dynamicTarget : 
			gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[gaitAnalysis.gaitParams.activeGaitParam].target,	//Target				// %f
			sequencer.currentMusic.baseBeats
			[sequencer.index_baseBeat].name,			//Rhythm Name			// %s
			gaitAnalysis.staticBalance_Div_Pitch,		//AP Multiplier			// %f
			gaitAnalysis.staticBalance_Div_Roll,		//ML Multiplier			// %f
			(float)gaitAnalysis.staticBalance_ZoneMap_Current	//Change Rate			// %f
		);
		break;
	case 4:
		fprintf(sensorRecording,						//FID
			format_GP.c_str(),							//Format Specifier
			(float)exerciseMode_Present,						//Exercise Mode			// %f
			timeElapsed_REC,							//Recording Timestamp	// %f
			isPlaying ? 1.0 : 0.0,						//Is Playing?			// %f
			isStandby ? 1.0 : 0.0,						//Is Standby?			// %f
			gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[gaitAnalysis.gaitParams.activeGaitParam].name,	//Active MP Name		// %s
			value,										//MP Value				// %f
			audioParams.audioParam_ObjectArray[audioParams.activeAudioParam].name,					//AP Name				// %s
			isTargetDynamic ? dynamicTarget :
			gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[gaitAnalysis.gaitParams.activeGaitParam].target,	//Target				// %f
			sequencer.currentMusic.baseBeats
			[sequencer.index_baseBeat].name,			//Rhythm Name			// %s
			gaitAnalysis.sitStand_Thresh_Sit,			//Sit Thresh			// %f
			gaitAnalysis.sitStand_Thresh_Stand,			//Stand Thresh			// %f
			0											//Blank			// %f
		);
		break;
	case 5:
		fprintf(sensorRecording,						//FID
			format_GP.c_str(),							//Format Specifier
			(float)exerciseMode_Present,						//Exercise Mode			// %f
			timeElapsed_REC,							//Recording Timestamp	// %f
			isPlaying ? 1.0 : 0.0,						//Is Playing?			// %f
			isStandby ? 1.0 : 0.0,						//Is Standby?			// %f
			gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[gaitAnalysis.gaitParams.activeGaitParam].name,	//Active MP Name		// %s
			value,										//MP Value				// %f
			audioParams.audioParam_ObjectArray[audioParams.activeAudioParam].name,					//AP Name				// %s
			isTargetDynamic ? dynamicTarget :
			gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[gaitAnalysis.gaitParams.activeGaitParam].target,	//Target				// %f
			sequencer.currentMusic.baseBeats
			[sequencer.index_baseBeat].name,			//Rhythm Name			// %s
			gaitAnalysis.isHalfTime ? 2.0 : 1.0,		//Timing Mode			// %f
			gaitAnalysis.HS_IntervalTolerance,			//CoV Tolerance			// %f
			0											//Blank			// %f
		);
		break;
	default:
		fprintf(sensorRecording,						//FID
			format_GP.c_str(),							//Format Specifier
			(float)exerciseMode_Present,						//Exercise Mode			// %f
			timeElapsed_REC,							//Recording Timestamp	// %f
			isPlaying ? 1.0 : 0.0,						//Is Playing?			// %f
			isStandby ? 1.0 : 0.0,						//Is Standby?			// %f
			gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[gaitAnalysis.gaitParams.activeGaitParam].name,	//Active MP Name		// %s
			value,										//MP Value				// %f
			audioParams.audioParam_ObjectArray[audioParams.activeAudioParam].name,					//AP Name				// %s
			isTargetDynamic ? dynamicTarget :
			gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[gaitAnalysis.gaitParams.activeGaitParam].target,	//Target				// %f
			sequencer.currentMusic.baseBeats
			[sequencer.index_baseBeat].name,			//Rhythm Name			// %s
			(float)gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[gaitAnalysis.gaitParams.activeGaitParam].desiredBehavior,	//Desired Behavior		// %f
			0,											//Blank					// %f
			0											//Blank					// %f
		);
		break;
	}

	//RAW DATA
	for (int i = 0; i < gaitAnalysis.sensorInfo.numSensorsMax; i++)
	{
		if (gaitAnalysis.sensorInfo.isOnline[i])
		{
			fprintf(rawSensorData[i], format_RAW.c_str(),
				gaitAnalysis.sensors_OSCReceivers[i].acc[0],
				gaitAnalysis.sensors_OSCReceivers[i].acc[1],
				gaitAnalysis.sensors_OSCReceivers[i].acc[2],
				gaitAnalysis.sensors_OSCReceivers[i].gyr[0],
				gaitAnalysis.sensors_OSCReceivers[i].gyr[1],
				gaitAnalysis.sensors_OSCReceivers[i].gyr[2],
				gaitAnalysis.sensors_OSCReceivers[i].isMessageRecvd_smpl_z0);
		}
	}
	
}

//FIRST TIME SET SENSOR PULSE INTERVALS, NEXT SIXTEENTH MUSIC PULSE TIME
void GaitSonificationAudioProcessor::initializeClocking()
{
	pulseInterval_sensorCallback = 1000 / gaitAnalysis.sensorInfo.IMU_SampleRate;
	nextPulseTime += interPulseIntervalMs * 0.001;
}

// FIRST TIME INITIALIZE MUTES, VARIANTS, MASTER GAIN, MAP
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

// MAP CURRENT VARIANT GAIN - SINGLE TRACK
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

// APPLY AND MAP CURRENT VARIANT GAIN - SINGLE TRACK (on Variant Change)
void GaitSonificationAudioProcessor::setTrackGains(int trackIndex, float value)
{
	mixerSettings.trackGain_Offsets[sequencer.index_baseBeat][trackIndex] = value;
	applyCurrentVariantGain(trackIndex);
}

// APPLY AND MAP MUTE VALUE - SINGLE TRACK
void GaitSonificationAudioProcessor::setTrackMutes(int trackIndex, int value)
{
	std::string address = faustStrings.baseName + faustStrings.trackMutes[trackIndex];
	dspFaust.setParamValue(address.c_str(), value);
}

// APPLY AND MAP VARIANT EQ - SINGLE TRACK (on Variant Change)
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

// APPLY AND MAP VARIANT COMP - SINGLE TRACK (on Variant Change)
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

// SWITCH INSTRUMENT VARIANT - SINGLE TRACK
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

// LOAD MIDI SONG FILE
void GaitSonificationAudioProcessor::setFilename(String name)
{
	stopMusic();
	sequencer.loadNewFile_MIDI(name);							// Load MIDI Song File from PATH
	togglePlayPause();
}

// HANDLE TAP TEMPO BUTTON PRESS
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

// PAUSE OR PLAY IF FILE HAS BEEN LOADED
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

// STOP MUSIC, RESET COUNTERS AND TIME
void GaitSonificationAudioProcessor::stopMusic()
{
	if (isPlaying)
	{
		isPlaying = false;
		dspFaust.stop();
		sequencer.resetCounters();
		sequencer.resetMidiPlayback();
		midiTicksElapsed = 0;
		timeElapsed = 0;
		nextPulseTime = 0;
		sequencer.musicPhase.resetPhase();
	}
}

void GaitSonificationAudioProcessor::hiResTimerCallback()
{
	gaitAnalysis.timeElapsed = globalTimeMs++/ 1000.0;
	if (globalTimeMs % 2000 == 0)
		gaitAnalysis.updateSensorStatus();
	pulsesElapsed += 1;
	if (isPlaying)
	{
		clockCallback();
		timeElapsed += 0.001;
		dynamicTarget_phaseTime += 0.001;
		pulsesElapsed = pulsesElapsed % 100;
		if (musicMode == 1 || musicMode == 3)
		midiTicksElapsed += ticksPerMS;

		if (tapTempoCounter <= 1)
			tapTempoCounter += 0.001;

		if(getSongProgress())					// TRUE if complete. 
			stopMusic();
	}
	if (isRecording_Sensor)
		timeElapsed_REC += 0.001;
	if (pulsesElapsed % pulseInterval_sensorCallback == 0)
		sensorCallback();
}

void GaitSonificationAudioProcessor::clockCallback()
{
	double clockPulseLength = 0;
	double timeMeasure = 0;
	switch (musicMode)
	{
	case 1: 
		clockPulseLength = 70 * ticksPerMS;
		timeMeasure = midiTicksElapsed;
		break;
	case 2:
		clockPulseLength = 0.07;
		timeMeasure = timeElapsed;
		break;
	case 3:
		clockPulseLength = 70 * ticksPerMS;
		timeMeasure = midiTicksElapsed;
		break;
	}

	if (clockTriggeredLast && (timeMeasure - lastPulseTime >= clockPulseLength))
		triggerClock(false);

	if (checkIfPulseDue())
		triggerClock(true);

	if (musicMode == 1 || musicMode == 3)
	{
		if (fetch_MusicInfo_Mode_MIDI())
			infoMapped_CurrentPulse_MIDI = mapMusicInfo_Mode_MIDI();
	}	

	sequencer.musicPhase.updatePhase();

	// ADD CUEING MAPPING
	dspFaust.setParamValue(soniAddress_Cue.c_str(), sequencer.musicPhase.emphFunc_Present); // Map Primary Sonification
}

void GaitSonificationAudioProcessor::fetch_MusicInfo_Mode_COMMON()
{
	//info_R_CSV[0] = sequencer.fetchNewMusicInfo(5, 1, 0);			// Riff Vel
	//sequencer.fetchChordNoteVelocities(info_C_V_COMMON);						// Chord Vel
	//info_CS_V_COMMON = sequencer.fetchNewMusicInfo(7, 1, 0);			// CS Vel
}

bool GaitSonificationAudioProcessor::fetch_MusicInfo_Mode_MIDI()
{
	bool isNew_MIDI_M =
		sequencer.checkMIDIEventsDue(1, 1, false, ticksPerMS, midiTicksElapsed, info_M_MIDI
			,audioParams.audioParam_ObjectArray[audioParams.activeCueParam].name);
	bool isNew_MIDI_C =
		sequencer.checkMIDIEventsDue(2, 4, true, ticksPerMS, midiTicksElapsed, info_C_MIDI
			, audioParams.audioParam_ObjectArray[audioParams.activeCueParam].name);
	bool isNew_MIDI_R = 
		sequencer.checkMIDIEventsDue(3, 1, true, ticksPerMS, midiTicksElapsed, info_R_MIDI
			, audioParams.audioParam_ObjectArray[audioParams.activeCueParam].name);
	info_R_MIDI[0] = sequencer.midiNoteLimit(info_R_MIDI[0],
		mixerSettings.var_noteMin[4][mixerSettings.currentVariant[sequencer.index_baseBeat][4] - 1],
		mixerSettings.var_noteMax[4][mixerSettings.currentVariant[sequencer.index_baseBeat][4] - 1]);
	if (isNew_MIDI_C) arrangeChordNotes_Asc(info_C_MIDI, 8);
	// CHECK PERC EVENTS NEW
	bool isNew_MIDI_PERC_BASE = 
		sequencer.checkMIDIEventsDue(4, 4, false, ticksPerMS, midiTicksElapsed, info_PERC_V_COMMON
			, audioParams.audioParam_ObjectArray[audioParams.activeCueParam].name);
	bool isNew_MIDI_PERC_SnareFlurry = false;
	if (audioParams.audioParam_ObjectArray[audioParams.activeCueParam].name == "Snare Flurry")
	{
		info_PERC_V_COMMON[1] = 0;
		isNew_MIDI_PERC_SnareFlurry = sequencer.checkMIDIEventsDue(6, 4, false, ticksPerMS, midiTicksElapsed, info_PERC_V_COMMON
			, audioParams.audioParam_ObjectArray[audioParams.activeCueParam].name);
	}
	return isNew_MIDI_M || isNew_MIDI_C || isNew_MIDI_R || isNew_MIDI_PERC_BASE || isNew_MIDI_PERC_SnareFlurry;
}
void GaitSonificationAudioProcessor::map_MusicInfo_Mode_COMMON()
{
	dspFaust.setParamValue(faustStrings.CV1.c_str(), info_C_V_COMMON[0]);
	dspFaust.setParamValue(faustStrings.CV2.c_str(), info_C_V_COMMON[1]);
	dspFaust.setParamValue(faustStrings.CV3.c_str(), info_C_V_COMMON[2]);
	dspFaust.setParamValue(faustStrings.CV4.c_str(), info_C_V_COMMON[3]);
	
	dspFaust.setParamValue(faustStrings.CSV.c_str(),	info_CS_V_COMMON);
	dspFaust.setParamValue(faustStrings.CurrentStyle.c_str(), 0);

	short muteVal = 0;
	for (int i = 0; i < 8; i++)
	{
		std::string paramAddress = faustStrings.getTrackMuteString(i);
		muteVal = muteValuesManual[i];
		dspFaust.setParamValue(paramAddress.c_str(), muteVal);
	}
}

bool GaitSonificationAudioProcessor::mapMusicInfo_Mode_MIDI()
{
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

void GaitSonificationAudioProcessor::sensorCallback()
{
	for (int i = 0; i < gaitAnalysis.sensorInfo.numSensorsMax; i++)
		if (gaitAnalysis.sensorInfo.isOnline[i])
		gaitAnalysis.sensors_OSCReceivers[i].updateBuffers();						//UPDATE BUFFERS

	gaitAnalysis.compute(gaitAnalysis.gaitParams.activeGaitParam, isCalibrating);	//COMPUTE MOVEMENT PARAM
	if (pulsesElapsed % 3000 == 0)
		gaitAnalysis.calibrateRest(gaitAnalysis.sensors_OSCReceivers[gaitAnalysis.idx_Sensor_Trunk].acc_Buf);
	
	mapVal = jlimit(audioParams.audioParam_ObjectArray[audioParams.activeAudioParam].minVal,
					audioParams.audioParam_ObjectArray[audioParams.activeAudioParam].maxVal,
		getCurrentMappingValue());
	if (isPlaying)
	{
	mapVal = isStandby ? 0 : mapVal;
	if(audioParams.audioParam_ObjectArray[audioParams.activeAudioParam].type == 1)
	dspFaust.setParamValue(soniAddress_Primary.c_str(),mapVal); // Map Primary Sonification
	/*if (audioParams.audioParam_ObjectArray[audioParams.activeAudioParam].type == 2)
	applySequencerSonifications();*/
	}

	if (isRecording_Sensor)
		writeSensorValue_ToFile(gaitAnalysis.gaitParams.gaitParam_ObjectArray
		[gaitAnalysis.gaitParams.activeGaitParam].currentValue);
}

void GaitSonificationAudioProcessor::applySequencerSonifications()
{
	switch (audioParams.activeAudioParam)
	{
		
	}
}

float GaitSonificationAudioProcessor::getCurrentMappingValue()
{
	short activeGaitParam = gaitAnalysis.gaitParams.activeGaitParam;
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

	if (gaitAnalysis.gaitParams.activeGaitParam == 6)									// STATIC PROJECTION
	{
		if (!soniMappingCompute.isSoniSource_Slider)
			return gaitAnalysis.staticBalance_ZoneMaps[gaitAnalysis.staticBalance_ZoneMap_Current - 1]
			[(int)gaitAnalysis.gaitParams.gaitParam_ObjectArray[2].currentValue - 1];
		else
			return gaitAnalysis.staticBalance_ZoneMaps[gaitAnalysis.staticBalance_ZoneMap_Current - 1]
			[(int)(soniMappingCompute.soniVal_Slider * 5)];
	}

	if (isTargetDynamic)
	{
		targetValue = soniMappingCompute.getDynamicTarget(targetValue, soniMappingCompute.movementFunc_Order,
															tempo,dynamicTarget_phaseTime);
		dynamicTarget = targetValue;
		toleranceBW = soniMappingCompute.movement_ErrorTolerance;
	}
	
	float functionParamSet[6] = {order_MappingFunc, toleranceBW, numQuantLevels, smoothingFc, range, desiredBehavior};

	if (!soniMappingCompute.isSoniSource_Slider)
		return jlimit((float)0,(float)1,soniMappingCompute.computeParamValue(currentValue, targetValue, functionParamSet));
	else
		return jlimit((float)0, (float)1, soniMappingCompute.computeParamValue_SliderSource(soniMappingCompute.soniVal_Slider,targetValue, functionParamSet));
}

void GaitSonificationAudioProcessor::onStartMusic()
{
	dspFaust.start();
	initializeTrackGains();
}

void GaitSonificationAudioProcessor::applyMasterGain(float value)
{
	mixerSettings.masterGain = value;
	std::string address = faustStrings.baseName + faustStrings.MasterVol;
	dspFaust.setParamValue(address.c_str(), value);
}

void GaitSonificationAudioProcessor::triggerClock(bool polarity)
{
	std::string masterClockAddress = faustStrings.baseName + faustStrings.MasterClock;
	if (polarity == true)
	{
		//FAUST SET ON
		handleNewClockPulse();
		dspFaust.setParamValue(masterClockAddress.c_str(), 1.0);
		lastPulseTime = nextPulseTime;
		switch (musicMode)
		{
		case 1:
			nextPulseTime += midiTickIncrement;
			break;
		case 2:
			nextPulseTime += interPulseIntervalMs * 0.001;
			break;
		case 3:
			nextPulseTime += midiTickIncrement;
			break;
		}
		clockTriggeredLast = true;

		if (isTargetDynamic && isCalibrated_dynTargetPhase && sequencer.barsElapsed_withinMeasure == 3)
		{
			isCalibrated_dynTargetPhase = false;
		}

		if (isTargetDynamic && !isCalibrated_dynTargetPhase && sequencer.barsElapsed_withinMeasure == 0)
		{
			dynamicTarget_phaseTime = 0;
			isCalibrated_dynTargetPhase = true;
		}
	}
	else
	{
		//FAUST SET OFF
		dspFaust.setParamValue(masterClockAddress.c_str(), 0);
		clockTriggeredLast = false;
	}
}

void GaitSonificationAudioProcessor::handleNewClockPulse()
{
	sequencer.incrementPulseCounter();
	fetch_MusicInfo_Mode_COMMON();

	map_MusicInfo_Mode_COMMON();
}

void GaitSonificationAudioProcessor::toggleTrackMuteManual(bool muted, short trackIndex)
{
	int val = muted ? 1 : 0;
	muteValuesManual[trackIndex] = val;
}

//==============================================================================
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
                // so this should be at least 1, even if you're not really implementing programs.
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

//==============================================================================
void GaitSonificationAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void GaitSonificationAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
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

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool GaitSonificationAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* GaitSonificationAudioProcessor::createEditor()
{
    return new GaitSonificationAudioProcessorEditor (*this);
}

//==============================================================================
void GaitSonificationAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GaitSonificationAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GaitSonificationAudioProcessor();
}
