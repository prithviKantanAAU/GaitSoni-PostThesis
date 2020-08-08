#include "PluginProcessor.h"
#include "PluginEditor.h"

// CONSTRUCTOR
GaitSonificationAudioProcessorEditor::GaitSonificationAudioProcessorEditor (GaitSonificationAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
	// SET INTERFACE DIMENSIONS
	setSize(1200, 900);
	// ADD TABBED INTERFACE
	tabs = std::make_unique<TabbedComponent>(TabbedButtonBar::TabsAtTop);
	addAndMakeVisible(*tabs);
	tabs->addTab("Peripherals", Colours::black, Component::getParentComponent(), 1);
	tabs->addTab("Music Controls", Colours::black, Component::getParentComponent(), 1);
	tabs->addTab("Sonification Controls", Colours::black, Component::getParentComponent(), 1);
	tabs->setBounds(0, 0, getWidth(), getHeight());
	// ADD CONTROLS
	addControls_ALL();
	// START UI TIMER CALLBACK
	startTimerHz(UIRefreshFreq);
	//CONFIGURE CONTROLS, UPDATE PRESENT TAB
	sensorConfig_initializeControls();
	configureMusicControls();
	init_RingVisualize();
	configureSonificationControls();
	configureRealTimeVisualizer();
	setGainSliders();
	switchTab(presentTab);
}

// DESTRUCTOR
GaitSonificationAudioProcessorEditor::~GaitSonificationAudioProcessorEditor()
{
	tabs->~TabbedComponent();
}

// CONFIGURE SENSOR CONFIG BEHAVIOR - FULL TAB
void GaitSonificationAudioProcessorEditor::sensorConfig_initializeControls()
{
	String statusText = "";
	for (int i = 0; i < 3; i++)
	{
		statusText = processor.gaitAnalysis.sensorInfo.isOnline[i] ? "ON" : "OFF";
		ui_sensorCon.Status[i].setText(statusText, dontSendNotification);

		ui_sensorCon.Port[i].setText(String(processor.gaitAnalysis.sensorInfo.UDP_Ports[i])
			, dontSendNotification);

		ui_sensorCon.Location[i].addListener(this);

		ui_sensorCon.BiasComp[i].onClick = [this, i]
		{
			if (processor.isPlaying)
				processor.stopMusic();

			if (!processor.gaitAnalysis.sensors_OSCReceivers[i].isBiasComp_ON)
			{
				processor.gaitAnalysis.sensors_OSCReceivers[i].isBiasComp_DONE = false;
				processor.gaitAnalysis.sensors_OSCReceivers[i].isBiasComp_ON = true;
			}
		};
	}
}

// CONFIGURE MUSIC CONTROL BEHAVIOR - FULL TAB
void GaitSonificationAudioProcessorEditor::configureMusicControls()
{
	// Song Play Pause
	ui_musiCon_gen.song_PlayPause.onClick = [this]
	{
		processor.togglePlayPause();
	};

	// Song Progress Bar
	ui_musiCon_gen.song_Progress.setColour(ui_musiCon_gen.song_Progress.backgroundColourId, Colours::darkblue);
	ui_musiCon_gen.song_Progress.setColour(ui_musiCon_gen.song_Progress.foregroundColourId, Colours::darkred);
	ui_musiCon_gen.song_Progress.setPercentageDisplay(false);

	// Song Stop
	ui_musiCon_gen.song_Stop.onClick = [this]
	{
		processor.stopMusic();
	};

	// Tap Tempo
	ui_musiCon_gen.tempo_Tap.onClick = [this]
	{
		processor.handleTapTempoPress();
		ui_musiCon_gen.tempo_Slider.setValue(processor.tempo);
	};

	// Load Song File
	ui_musiCon_gen.song_LoadFile.onClick = [this]
	{
		String filePath = "";
		String extension = ui_musiCon_gen.fileExtensions[processor.musicMode];
		FileChooser songChooser("Please select a valid song file:",
		File::getSpecialLocation(File::userHomeDirectory), extension);
		if (songChooser.browseForFileToOpen())
		{
			File songFile(songChooser.getResult());
			filePath = songFile.getFullPathName();
			processor.sequencer.currentMusic.songName = songFile.getFileName().upToFirstOccurrenceOf("_",false,false).toStdString();
			if (processor.musicMode == 1)
			{
				processor.sequencer.setTimingMode(filePath);
				processor.setTempo(ui_musiCon_gen.tempo_Slider.getValue());
				setRhythmSpecificVariants();
				processor.initializeTrackGains();
				setGainSliders();
			}
			processor.setFilename(filePath);
		}
		updateMusicControlValues();
		refreshBeatLabels();					// Based on song specifications
	};

	// Set next rhythm
	ui_musiCon_gen.rhythm_SetNext.onClick = [this]
	{
		processor.sequencer.nextRhythm(processor.musicMode);
		processor.sequencer.resetPercMIDIOnChange(processor.midiTicksElapsed);
		setRhythmSpecificVariants();
		processor.initializeTrackGains();
		setGainSliders();
		refreshBeatLabels();
		channel_refreshSliders(ui_musiCon_indiv.channel_ActiveTrack);
	};

	// Set tempo slider
	ui_musiCon_gen.tempo_Slider.onValueChange = [this]
	{
		processor.setTempo(ui_musiCon_gen.tempo_Slider.getValue());
		ringVisualizeBoxConfig();
	};

	// Master gain slider
	ui_musiCon_gen.song_master_Gain.onValueChange = [this]
	{
		processor.applyMasterGain(ui_musiCon_gen.song_master_Gain.getValue());
	};
	
	// Music Mode
	ui_musiCon_gen.music_Mode.addListener(this);

	// Master EQ Set Band 1 F
	ui_musiCon_gen.song_master_EQ_B1_F.onValueChange = [this]
	{
		processor.dspFaust.setParamValue(processor.faustStrings.MasterEQ_1_F.c_str(),
			ui_musiCon_gen.song_master_EQ_B1_F.getValue());
		ui_musiCon_gen.song_master_EQ_B1_F_Lab.setText(String(ui_musiCon_gen.song_master_EQ_B1_F.getValue(), 0), dontSendNotification);
	};

	// Master EQ Set Band 2 F
	ui_musiCon_gen.song_master_EQ_B2_F.onValueChange = [this]
	{
		processor.dspFaust.setParamValue(processor.faustStrings.MasterEQ_2_F.c_str(), ui_musiCon_gen.song_master_EQ_B2_F.getValue());
		ui_musiCon_gen.song_master_EQ_B2_F_Lab.setText(String(ui_musiCon_gen.song_master_EQ_B2_F.getValue(), 0), dontSendNotification);
	};

	// Master EQ Set Band 1 G
	ui_musiCon_gen.song_master_EQ_B1_G.onValueChange = [this]
	{
		processor.dspFaust.setParamValue(processor.faustStrings.MasterEQ_1_Gain.c_str(), ui_musiCon_gen.song_master_EQ_B1_G.getValue());
	};

	// Master EQ Set Band 2 G
	ui_musiCon_gen.song_master_EQ_B2_G.onValueChange = [this]
	{
		processor.dspFaust.setParamValue(processor.faustStrings.MasterEQ_2_Gain.c_str(), ui_musiCon_gen.song_master_EQ_B2_G.getValue());
	};

	for (int i = 0; i < 8; i++)
	{
		// Instrument Variant Names - Populate
		ui_musiCon_gen.inst_Variant[i].addItem(processor.mixerSettings.varNames[i][0], 1);
		ui_musiCon_gen.inst_Variant[i].addItem(processor.mixerSettings.varNames[i][1], 2);
		ui_musiCon_gen.inst_Variant[i].addItem(processor.mixerSettings.varNames[i][2], 3);
		ui_musiCon_gen.inst_Variant[i].addListener(this);
		ui_musiCon_gen.inst_Variant[i].setSelectedId(processor.mixerSettings.currentVariant[processor.sequencer.index_baseBeat][i]);
		ui_musiCon_gen.inst_Variant_Lab[i].setText(ui_musiCon_gen.inst_Names[i], dontSendNotification);
		ui_musiCon_gen.inst_Variant_Lab[i].attachToComponent(&ui_musiCon_gen.inst_Variant[i], true);

		// Instrument Mute Initialize
		ui_musiCon_gen.song_track_Mute[i].onStateChange = [this, i]
		{
			processor.toggleTrackMuteManual(ui_musiCon_gen.song_track_Mute[i].getToggleState(), i);
		};
		
		// Track Gain Offset
		ui_musiCon_gen.song_track_GainOffset[i].onValueChange = [this,i]
		{
			processor.setTrackGains(i, ui_musiCon_gen.song_track_GainOffset[i].getValue());
			ui_musiCon_gen.song_track_GainOffset_Lab[i].setText(String(ui_musiCon_gen.song_track_GainOffset[i].getValue(), 1), dontSendNotification);
		};		
	}
}

// CONFIGURE SONIFICATION CONTROL BEHAVIOR - FULL TAB
void GaitSonificationAudioProcessorEditor::configureSonificationControls()
{
	// STANDBY
	ui_bmbf_gen.isStandbyToggle.onStateChange = [this]
	{
		processor.isStandby = ui_bmbf_gen.isStandbyToggle.getToggleState();
		if (ui_bmbf_gen.isStandbyToggle.getToggleState())
		{
			ui_bmbf_gen.isStandbyToggleLabel.setColour(ui_bmbf_gen.isStandbyToggleLabel.backgroundColourId, Colours::red);
			ui_bmbf_gen.isStandbyToggleLabel.setText("Standby ON", dontSendNotification);
		}
		else
		{
			ui_bmbf_gen.isStandbyToggleLabel.setColour(ui_bmbf_gen.isStandbyToggleLabel.backgroundColourId, Colours::green);
			ui_bmbf_gen.isStandbyToggleLabel.setText("Standby OFF", dontSendNotification);
		}
	};
	ui_bmbf_gen.isStandbyToggleLabel.setText("Standby OFF", dontSendNotification);
	ui_bmbf_gen.isStandbyToggleLabel.setColour(ui_bmbf_gen.isStandbyToggleLabel.textColourId, Colours::black);
	ui_bmbf_gen.isStandbyToggleLabel.setColour(ui_bmbf_gen.isStandbyToggleLabel.backgroundColourId, Colours::green);
	ui_bmbf_gen.isStandbyToggleLabel.attachToComponent(&ui_bmbf_gen.isStandbyToggle, true);

	// DESIRED PARAM BEHAVIOR
	ui_bmbf_gen.desiredBehavior.addListener(this);

	// RECORD GAIT PARAM
	ui_bmbf_gen.recordGaitParam.setButtonText("Rec Gait Param");
	ui_bmbf_gen.recordGaitParam.setColour(ui_bmbf_gen.recordGaitParam.buttonColourId, Colours::red);
	ui_bmbf_gen.recordGaitParam.onClick = [this]
	{
		if (processor.gaitAnalysis.areRequiredSensorsOnline())
		{
			if (!processor.imuRecord.isRecording_Sensor)
			{
				processor.startRecording_Sensor();
				ui_bmbf_gen.recordGaitParam.setButtonText("Stop Rec");
				ui_bmbf_gen.recordGaitParam.setColour(ui_bmbf_gen.recordGaitParam.buttonColourId, Colours::green);
			}
			else
			{
				processor.stopRecording_Sensor();
				ui_bmbf_gen.recordGaitParam.setButtonText("Rec Gait Param");
				ui_bmbf_gen.recordGaitParam.setColour(ui_bmbf_gen.recordGaitParam.buttonColourId, Colours::red);
			}
		}
	};

	// TARGET SETTER
	ui_bmbf_gen.gaitParam_setTarget.onValueChange = [this]
	{
		processor.gaitAnalysis.gaitParams.setTargetValue(ui_bmbf_gen.gaitParam_setTarget.getValue());
		ui_bmbf_gen.gaitParam_targetValue.setText
		("Target Value: " + String(processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[processor.gaitAnalysis.gaitParams.activeGaitParam].target,2), dontSendNotification);
	};
	ui_bmbf_gen.gaitParam_targetValue.setText
	("Target Value: " + String(processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
		[processor.gaitAnalysis.gaitParams.activeGaitParam].target, 2), dontSendNotification);

	// SONIFICATION MODE: SLIDER / SENSOR

	ui_bmbf_gen.soni_isSliderSource.onStateChange = [this]
	{
		processor.soniMappingCompute.setSonificationMode(ui_bmbf_gen.soni_isSliderSource.getToggleState());
		if (ui_bmbf_gen.soni_isSliderSource.getToggleState())
		{
			ui_bmbf_gen.soni_isSliderSource_Label.setText("Source: Slider", dontSendNotification);
			ui_bmbf_gen.soni_isSliderSource_Label.setColour(ui_bmbf_gen.soni_isSliderSource_Label
				.backgroundColourId, Colours::red);
			ui_bmbf_gen.soni_sliderSource.setVisible(true);
		}
		else
		{
			ui_bmbf_gen.soni_isSliderSource_Label.setText("Source: Sensor", dontSendNotification);
			ui_bmbf_gen.soni_isSliderSource_Label.setColour(ui_bmbf_gen.soni_isSliderSource_Label.
				backgroundColourId, Colours::green);
			ui_bmbf_gen.soni_sliderSource.setVisible(false);
		}
	};

	// SLIDER - SONIFICATION SOURCE

	ui_bmbf_gen.soni_sliderSource.onValueChange = [this]
	{
		processor.soniMappingCompute.setSoniVal_Slider(ui_bmbf_gen.soni_sliderSource.getValue());
	};
	updateControls_gaitParam(false);
	updateControls_audioParam(false);

	// EXERCISE MODE
	for (int i = 0; i < processor.gaitAnalysis.gaitParams.num_UseScenarios; i++)
		ui_bmbf_gen.exerciseMode.addItem(processor.gaitAnalysis.gaitParams.exerciseModes[i],i + 1);
	ui_bmbf_gen.exerciseMode.setSelectedId(1);
	ui_bmbf_gen.exerciseMode.addListener(this);

	ui_bmbf_gen.gaitParam_Current.addListener(this);
	ui_bmbf_gen.audioParam_Current.addListener(this);
	repopulateLists(1);

	//Trunk Balance - Center X
	ui_bmbf_ex.staticBalance_Ctrl_X.onValueChange = [this]
	{
		processor.gaitAnalysis.staticBalance_BoundsCoordinates[0][0] = 
			ui_bmbf_ex.staticBalance_Ctrl_X.getValue();
	};

	//Trunk Balance - Center Y
	ui_bmbf_ex.staticBalance_Ctrl_Y.onValueChange = [this]
	{
		processor.gaitAnalysis.staticBalance_BoundsCoordinates[0][1] = 
			-1* ui_bmbf_ex.staticBalance_Ctrl_Y.getValue();
	};

	//Trunk Balance - Zone Width
	
	ui_bmbf_ex.staticBalance_Div_Roll.onValueChange = [this]
	{
		processor.gaitAnalysis.staticBalance_Div_Roll = 
			ui_bmbf_ex.staticBalance_Div_Roll.getValue();
	};

	//Trunk Balance - Zone Height
	ui_bmbf_ex.staticBalance_Div_Pitch.onValueChange = [this]
	{
		processor.gaitAnalysis.staticBalance_Div_Pitch = 
			10 - ui_bmbf_ex.staticBalance_Div_Pitch.getValue();
	};

	//Trunk Balance - Feedback Slope
	ui_bmbf_ex.staticBalance_FeedbackSlope.addListener(this);

	// SB Calibration Mode
	ui_bmbf_ex.staticBalance_calibrationMode.addListener(this);

	// SST - Stand Thresh
	ui_bmbf_ex.sitStand_Thresh_Stand.onValueChange = [this]
	{
		processor.gaitAnalysis.sitStand_Thresh_Stand = ui_bmbf_ex.sitStand_Thresh_Stand.getValue();
	};

	// SST - Sit Thresh
	ui_bmbf_ex.sitStand_Thresh_Sit.onValueChange = [this]
	{
		processor.gaitAnalysis.sitStand_Thresh_Sit = ui_bmbf_ex.sitStand_Thresh_Sit.getValue();
	};

	// SST - Flip State
	ui_bmbf_ex.sitStand_FlipState.onClick = [this]
	{
		processor.gaitAnalysis.sitStand_FlipState();
	};

	// SST - Is Standing Label
	String isStandingText = processor.gaitAnalysis.sitStand_isStanding ? "Standing" : "Sitting";
	ui_bmbf_ex.sitStand_isStanding.setText(isStandingText, dontSendNotification);

	// Timing Mode
	ui_bmbf_ex.HS_TimingMode.addListener(this);

	// Tolerance
	ui_bmbf_ex.HS_Tolerance.onValueChange = [this]
	{
		processor.gaitAnalysis.HS_IntervalTolerance = ui_bmbf_ex.HS_Tolerance.getValue();
	};

	//Enable Dynamic Target
	ui_bmbf_gen.enable_dynTarget.onStateChange = [this]
	{
		processor.isTargetDynamic = ui_bmbf_gen.enable_dynTarget.getToggleState();
		ui_dyn.toggleVisible(ui_bmbf_gen.enable_dynTarget.getToggleState());
	};

	//Desired Movement Amplitude
	ui_dyn.desired_Amplitude.onValueChange = [this]
	{
		processor.soniMappingCompute.dyn_desired_Amplitude = ui_dyn.desired_Amplitude.getValue();
	};

	//Desired Movement Frequency
	ui_dyn.desired_MovementFreq.addListener(this);

	//Movement Function Order
	ui_dyn.movementFunc_Order.onValueChange = [this]
	{
		processor.soniMappingCompute.movementFunc_Order = ui_dyn.movementFunc_Order.getValue();
	};

	//Movement Error Tolerance
	ui_dyn.movement_errorTolerance.onValueChange = [this]
	{
		processor.soniMappingCompute.movement_ErrorTolerance = ui_dyn.movement_errorTolerance.getValue();
	};

	//Calibrate Target
	ui_mpCal.calibrateTarget.onClick = [this]
	{
		calibrateTarget_handlePress();
	};

	//Target Label
	ui_mpCal.calibrate_presentTarget.setText("Current Target: "
		+ String(processor.gaitAnalysis.calibrationValues_Temp[processor.gaitAnalysis.gaitParams.activeGaitParam],2), dontSendNotification);
	updateCalibrationLabels();
	
	//Save Calibration
	ui_mpCal.calibrateTarget_Save.onClick = [this]
	{
		calibrateTarget_SaveHandle();
	};

	// Discard Calibration
	ui_mpCal.calibrateTarget_Discard.onClick = [this]
	{
		calibrateTarget_DiscardHandle();
	};

	//Enable RTV
	ui_bmbf_gen.realTimeVisualize.onStateChange = [this]
	{
		isRealTimeVisualize = ui_bmbf_gen.realTimeVisualize.getToggleState();
		ui_rtv_1d.toggleVisible(isRealTimeVisualize);
	};
}

// CONFIGURE 1D VISUALIZER
void GaitSonificationAudioProcessorEditor::configureRealTimeVisualizer()
{
	float minValue = processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
		[processor.gaitAnalysis.gaitParams.activeGaitParam].minVal;
	float maxValue = processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
		[processor.gaitAnalysis.gaitParams.activeGaitParam].maxVal;
	ui_rtv_1d.configureBounds(minValue, maxValue);
}

// COMBO BOX CHANGE HANDLING - ALL ACROSS UI
void GaitSonificationAudioProcessorEditor::comboBoxChanged(ComboBox *box)
{
	for (int i = 0; i < 8; i++)
	{
		if (box == &ui_musiCon_gen.inst_Variant[i])
		{
			processor.switchInstVariant(i, ui_musiCon_gen.inst_Variant[i].getSelectedId());
			channel_refreshSliders(ui_musiCon_indiv.channel_ActiveTrack);
		}
	}

	if (box == &ui_bmbf_gen.exerciseMode)
	{
		if (processor.imuRecord.isRecording_Sensor)
		{
			processor.stopRecording_Sensor();
			ui_bmbf_gen.recordGaitParam.setButtonText("Rec Gait Param");
			ui_bmbf_gen.recordGaitParam.setColour(ui_bmbf_gen.recordGaitParam.buttonColourId, Colours::red);
		}
		processor.setExerciseMode(ui_bmbf_gen.exerciseMode.getSelectedId());

		if (ui_bmbf_gen.exerciseMode.getSelectedId() > 1)
		{
			ui_bmbf_gen.exerciseMode.setColour(ui_bmbf_gen.exerciseMode.backgroundColourId, Colours::green);
		}
		else
		{
			ui_bmbf_gen.exerciseMode.setColour(ui_bmbf_gen.exerciseMode.backgroundColourId, Colours::red);
		}

		// Toggle Exercise Specific Controls
		ui_mpCal.changeButtonText(ui_bmbf_gen.exerciseMode.getSelectedId());
		ui_bmbf_ex.toggleVisible(ui_bmbf_gen.exerciseMode.getSelectedId(), isSonificationTab);
		
		repopulateLists(ui_bmbf_gen.exerciseMode.getSelectedId());
		updateCalibrationLabels();
	}

	for (int i = 0; i < processor.gaitAnalysis.sensorInfo.numSensorsMax; i++)
	{
		if (box == &ui_sensorCon.Location[i])
		{
			processor.gaitAnalysis.sensorInfo.bodyLocation[i] = ui_sensorCon.Location[i].getSelectedId();
		}
	}

	if (box == &ui_bmbf_gen.gaitParam_Current)
	{
		processor.gaitAnalysis.gaitParams.updateGaitParameter(ui_bmbf_gen.gaitParam_Current.getSelectedId());
		updateControls_gaitParam(false);
		updateCalibrationLabels();
		ui_rtv_1d.rtv_minBound.setText(std::to_string((int)processor.gaitAnalysis.gaitParams.
			gaitParam_ObjectArray[processor.gaitAnalysis.gaitParams.activeGaitParam].minVal), dontSendNotification);
		ui_rtv_1d.rtv_minBound.setText(std::to_string((int)processor.gaitAnalysis.gaitParams.
			gaitParam_ObjectArray[processor.gaitAnalysis.gaitParams.activeGaitParam].maxVal), dontSendNotification);
	}

	if (box == &ui_bmbf_gen.audioParam_Current)
	{
		processor.updateAudioParameter(ui_bmbf_gen.audioParam_Current.getSelectedId(), 1);
		updateControls_audioParam(false);
	}

	if (box == &ui_dyn.desired_MovementFreq)
	{
		processor.soniMappingCompute.movementFreqSetting = ui_dyn.desired_MovementFreq.getSelectedId();
	}

	if (box == &ui_bmbf_gen.desiredBehavior)
	{
		processor.gaitAnalysis.gaitParams.setDesiredBehavior((int)ui_bmbf_gen.desiredBehavior.getSelectedId() - 1);
		switch (ui_bmbf_gen.desiredBehavior.getSelectedId())
		{
		case 1:
			ui_bmbf_gen.desiredBehavior.setColour(ui_bmbf_gen.desiredBehavior.backgroundColourId, Colours::green);
			ui_rtv_1d.rtv_targetRange.setColour(ui_rtv_1d.rtv_targetRange.backgroundColourId, Colours::green);
			break;
		case 2:
			ui_bmbf_gen.desiredBehavior.setColour(ui_bmbf_gen.desiredBehavior.backgroundColourId, Colours::blue);
			ui_rtv_1d.rtv_targetRange.setColour(ui_rtv_1d.rtv_targetRange.backgroundColourId, Colours::blue);
			break;
		case 3:
			ui_bmbf_gen.desiredBehavior.setColour(ui_bmbf_gen.desiredBehavior.backgroundColourId, Colours::grey);
			ui_rtv_1d.rtv_targetRange.setColour(ui_rtv_1d.rtv_targetRange.backgroundColourId, Colours::grey);
			break;
		}
	}

	if (box == &ui_bmbf_ex.staticBalance_FeedbackSlope)
		processor.gaitAnalysis.staticBalance_ZoneMap_Current = ui_bmbf_ex.staticBalance_FeedbackSlope.getSelectedId();

	if (box == &ui_bmbf_ex.staticBalance_calibrationMode)
		processor.gaitAnalysis.staticBalance_calibrationMode = ui_bmbf_ex.staticBalance_calibrationMode.getSelectedId() - 1;

	if (box == &ui_bmbf_ex.HS_TimingMode)
		processor.gaitAnalysis.isHalfTime = ui_bmbf_ex.HS_TimingMode.getSelectedId() == 1 ? false : true;

	if (box == &ui_musiCon_indiv.channel_TrackIdx)
	{
		ui_musiCon_indiv.channel_ActiveTrack = ui_musiCon_indiv.channel_TrackIdx.getSelectedId() - 1;
		channel_refreshSliders(ui_musiCon_indiv.channel_ActiveTrack);
	}

	if (box == &ui_musiCon_indiv.channel_EQ_FiltIdx)
	{
		ui_musiCon_indiv.channel_EQ_ActiveFiltIdx = ui_musiCon_indiv.channel_EQ_FiltIdx.getSelectedId() - 1;
		channel_refreshSliders(ui_musiCon_indiv.channel_ActiveTrack);
	}

	
	if (box == &ringVisualize.emph_Strategy)
	{
		processor.updateAudioParameter(box->getSelectedId(), 2);
	}

	if (box == &ui_musiCon_gen.music_Mode)
	{
		processor.musicMode = box->getSelectedId();
		processor.sequencer.musicMode = box->getSelectedId();
		switch (processor.musicMode)							// UI Visibility
		{
		case 1:
			if (presentTab == 1) ui_musiCon_gen.song_LoadFile.setVisible(true);
			ringVisualize.inbuilt_ChooseMIDI.setVisible(false);
			ringVisualize.inbuilt_Scale.setVisible(false);
			ringVisualize.inbuilt_Tonic.setVisible(false);
			break;
		case 2:
			if (presentTab == 1) ui_musiCon_gen.song_LoadFile.setVisible(true);
			ringVisualize.inbuilt_ChooseMIDI.setVisible(false);
			ringVisualize.inbuilt_Scale.setVisible(false);
			ringVisualize.inbuilt_Tonic.setVisible(false);
			break;
		case 3:
			ui_musiCon_gen.song_LoadFile.setVisible(false);
			if (presentTab == 1) ringVisualize.inbuilt_ChooseMIDI.setVisible(true);
			if (presentTab == 1) ringVisualize.inbuilt_Scale.setVisible(true);
			if (presentTab == 1) ringVisualize.inbuilt_Tonic.setVisible(true);
			break;
		}
	}

	if (box == &ringVisualize.inbuilt_ChooseMIDI)
	{
		String path = "";
		if (processor.musicMode == 3 && box->getSelectedId() != 1)
		{
			path = processor.sequencer.currentMusic.MelLibPath
				+ processor.sequencer.currentMusic.MelLibFiles[box->getSelectedId() - 2];
			processor.sequencer.setTimingMode(path);
			processor.setTempo(ui_musiCon_gen.tempo_Slider.getValue());
			setRhythmSpecificVariants();
			processor.initializeTrackGains();
			setGainSliders();		
			processor.setFilename(path);
			updateMusicControlValues();
			refreshBeatLabels();
		}
	}

	if (box == &ringVisualize.inbuilt_Scale)
	{
		processor.sequencer.scaleID_TRANS = box->getSelectedId() - 1;
	}

	if (box == &ringVisualize.inbuilt_Tonic)
	{
		processor.sequencer.tonicOffset_TRANS = box->getSelectedId() - 1;
	}
}

// HANDLE EXERCISE MODE CHANGE - REPOPULATE MP AP LISTS
void GaitSonificationAudioProcessorEditor::repopulateLists(short exerciseMode)
{
	ui_bmbf_gen.gaitParam_Current.clear();
	ui_bmbf_gen.audioParam_Current.clear();
	int index = 0;

	for (int i = 0; i < processor.gaitAnalysis.gaitParams.numMovementParams; i++)
	{
		if (processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray[i].isIncluded_UseScenarios[exerciseMode - 1])
			ui_bmbf_gen.gaitParam_Current.addItem(processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray[i].name, i + 1);
	}
	ui_bmbf_gen.gaitParam_Current.setSelectedId(processor.gaitAnalysis.gaitParams.indices_MP[exerciseMode - 1][0] + 1);

	for (int i = 0; i < processor.audioParams.numSoni_Musical; i++)
	{
		if (processor.audioParams.audioParam_ObjectArray[i].isIncluded_UseScenarios[exerciseMode - 1])
			ui_bmbf_gen.audioParam_Current.addItem(processor.audioParams.audioParam_ObjectArray[i].name, i+ 1);
	}
	ui_bmbf_gen.audioParam_Current.setSelectedId(processor.audioParams.indices_AP[exerciseMode - 1][0] + 1);
}

// HANDLE CHOSEN MP CHANGE - UI UPDATE
void GaitSonificationAudioProcessorEditor::updateControls_gaitParam(bool isCallback)
{
	if (isCallback)
	{
		ui_bmbf_gen.gaitParam_CurrentValue.setText("Current Value: "
			+ String(processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
				[processor.gaitAnalysis.gaitParams.activeGaitParam].currentValue,2), dontSendNotification);

		ui_bmbf_gen.gaitParam_targetValue.setText("Target Value: "
			+ String(processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
				[processor.gaitAnalysis.gaitParams.activeGaitParam].target,2), dontSendNotification);
	}
	else
	{
		ui_bmbf_gen.gaitParam_Name.setText(processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[processor.gaitAnalysis.gaitParams.activeGaitParam].name, dontSendNotification);
		
		ui_bmbf_gen.gaitParam_DesiredBehavior_Label.setText
		(processor.gaitAnalysis.gaitParams.names_DesiredBehavior
			[processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[processor.gaitAnalysis.gaitParams.activeGaitParam].desiredBehavior], dontSendNotification);
		ui_bmbf_gen.desiredBehavior.setSelectedId(processor.gaitAnalysis.gaitParams.
			gaitParam_ObjectArray[processor.gaitAnalysis.gaitParams.activeGaitParam].desiredBehavior + 1);

		float targetSliderVal = (processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[processor.gaitAnalysis.gaitParams.activeGaitParam].target
								- processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[processor.gaitAnalysis.gaitParams.activeGaitParam].minVal)
								/(processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[processor.gaitAnalysis.gaitParams.activeGaitParam].maxVal
									- processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[processor.gaitAnalysis.gaitParams.activeGaitParam].minVal);

		ui_bmbf_gen.gaitParam_setTarget.setValue(targetSliderVal);
	}
}

// HANDLE CHOSEN AP CHANGE - UI UPDATE
void GaitSonificationAudioProcessorEditor::updateControls_audioParam(bool isCallback)
{
	if (!isCallback)
		ui_bmbf_gen.audioParam_Name.setText(processor.audioParams.audioParam_ObjectArray
			[processor.audioParams.activeAudioParam].name, dontSendNotification);
}

// TOGGLE TAB ON/OFF - PERIPHERALS
void GaitSonificationAudioProcessorEditor::togglePeripheralsTab(bool on)
{
	ui_sensorCon.toggleVisible(on);
}

// TOGGLE TAB ON/OFF - SONIFICATION
void GaitSonificationAudioProcessorEditor::toggleSonificationControlsTab(bool on)
{
	ui_bmbf_gen.toggleVisible(on);
	ui_dyn.toggleVisible(on && processor.isTargetDynamic);
	ui_bmbf_ex.toggleVisible(ui_bmbf_gen.exerciseMode.getSelectedId(), on);
	ui_mpCal.toggleVisible(on);
	ui_rtv_1d.toggleVisible(on);
}

// UI TIMER CALLBACK FUNCTION
void GaitSonificationAudioProcessorEditor::timerCallback()
{
	// UPDATE OSC CONNECTION STATUS DISPLAY
	updateOSC_ConnectionDisplay();

	// CHECK FOR TAB CHANGES
	int currentTab = tabs->getCurrentTabIndex();
	if (currentTab == 0)							sensorConfig_updateLabels();
	if (previousTab != currentTab)					switchTab(currentTab);
	previousTab = currentTab;
	handleProgressBarColour();
	// IF SONIFICATION TAB, UPDATE UI IN REALTIME
	if (currentTab == 2)
	{
		updateControls_audioParam(true);
		updateControls_gaitParam(true);
		ui_dyn.dynTargetLabel.setText("Target: " +
			String(processor.dynamicTarget,2), dontSendNotification);
	}
	if (currentTab == 1 && processor.isPlaying)		updateTimeLabels();

	// UPDATE STS TEXT
	if (processor.exerciseMode_Present == 4 || processor.exerciseMode_Present == 5)
	{
		String isStandingText = processor.gaitAnalysis.sitStand_isStanding ? "Standing" : "Sitting";
		ui_bmbf_ex.sitStand_isStanding.setText(isStandingText, dontSendNotification);
	}

	// CHECK IF CALIBRATING, UPDATE UI
	if (processor.isCalibrating)
		updateCalibrationLabels();

	// CHECK IF VISUALIZING, UPDATE UI
	if (isRealTimeVisualize)
		updateRealTimeVisualizer();

	// CHECK IF TRUNK PROJECTION ZONE, UPDATE UI
	if (processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
		[processor.gaitAnalysis.gaitParams.activeGaitParam].name == "Trunk Projection Zone")
		updateProjectionZoneVisualizer();
}

// SWITCH UI TAB
void GaitSonificationAudioProcessorEditor::switchTab(int currentTab)
{
	// SET FLAGS BASED ON TAB INDEX
	switch (currentTab)
	{
	case 0:
		isPeripheralsTab = true;
		isMusicControlsTab = false;
		isSonificationTab = false;
		break;
	case 1:
		isPeripheralsTab = false;
		isMusicControlsTab = true;
		isSonificationTab = false;
		break;
	case 2:
		isPeripheralsTab = false;
		isMusicControlsTab = false;
		isSonificationTab = true;
		break;
	}
	togglePeripheralsTab(isPeripheralsTab);
	toggleMusicControlsTab(isMusicControlsTab);
	toggleMusicControls_Secondary(currentTab);
	toggleSonificationControlsTab(isSonificationTab);
}

// JUCE PAINT
void GaitSonificationAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.setColour (Colours::white);
    g.setFont (15.0f);
}

// SET UI LAYOUT
void GaitSonificationAudioProcessorEditor::resized()
{	
	ui_bmbf_gen.setLayout();
	ui_bmbf_ex.setLayout();
	ui_dyn.setLayout();
	ui_mpCal.setLayout();
	ui_musiCon_gen.setLayout();
	ui_musiCon_inbuilt.setLayout();
	ui_musiCon_indiv.setLayout();
	ui_rtv_1d.setLayout();
	ui_sensorCon.setLayout();
	ui_tabSpec.setLayout();
	ui_zv_2d.setLayout();
	ringVisualize.setLayout();
}

// REAL TIME UPDATE 2D VISUALIZER
void GaitSonificationAudioProcessorEditor::updateProjectionZoneVisualizer()
{
	int MPIndex_Projection = 2;
	int MPIndex_ML = 0;
	int MPIndex_AP = 1;
	int currentZone = processor.gaitAnalysis.gaitParams.
		gaitParam_ObjectArray[MPIndex_Projection].currentValue;
	float MPVal_ML = processor.gaitAnalysis.gaitParams.
		gaitParam_ObjectArray[MPIndex_ML].currentValue;
	float MPVal_AP = processor.gaitAnalysis.gaitParams.
		gaitParam_ObjectArray[MPIndex_AP].currentValue;
	float ML_CenterOffset = processor.gaitAnalysis.staticBalance_BoundsCoordinates[0][0];
	float AP_CenterOffset = processor.gaitAnalysis.staticBalance_BoundsCoordinates[0][1];
	float pixelsPerDegree = ui_zv_2d.ZV_totalLength / 40;
	Colour colourSet[6] = {Colours::green,Colours::yellowgreen,Colours::yellow,Colours::orange,Colours::red,Colours::red};
	float roll_Thresh = processor.gaitAnalysis.staticBalance_Div_Roll;
	float innerRect_startingX = fmax(950, ui_zv_2d.ZV_zeroPos_H + ML_CenterOffset*pixelsPerDegree + 10 - roll_Thresh * pixelsPerDegree);
	float innerRect_width = fmin(2 * roll_Thresh * pixelsPerDegree, 1150 - innerRect_startingX);

	float pitch_Thresh = processor.gaitAnalysis.staticBalance_Div_Pitch;
	float innerRect_AP_startingY = max(330, ui_zv_2d.ZV_zeroPos_V + AP_CenterOffset * pixelsPerDegree + 10 - pitch_Thresh * pixelsPerDegree);
	float innerRect_AP_height = min(2 * pitch_Thresh * pixelsPerDegree, 530 - innerRect_AP_startingY);
	ui_zv_2d.ZV_InnerRect.setBounds(innerRect_startingX, 330, innerRect_width, ui_zv_2d.ZV_totalLength);
	ui_zv_2d.ZV_InnerRect_AP.setBounds(950, innerRect_AP_startingY, ui_zv_2d.ZV_totalLength, innerRect_AP_height);
	
	float currentPos_H = ui_zv_2d.ZV_zeroPos_H - max(-18, min(MPVal_ML, 18)) * pixelsPerDegree;
	float currentPos_V = ui_zv_2d.ZV_zeroPos_V - max(-18, min(MPVal_AP, 18)) * pixelsPerDegree;
	ui_zv_2d.ZV_CurrentPos.setBounds(currentPos_H, currentPos_V, 20, 20);
	ui_zv_2d.ZV_CurrentPos.setText(String(currentZone), dontSendNotification);
	ui_zv_2d.ZV_CurrentPos.setColour(ui_zv_2d.ZV_CurrentPos.backgroundColourId, colourSet[currentZone - 1]);
}

// REAL TIME UPDATE 1D VISUALIZER
void GaitSonificationAudioProcessorEditor::updateRealTimeVisualizer()
{
	float originalTarget = processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
		[processor.gaitAnalysis.gaitParams.activeGaitParam].target;
	float minValue = processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
		[processor.gaitAnalysis.gaitParams.activeGaitParam].minVal;
	float maxValue = processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
		[processor.gaitAnalysis.gaitParams.activeGaitParam].maxVal;
	float currentValue = processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
		[processor.gaitAnalysis.gaitParams.activeGaitParam].currentValue;
	short desiredBeh = processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
		[processor.gaitAnalysis.gaitParams.activeGaitParam].desiredBehavior;

	float chosenTarget = processor.isTargetDynamic ?
		processor.dynamicTarget : originalTarget;
	float paramRange = (maxValue - minValue);
	float paramMin = minValue;
	float paramMax = maxValue;
	float currentVal = processor.soniMappingCompute.isSoniSource_Slider ? paramMin + processor.soniMappingCompute.soniVal_Slider * paramRange :
						min(max(currentValue,
						paramMin),paramMax);
	float targetStartPoint = 0;		float targetWidth = 0;	
	float pixelMultiplier = 1 / paramRange * ui_rtv_1d.rtv_width;
	float currentStartPoint = (currentVal-paramMin) * pixelMultiplier;
	switch (desiredBeh)
	{
	case 0:
		targetStartPoint = 0;
		targetWidth = fmax(20.0, chosenTarget * pixelMultiplier);
		break;
	case 1:
		targetStartPoint = (chosenTarget - paramMin) * pixelMultiplier;
		targetWidth = 20;
		break;
	case 2:
		targetStartPoint = chosenTarget * pixelMultiplier;
		targetWidth = fmax(20.0,(maxValue - 
			chosenTarget) * pixelMultiplier);
		break;
	}
	ui_rtv_1d.rtv_targetRange.setBounds(ui_rtv_1d.rtv_startX + targetStartPoint, 600 - ui_rtv_1d.rtv_ht / 2, targetWidth, ui_rtv_1d.rtv_ht);
	ui_rtv_1d.rtv_currentValue.setBounds(ui_rtv_1d.rtv_startX + currentStartPoint, 600 + ui_rtv_1d.rtv_ht / 2, 20, ui_rtv_1d.rtv_ht);
	if (processor.mapVal < 0.001)
	{
		ui_rtv_1d.rtv_currentValue.setColour(ui_rtv_1d.rtv_currentValue.backgroundColourId, Colours::yellow);
		ui_rtv_1d.rtv_currentValue.setColour(ui_rtv_1d.rtv_currentValue.textColourId, Colours::black);
	}
	else
	{
		ui_rtv_1d.rtv_currentValue.setColour(ui_rtv_1d.rtv_currentValue.backgroundColourId, Colours::red);
		ui_rtv_1d.rtv_currentValue.setColour(ui_rtv_1d.rtv_currentValue.textColourId, Colours::white);
	}
}

// HANDLE BUTTON PRESS - CALIBRATE
void GaitSonificationAudioProcessorEditor::calibrateTarget_handlePress()
{
	if (processor.exerciseMode_Present == 1)
		return;
	if (!processor.isCalibrating)
	{
		processor.stopMusic();
		processor.isCalibrating = true;
		processor.gaitAnalysis.calibrationValues_Temp[processor.gaitAnalysis.gaitParams.activeGaitParam] = 0.0;
		processor.gaitAnalysis.isParamCalibrated[processor.gaitAnalysis.gaitParams.activeGaitParam] = false;
		ui_mpCal.calibrateTarget.setVisible(false);
		ui_mpCal.calibrateTarget_Discard.setVisible(true);
		ui_mpCal.calibrateTarget_Save.setVisible(true);
	}
	else
	{
		processor.isCalibrating = false;
		ui_mpCal.calibrateTarget.setVisible(true);
		ui_mpCal.calibrateTarget_Discard.setVisible(false);
		ui_mpCal.calibrateTarget_Save.setVisible(false);
	}
}

// HANDLE BUTTON PRESS - CALIBRATE SAVE
void GaitSonificationAudioProcessorEditor::calibrateTarget_SaveHandle()
{
	processor.isCalibrating = false;
	processor.gaitAnalysis.saveCalibration(processor.gaitAnalysis.gaitParams.activeGaitParam);
	updateCalibrationLabels();
	float paramRange = processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray[processor.gaitAnalysis.gaitParams.activeGaitParam].maxVal -
		processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray[processor.gaitAnalysis.gaitParams.activeGaitParam].minVal;
	float newTempo = 0;
	ui_mpCal.calibrateTarget.setVisible(true);
	ui_mpCal.calibrateTarget_Discard.setVisible(false);
	ui_mpCal.calibrateTarget_Save.setVisible(false);
	float staticBalance_coordinateDistance = 0;

	if (processor.gaitAnalysis.gaitParams.activeGaitParam == 11 || processor.gaitAnalysis.gaitParams.activeGaitParam == 12) //HANDLE TEMPO SETTING
	{
		newTempo = 120 / processor.gaitAnalysis.calibrationValues[processor.gaitAnalysis.gaitParams.activeGaitParam];
		ui_musiCon_gen.tempo_Slider.setValue(newTempo);
		ui_bmbf_ex.HS_Tolerance.setValue(processor.gaitAnalysis.strideDur_COV);
	}

	if (processor.gaitAnalysis.gaitParams.activeGaitParam == 2)
	{
		staticBalance_coordinateDistance =
			sqrt(pow(processor.gaitAnalysis.staticBalance_BoundsCoordinates[1][0]
				- processor.gaitAnalysis.staticBalance_BoundsCoordinates[0][0], 2) +
				pow(processor.gaitAnalysis.staticBalance_BoundsCoordinates[1][1]
					- processor.gaitAnalysis.staticBalance_BoundsCoordinates[0][1], 2));
		if (processor.gaitAnalysis.staticBalance_calibrationMode == 1)
		{
			ui_bmbf_ex.staticBalance_Div_Pitch.setValue(staticBalance_coordinateDistance);
			ui_bmbf_ex.staticBalance_Div_Roll.setValue(staticBalance_coordinateDistance);
		}
	}

	else
		ui_bmbf_gen.gaitParam_setTarget.setValue(processor.gaitAnalysis.calibrationValues[processor.gaitAnalysis.gaitParams.activeGaitParam] / paramRange);
}

// HANDLE BUTTON PRESS - CALIBRATE DISCARD
void GaitSonificationAudioProcessorEditor::calibrateTarget_DiscardHandle()
{
	processor.isCalibrating = false;
	processor.gaitAnalysis.discardCalibration(processor.gaitAnalysis.gaitParams.activeGaitParam);
	ui_mpCal.calibrate_presentTarget.setText("Current Target: "
		+ String(processor.gaitAnalysis.calibrationValues[processor.gaitAnalysis.gaitParams.activeGaitParam], 2), dontSendNotification);
	ui_mpCal.calibrateTarget.setVisible(true);
	ui_mpCal.calibrateTarget_Discard.setVisible(false);
	ui_mpCal.calibrateTarget_Save.setVisible(false);

	if (processor.gaitAnalysis.gaitParams.activeGaitParam == 2)
	{
		processor.gaitAnalysis.staticBalance_BoundsCoordinates[processor.gaitAnalysis.staticBalance_calibrationMode][0] = 0;
		processor.gaitAnalysis.staticBalance_BoundsCoordinates[processor.gaitAnalysis.staticBalance_calibrationMode][0] = 1;
	}

	if (processor.gaitAnalysis.gaitParams.activeGaitParam == 11 || processor.gaitAnalysis.gaitParams.activeGaitParam == 12)
		processor.gaitAnalysis.flush_HSArray();
}
