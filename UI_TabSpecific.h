#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class UI_TabSpecific
{
public:
	UI_TabSpecific()
	{
		configure();
	};
	~UI_TabSpecific() {};

	Label status_SensorConnection;					// Shows connection status

	void configure()
	{

	}

	void toggleVisible(bool on)
	{
	}

	void setLayout()
	{
		status_SensorConnection.setBounds(1000, 550, 200, 20);
	}
};
