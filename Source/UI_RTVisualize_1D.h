#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class UI_RTVisualize_1D
{
public:
	UI_RTVisualize_1D()
	{
		addElements();
		configure();
	};
	~UI_RTVisualize_1D() {};

	Label realTimeVisualize_Label;
	Label rtv_minBound;		Label rtv_maxBound;
	Label rtv_fullRangeBar;
	Label rtv_currentValue;
	Label rtv_targetRange;
	int rtv_startX = 50;
	int rtv_width = 600;
	int rtv_ht = 50;
	int rtv_startY = 550;

	void addElements()
	{
	}

	void configure()
	{
	}

	void configureBounds(float minValue, float maxValue)
	{
		rtv_minBound.setText(std::to_string((int)minValue), dontSendNotification);
		rtv_maxBound.setText(std::to_string((int)maxValue), dontSendNotification);
		rtv_fullRangeBar.setColour(rtv_fullRangeBar.backgroundColourId, Colours::grey);
		rtv_targetRange.setColour(rtv_targetRange.backgroundColourId, Colours::green);
		rtv_targetRange.setText("T", dontSendNotification);
		rtv_targetRange.setJustificationType(Justification::centred);
		rtv_currentValue.setColour(rtv_currentValue.backgroundColourId, Colours::red);
		rtv_currentValue.setText("C", dontSendNotification);
		rtv_currentValue.setJustificationType(Justification::centred);
	}

	void toggleVisible(bool on)
	{
		rtv_minBound.setVisible(on);
		rtv_maxBound.setVisible(on);
		rtv_currentValue.setVisible(on);
		rtv_targetRange.setVisible(on);
		rtv_fullRangeBar.setVisible(on);
	}

	void setLayout()
	{
		rtv_minBound.setBounds(rtv_startX, rtv_startY, 100, 30);
		rtv_maxBound.setBounds(rtv_startX + rtv_width, rtv_startY, 100, 30);
		rtv_targetRange.setBounds(rtv_startX, rtv_startY - 40 - rtv_ht / 2, 100, rtv_ht);
		rtv_currentValue.setBounds(rtv_startX + 20, rtv_startY - 40 + rtv_ht / 2, 40, rtv_ht);
	}
};

