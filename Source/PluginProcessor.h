#pragma once
#include <ctime>
#include "../JuceLibraryCode/JuceHeader.h"
#include "OSC_Class.h"
#include "imuRecording.h"
#include "GaitAnalysis.h"
#include "Sequencer.h"
#include "FaustStrings.h"
#include "SoniMappingCompute.h"
#include "audioParamInfo.h"
#include "windows.h"

class GaitSonificationAudioProcessor : public AudioProcessor, public HighResolutionTimer
{
public:
	GaitSonificationAudioProcessor();
	~GaitSonificationAudioProcessor();

	int globalTimeMs = 0;										// Time since program start (milliseconds)
	// Playback Controls //
	short musicMode = 1;										// Music Playback Mode // 1 = MIDI // 3 = INBUILT

	// Sensor Logging //
	imuRecording imuRecord;										// IMU Recording Class Object
	
	// Start sensor logging															
	void startRecording_Sensor()								
	{
		// CREATE NEW FILEPATH FOR RECORDINGS
		imuRecord.filePath_SensorRec = imuRecord.forRootDirectory.getSpecialLocation(File::currentApplicationFile).getFullPathName();
		imuRecord.filePath_SensorRec = imuRecord.filePath_SensorRec.upToLastOccurrenceOf("\\", true, false);
		imuRecord.filePath_SensorRec += String(exerciseMode_Present) + " - "
			+ String(gaitAnalysis.gaitParams.gaitParam_ObjectArray
				[gaitAnalysis.gaitParams.activeGaitParam].name) + " - REC - " + imuRecord.getCurrentTime();
		// FULL LOG
		String sensorFileName = imuRecord.filePath_SensorRec + "\\Full Log.csv";
		String sensorFileNameRaw = "";
		CreateDirectory(imuRecord.filePath_SensorRec.toStdString().c_str(), NULL);
		imuRecord.sensorRecording = fopen(sensorFileName.toStdString().c_str(), "w");
		short bodyPartIndex = 0;

		// RAW LOGS FOR ALL SENSORS
		for (int i = 0; i < gaitAnalysis.sensorInfo.numSensorsMax; i++)
		{
			if (gaitAnalysis.sensorInfo.isOnline[i])
			{
				bodyPartIndex = gaitAnalysis.sensorInfo.bodyLocation[i];
				sensorFileNameRaw = imuRecord.filePath_SensorRec + "\\Raw IMU Data - " + String(bodyPartIndex) + ".csv";
				imuRecord.rawSensorData[i] = fopen(sensorFileNameRaw.toStdString().c_str(), "w");
			}
		}

		// RESET TIME, ENABLE FLAG
		imuRecord.timeElapsed_REC = 0;
		imuRecord.isRecording_Sensor = true;
	}
	
	// Stop sensor logging
	void stopRecording_Sensor()
	{
		//DISABLE FLAG, CLOSE FILES
		imuRecord.isRecording_Sensor = false;
		//MAIN LOG
		fclose(imuRecord.sensorRecording);
		//RAW LOGS
		for (int i = 0; i < gaitAnalysis.sensorInfo.numSensorsMax; i++)
		{
			if (gaitAnalysis.sensorInfo.isOnline[i])
			{
				fclose(imuRecord.rawSensorData[i]);
			}
		}
	}
	
	// Write Present Line of Sensor Log
	void writeSensorValue_ToFile(float value)
	{
		// CONSTANT COLUMNS
		imuRecord.currentRow_FullLog_FLOAT[0] = (float)exerciseMode_Present;
		imuRecord.currentRow_FullLog_FLOAT[1] = imuRecord.timeElapsed_REC;
		imuRecord.currentRow_FullLog_FLOAT[2] = sequencer.isPlaying ? 1.0 : 0.0;
		imuRecord.currentRow_FullLog_FLOAT[3] = isStandby ? 1.0 : 0.0;
		imuRecord.currentRow_FullLog_STRING[0] = gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[gaitAnalysis.gaitParams.activeGaitParam].name;
		imuRecord.currentRow_FullLog_FLOAT[4] = value;
		imuRecord.currentRow_FullLog_STRING[1] = audioParams.audioParam_ObjectArray
			[audioParams.activeAudioParam].name;
		imuRecord.currentRow_FullLog_FLOAT[5] = isTargetDynamic ? dynamicTarget :
			gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[gaitAnalysis.gaitParams.activeGaitParam].target;
		imuRecord.currentRow_FullLog_STRING[2] = sequencer.currentMusic.baseBeats
			[sequencer.index_baseBeat].name;

		// EXERCISE DEPENDENT COLUMNS
		switch (exerciseMode_Present)
		{
		case 2:		// SB
			imuRecord.currentRow_FullLog_FLOAT[6] = gaitAnalysis.staticBalance_Div_Pitch;
			imuRecord.currentRow_FullLog_FLOAT[7] = gaitAnalysis.staticBalance_Div_Roll;
			imuRecord.currentRow_FullLog_FLOAT[8] = (float)gaitAnalysis.staticBalance_ZoneMap_Current;
			break;
		case 3:		// DB
			imuRecord.currentRow_FullLog_FLOAT[6] = gaitAnalysis.staticBalance_Div_Pitch;
			imuRecord.currentRow_FullLog_FLOAT[7] = gaitAnalysis.staticBalance_Div_Roll;
			imuRecord.currentRow_FullLog_FLOAT[8] = (float)gaitAnalysis.staticBalance_ZoneMap_Current;
			break;
		case 4:		// STS - JERK
			imuRecord.currentRow_FullLog_FLOAT[6] = 0;
			imuRecord.currentRow_FullLog_FLOAT[7] = 0;
			imuRecord.currentRow_FullLog_FLOAT[8] = 0;
			break;
		case 5:		// STS - ANGLE
			imuRecord.currentRow_FullLog_FLOAT[6] = gaitAnalysis.sitStand_Thresh_Sit;
			imuRecord.currentRow_FullLog_FLOAT[7] = gaitAnalysis.sitStand_Thresh_Stand;
			imuRecord.currentRow_FullLog_FLOAT[8] = 0;
			break;
		case 6:		// GAIT - RHYTHM
			imuRecord.currentRow_FullLog_FLOAT[6] = gaitAnalysis.isHalfTime ? 2.0 : 1.0;
			imuRecord.currentRow_FullLog_FLOAT[7] = gaitAnalysis.HS_IntervalTolerance;
			imuRecord.currentRow_FullLog_FLOAT[8] = 0;
			break;
		}

		imuRecord.writeToLog_Line_FullLog();

		//RAW DATA
		for (int i = 0; i < gaitAnalysis.sensorInfo.numSensorsMax; i++)
		{
			if (gaitAnalysis.sensorInfo.isOnline[i])
			{
				imuRecord.currentRow_RawLog_FLOAT[0] = gaitAnalysis.sensors_OSCReceivers[i].acc[0];
				imuRecord.currentRow_RawLog_FLOAT[1] = gaitAnalysis.sensors_OSCReceivers[i].acc[1];
				imuRecord.currentRow_RawLog_FLOAT[2] = gaitAnalysis.sensors_OSCReceivers[i].acc[2];
				imuRecord.currentRow_RawLog_FLOAT[3] = gaitAnalysis.sensors_OSCReceivers[i].gyr[0];
				imuRecord.currentRow_RawLog_FLOAT[4] = gaitAnalysis.sensors_OSCReceivers[i].gyr[1];
				imuRecord.currentRow_RawLog_FLOAT[5] = gaitAnalysis.sensors_OSCReceivers[i].gyr[2];
				imuRecord.currentRow_RawLog_FLOAT[6] = gaitAnalysis.sensors_OSCReceivers[i].isMessageRecvd_smpl_z0;
				imuRecord.writeToLog_Line_RAW(i);
			}
		}
	}

	// Gait Analysis //
	int pulseInterval_sensorCallback = 10;						// Pulse Interval - Sensor -> GaitAnalysis								
	GaitAnalysis gaitAnalysis;									// MP Computation Object
	void sensorCallback();										// Sensor Callback
	
	// Update Present AP
	void updateAudioParameter(int index, short type)			
	{ 
		float resetValue = 0;
		if (type == 1)											// MP Sonification Param
		{
			sequencer.dspFaust.setParamValue(soniAddress_Primary.c_str(), resetValue);
			audioParams.activeAudioParam = index - 1;
			soniAddress_Primary = audioParams.audioParam_ObjectArray
				[audioParams.activeAudioParam].faustAddress.toStdString();
			
		}
		if (type == 2)											// Music Cue Param
		{
			sequencer.dspFaust.setParamValue(soniAddress_Cue.c_str(), resetValue);
			audioParams.activeCueParam = index - 1;
			soniAddress_Cue = audioParams.audioParam_ObjectArray
				[audioParams.activeCueParam].faustAddress.toStdString();
		}
	};
	void applySequencerSonifications();							// Apply Sequencer-based Sonifications

	// Audio Parameter Calculation 
	bool isStandby = false;										// Standby Mode - No MP Sonification
	audioParamInfo audioParams;									// Audio Param Info
	float getCurrentMappingValue();								// Get Present Soni Mapping Value - Sensor
	std::string soniAddress_Primary = "";						// MP Sonification AP - FAUST Address
	std::string soniAddress_Cue = "";							// Cue AP - FAUST Address

	// Music Clocking

	void hiResTimerCallback();									// Precise Callback for All Operations
	int pulsesElapsed = 0;										// Hi Res Clock Pulses Elapsed
	float tempo = 0;											// Music Tempo -> Sequencer
	
	// Sets music tempo - Outreach: Sequencer, DspFaust, GaitAnalysis
	void setTempo(float value)									
	{
		tempo = value;
		double intervalMs = 60000 / tempo * 0.25;
		double intervalMultiplier = sequencer.isTripletMode ? 4.0 / 3.0 : 1.0;
		sequencer.midiTickIncrement = sequencer.isTripletMode ? 320 : 240;
		sequencer.ticksPerMS = 960 / (4 * intervalMs) * intervalMultiplier;
		interPulseIntervalMs = intervalMs * intervalMultiplier;
		sequencer.dspFaust.setParamValue(faustStrings.Tempo.c_str(),value);
		gaitAnalysis.beatInterval = 60/value;
		sequencer.musicPhase.setPhaseInc(tempo, 1000);
		if (isTargetDynamic)
			isCalibrated_dynTargetPhase = false;
	};
	double interPulseIntervalMs = 0.0;							// Tempo Dependent - Expected Pulse Time INC
	
	bool clockTriggeredLast = false;							// Is Clock Presently in Triggered State?
	
	void initializeClocking();									// Basic Clocking Init
	
	// Check - 16th Note Pulse Due?? Only for timekeeping, no more clocking
	bool checkIfPulseDue()										
	{
		double timeMeasure = sequencer.midiTicksElapsed;
		if (timeMeasure >= sequencer.nextPulseTime)
			return true;
		else return false;
	};
	void triggerClock(bool polarity);							//Set FAUST Master Clock to 1/0
	void clockCallback();										//Operations when clock triggered

	// Music Mapping

	FaustStrings faustStrings;									// Faust Address Strings

	// Music Sequencing

	Sequencer sequencer;										//Sequencer Object

	//Toggle Mute Status -> Sequencer
	void toggleTrackMuteManual(bool muted, short trackNum)				
	{
		std::string address = sequencer.faustStrings.getTrackMuteString(trackNum);
		int val = muted ? 1 : 0;
		muteValuesManual[trackNum] = val;
		sequencer.dspFaust.setParamValue(address.c_str(), val);
	}
	short muteValuesManual[8] = { 0 };									//Track Mute Status For DspFaust -> Sequencer

	//Arrange Note KeyNumbers Asc -> Sequencer
	void arrangeChordNotes_Asc(float *infoArray, int totalLength)		
	{
		float temp = 0;
		for (int i = 0; i < totalLength/2; i++)
		{
			for (int j = i + 1; j < totalLength/2; j++)
			{
				if (infoArray[j] < infoArray[i])
				{
					temp = infoArray[i];
					infoArray[i] = infoArray[j];
					infoArray[j] = temp;
				}
			}
		}
	}

	//Sonification
	SoniMappingCompute soniMappingCompute;								// Compute AP Value
	float mapVal = 0;													// AP Value					
	short exerciseMode_Present = 0;										
	
	// Exercise Mode
	void setExerciseMode(short exerciseMode)							// Set Exercise Mode
	{
		exerciseMode_Present = exerciseMode;
	};
	bool isTargetDynamic = false;										// Target Static/Dynamic -> SoniMappingCompute
	float dynamicTarget = 0;											// Dyn Target Value -> SoniMappingCompute
	float dynamicTarget_phaseTime = 0;									// Dyn Target Phase Time -> SoniMappingCompute
	bool isCalibrated_dynTargetPhase = false;							// is Dyn Target Calibrated? -> SoniMappingCompute
	bool isCalibrating = false;											// Is MP Calibrating? -> GaitAnalysis?

	// UNWANTED // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;
#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif
	void processBlock(AudioBuffer<float>&, MidiBuffer&) override;
	AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;
	const String getName() const override;
	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const String getProgramName(int index) override;
	void changeProgramName(int index, const String& newName) override;
	void getStateInformation(MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GaitSonificationAudioProcessor)
};
