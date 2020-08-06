#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class UI_BMBFSettings_General
{
public:
	UI_BMBFSettings_General()
	{
		configure();
	};
	~UI_BMBFSettings_General() {};

	ToggleButton isStandbyToggle;
	Label isStandbyToggleLabel;
	Label gaitParam_Name;
	Label gaitParam_CurrentValue;
	TextButton recordGaitParam;
	Label gaitParam_targetValue;
	Slider gaitParam_setTarget;
	ToggleButton enable_dynTarget;
	Label enable_dynTarget_Label;
	Label audioParam_Name;
	ToggleButton soni_isSliderSource;
	Label soni_isMusical_Label;
	Label soni_isSliderSource_Label;
	Slider soni_sliderSource;
	Label soni_sliderSource_Label;
	ComboBox desiredBehavior;
	Label gaitParam_DesiredBehavior_Label;
	ToggleButton realTimeVisualize;
	Label realTimeVisualize_Label;
	ComboBox exerciseMode;
	Label exerciseModeLabel;
	ComboBox gaitParam_Current;
	Label gaitParam_Current_Label;
	ComboBox audioParam_Current;
	Label audioParam_Current_Label;

	void configure()
	{
		// STANDBY
		isStandbyToggle.setToggleState(false, dontSendNotification);
		isStandbyToggleLabel.setText("Standby OFF", dontSendNotification);
		isStandbyToggleLabel.setColour(isStandbyToggleLabel.textColourId, Colours::black);
		isStandbyToggleLabel.setColour(isStandbyToggleLabel.backgroundColourId, Colours::green);
		isStandbyToggleLabel.attachToComponent(&isStandbyToggle, true);

		// DESIRED PARAM BEHAVIOR
		desiredBehavior.addItem("Less Than", 1);
		desiredBehavior.addItem("Equal To", 2);
		desiredBehavior.addItem("Greater Than", 3);
		desiredBehavior.setSelectedId(1);
		desiredBehavior.setColour(desiredBehavior.backgroundColourId, Colours::green);

		// RECORD GAIT PARAM
		recordGaitParam.setButtonText("Rec Gait Param");
		recordGaitParam.setColour(recordGaitParam.buttonColourId, Colours::red);

		// TARGET SETTER
		gaitParam_setTarget.setRange(0, 1);
		gaitParam_setTarget.setNumDecimalPlacesToDisplay(2);
		gaitParam_setTarget.setValue(0);
		gaitParam_setTarget.setColour(gaitParam_setTarget.backgroundColourId, Colours::green);

		// SONIFICATION MODE: SLIDER / SENSOR

		soni_isSliderSource.setToggleState(true, dontSendNotification);
		soni_isSliderSource_Label.setText("Source: Slider", dontSendNotification);
		soni_isSliderSource_Label.setColour(soni_isSliderSource_Label.backgroundColourId, Colours::red);
		soni_isSliderSource_Label.attachToComponent(&soni_isSliderSource, true);

		// SLIDER - SONIFICATION SOURCE

		soni_sliderSource.setRange(0, 1);
		soni_sliderSource.setNumDecimalPlacesToDisplay(2);
		soni_sliderSource.setValue(0);
		soni_sliderSource.setColour(soni_sliderSource.backgroundColourId, Colours::red);
		soni_sliderSource_Label.setText("Manual Distance", dontSendNotification);
		soni_sliderSource_Label.attachToComponent(&soni_sliderSource, true);

		// EXERCISE MODE
		exerciseMode.setColour(exerciseMode.backgroundColourId, Colours::red);
		exerciseModeLabel.setText("Exercise Mode", dontSendNotification);
		exerciseModeLabel.attachToComponent(&exerciseMode, true);

		//Enable Dynamic Target
		enable_dynTarget.setToggleState(false, dontSendNotification);
		enable_dynTarget_Label.setText("Enable Dynamic Target", dontSendNotification);
		enable_dynTarget_Label.attachToComponent(&enable_dynTarget, true);

		//Enable Real Time Visualize
		realTimeVisualize.setToggleState(true, dontSendNotification);
		realTimeVisualize_Label.setText("Visualize Performance", dontSendNotification);
		realTimeVisualize_Label.attachToComponent(&realTimeVisualize, true);
	}

	void toggleVisible(bool on)
	{
		isStandbyToggle.setVisible(on);
		gaitParam_CurrentValue.setVisible(on);
		recordGaitParam.setVisible(on);
		desiredBehavior.setVisible(on);
		gaitParam_DesiredBehavior_Label.setVisible(on);
		gaitParam_targetValue.setVisible(on);
		gaitParam_setTarget.setVisible(on);
		soni_isSliderSource.setVisible(on);
		soni_sliderSource.setVisible(on);
		soni_isMusical_Label.setVisible(on);
		soni_isSliderSource_Label.setVisible(on);
		soni_sliderSource_Label.setVisible(on);
		exerciseMode.setVisible(on);
		gaitParam_Current.setVisible(on);
		audioParam_Current.setVisible(on);
		enable_dynTarget.setVisible(on);
		enable_dynTarget_Label.setVisible(on);
		realTimeVisualize.setVisible(on);
	}

	void setLayout()
	{
		enable_dynTarget.setBounds(120, 260, 150, 40);
		isStandbyToggle.setBounds(1050, 50, 150, 30);
		gaitParam_Current.setBounds(120, 50, 200, 40);
		recordGaitParam.setBounds(350, 50, 200, 40);
		gaitParam_CurrentValue.setBounds(120, 100, 150, 40);
		desiredBehavior.setBounds(120, 140, 200, 40);
		gaitParam_targetValue.setBounds(120, 180, 150, 40);
		gaitParam_setTarget.setBounds(120, 220, 200, 40);
		audioParam_Current.setBounds(650, 50, 200, 40);
		soni_isSliderSource.setBounds(750, 130, 150, 40);
		soni_sliderSource.setBounds(650, 170, 200, 40);
		exerciseMode.setBounds(650, 220, 200, 40);
		realTimeVisualize.setBounds(120, 550, 100, 30);
	}
};

