#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class UI_SensorConfig
{
public:
	UI_SensorConfig()
	{
		configure();
	};
	~UI_SensorConfig() {};

	String Locations[4] = { "Trunk", "Left Foot", "Right Foot", "<UNASSIGNED>" };
	Label SrNo_Header;					// Column Header - Sr. No
	Label SrNo[3];						// Column Values - Sr. No
	Label Status_Header;				// Column Header - Online status
	Label Status[3];					// Column Values - Online status
	Label Port_Header;					// Column Header - UDP Port
	Label Port[3];						// Column Values - UDP Port
	Label Location_Header;				// Column Header - Sensor Bodily Location
	Label BiasComp_Header;				// Column Header - IMU Bias Compensation
	TextButton BiasComp[3];				// Column Button - IMU Bias Compensation
	ComboBox Location[3];				// Column Values - Sensor Bodily Location	

	void addElements()
	{

	}

	void configure()
	{
		SrNo_Header.setText("Serial Num", dontSendNotification);
		Status_Header.setText("Status", dontSendNotification);
		Port_Header.setText("UDP Port", dontSendNotification);
		Location_Header.setText("Body Location", dontSendNotification);
		BiasComp_Header.setText("Bias Compensation", dontSendNotification);
		for (int i = 0; i < 3; i++)
		{
			SrNo[i].setText(String(i + 1), dontSendNotification);

			Location[i].addItem(Locations[0], 1);
			Location[i].addItem(Locations[1], 2);
			Location[i].addItem(Locations[2], 3);
			Location[i].addItem(Locations[3], 4);
			Location[i].setSelectedId(4);						
			
			BiasComp[i].setButtonText("Calibrate");				
			BiasComp[i].setColour(BiasComp[i].buttonColourId, Colours::red);
		}
	}

	void toggleVisible(bool on)
	{
		SrNo_Header.setVisible(on);
		Status_Header.setVisible(on);
		Location_Header.setVisible(on);
		Port_Header.setVisible(on);
		BiasComp_Header.setVisible(on);
		for (int i = 0; i < 3; i++)
		{
			Location[i].setVisible(on);
			Port[i].setVisible(on);
			SrNo[i].setVisible(on);
			Status[i].setVisible(on);
			BiasComp[i].setVisible(on);
		}
	}

	void setLayout()
	{
		SrNo_Header.setBounds(100, 400, 100, 40);
		Status_Header.setBounds(200, 400, 100, 40);
		Port_Header.setBounds(300, 400, 100, 40);
		Location_Header.setBounds(400, 400, 100, 40);
		BiasComp_Header.setBounds(550, 400, 200, 40);

		for (int i = 0; i < 3; i++)
		{
			SrNo[i].setBounds(100, 400 + 50 * (i + 1), 100, 40);
			Status[i].setBounds(200, 400 + 50 * (i + 1), 100, 40);
			Port[i].setBounds(300, 400 + 50 * (i + 1), 100, 40);
			Location[i].setBounds(400, 400 + 50 * (i + 1), 100, 40);
			BiasComp[i].setBounds(550, 400 + 50 * (i + 1), 200, 40);
		}
	}
};

