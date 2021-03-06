#pragma once
#include <ctime>
#include "../JuceLibraryCode/JuceHeader.h"
#include "OSC_Class.h"
#include "systemSnapshot.h"
#include "imuRecording.h"
#include "GaitAnalysis.h"
#include "Sequencer.h"
#include "FaustStrings.h"
#include "audioParamInfo.h"
#include "dynTrajectory.h"
#include "windows.h"
#include <windows.h>
#include <wlanapi.h>
#include <iphlpapi.h>
#include <netioapi.h>

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

	// System Snapshot //
	systemSnapshot sysSnapshot;

	// Populate Drum Grooves
	void populateGrooves()
	{
		File forAppDirectory;
		String appPath = forAppDirectory.getSpecialLocation(File::currentApplicationFile).getFullPathName();
		appPath = appPath.upToLastOccurrenceOf("\\", true, false);
		appPath += "MIDI Drum Library\\Base\\";
		sequencer.currentMusic.populateStyles(appPath);
	}

	void storeSystemSnapshot()
	{
		// INITIALIZE PATHS, STORE INFO
		sysSnapshot.filePath_Snapshot = sysSnapshot.forRootDirectory.getSpecialLocation(File::currentApplicationFile).getFullPathName();
		sysSnapshot.filePath_Snapshot = sysSnapshot.filePath_Snapshot.upToLastOccurrenceOf("\\", true, false);
		sysSnapshot.filePath_Snapshot += "Settings Snapshot" + imuRecord.getCurrentTime();
		String snapshotFilename = sysSnapshot.filePath_Snapshot + +"\\Settings.csv";
		CreateDirectory(sysSnapshot.filePath_Snapshot.toStdString().c_str(), NULL);
		sysSnapshot.snapshot = fopen(snapshotFilename.toStdString().c_str(), "w");

		// COPY INFO TO OBJECT
		sysSnapshot.exerciseMode = gaitAnalysis.gaitParams.exerciseModes[exerciseMode_Present - 1];
		sysSnapshot.MP = gaitAnalysis.gaitParams.gaitParam_ObjectArray
						[gaitAnalysis.gaitParams.activeGaitParam].name;
		sysSnapshot.order_MapFunc = gaitAnalysis.gaitParams.order_MapFunc;
		sysSnapshot.quantLevels = gaitAnalysis.gaitParams.numQuant;
		sysSnapshot.target_MIN = gaitAnalysis.gaitParams.gaitParam_ObjectArray
								[gaitAnalysis.gaitParams.activeGaitParam].target_MIN;
		sysSnapshot.target_MAX = gaitAnalysis.gaitParams.gaitParam_ObjectArray
								[gaitAnalysis.gaitParams.activeGaitParam].target_MAX;
		sysSnapshot.polarity = gaitAnalysis.gaitParams.isPolarityNormal ? 1 : -1;
		sysSnapshot.tempo = tempo;
		sysSnapshot.rhythm = sequencer.currentMusic.styles[sequencer.currentMusic.style_current].name;
		
		for (int i = 0; i < 8; i++)
		{
			sysSnapshot.trackMutes[i] = sequencer.muteValues[i];
			sysSnapshot.variants[i] = sequencer.currentMusic.styles[sequencer.currentMusic.style_current].variantConfig[i];
			sysSnapshot.trackGains[i] = sequencer.currentMusic.styles[sequencer.currentMusic.style_current].variantConfig_GAINS[i];
		}

		sysSnapshot.SB_C_X = gaitAnalysis.staticBalance_BoundsCoordinates[0][0];
		sysSnapshot.SB_C_Y = gaitAnalysis.staticBalance_BoundsCoordinates[0][1];
		sysSnapshot.SB_Wid_X = gaitAnalysis.staticBalance_Div_Roll;
		sysSnapshot.SB_Wid_Y = gaitAnalysis.staticBalance_Div_Pitch;

		sysSnapshot.DB_Shape = dynZoneCenter.shapes[dynZoneCenter.currentShape];
		sysSnapshot.DB_Mirrored = dynZoneCenter.isMirrored ? 1 : -1;
		sysSnapshot.DB_Period = dynZoneCenter.period_Bars;
		sysSnapshot.DB_Radius = dynZoneCenter.radius_Deg;

		sysSnapshot.STS_angle_preSit = gaitAnalysis.sitStand_Thresh_Sit;
		sysSnapshot.STS_angle_preStand = gaitAnalysis.sitStand_Thresh_Stand;

		sysSnapshot.Gait_TimingMode = gaitAnalysis.isHalfTime ? "Half Time" : "Normal";
		sysSnapshot.Gait_ErrorTolerance = gaitAnalysis.HS_IntervalTolerance;
		sysSnapshot.Gait_AccThresh = gaitAnalysis.HS_thresh_pos;

		sysSnapshot.writeFile();
	}
	
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
		String repFileName = imuRecord.filePath_SensorRec + "\\Repetition Log.csv";
		String sensorFileNameRaw = "";
		CreateDirectory(imuRecord.filePath_SensorRec.toStdString().c_str(), NULL);
		imuRecord.sensorRecording = fopen(sensorFileName.toStdString().c_str(), "w");
		imuRecord.repetitionData = fopen(repFileName.toStdString().c_str(), "w");
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
		// REP LOGS FOR EXERCISE
		for (int i = 0; i < gaitAnalysis.gaitParams.reps_Completed[exerciseMode_Present - 1]; i++)
		{
			imuRecord.currentRow_Reps_FLOAT[0] = i + 1;
			imuRecord.currentRow_Reps_FLOAT[1] = gaitAnalysis.gaitParams.repTime_Sec[i][exerciseMode_Present - 1];
			imuRecord.writeToLog_Line_Reps();
		}

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

		// REP LOG
		fclose(imuRecord.repetitionData);
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
		imuRecord.currentRow_FullLog_FLOAT[5] = 
			gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[gaitAnalysis.gaitParams.activeGaitParam].target_MIN;
		imuRecord.currentRow_FullLog_FLOAT[6] = 
			gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[gaitAnalysis.gaitParams.activeGaitParam].target_MAX;
		imuRecord.currentRow_FullLog_STRING[2] = sequencer.currentMusic.styles
			[sequencer.currentMusic.style_current].name;

		// EXERCISE DEPENDENT COLUMNS
		switch (exerciseMode_Present)
		{
		case 2:		// SB
			imuRecord.currentRow_FullLog_FLOAT[7] = gaitAnalysis.staticBalance_Div_Pitch;
			imuRecord.currentRow_FullLog_FLOAT[8] = gaitAnalysis.staticBalance_Div_Roll;
			imuRecord.currentRow_FullLog_FLOAT[9] = (float)gaitAnalysis.staticBalance_ZoneMap_Current;
			break;
		case 3:		// DB
			imuRecord.currentRow_FullLog_FLOAT[7] = gaitAnalysis.staticBalance_Div_Pitch;
			imuRecord.currentRow_FullLog_FLOAT[8] = gaitAnalysis.staticBalance_Div_Roll;
			imuRecord.currentRow_FullLog_FLOAT[9] = (float)gaitAnalysis.staticBalance_ZoneMap_Current;
			break;
		case 4:		// STS - JERK
			imuRecord.currentRow_FullLog_FLOAT[7] = 0;
			imuRecord.currentRow_FullLog_FLOAT[8] = 0;
			imuRecord.currentRow_FullLog_FLOAT[9] = 0;
			break;
		case 5:		// STS - ANGLE
			imuRecord.currentRow_FullLog_FLOAT[7] = gaitAnalysis.sitStand_Thresh_Sit;
			imuRecord.currentRow_FullLog_FLOAT[8] = gaitAnalysis.sitStand_Thresh_Stand;
			imuRecord.currentRow_FullLog_FLOAT[9] = 0;
			break;
		case 6:		// GAIT - RHYTHM
			imuRecord.currentRow_FullLog_FLOAT[7] = gaitAnalysis.isHalfTime ? 2.0 : 1.0;
			imuRecord.currentRow_FullLog_FLOAT[8] = gaitAnalysis.HS_IntervalTolerance;
			imuRecord.currentRow_FullLog_FLOAT[9] = 0;
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
	void ipVerify_AssignedSensors();								// Send out OSC Packets for IP Verification

	// UPDATE PRESENT AP
	void updateAudioParameter(int index, short fbType_1D_2DXY)
	{
		// RESET ALL FEEDBACK IN ALL CASES
		float resetValue_1D = audioParams.audioParam_ObjectArray[audioParams.activeAudioParam].resetVal;
		sequencer.dspFaust.setParamValue(soniAddress_Primary.c_str(), resetValue_1D);
		float resetValue_2D_X = audioParams.audioParam_ObjectArray[audioParams.activeAudioParam_DynTarget_X].resetVal;
		sequencer.dspFaust.setParamValue(soniAddress_2D_X.c_str(), resetValue_2D_X);
		float resetValue_2D_Y = audioParams.audioParam_ObjectArray[audioParams.activeAudioParam_DynTarget_Y].resetVal;
		sequencer.dspFaust.setParamValue(soniAddress_2D_Y.c_str(), resetValue_2D_Y);
		int updatedParamIdx_Prev = 0;
		float resetValue_ACTUAL = 0;

		// INCREMENT ACTIVE INDEX, SET AP NAMES AND FAUST ADDRESSES
		switch (fbType_1D_2DXY)
		{
		case 1:
			updatedParamIdx_Prev = audioParams.activeAudioParam;
			audioParams.activeAudioParam = index - 1;
			resetValue_ACTUAL = resetValue_1D;
			sequencer.soni_AP_Name = audioParams.audioParam_ObjectArray[audioParams.activeAudioParam].name;
			soniAddress_Primary = sequencer.faustStrings.getSonificationStringAddress
			(audioParams.audioParam_ObjectArray[audioParams.activeAudioParam].faustIndex).toStdString();
			break;
		case 3:
			updatedParamIdx_Prev = audioParams.activeAudioParam_DynTarget_X;
			audioParams.activeAudioParam_DynTarget_X = index - 1;
			resetValue_ACTUAL = resetValue_2D_X;
			soniAddress_2D_X = sequencer.faustStrings.getSonificationStringAddress
			(audioParams.audioParam_ObjectArray[audioParams.activeAudioParam_DynTarget_X].faustIndex).toStdString();
			sequencer.X_2D_AP_Name = audioParams.audioParam_ObjectArray
			[audioParams.activeAudioParam_DynTarget_X].name;
			break;
		case 4:
			updatedParamIdx_Prev = audioParams.activeAudioParam_DynTarget_Y;
			audioParams.activeAudioParam_DynTarget_Y = index - 1;
			resetValue_ACTUAL = resetValue_2D_Y;
			soniAddress_2D_Y = sequencer.faustStrings.getSonificationStringAddress
			(audioParams.audioParam_ObjectArray[audioParams.activeAudioParam_DynTarget_Y].faustIndex).toStdString();
			sequencer.Y_2D_AP_Name = audioParams.audioParam_ObjectArray
			[audioParams.activeAudioParam_DynTarget_Y].name;
			break;
		}

		// CASEWISE AP HYPERPARAMETER RESET - GET PREVIOUS AP NAME
		String apName = audioParams.audioParam_ObjectArray[updatedParamIdx_Prev].name;
		
		// TEMPO
		if (apName == "Tempo")
			sequencer.tempoTickInc.ap_forSkew = resetValue_ACTUAL;

		// TEMPO 1D
		if (apName == "Tempo 1D")
			sequencer.tempoTickInc.ap_forSkew_1D = resetValue_ACTUAL;
	}
	
	void applySequencerSonifications();							// Apply Sequencer-based Sonifications

	// Audio Parameter Calculation 
	bool isStandby = false;										// Standby Mode - No MP Sonification
	audioParamInfo audioParams;									// Audio Param Info
	std::string soniAddress_Primary = "";						// MP Sonification AP - FAUST Address
	std::string soniAddress_Cue = "";							// Cue AP - FAUST Address
	std::string soniAddress_2D_X = "";							// 2D X AP - FAUST Address
	std::string soniAddress_2D_Y = "";							// 2D Y AP - FAUST Address

	// Music Clocking

	void hiResTimerCallback();									// Precise Callback for All Operations
	int pulsesElapsed = 0;										// Hi Res Clock Pulses Elapsed
	float tempo = 0;											// Music Tempo -> Sequencer
	
	// Sets music tempo - Outreach: Sequencer, DspFaust, GaitAnalysis
	void setTempo(float value)									
	{
		tempo = value;
		sequencer.tempo = value;
		double intervalMs = 60000 / tempo * 0.25;
		double intervalMultiplier = sequencer.isTripletMode ? 4.0 / 3.0 : 1.0;
		sequencer.midiTickIncrement = sequencer.isTripletMode ? 320 : 240;
		sequencer.ticksPerMS = 960 / (4 * intervalMs) * intervalMultiplier;
		interPulseIntervalMs = intervalMs * intervalMultiplier;
		sequencer.dspFaust.setParamValue(sequencer.faustStrings.Tempo.c_str(),value);
		gaitAnalysis.beatInterval = 60/value;
		sequencer.musicPhase.setPhaseInc(tempo, 1000);
		dynZoneCenter.calcAnticipationPhase(value);
	};
	
	double interPulseIntervalMs = 0.0;							// Tempo Dependent - Expected Pulse Time INC
	bool clockTriggeredLast = false;							// Is Clock Presently in Triggered State?
	void initializeClocking();									// Basic Clocking Init
	
	// Check - 16th Note Pulse Due?? Only for timekeeping, no more clocking
	bool checkIfPulseDue()										
	{
		return (sequencer.midiTicksElapsed >= sequencer.nextPulseTime);
	};
	void triggerClock();							//Set FAUST Master Clock to 1/0
	void clockCallback(double tickInc);							//Operations when clock triggered

	// Music Sequencing
	Sequencer sequencer;										

	// Dyn Trajectory
	dynTrajectory dynZoneCenter;

	//Sonification
	float mapVal = 0;													// AP Value					
	short exerciseMode_Present = 0;										
	void mapMBFvar_FAUST(float mapVal_1D, float mapVal_2D_X, float mapVal_2D_Y);
	void dynTrajectory_updateCenterCoordinates()
	{
		if (exerciseMode_Present == 3)
		{
			// IF DYNAMIC REACHING, ENABLE DYN TRAJECTORY, UPDATE CENTER COORDINATES
			float dynReach_CenterCoordinates[2] = { 0.0, 0.0 };
			float dynReach_CenterCoordinates_ANTICIPATED[2] = { 0.0, 0.0 };

			// FETCH PRESENT INFO ON 2D CENTER COORDINATES
			dynZoneCenter.barsElapsed = sequencer.barsElapsed;
			dynReach_CenterCoordinates[0] = gaitAnalysis.staticBalance_BoundsCoordinates[0][0];
			dynReach_CenterCoordinates[1] = gaitAnalysis.staticBalance_BoundsCoordinates[0][1];

			// GET NEW 2D TARGET CENTER COORDINATES AND ASSIGN 
			dynZoneCenter.getCenterCoordinates(sequencer.musicPhase.presentPhase_Rad,
				dynReach_CenterCoordinates);
			gaitAnalysis.staticBalance_BoundsCoordinates[0][0] = dynReach_CenterCoordinates[0];
			gaitAnalysis.staticBalance_BoundsCoordinates[0][1] = dynReach_CenterCoordinates[1];

			// Feedback Type: Anticipated Distance Error (2D)
			if (gaitAnalysis.staticBalance_FB_TYPE == 3)
			{
				// GET ANTICIPATED CENTER COORDINATES AND ASSIGN
				dynZoneCenter.getCenterCoordinates(sequencer.musicPhase.presentPhase_Rad + dynZoneCenter.anticipationPhase,
					dynReach_CenterCoordinates_ANTICIPATED);
				gaitAnalysis.staticBalance_CenterXY_ANTICIPATED[0] = dynReach_CenterCoordinates[0];
				gaitAnalysis.staticBalance_CenterXY_ANTICIPATED[1] = dynReach_CenterCoordinates[1];
			}
		};
	}
	
	// Exercise Mode
	void setExerciseMode(short exerciseMode)							// Set Exercise Mode
	{
		if (exerciseMode != 3) gaitAnalysis.gaitParams.feedback_DIM = 1;
		else if (gaitAnalysis.staticBalance_FB_TYPE > 1) gaitAnalysis.gaitParams.feedback_DIM = 2;
		else gaitAnalysis.gaitParams.feedback_DIM = 1;
		exerciseMode_Present = exerciseMode;
		gaitAnalysis.gaitParams.exerciseMode_Present = exerciseMode;
	};
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
