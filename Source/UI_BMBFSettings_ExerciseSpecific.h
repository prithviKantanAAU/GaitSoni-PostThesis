#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class UI_BMBFSettings_ExerciseSpecific
{
public:
	UI_BMBFSettings_ExerciseSpecific() 
	{ 
		configure();
	};
	~UI_BMBFSettings_ExerciseSpecific() {};

	Slider staticBalance_Div_Roll;	
	Label staticBalance_Div_Roll_Label;
	Slider staticBalance_Div_Pitch; 
	Label staticBalance_Div_Pitch_Label;
	Slider staticBalance_Ctrl_X;
	Slider staticBalance_Ctrl_Y;
	ComboBox staticBalance_FeedbackSlope; 
	Label staticBalance_FeedbackSlope_Label;
	ComboBox staticBalance_calibrationMode;

	ComboBox HS_TimingMode;
	Slider HS_Tolerance;
	Label HS_Tolerance_Label;
	Slider HS_AccThresh;
	Label HS_AccThresh_Label;

	Slider sitStand_Thresh_Stand;
	Label sitStand_Thresh_Stand_Label;
	Slider sitStand_Thresh_Sit;
	Label sitStand_Thresh_Sit_Label;
	TextButton sitStand_FlipState;
	Label sitStand_isStanding;

	void configure()
	{
		//Trunk Balance - Center X
		staticBalance_Ctrl_X.setValue(0);
		staticBalance_Ctrl_X.setRange(-20, 20);
		staticBalance_Ctrl_X.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, 10, 10);

		//Trunk Balance - Center Y
		staticBalance_Ctrl_Y.setValue(0);
		staticBalance_Ctrl_Y.setSliderStyle(Slider::SliderStyle::LinearVertical);
		staticBalance_Ctrl_Y.setRange(-20, 20);
		staticBalance_Ctrl_Y.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, 10, 10);
		
		//Trunk Balance - Zone Width
		staticBalance_Div_Roll.setValue(2);
		staticBalance_Div_Roll.setRange(2, 10);
		staticBalance_Div_Roll.setColour(staticBalance_Div_Roll.trackColourId, Colours::green);
		staticBalance_Div_Roll.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 10, 10);

		//Trunk Balance - Zone Height
		staticBalance_Div_Pitch.setValue(8);
		staticBalance_Div_Pitch.setRange(0, 8);
		staticBalance_Div_Pitch.setColour(staticBalance_Div_Pitch.backgroundColourId, Colours::green);
		staticBalance_Div_Pitch.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		staticBalance_Div_Pitch.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 10, 10);

		//Trunk Balance - Feedback Slope
		staticBalance_FeedbackSlope.addItem("Gradual", 1);
		staticBalance_FeedbackSlope.addItem("Medium", 2);
		staticBalance_FeedbackSlope.addItem("Steep", 3);
		staticBalance_FeedbackSlope.setSelectedId(1);
		staticBalance_FeedbackSlope_Label.setText("Change Rate", dontSendNotification);
		staticBalance_FeedbackSlope_Label.attachToComponent(&staticBalance_FeedbackSlope, false);

		//Trunk Balance - Calibration Mode
		staticBalance_calibrationMode.addItem("Calibrate Ideal", 1);
		staticBalance_calibrationMode.addItem("Calibrate Extreme", 2);
		staticBalance_calibrationMode.setSelectedId(1);

		// SST - Stand Angle Thresh
		sitStand_Thresh_Stand.setValue(15);
		sitStand_Thresh_Stand.setRange(5, 30);
		sitStand_Thresh_Stand.setNumDecimalPlacesToDisplay(1);
		sitStand_Thresh_Stand_Label.setText("Pre-Stand Angle", dontSendNotification);
		sitStand_Thresh_Stand_Label.attachToComponent(&sitStand_Thresh_Stand, false);
		
		// SST - Sit Angle Thresh
		sitStand_Thresh_Sit.setValue(10);
		sitStand_Thresh_Sit.setRange(5, 30);
		sitStand_Thresh_Sit.setNumDecimalPlacesToDisplay(1);
		sitStand_Thresh_Sit_Label.setText("Pre-Sit Angle", dontSendNotification);
		sitStand_Thresh_Sit_Label.attachToComponent(&sitStand_Thresh_Sit, false);

		// SST - Flip State
		sitStand_FlipState.setButtonText("Flip State");
		sitStand_FlipState.setVisible(false);

		// SST - Is Standing Label
		String isStandingText = "Sitting";
		sitStand_isStanding.setText(isStandingText, dontSendNotification);

		// HS - Timing Mode
		HS_TimingMode.addItem("Standard Time", 1);
		HS_TimingMode.addItem("Half Time", 2);
		HS_TimingMode.setSelectedId(1);

		// HS - Timing Tolerance
		HS_Tolerance.setValue(0, dontSendNotification);
		HS_Tolerance.setRange(0, 1);
		HS_Tolerance.setNumDecimalPlacesToDisplay(2);
		HS_Tolerance_Label.setText("Error Tolerance", dontSendNotification);
		HS_Tolerance_Label.attachToComponent(&HS_Tolerance, false);

		// HS - Acc Thresh
		HS_AccThresh.setValue(1.84, dontSendNotification);
		HS_AccThresh.setRange(0, 5);
		HS_AccThresh.setNumDecimalPlacesToDisplay(2);
		HS_AccThresh_Label.setText("HS ACC Threshold", dontSendNotification);
		HS_AccThresh_Label.attachToComponent(&HS_AccThresh, false);
	}

	void toggleVisible(short exMode, bool soniTab)
	{
		bool isTesting = (exMode == 1);
		bool isSB_orDB = (exMode == 2 || exMode == 3);
		bool isSTS = (exMode == 4 || exMode == 5);
		bool isGait = (exMode == 6);

		// SB DB
		staticBalance_Div_Roll.setVisible(soniTab && isSB_orDB);
		staticBalance_Div_Pitch.setVisible(soniTab && isSB_orDB);
		staticBalance_FeedbackSlope.setVisible(soniTab && isSB_orDB);
		staticBalance_calibrationMode.setVisible(soniTab && isSB_orDB);
		staticBalance_Ctrl_X.setVisible(soniTab && isSB_orDB);
		staticBalance_Ctrl_Y.setVisible(soniTab && isSB_orDB);

		// STS
		sitStand_Thresh_Stand.setVisible(soniTab && isSTS);
		sitStand_Thresh_Sit.setVisible(soniTab && isSTS);
		sitStand_FlipState.setVisible(soniTab && isSTS);
		sitStand_isStanding.setVisible(soniTab && isSTS);

		// Gait
		HS_TimingMode.setVisible(soniTab && isGait);
		HS_Tolerance.setVisible(soniTab && isGait);
		HS_AccThresh.setVisible(soniTab && isGait);
	}

	void setLayout()
	{
		staticBalance_FeedbackSlope.setBounds(50, 110, 200, 40);
		staticBalance_calibrationMode.setBounds(50, 190, 200, 40);
		staticBalance_Ctrl_X.setBounds(950, 310, 200, 20);
		staticBalance_Ctrl_Y.setBounds(930, 330, 20, 200);
		staticBalance_Div_Pitch.setBounds(920, 330, 20, 100);
		staticBalance_Div_Roll.setBounds(950, 300, 100, 20);

		sitStand_Thresh_Sit.setBounds(50, 110, 200, 40);
		sitStand_Thresh_Stand.setBounds(50, 190, 200, 40);
		sitStand_FlipState.setBounds(50, 270, 200, 40);
		sitStand_isStanding.setBounds(50, 350, 200, 40);

		HS_TimingMode.setBounds(50, 110, 200, 40);
		HS_Tolerance.setBounds(50, 190, 200, 40);
		HS_AccThresh.setBounds(50, 270, 200, 40);
	}
};

