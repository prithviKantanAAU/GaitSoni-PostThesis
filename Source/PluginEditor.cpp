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
	toggleMusicControls_Secondary(presentTab);
	channel_configureSliders();
	init_RingVisualize();
	config_musicControls_inbuilt();
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
			if (processor.sequencer.isPlaying)
				processor.sequencer.stopMusic();

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
		processor.sequencer.togglePlayPause();
	};

	// Song Progress Bar
	ui_musiCon_gen.song_Progress.setColour(ui_musiCon_gen.song_Progress.backgroundColourId, Colours::darkblue);
	ui_musiCon_gen.song_Progress.setColour(ui_musiCon_gen.song_Progress.foregroundColourId, Colours::darkred);
	ui_musiCon_gen.song_Progress.setPercentageDisplay(false);

	// Song Stop
	ui_musiCon_gen.song_Stop.onClick = [this]
	{
		processor.sequencer.stopMusic();
	};

	// Tap Tempo
	ui_musiCon_gen.tempo_Tap.onClick = [this]
	{
		if (processor.sequencer.handleTapTempoPress())
		{
			processor.setTempo(processor.sequencer.tempo);
			ringVisualizeBoxConfig();
		}
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
				processor.sequencer.initializeTracksForPlayback();
				setGainSliders();
			}
			processor.sequencer.setFilename(filePath);
		}
		updateMusicControlValues();
		refreshBeatLabels();					// Based on song specifications
	};

	// Set next rhythm
	ui_musiCon_gen.rhythm_SetNext.onClick = [this]
	{
		processor.sequencer.nextRhythm(processor.musicMode);
		processor.sequencer.resetPercMIDIOnChange(processor.sequencer.midiTicksElapsed);
		setRhythmSpecificVariants();
		processor.sequencer.initializeTracksForPlayback();
		setGainSliders();
		refreshBeatLabels();
		channel_refreshSliders(ui_musiCon_indiv.channel_ActiveTrack);
	};

	// Set tempo slider
	ui_musiCon_gen.tempo_Slider.onValueChange = [this]
	{
		processor.setTempo(ui_musiCon_gen.tempo_Slider.getValue());
		if (presentTab == 1 && presentMusiCon_Disp == 1)	ringVisualizeBoxConfig();
	};

	// Master gain slider
	ui_musiCon_gen.song_master_Gain.setValue(processor.sequencer.mixerSettings.masterGain);
	ui_musiCon_gen.song_master_Gain.onValueChange = [this]
	{
		processor.sequencer.applyMasterGain(ui_musiCon_gen.song_master_Gain.getValue());
	};
	
	// Music Mode
	ui_musiCon_gen.music_Mode.addListener(this);

	// Master EQ Set Band 1 F
	ui_musiCon_gen.song_master_EQ_B1_F.onValueChange = [this]
	{
		processor.sequencer.dspFaust.setParamValue(processor.sequencer.faustStrings.MasterEQ_1_F.c_str(),
			ui_musiCon_gen.song_master_EQ_B1_F.getValue());
		ui_musiCon_gen.song_master_EQ_B1_F_Lab.setText("F", dontSendNotification);
	};

	// Master EQ Set Band 2 F
	ui_musiCon_gen.song_master_EQ_B2_F.onValueChange = [this]
	{
		processor.sequencer.dspFaust.setParamValue(processor.sequencer.faustStrings.MasterEQ_2_F.c_str(), ui_musiCon_gen.song_master_EQ_B2_F.getValue());
		ui_musiCon_gen.song_master_EQ_B2_F_Lab.setText("F", dontSendNotification);
	};

	// Master EQ Set Band 1 G
	ui_musiCon_gen.song_master_EQ_B1_G.onValueChange = [this]
	{
		processor.sequencer.dspFaust.setParamValue(processor.sequencer.faustStrings.MasterEQ_1_Gain.c_str(), ui_musiCon_gen.song_master_EQ_B1_G.getValue());
	};

	// Master EQ Set Band 2 G
	ui_musiCon_gen.song_master_EQ_B2_G.onValueChange = [this]
	{
		processor.sequencer.dspFaust.setParamValue(processor.sequencer.faustStrings.MasterEQ_2_Gain.c_str(), ui_musiCon_gen.song_master_EQ_B2_G.getValue());
	};

	for (int i = 0; i < 8; i++)
	{
		// Instrument Variant Names - Populate
		ui_musiCon_gen.inst_Variant[i].addItem(processor.sequencer.mixerSettings.varNames[i][0], 1);
		ui_musiCon_gen.inst_Variant[i].addItem(processor.sequencer.mixerSettings.varNames[i][1], 2);
		ui_musiCon_gen.inst_Variant[i].addItem(processor.sequencer.mixerSettings.varNames[i][2], 3);
		ui_musiCon_gen.inst_Variant[i].addListener(this);
		ui_musiCon_gen.inst_Variant[i].setSelectedId(processor.sequencer.currentMusic.baseBeats[processor.sequencer.index_baseBeat].variantConfig[i]);
		ui_musiCon_gen.inst_Variant_Lab[i].setText(ui_musiCon_gen.inst_Names[i], dontSendNotification);
		ui_musiCon_gen.inst_Variant_Lab[i].attachToComponent(&ui_musiCon_gen.inst_Variant[i], true);

		// Instrument Mute
		ui_musiCon_gen.song_track_Mute[i].onStateChange = [this, i]
		{
			int val = ui_musiCon_gen.song_track_Mute[i].getToggleState() ? 1 : 0;
			processor.sequencer.setTrackMutes(i, val);
			Colour trackColor = val == 0 ? Colours::yellow : Colours::red;
			ui_musiCon_gen.song_track_GainOffset[i]
				.setColour(ui_musiCon_gen.song_track_GainOffset[i].trackColourId, trackColor);
		};
		
		// Track Gain Offset
		ui_musiCon_gen.song_track_GainOffset[i].onValueChange = [this,i]
		{
			processor.sequencer.setTrackGains(i, ui_musiCon_gen.song_track_GainOffset[i].getValue());
			ui_musiCon_gen.song_track_GainOffset_Lab[i].setText(String(ui_musiCon_gen.song_track_GainOffset[i].getValue(), 1), dontSendNotification);
		};	

		ui_musiCon_gen.showControls_Channel.onClick = [this]
		{
			presentMusiCon_Disp = 0;
			handleDisplay_secondaryMusic();
		};

		ui_musiCon_gen.showControls_RingVisualize.onClick = [this]
		{
			presentMusiCon_Disp = 1;
			handleDisplay_secondaryMusic();
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
		float target_MIN = ui_bmbf_gen.gaitParam_setTarget.getMinValue();
		float target_MAX = ui_bmbf_gen.gaitParam_setTarget.getMaxValue();
		processor.gaitAnalysis.gaitParams.setTargetValue(target_MIN, false);
		processor.gaitAnalysis.gaitParams.setTargetValue(target_MAX, true);
		ui_bmbf_gen.gaitParam_targetValue.setText
		(
			"Target Range: " + String(processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
			[processor.gaitAnalysis.gaitParams.activeGaitParam].target_MIN,2) + " to " + 
			String(processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
				[processor.gaitAnalysis.gaitParams.activeGaitParam].target_MAX, 2),
			dontSendNotification
		);
	};
	ui_bmbf_gen.gaitParam_targetValue.setText
	("Target Value: " + String(processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
		[processor.gaitAnalysis.gaitParams.activeGaitParam].target_MIN, 2), dontSendNotification);

	// SONIFICATION MODE: SLIDER / SENSOR

	ui_bmbf_gen.soni_isSliderSource.onStateChange = [this]
	{
		processor.gaitAnalysis.gaitParams.isSliderMode
		 = (ui_bmbf_gen.soni_isSliderSource.getToggleState());
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
		processor.gaitAnalysis.gaitParams.sliderVal = ui_bmbf_gen.soni_sliderSource.getValue();
	};
	updateControls_gaitParam(false);

	// EXERCISE MODE
	for (int i = 0; i < processor.gaitAnalysis.gaitParams.num_UseScenarios; i++)
		ui_bmbf_gen.exerciseMode.addItem(processor.gaitAnalysis.gaitParams.exerciseModes[i],i + 1);
	ui_bmbf_gen.exerciseMode.setSelectedId(1);
	ui_bmbf_gen.exerciseMode.addListener(this);

	ui_bmbf_gen.gaitParam_Current.addListener(this);
	ui_bmbf_gen.audioParam_Current.addListener(this);
	repopulateLists(1);

	// MAPPING FUNCTION ORDER
	ui_bmbf_gen.soni_Order.onValueChange = [this]
	{
		processor.gaitAnalysis.gaitParams.order_MapFunc = ui_bmbf_gen.soni_Order.getValue();
	};

	// NUM AP QUANTIZATION LEVELS
	ui_bmbf_gen.soni_QuantLevels.onValueChange = [this]
	{
		processor.gaitAnalysis.gaitParams.numQuant = ui_bmbf_gen.soni_QuantLevels.getValue();
	};

	// MAPPING POLARITY
	ui_bmbf_gen.invertPolarity.onClick = [this]
	{
		if (processor.gaitAnalysis.gaitParams.isPolarityNormal)
		{
			processor.gaitAnalysis.gaitParams.isPolarityNormal = false;
			ui_bmbf_gen.invertPolarity.setButtonText("Polarity Reversed");
			ui_bmbf_gen.invertPolarity.setColour(ui_bmbf_gen.invertPolarity.buttonColourId, Colours::darkgoldenrod);
		}
		else
		{
			processor.gaitAnalysis.gaitParams.isPolarityNormal = true;
			ui_bmbf_gen.invertPolarity.setButtonText("Polarity Normal");
			ui_bmbf_gen.invertPolarity.setColour(ui_bmbf_gen.invertPolarity.buttonColourId, Colours::green);
		}
	};

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

	// HS Acc Threshold
	// Simulation Frequency
	ui_bmbf_ex.HS_AccThresh.onValueChange = [this]
	{
		processor.gaitAnalysis.HS_thresh_pos = ui_bmbf_ex.HS_AccThresh.getValue();
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
		// HAS TO GO EVENTUALLY
	};

	//Desired Movement Frequency
	ui_dyn.desired_MovementFreq.addListener(this);

	//Movement Function Order
	ui_dyn.movementFunc_Order.onValueChange = [this]
	{
		//HAS TO GO EVENTUALLY
	};

	//Movement Error Tolerance
	ui_dyn.movement_errorTolerance.onValueChange = [this]
	{
		// HAS TO GO EVENTUALLY
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
	ui_rtv_1d.rtv_minBound.setText(String((int)minValue), dontSendNotification);
	ui_rtv_1d.rtv_maxBound.setText(String((int)maxValue), dontSendNotification);
	ui_rtv_1d.configureBounds(minValue, maxValue);
}

// COMBO BOX CHANGE HANDLING - ALL ACROSS UI
void GaitSonificationAudioProcessorEditor::comboBoxChanged(ComboBox *box)
{
	for (int i = 0; i < 8; i++)
	{
		if (box == &ui_musiCon_gen.inst_Variant[i])
		{
			processor.sequencer.switchInstVariant(i, ui_musiCon_gen.inst_Variant[i].getSelectedId());
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
		configureRealTimeVisualizer();
	}

	if (box == &ui_bmbf_gen.audioParam_Current)
	{
		processor.updateAudioParameter(ui_bmbf_gen.audioParam_Current.getSelectedId(), 1);
	}

	if (box == &ui_dyn.desired_MovementFreq)
	{
		//HAS TO GO EVENTUALLY
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
			ui_musiCon_inbuilt.toggleVisible(false);
			break;
		case 2:
			if (presentTab == 1) ui_musiCon_gen.song_LoadFile.setVisible(true);
			ui_musiCon_inbuilt.toggleVisible(false);
			break;
		case 3:
			ui_musiCon_gen.song_LoadFile.setVisible(false);
			ui_musiCon_inbuilt.toggleVisible(true);
			break;
		}
	}

	if (box == &ui_musiCon_inbuilt.inbuilt_ChooseMIDI)
	{
		String path = "";
		if (processor.musicMode == 3 && box->getSelectedId() != 1)
		{
			path = processor.sequencer.currentMusic.MelLibPath
				+ processor.sequencer.currentMusic.MelLibFiles[box->getSelectedId() - 2];
			processor.sequencer.setTimingMode(path);
			processor.setTempo(ui_musiCon_gen.tempo_Slider.getValue());
			setRhythmSpecificVariants();
			processor.sequencer.initializeTracksForPlayback();
			setGainSliders();		
			processor.sequencer.setFilename(path);
			updateMusicControlValues();
			refreshBeatLabels();
		}
	}

	if (box == &ui_musiCon_inbuilt.inbuilt_Scale)
	{
		processor.sequencer.scaleID_TRANS = box->getSelectedId() - 1;
	}

	if (box == &ui_musiCon_inbuilt.inbuilt_Tonic)
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
	float gp_MIN = 0;
	float gp_MAX = 0;
	float target_MIN = 0;
	float target_MAX = 0;
	gaitParamInfo *gaitParamPointer = &processor.gaitAnalysis.gaitParams;
	if (isCallback)
	{
		ui_bmbf_gen.gaitParam_targetValue.setText
		(
			"Target Range: " + String(gaitParamPointer->gaitParam_ObjectArray
				[gaitParamPointer->activeGaitParam].target_MIN, 2) + " to " +
			String(gaitParamPointer->gaitParam_ObjectArray
				[gaitParamPointer->activeGaitParam].target_MAX, 2),
			dontSendNotification
		);
	}
	else
	{
		target_MIN = gaitParamPointer->gaitParam_ObjectArray
			[gaitParamPointer->activeGaitParam].target_MIN;
		target_MAX = gaitParamPointer->gaitParam_ObjectArray
			[gaitParamPointer->activeGaitParam].target_MAX;
		gp_MIN = gaitParamPointer->gaitParam_ObjectArray
			[gaitParamPointer->activeGaitParam].minVal;
		gp_MAX = gaitParamPointer->gaitParam_ObjectArray
			[gaitParamPointer->activeGaitParam].maxVal;

		ui_bmbf_gen.gaitParam_setTarget.setMinValue((target_MIN - gp_MIN) / (gp_MAX - gp_MIN));
		ui_bmbf_gen.gaitParam_setTarget.setMaxValue((target_MAX - gp_MIN) / (gp_MAX - gp_MIN));
	}
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
	// CHECK FOR TAB CHANGES
	presentTab = tabs->getCurrentTabIndex();
	if (presentTab == 0)							sensorConfig_updateLabels();
	if (previousTab != presentTab)					switchTab(presentTab);
	previousTab = presentTab;

	// UPDATE OSC CONNECTION STATUS DISPLAY
	updateOSC_ConnectionDisplay();

	// CHECK IF TRUNK PROJECTION ZONE, UPDATE UI
	if (processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
		[processor.gaitAnalysis.gaitParams.activeGaitParam].name == "Trunk Projection Zone")
		updateProjectionZoneVisualizer();

	// SONG PROGRESS BAR
	if (presentTab == 1)
	{
		ui_musiCon_gen.song_Progress_Val = processor.sequencer.songProgress;
		if (processor.sequencer.isPlaying)		updateTimeLabels();
		//handleProgressBarColour();
	}

	// IF SONIFICATION TAB, UPDATE UI IN REALTIME
	if (presentTab == 2)
	{
		updateControls_gaitParam(true);

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
	}
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
	float originalTarget_MIN = processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
		[processor.gaitAnalysis.gaitParams.activeGaitParam].target_MIN;
	float originalTarget_MAX = processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
		[processor.gaitAnalysis.gaitParams.activeGaitParam].target_MAX;
	float minValue = processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
		[processor.gaitAnalysis.gaitParams.activeGaitParam].minVal;
	float maxValue = processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
		[processor.gaitAnalysis.gaitParams.activeGaitParam].maxVal;
	float currentValue = processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray
		[processor.gaitAnalysis.gaitParams.activeGaitParam].currentValue;

	float chosenTarget_MIN = processor.isTargetDynamic ?
		processor.dynamicTarget : originalTarget_MIN;
	float chosenTarget_MAX = processor.isTargetDynamic ?
		processor.dynamicTarget : originalTarget_MAX;
	float paramRange = (maxValue - minValue);
	float paramMin = minValue;
	float paramMax = maxValue;
	float currentVal = processor.gaitAnalysis.gaitParams.isSliderMode ? paramMin + processor.gaitAnalysis.gaitParams.sliderVal * paramRange :
						min(max(currentValue,paramMin),paramMax);
	float targetStartPoint = 0;		float targetWidth = 0;	
	float pixelMultiplier = 1 / paramRange * ui_rtv_1d.rtv_width;
	float currentStartPoint = (currentVal-paramMin) * pixelMultiplier;

	targetStartPoint = (chosenTarget_MIN - paramMin) * pixelMultiplier;
	targetWidth = fmax(20, (chosenTarget_MAX - chosenTarget_MIN) * pixelMultiplier);
	ui_rtv_1d.rtv_targetRange.setBounds(ui_rtv_1d.rtv_startX + targetStartPoint, ui_rtv_1d.rtv_startY - 90 - ui_rtv_1d.rtv_ht / 2, targetWidth, ui_rtv_1d.rtv_ht);
	ui_rtv_1d.rtv_currentValue.setBounds(ui_rtv_1d.rtv_startX + currentStartPoint, ui_rtv_1d.rtv_startY - 90 + ui_rtv_1d.rtv_ht / 2, 20, ui_rtv_1d.rtv_ht);
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
		processor.sequencer.stopMusic();
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
	short mp_active = processor.gaitAnalysis.gaitParams.activeGaitParam;
	processor.isCalibrating = false;
	processor.gaitAnalysis.saveCalibration(mp_active);
	updateCalibrationLabels();
	float param_Min = processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray[mp_active].minVal;
	float param_Max = processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray[mp_active].maxVal;
	float paramRange = param_Max - param_Min;
	float newTempo = 0;
	ui_mpCal.calibrateTarget.setVisible(true);
	ui_mpCal.calibrateTarget_Discard.setVisible(false);
	ui_mpCal.calibrateTarget_Save.setVisible(false);
	float staticBalance_coordinateDistance = 0;
	

	if ( processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray[mp_active].name == "HS Timing"
		|| processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray[mp_active].name == "HS Trigger") 
	{
		newTempo = 120 / processor.gaitAnalysis.calibrationValues[processor.gaitAnalysis.gaitParams.activeGaitParam];
		ui_musiCon_gen.tempo_Slider.setValue(newTempo);
		ui_bmbf_ex.HS_Tolerance.setValue(processor.gaitAnalysis.strideDur_COV);
	}

	if (processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray[mp_active].name == "Trunk Projection Zone")
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
	{
		ui_bmbf_gen.gaitParam_setTarget
			.setMaxValue((processor.gaitAnalysis.calibrationValues[mp_active] - param_Min + paramRange / 10)/ paramRange);
		if (param_Max == paramRange)
			ui_bmbf_gen.gaitParam_setTarget.setMinValue(0);
		else
			ui_bmbf_gen.gaitParam_setTarget
			.setMinValue((processor.gaitAnalysis.calibrationValues[mp_active] - param_Min - paramRange / 10) / paramRange);
	}
}

// HANDLE BUTTON PRESS - CALIBRATE DISCARD
void GaitSonificationAudioProcessorEditor::calibrateTarget_DiscardHandle()
{
	short mp_active = processor.gaitAnalysis.gaitParams.activeGaitParam;
	processor.isCalibrating = false;
	processor.gaitAnalysis.discardCalibration(mp_active);
	ui_mpCal.calibrate_presentTarget.setText("Current Target: "
		+ String(processor.gaitAnalysis.calibrationValues[mp_active], 2), dontSendNotification);
	ui_mpCal.calibrateTarget.setVisible(true);
	ui_mpCal.calibrateTarget_Discard.setVisible(false);
	ui_mpCal.calibrateTarget_Save.setVisible(false);

	if (processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray[mp_active].name == "Trunk Projection Zone")
	{
		processor.gaitAnalysis.staticBalance_BoundsCoordinates[processor.gaitAnalysis.staticBalance_calibrationMode][0] = 0;
		processor.gaitAnalysis.staticBalance_BoundsCoordinates[processor.gaitAnalysis.staticBalance_calibrationMode][0] = 1;
	}

	if (processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray[mp_active].name == "HS Timing"
		|| processor.gaitAnalysis.gaitParams.gaitParam_ObjectArray[mp_active].name == "HS Trigger")
		processor.gaitAnalysis.flush_HSArray();
}
