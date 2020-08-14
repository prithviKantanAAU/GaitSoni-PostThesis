#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "UI_MusicControls_General.h"
#include "UI_MusicControls_Inbuilt.h"
#include "UI_MusicControls_IndividualTrack.h"
#include "UI_BMBFSettings_General.h"
#include "UI_DynTarget.h"
#include "UI_MPCalibrate.h"
#include "UI_SensorConfig.h"
#include "UI_TabSpecific.h"
#include "UI_BMBFSettings_ExerciseSpecific.h"
#include "UI_RTVisualize_1D.h"
#include "UI_ZoneVisualize_2D.h"
#include "RingMusicVisualize.h"

class GaitSonificationAudioProcessorEditor : public AudioProcessorEditor, public Timer, public ComboBox::Listener
{
public:
	GaitSonificationAudioProcessorEditor(GaitSonificationAudioProcessor&);
	~GaitSonificationAudioProcessorEditor();

	UI_BMBFSettings_ExerciseSpecific ui_bmbf_ex;
	UI_BMBFSettings_General ui_bmbf_gen;
	UI_DynTarget ui_dyn;
	UI_MPCalibrate ui_mpCal;
	UI_MusicControls_General ui_musiCon_gen;
	UI_MusicControls_Inbuilt ui_musiCon_inbuilt;
	UI_MusicControls_IndividualTrack ui_musiCon_indiv;
	UI_RTVisualize_1D ui_rtv_1d;
	UI_SensorConfig ui_sensorCon;
	UI_TabSpecific ui_tabSpec;
	UI_ZoneVisualize_2D ui_zv_2d;
	RingMusicVisualize ringVisualize;

	void paint(Graphics&) override;
	void timerCallback() override;
	void resized() override;

private:
	
	GaitSonificationAudioProcessor& processor;
	std::unique_ptr<TabbedComponent> tabs;
	int UIRefreshFreq = 30;							// UI Real Time Refresh Frequency (Hz)
	short presentTab = 0;							// Global tab index
	int previousTab = 0;							// To detect changes
	short presentMusiCon_Disp = 0;					// 0 = Channel EQ/Comp // 1 = Ring Visualize

	//SENSOR CONFIGURATION SECTION
	
	// Initialize Sensor Config Controls
	void sensorConfig_initializeControls();
	
	// Update Sensor Config Status in Real Time
	void sensorConfig_updateLabels()				
	{
		String text = "";
		for (int i = 0; i < 3; i++)
		{
			text = processor.gaitAnalysis.sensorInfo.isOnline[i] ? "ON" : "OFF";
			ui_sensorCon.Status[i].setText(text,dontSendNotification);
			if (processor.gaitAnalysis.sensorInfo.isOnline[i])
				ui_sensorCon.BiasComp[i].setVisible(true);
			else
				ui_sensorCon.BiasComp[i].setVisible(false);
			if (processor.gaitAnalysis.sensors_OSCReceivers[i].isBiasComp_ON)
			{
				ui_sensorCon.BiasComp[i].setButtonText("Calibrating");
				ui_sensorCon.BiasComp[i].setColour(ui_sensorCon.BiasComp[i].buttonColourId, Colours::blue);
			}
			if (processor.gaitAnalysis.sensors_OSCReceivers[i].isBiasComp_DONE)
			{
				ui_sensorCon.BiasComp[i].setButtonText("Calibrated");
				ui_sensorCon.BiasComp[i].setColour(ui_sensorCon.BiasComp[i].buttonColourId, Colours::green);
			}
		}
	};
	
	// Update Required Sensor Status in real time
	void updateOSC_ConnectionDisplay()						
	{
		if (processor.gaitAnalysis.areRequiredSensorsOnline())
		{
			ui_tabSpec.status_SensorConnection.setText("Reqd. Sensors: ONLINE", dontSendNotification);
			ui_tabSpec.status_SensorConnection.setColour
			(ui_tabSpec.status_SensorConnection.textColourId, Colours::green);
			if (!processor.imuRecord.isRecording_Sensor)				// WHY THE FUCK IS THIS HERE?
			{
				ui_bmbf_gen.recordGaitParam.setButtonText("Rec Gait Param");
				ui_bmbf_gen.recordGaitParam.setColour(ui_bmbf_gen.recordGaitParam.buttonColourId, Colours::red);
			}
		}
		else
		{
			ui_tabSpec.status_SensorConnection.setText("Reqd. Sensors: OFFLINE", dontSendNotification);
			ui_bmbf_gen.recordGaitParam.setColour(ui_bmbf_gen.recordGaitParam.buttonColourId, Colours::darkred);
			ui_tabSpec.status_SensorConnection.setColour(ui_tabSpec.status_SensorConnection.textColourId, Colours::red);
		}
	}
	
										//ADD AND MAKE ALL UI CONTROLS VISIBLE

	void addControls_ALL()
	{
		addControls_BMBF_ExSpec();
		addControls_BMBF_GEN();
		addControls_DynTarget();
		addControls_MPCalibrate();
		addControls_Music_GEN();
		addControls_Music_INBUILT();
		addControls_Music_INDIVIDUAL();
		addControls_RTVisualize_1D();
		addControls_SensorConfig();
		addControls_TabSpecific();
		addControls_ZoneVisualize_2D();
		addControls_RingVisualize();
	}
	void addControls_BMBF_ExSpec()
	{
		addAndMakeVisible(ui_bmbf_ex.staticBalance_Div_Pitch);
		addAndMakeVisible(ui_bmbf_ex.staticBalance_Div_Pitch_Label);
		addAndMakeVisible(ui_bmbf_ex.staticBalance_Div_Roll);
		addAndMakeVisible(ui_bmbf_ex.staticBalance_Div_Roll_Label);
		addAndMakeVisible(ui_bmbf_ex.staticBalance_FeedbackSlope);
		addAndMakeVisible(ui_bmbf_ex.staticBalance_calibrationMode);
		addAndMakeVisible(ui_bmbf_ex.staticBalance_FeedbackSlope_Label);
		addAndMakeVisible(ui_bmbf_ex.staticBalance_Ctrl_X);
		addAndMakeVisible(ui_bmbf_ex.staticBalance_Ctrl_Y);

		addAndMakeVisible(ui_bmbf_ex.sitStand_Thresh_Sit);
		addAndMakeVisible(ui_bmbf_ex.sitStand_Thresh_Sit_Label);
		addAndMakeVisible(ui_bmbf_ex.sitStand_Thresh_Stand);
		addAndMakeVisible(ui_bmbf_ex.sitStand_Thresh_Stand_Label);
		addAndMakeVisible(ui_bmbf_ex.sitStand_FlipState);
		addAndMakeVisible(ui_bmbf_ex.sitStand_isStanding);

		addAndMakeVisible(ui_bmbf_ex.HS_TimingMode);
		addAndMakeVisible(ui_bmbf_ex.HS_Tolerance);
		addAndMakeVisible(ui_bmbf_ex.HS_Tolerance_Label);
	}
	void addControls_BMBF_GEN()
	{
		addAndMakeVisible(ui_bmbf_gen.isStandbyToggle);
		addAndMakeVisible(ui_bmbf_gen.isStandbyToggleLabel);
		addAndMakeVisible(ui_bmbf_gen.recordGaitParam);
		addAndMakeVisible(ui_bmbf_gen.gaitParam_Name);
		addAndMakeVisible(ui_bmbf_gen.gaitParam_CurrentValue);
		addAndMakeVisible(ui_bmbf_gen.desiredBehavior);
		addAndMakeVisible(ui_bmbf_gen.gaitParam_DesiredBehavior_Label);
		addAndMakeVisible(ui_bmbf_gen.gaitParam_targetValue);
		addAndMakeVisible(ui_bmbf_gen.gaitParam_setTarget);
		addAndMakeVisible(ui_bmbf_gen.audioParam_Name);
		addAndMakeVisible(ui_bmbf_gen.soni_isSliderSource);
		addAndMakeVisible(ui_bmbf_gen.soni_sliderSource);
		addAndMakeVisible(ui_bmbf_gen.soni_isSliderSource_Label);
		addAndMakeVisible(ui_bmbf_gen.soni_sliderSource_Label);
		addAndMakeVisible(ui_bmbf_gen.exerciseMode);
		addAndMakeVisible(ui_bmbf_gen.gaitParam_Current);
		addAndMakeVisible(ui_bmbf_gen.audioParam_Current);
		addAndMakeVisible(ui_bmbf_gen.realTimeVisualize);
	}
	void addControls_DynTarget()
	{
		addAndMakeVisible(ui_bmbf_gen.enable_dynTarget);
		addAndMakeVisible(ui_dyn.dynTargetLabel);
		addAndMakeVisible(ui_bmbf_gen.enable_dynTarget_Label);
		addAndMakeVisible(ui_dyn.desired_Amplitude);
		addAndMakeVisible(ui_dyn.desired_Amplitude_Label);
		addAndMakeVisible(ui_dyn.desired_MovementFreq);
		addAndMakeVisible(ui_dyn.desired_MovementFreq_Label);
		addAndMakeVisible(ui_dyn.movementFunc_Order);
		addAndMakeVisible(ui_dyn.movementFunc_Order_Label);
		addAndMakeVisible(ui_dyn.movement_errorTolerance);
		addAndMakeVisible(ui_dyn.movement_errorTolerance_Label);
	}
	void addControls_MPCalibrate()
	{
		addAndMakeVisible(ui_mpCal.calibrateTarget);
		addAndMakeVisible(ui_mpCal.calibrateTarget_Save);
		addAndMakeVisible(ui_mpCal.calibrateTarget_Discard);
		addAndMakeVisible(ui_mpCal.calibrate_presentTarget);
		addAndMakeVisible(ui_mpCal.calibrate_timeElapsed);
	}
	void addControls_Music_GEN()								// ADD AND MAKE VISIBLE
	{
		addAndMakeVisible(ui_musiCon_gen.song_PlayPause);
		addAndMakeVisible(ui_musiCon_gen.song_Progress);
		addAndMakeVisible(ui_musiCon_gen.song_Stop);
		addAndMakeVisible(ui_musiCon_gen.song_Filename);
		addAndMakeVisible(ui_musiCon_gen.music_Mode);
		addAndMakeVisible(ui_musiCon_gen.rhythm_SetNext);
		addAndMakeVisible(ui_musiCon_gen.song_LoadFile);
		addAndMakeVisible(ui_musiCon_gen.tempo_Tap);
		addAndMakeVisible(ui_musiCon_gen.tempo_Slider);
		addAndMakeVisible(ui_musiCon_gen.rhythm_Prev);
		addAndMakeVisible(ui_musiCon_gen.rhythm_Now);
		addAndMakeVisible(ui_musiCon_gen.rhythm_Next);
		addAndMakeVisible(ui_musiCon_gen.song_master_Gain);
		addAndMakeVisible(ui_musiCon_gen.song_master_Gain_Lab);
		addAndMakeVisible(ui_musiCon_gen.song_master_EQ_B1_F);
		addAndMakeVisible(ui_musiCon_gen.song_master_EQ_B1_F_Lab);
		addAndMakeVisible(ui_musiCon_gen.song_master_EQ_B2_F);
		addAndMakeVisible(ui_musiCon_gen.song_master_EQ_B2_F_Lab);
		addAndMakeVisible(ui_musiCon_gen.song_master_EQ_B1_G);
		addAndMakeVisible(ui_musiCon_gen.song_master_EQ_B1_G_Lab);
		addAndMakeVisible(ui_musiCon_gen.song_master_EQ_B2_G);
		addAndMakeVisible(ui_musiCon_gen.song_master_EQ_B2_G_Lab);
		addAndMakeVisible(ui_musiCon_gen.song_TimeLeft);
		addAndMakeVisible(ui_musiCon_gen.song_BarBeat_Counter);
		for (int i = 0; i < 8; i++)
		{
			addAndMakeVisible(ui_musiCon_gen.song_track_Mute[i]);
			addAndMakeVisible(ui_musiCon_gen.song_track_Mute_Lab[i]);
			addAndMakeVisible(ui_musiCon_gen.song_track_GainOffset[i]);
			addAndMakeVisible(ui_musiCon_gen.song_track_GainOffset_Lab[i]);
			addAndMakeVisible(ui_musiCon_gen.inst_Variant[i]);
			addAndMakeVisible(ui_musiCon_gen.inst_Variant_Lab[i]);
		}
		addAndMakeVisible(ui_musiCon_gen.showControls_Channel);
		addAndMakeVisible(ui_musiCon_gen.showControls_RingVisualize);
	}
	void addControls_Music_INBUILT()
	{
		addAndMakeVisible(ui_musiCon_inbuilt.inbuilt_ChooseMIDI);
		addAndMakeVisible(ui_musiCon_inbuilt.inbuilt_Scale);
		addAndMakeVisible(ui_musiCon_inbuilt.inbuilt_Tonic);
	}
	void addControls_Music_INDIVIDUAL()
	{
		addAndMakeVisible(ui_musiCon_indiv.channel_TrackIdx);
		addAndMakeVisible(ui_musiCon_indiv.channel_EQ_FiltIdx);
		for (int i = 0; i < 4; i++)
		{
			addAndMakeVisible(ui_musiCon_indiv.channel_Comp_Settings[i]);
			addAndMakeVisible(ui_musiCon_indiv.channel_Comp_Labels[i]);
		}
		for (int j = 0; j < 3; j++)
		{
			addAndMakeVisible(ui_musiCon_indiv.channel_EQ_Settings[j]);
			addAndMakeVisible(ui_musiCon_indiv.channel_EQ_Labels[j]);
		}
	}
	void addControls_RTVisualize_1D()
	{
		addAndMakeVisible(ui_rtv_1d.rtv_minBound);
		addAndMakeVisible(ui_rtv_1d.rtv_maxBound);
		addAndMakeVisible(ui_rtv_1d.rtv_currentValue);
		addAndMakeVisible(ui_rtv_1d.rtv_targetRange);
		addAndMakeVisible(ui_rtv_1d.rtv_fullRangeBar);
	}
	void addControls_SensorConfig()
	{
		addAndMakeVisible(ui_sensorCon.SrNo_Header);
		addAndMakeVisible(ui_sensorCon.Status_Header);
		addAndMakeVisible(ui_sensorCon.Port_Header);
		addAndMakeVisible(ui_sensorCon.Location_Header);
		addAndMakeVisible(ui_sensorCon.BiasComp_Header);
		for (int i = 0; i < 3; i++)
		{
			addAndMakeVisible(ui_sensorCon.SrNo[i]);
			addAndMakeVisible(ui_sensorCon.Status[i]);
			addAndMakeVisible(ui_sensorCon.Port[i]);
			addAndMakeVisible(ui_sensorCon.Location[i]);
			addAndMakeVisible(ui_sensorCon.BiasComp[i]);
		}
	}
	void addControls_TabSpecific()
	{
		addAndMakeVisible(ui_tabSpec.status_SensorConnection);
	}
	void addControls_ZoneVisualize_2D()
	{
		addAndMakeVisible(ui_zv_2d.ZV_CurrentPos);
		addAndMakeVisible(ui_zv_2d.ZV_InnerRect);
		addAndMakeVisible(ui_zv_2d.ZV_InnerRect_AP);
		addAndMakeVisible(ui_zv_2d.ZV_TotalSpace);
		addAndMakeVisible(ui_zv_2d.ZV_F);
		addAndMakeVisible(ui_zv_2d.ZV_B);
		addAndMakeVisible(ui_zv_2d.ZV_L);
		addAndMakeVisible(ui_zv_2d.ZV_R);
	}
	void addControls_RingVisualize()
	{
		for (int i = 0; i < 32; i++)
		{
			addAndMakeVisible(ringVisualize.beatSubBox[i]);
		}
		addAndMakeVisible(ringVisualize.emphFunc_MAX_Label);
		addAndMakeVisible(ringVisualize.emphFunc_MAX);
		addAndMakeVisible(ringVisualize.emphFunc_MIN_Label);
		addAndMakeVisible(ringVisualize.emphFunc_MIN);
		addAndMakeVisible(ringVisualize.emphFunc_C_Label);
		addAndMakeVisible(ringVisualize.emphFunc_C);
		addAndMakeVisible(ringVisualize.emphFunc_WID_Label);
		addAndMakeVisible(ringVisualize.emphFunc_WID);
		addAndMakeVisible(ringVisualize.emphFunc_ORDER_Label);
		addAndMakeVisible(ringVisualize.emphFunc_ORDER);
		addAndMakeVisible(ringVisualize.emph_Strategy);
		addAndMakeVisible(ui_musiCon_inbuilt.inbuilt_ChooseMIDI);
		addAndMakeVisible(ui_musiCon_inbuilt.inbuilt_Tonic);
		addAndMakeVisible(ui_musiCon_inbuilt.inbuilt_Scale);
	}

	//Real time update of a) Song remaining time and b) Ring box positions
	void updateTimeLabels()				
	{
		ui_musiCon_gen.song_TimeLeft.setText("Remaining Time: "
			+ std::to_string((int)processor.sequencer.timeLeft_Song) + " s", dontSendNotification);
		ringVisualize.refreshBoxPositions(processor.sequencer.musicPhase.presentPhase_Rad);
		refreshRingBoxPositions();
	}
	
	//Change song progress bar colour
	void handleProgressBarColour()		
	{
		if (processor.sequencer.songProgress < 0.25)
			ui_musiCon_gen.song_Progress.setColour(ui_musiCon_gen.song_Progress.foregroundColourId, Colours::darkred);
		else if (processor.sequencer.songProgress < 0.75)
			ui_musiCon_gen.song_Progress.setColour(ui_musiCon_gen.song_Progress.foregroundColourId, Colours::red);
		if (processor.sequencer.songProgress >= 0.75)
			ui_musiCon_gen.song_Progress.setColour(ui_musiCon_gen.song_Progress.foregroundColourId, Colours::green);
	}

	// Handle Secondary Music Control Buttons
	void handleDisplay_secondaryMusic()
	{
		switch (presentMusiCon_Disp)
		{
		case 0:
			ui_musiCon_gen.showControls_Channel.setColour(
				ui_musiCon_gen.showControls_Channel.buttonColourId,
				Colours::blue);
			ui_musiCon_gen.showControls_RingVisualize.setColour(
				ui_musiCon_gen.showControls_RingVisualize.buttonColourId,
				Colours::red);
			ringVisualize.toggleVisibility(false, processor.musicMode);
			ui_musiCon_indiv.toggleVisible(true);
			break;
		case 1:
			ui_musiCon_gen.showControls_Channel.setColour(
				ui_musiCon_gen.showControls_Channel.buttonColourId,
				Colours::red);
			ui_musiCon_gen.showControls_RingVisualize.setColour(
				ui_musiCon_gen.showControls_RingVisualize.buttonColourId,
				Colours::blue);
			ringVisualize.toggleVisibility(true, processor.musicMode);
			ui_musiCon_indiv.toggleVisible(false);
			break;
		}
	}
	
	//Set instrument variants for selected rhythm
	void setRhythmSpecificVariants()	
	{
		for (int i = 0; i < 8; i++)
			ui_musiCon_gen.inst_Variant[i].setSelectedId(processor.sequencer.mixerSettings.currentVariant[processor.sequencer.index_baseBeat][i]);
	};
	
	//UPDATE RHYTHM NAME LABELS WHEN RHYTHM IS CHANGED
	void refreshBeatLabels()			
	{
		// Init Helper Variables
		int currentBeat = processor.sequencer.index_baseBeat;
		int nextBeatIdx = 0;
		int lastBeatIdx = 0;
		int timingMode = 0;
		short indices_correctTimingMode[50] = { 0 };
		short indices_filled = 0;
		timingMode = processor.sequencer.timingMode;
		
		// Refresh Indices for All 3 Labels
		for (int i = 0; i < processor.sequencer.currentMusic.drum_numBase; i++)
			{
				if (processor.sequencer.currentMusic.drum_beatTypes[i] == timingMode)
				{
					indices_correctTimingMode[indices_filled] = i;
					indices_filled++;
				}
			}
			for (int j = 0; j < indices_filled; j++)
			{
				if (indices_correctTimingMode[(j + 1) % indices_filled] == currentBeat)
					lastBeatIdx = indices_correctTimingMode[j];
				if (indices_correctTimingMode[(j - 1 < 0) ? j - 1 + indices_filled : j - 1 ] == currentBeat)
					nextBeatIdx = indices_correctTimingMode[j];
			}
		// Change Text
		String text_LastBeat = processor.sequencer.currentMusic.baseBeats[lastBeatIdx].name;
		ui_musiCon_gen.rhythm_Now.setText(processor.sequencer.currentMusic.baseBeats[currentBeat].name, dontSendNotification);
		ui_musiCon_gen.rhythm_Prev.setText(text_LastBeat, dontSendNotification);
		ui_musiCon_gen.rhythm_Next.setText(processor.sequencer.currentMusic.baseBeats[nextBeatIdx].name, dontSendNotification);
		
		// Change Color
		ui_musiCon_gen.rhythm_Now.setColour(ui_musiCon_gen.rhythm_Now.backgroundColourId, Colours::yellow);
		ui_musiCon_gen.rhythm_Now.setColour(ui_musiCon_gen.rhythm_Now.textColourId, Colours::black);
		ui_musiCon_gen.rhythm_Now.setJustificationType(Justification::centred);
		ui_musiCon_gen.rhythm_Prev.setColour(ui_musiCon_gen.rhythm_Prev.backgroundColourId, Colours::darkred);
		ui_musiCon_gen.rhythm_Prev.setJustificationType(Justification::centred);
		ui_musiCon_gen.rhythm_Next.setColour(ui_musiCon_gen.rhythm_Next.backgroundColourId, Colours::green);
		ui_musiCon_gen.rhythm_Next.setJustificationType(Justification::centred);
	}
	
	//Set track gain offset sliders
	void setGainSliders()				
	{
		for (int i = 0; i < 8; i++)
		{
			ui_musiCon_gen.song_track_GainOffset[i].setValue(processor.sequencer.mixerSettings
				.trackGain_Offsets[processor.sequencer.index_baseBeat][i]);
		}
	}
	
	void configureMusicControls();								//Split
	bool isPeripheralsTab = false;								//Flag - Tab 1
	bool isMusicControlsTab = false;							//Flag - Tab 2
	bool isSonificationTab = false;								//Flag - Tab 3
	
	// Update Filename Label
	void updateMusicControlValues()
	{
		ui_musiCon_gen.song_Filename.setText("Current File: " 
			+ processor.sequencer.currentMusic.songName, dontSendNotification);
	}
	
	//Sonification Controls
	void configureSonificationControls();
	void repopulateLists(short exerciseMode);
	void comboBoxChanged(ComboBox *box) override;
	void updateControls_gaitParam(bool isCallback);
	void updateControls_audioParam(bool isCallback);

	// Calibration
	void calibrateTarget_handlePress();
	void calibrateTarget_SaveHandle();
	void calibrateTarget_DiscardHandle();
	
	// Refresh Calibration Status Label when MP changed
	void updateCalibrationLabels()
	{
		if (processor.gaitAnalysis.isParamCalibrated[processor.gaitAnalysis.gaitParams.activeGaitParam] || processor.isCalibrating)
		{
			ui_mpCal.calibrate_presentTarget.setText("Calibrated Target: "
				+ String(processor.gaitAnalysis.calibrationValues_Temp[processor.gaitAnalysis.gaitParams.activeGaitParam],2), dontSendNotification);
			ui_mpCal.calibrate_presentTarget.setColour(
				ui_mpCal.calibrate_presentTarget.backgroundColourId, Colours::green);
		}
		else
		{
			ui_mpCal.calibrate_presentTarget.setText("NOT CALIBRATED", dontSendNotification);
			ui_mpCal.calibrate_presentTarget.setColour(
				ui_mpCal.calibrate_presentTarget.backgroundColourId, Colours::red);
		}
	}

	// Real Time Visualizer
	bool isRealTimeVisualize = true;
	void configureRealTimeVisualizer();
	void updateRealTimeVisualizer();
	void updateProjectionZoneVisualizer();

	// TAB AND CONTROLS TOGGLE
	void togglePeripheralsTab(bool on);
	void toggleMusicControlsTab(bool on)
	{
		updateMusicControlValues();
		ui_musiCon_gen.toggleVisible(on);
	};
	void toggleSonificationControlsTab(bool on);
	
	// TOGGLE WITHIN TAB MUSIC CONTROLS - CHANNEL / RING VISUALIZER
	void toggleMusicControls_Secondary(short tab)
	{
		bool isCorrectTab = (tab == 1);
		if (!isCorrectTab)
		{
			ringVisualize.toggleVisibility(false, processor.musicMode);
			ui_musiCon_indiv.toggleVisible(false);
		}
		else
		{
			switch (presentMusiCon_Disp)
			{
			case 0:
				ringVisualize.toggleVisibility(false, processor.musicMode);
				ui_musiCon_indiv.toggleVisible(true);
				break;
			case 1:
				ringVisualize.toggleVisibility(true, processor.musicMode);
				ui_musiCon_indiv.toggleVisible(false);
				break;
			}
		}
	}
	
	// SWITCH UI TO NEW TAB
	void switchTab(int currentTab);

	// Configure Channel Sliders (First Time)
	void channel_configureSliders()
	{
		ui_musiCon_indiv.channel_TrackIdx.addListener(this);
		ui_musiCon_indiv.channel_EQ_FiltIdx.addListener(this);

		for (int i = 0; i < 4; i++)
		{
			ui_musiCon_indiv.channel_Comp_Settings[i].onValueChange = [this, i]
			{
				short rhythm_Idx = processor.sequencer.index_baseBeat;
				short activeTrack = ui_musiCon_indiv.channel_ActiveTrack;
				short currentVariant = processor.sequencer.mixerSettings.currentVariant[rhythm_Idx][ui_musiCon_indiv.channel_ActiveTrack];
				processor.sequencer.mixerSettings.compSettings[currentVariant - 1][activeTrack][i] = ui_musiCon_indiv.channel_Comp_Settings[i].getValue();
				processor.sequencer.applyCurrentVariantComp(activeTrack);
			};
		}

		for (int i = 0; i < 3; i++)
		{
			ui_musiCon_indiv.channel_EQ_Settings[i].onValueChange = [this, i]
			{
				short rhythm_Idx = processor.sequencer.index_baseBeat;
				short activeTrack = ui_musiCon_indiv.channel_ActiveTrack;
				short currentVariant = processor.sequencer.mixerSettings.currentVariant[rhythm_Idx][ui_musiCon_indiv.channel_ActiveTrack];
				processor.sequencer.mixerSettings.eqSettings[currentVariant - 1][activeTrack][3 * ui_musiCon_indiv.channel_EQ_ActiveFiltIdx + i] =
					ui_musiCon_indiv.channel_EQ_Settings[i].getValue();
				processor.sequencer.applyCurrentVariantEQ(activeTrack);
			};
		}
	};
	
	// Refresh Channel Sliders on Track Change
	void channel_refreshSliders(int trackIdx)		// 0 - 7 // STAYS HERE
	{
		short currentVariant = 0;
		short currentRhythm = processor.sequencer.index_baseBeat;
		float value = 0;
		for (int i = 0; i < 4; i++)
		{
			currentVariant = processor.sequencer.mixerSettings.currentVariant[currentRhythm][trackIdx] - 1;
			value = processor.sequencer.mixerSettings.compSettings[currentVariant][trackIdx][i];
			ui_musiCon_indiv.channel_Comp_Settings[i].setValue(value);
		}
		for (int i = 0; i < 3; i++)
		{
			value = processor.sequencer.mixerSettings.eqSettings[currentVariant][trackIdx][3* ui_musiCon_indiv.channel_EQ_ActiveFiltIdx + i];
			ui_musiCon_indiv.channel_EQ_Settings[i].setValue(value);
		}
	}

	// Configure Phase Ring Controls
	void init_RingVisualize()
	{
		if (presentTab == 1 && presentMusiCon_Disp == 1)
		{
			for (int i = 0; i < ringVisualize.numBoxes; i++)
				ringVisualize.beatSubBox[i].setVisible(true);
		}
		// Ring Visualizer Circle
		ringVisualize.setCircleCenter_Rad(600, 520, 150);
		ringVisualize.setPhaseInc(processor.tempo, UIRefreshFreq);
		ringVisualize.refreshBoxPositions(processor.sequencer.musicPhase.presentPhase_Rad);
		for (int i = 0; i < ringVisualize.numBoxes; i++)
		{
			// Beat Positions
			if (i % (ringVisualize.numBoxes / 4) == 0)
			{
				ringVisualize.beatSubBox[i].setColour(ringVisualize.beatSubBox[i].backgroundColourId, Colours::yellow);
				ringVisualize.beatSubBox[i].setColour(ringVisualize.beatSubBox[i].textColourId, Colours::black);
				ringVisualize.beatSubBox[i].setFont(Font(32.0f, Font::bold));
				ringVisualize.beatSubBox[i].setText(String(i / (ringVisualize.numBoxes / 4) + 1), dontSendNotification);
				ringVisualize.beatSubBox[i].setJustificationType(Justification::centred);
			}
			
			// Subdivisions
			else
				ringVisualize.beatSubBox[i].setColour(ringVisualize.beatSubBox[i].backgroundColourId, Colours::blue);
				ringVisualize.beatSubBox[i].setBounds(ringVisualize.box_Pos_X[i],
				ringVisualize.box_Pos_Y[i],	ringVisualize.box_Side[i],ringVisualize.box_Side[i]);
		}

		// Emph Function Properties
		ringVisualize.emphFunc_MAX.onValueChange = [this]
		{	processor.sequencer.musicPhase.emphFunc_MAX = ringVisualize.emphFunc_MAX.getValue();	};
		ringVisualize.emphFunc_MIN.onValueChange = [this]
		{	processor.sequencer.musicPhase.emphFunc_MIN = ringVisualize.emphFunc_MIN.getValue();	};
		ringVisualize.emphFunc_C.onValueChange = [this]
		{	processor.sequencer.musicPhase.emphFunc_C = (int)ringVisualize.emphFunc_C.getValue() - 1;	};
		ringVisualize.emphFunc_WID.onValueChange = [this]
		{	processor.sequencer.musicPhase.emphFunc_WID = ringVisualize.emphFunc_WID.getValue();	};
		ringVisualize.emphFunc_ORDER.onValueChange = [this]
		{	processor.sequencer.musicPhase.emphFunc_ORDER = ringVisualize.emphFunc_ORDER.getValue();	};

		// INIT COMBOBOXES
		if (ringVisualize.emph_Strategy.getNumItems() == 0)						// PREVENT DOUBLE
		{
			// Emph Audio Param
			for (int i = 0; i < processor.audioParams.numSoni_Musical; i++)
			{
				if (processor.audioParams.audioParam_ObjectArray[i].isIncluded_UseScenarios[6])
				{
					ringVisualize.emph_Strategy.
						addItem(processor.audioParams.audioParam_ObjectArray[i].name, i + 1);
				}
			}
			ringVisualize.emph_Strategy.setSelectedId(processor.audioParams.indices_AP[5][0]);
			ringVisualize.emph_Strategy.addListener(this);
		}
	}

	void config_musicControls_inbuilt()
	{
		// Choose Inbuilt MIDI
		ui_musiCon_inbuilt.inbuilt_ChooseMIDI.addItem("NONE", 1);
		for (int i = 1; i <= processor.sequencer.currentMusic.num_Inbuilt; i++)
			ui_musiCon_inbuilt.inbuilt_ChooseMIDI.addItem(processor.sequencer.currentMusic.MelLibFiles[i - 1], i + 1);
		ui_musiCon_inbuilt.inbuilt_ChooseMIDI.setSelectedId(1);
		ui_musiCon_inbuilt.inbuilt_ChooseMIDI.addListener(this);

		// Choose Tonic
		for (int i = 0; i < processor.sequencer.scaleTonicTrans.numTonics; i++)
			ui_musiCon_inbuilt.inbuilt_Tonic.addItem(processor.sequencer.scaleTonicTrans.tonics_Names[i], i + 1);
		ui_musiCon_inbuilt.inbuilt_Tonic.setSelectedId(1);
		ui_musiCon_inbuilt.inbuilt_Tonic.addListener(this);

		// Choose Scale
		for (int i = 0; i < processor.sequencer.scaleTonicTrans.numScales; i++)
			ui_musiCon_inbuilt.inbuilt_Scale.addItem(processor.sequencer.scaleTonicTrans.scales_Names[i], i + 1);
		ui_musiCon_inbuilt.inbuilt_Scale.setSelectedId(1);
		ui_musiCon_inbuilt.inbuilt_Scale.addListener(this);
	}

	// Configure Boxes in Phase Ring
	void ringVisualizeBoxConfig()
	{
		ringVisualize.setPhaseInc(processor.tempo, UIRefreshFreq);
		if (ui_musiCon_gen.tempo_Slider.getValue() > 110)
			ringVisualize.setNumBoxes(8);
		else if (ui_musiCon_gen.tempo_Slider.getValue() > 80)
			ringVisualize.setNumBoxes(16);
		else ringVisualize.setNumBoxes(32);

		if (presentTab == 1 && presentMusiCon_Disp == 1)
		{
			for (int i = 0; i < 32; i++)
			{
				if (i >= ringVisualize.numBoxes)
					ringVisualize.beatSubBox[i].setVisible(false);
				else
					ringVisualize.beatSubBox[i].setVisible(true);
			}
		}
		init_RingVisualize();
		refreshRingBoxPositions();
	}

	// Real Time Ring Update
	void refreshRingBoxPositions()
	{
		float boxAlpha_Segment = 0;
		float boxAlpha_PhaseEmph = 0;
		float emphFunc_Min = processor.sequencer.musicPhase.emphFunc_MIN;
		float emphFunc_Max = processor.sequencer.musicPhase.emphFunc_MAX;

		float rgbArray[3] = { 0.0 };
		for (int i = 0; i < ringVisualize.numBoxes; i++)
		{
			ringVisualize.beatSubBox[i].
				setBounds(ringVisualize.box_Pos_X[i],
					ringVisualize.box_Pos_Y[i],
					ringVisualize.box_Side[i],
					ringVisualize.box_Side[i]);
			boxAlpha_Segment = processor.sequencer.musicPhase.emphFunc_getValue_Overall
			(1, 0, ringVisualize.box_Phase[i], 1, 0, M_PI/2, 0.4);
			boxAlpha_PhaseEmph = processor.sequencer.musicPhase.emphFunc_Present;
			ringVisualize.RGBFromEmph(rgbArray, boxAlpha_PhaseEmph, emphFunc_Min, emphFunc_Max);
			ringVisualize.beatSubBox[i].setColour(ringVisualize.beatSubBox[i].backgroundColourId,
				Colour::fromFloatRGBA(rgbArray[0], rgbArray[1], rgbArray[2], boxAlpha_Segment));
		}
	};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GaitSonificationAudioProcessorEditor)
};
