#pragma once
#include "OSC_Class.h"

class RemoteControl
{
public:
	RemoteControl() 
	{
		inputReceiver.setupPortAndListener(port, OSCAddress);
		inputReceiver.setSampleRate(Fs);
		inputReceiver.setMessageSize(numControls);
	};
	~RemoteControl() 
	{
	};

	OSCReceiverUDP_Sensor inputReceiver;
	int Fs = 25;
	int port = 9998;
	float ceiling = 4096;
	std::string OSCAddress = "/Remote";
	int numControls = 6;
	int numButtons = 3;
	int numKnobs = 3;
	float lastValues[15] = { 0.0 };
	float currentValues[15] = { 0.0 };

	void checkInputChange(bool *changeArray)
	{
		for (int i = 0; i < numControls; i++)
		{
			currentValues[i] = inputReceiver.getRemoteValue(i);
			if (i < numButtons)
			{
				currentValues[i] = applySwitchThresh(ceiling / 2, currentValues[i]);
				if (currentValues[i] == 1 && lastValues[i] == 0)
					changeArray[i] = true;
				else
					changeArray[i] = false;
			}

			else
			{
				//currentValues[i] = applyKnobQuantize(75, currentValues[i]);

				if (fabs(currentValues[i] - lastValues[i]) > 10 && lastValues[i] != 0)
					changeArray[i] = true;
				else
					changeArray[i] = false;
			}
		}
		shuffleLastValues();
	}

	void shuffleLastValues()
	{
		for (int i = 0; i < numControls; i++)
		{
			lastValues[i] = currentValues[i];
		}
	}

	float getMappingValue(float lowerLim, float upperLim, float remoteVal)
	{
		return lowerLim + (upperLim - lowerLim) * remoteVal / ceiling;
	}

	float applySwitchThresh(float thresh, float input)
	{	return input > thresh ? 1 : 0;	}
};

