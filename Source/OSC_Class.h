/*
  ==============================================================================

   This file is part of the JUCE examples.
   Copyright (c) 2017 - ROLI Ltd.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             OSCDemo
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Application using the OSC protocol.

 dependencies:     juce_core, juce_data_structures, juce_events, juce_graphics,
				   juce_gui_basics, juce_osc
 exporters:        xcode_mac, vs2017, linux_make

 type:             Component
 mainClass:        OSCDemo

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "BiQuad.h"


//==============================================================================

//==============================================================================
class OSCReceiverUDP_Sensor : public Component,
	private OSCReceiver,
	private OSCReceiver::ListenerWithOSCAddress<OSCReceiver::MessageLoopCallback>
{
public:
	//==============================================================================
	bool connectionStatus = false;
	int portNumber = 0;
	std::string oscAddress = "";
	float oscDataArray[10] = { 0.0 };
	bool messageReady = false;
	short messageSize = 6;
	float acc[3] = { 0.0 }; float acc_Buf[3] = { 0.0 };
	float gyr[3] = { 0.0 }; float gyr_Buf[3] = { 0.0 };
	int messageCount_Recvd = 0;
	int messageCount_Recvd_z1 = 0;
	short messageCount_Recvd_smpl_z0 = 0;
	short messageCount_Recvd_smpl_z1 = 0;
	float isMessageRecvd_smpl_z0 = 0;

	float fs = 100;
	BiQuad LPF_Acc[3];
	BiQuad LPF_Gyr[3];
	float filterFc = 20;
	//Modes: 0 - trunk inclination
	short sensorMode = 0;

	OSCReceiverUDP_Sensor()
	{
		initializeFilters();
	}

	~OSCReceiverUDP_Sensor()
	{
		disconnect();
	}

	void setSensorMode(short newMode)
	{
		sensorMode = newMode;
	}

	void setSampleRate(float value)
	{
		fs = value;
		initializeFilters();
	}

	void initializeFilters()
	{
		for (int i = 0; i < 3; i++)
		{
			LPF_Acc[i].flushDelays();
			LPF_Acc[i].calculateLPFCoeffs(filterFc, 0.7,fs);
			LPF_Gyr[i].flushDelays();
			LPF_Gyr[i].calculateLPFCoeffs(filterFc, 0.7,fs);
		}
	}

	void setupPortAndListener(int localPort, std::string address)
	{
		portNumber = localPort;
		connectAndCheckStatus();
		addListener(this, address.c_str());
	}
	
	void connectAndCheckStatus()
	{
		connectionStatus = (connect(portNumber)) ? true : false;
	}

	void setMessageSize(short size)
	{
		messageSize = size;
	}

	void refreshConnection()
	{
		disconnect();
		connect(portNumber);
	}

	float getRemoteValue(int index)
	{
		return oscDataArray[index];
	}

	float getSensorValue(int index)
	{
		if (index < 3)
			return acc[index % 3];
		else return gyr[index % 3];
	}

	void updateBuffers()
	{
		for (int i = 0; i < 3; i++)
		{
			acc_Buf[i] = LPF_Acc[i].doBiQuad(acc[i], 0);
			gyr_Buf[i] = LPF_Gyr[i].doBiQuad(gyr[i], 0);
		}
		updateBias(acc_Buf, gyr_Buf);
		compensateBias(acc_Buf, gyr_Buf);

		messageCount_Recvd_smpl_z0 = messageCount_Recvd;
		isMessageRecvd_smpl_z0 = (messageCount_Recvd_smpl_z0 > messageCount_Recvd_smpl_z1) ? 1.0 : 0.0;
		messageCount_Recvd_smpl_z1 = messageCount_Recvd_smpl_z0;
	}

	//==============================================================================
	void oscMessageReceived(const OSCMessage& message) override
	{
		messageReady = false;
		messageCount_Recvd++;
		auto it = message.begin();

		for (int i = 0; i < message.size(); i++)
		{
			if (message.size() == 6 && message[i].isFloat32())
				if (message.size() == messageSize && message[i].isFloat32())
				{
					oscDataArray[i] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
					if (i < 3)
					{
						acc[i % 3] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
					}
					else
					{
						gyr[i % 3] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
					}
				}
		}
		messageReady = true;
	}

	bool isSensorActive()
	{
		if (messageCount_Recvd > messageCount_Recvd_z1)
		{
			messageCount_Recvd_z1 = messageCount_Recvd;
			return true;
		}
		return false;
	};

	void showConnectionErrorMessage(const String& messageText)
	{
		AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,"Connection error",messageText,	"OK");
	}

	double bias_mean_acc[3] = { 0.0 };
	double bias_mean_gyr[3] = { 0.0 };
	double rest_mean_acc[3] = { 0.0, 0.0, -1.0 };
	double rest_mean_gyr[3] = { 0.0, 0.0, 0.0 };
	float biasComp_avgInterval_sec = 10;
	float biasComp_avgInterval_smpl = biasComp_avgInterval_sec * fs;
	int biasComp_elapsedInterval_smpl = 0;
	bool isBiasComp_ON = false;
	bool isBiasComp_DONE = false;

	double updateMean(double oldMean, int count, float newVal)
	{
		return (oldMean*count + newVal) / (count + 1);
	};

	void updateBias(float *accBuf, float *gyrBuf)
	{
		if (isBiasComp_ON)
		{
			if (biasComp_elapsedInterval_smpl < biasComp_avgInterval_smpl)		//ONGOING
			{
				for (int i = 0; i < 3; i++)
				{
					bias_mean_acc[i] = updateMean(bias_mean_acc[i], biasComp_elapsedInterval_smpl, accBuf[i]);
					bias_mean_gyr[i] = updateMean(bias_mean_gyr[i], biasComp_elapsedInterval_smpl, gyrBuf[i]);
				}
				biasComp_elapsedInterval_smpl++;
			}
			else
			{
				for (int i = 0; i < 3; i++)
					bias_mean_acc[i] -= -rest_mean_acc[i];

				isBiasComp_DONE = true;
				isBiasComp_ON = false;
				biasComp_elapsedInterval_smpl = 0;
			}
		}
	};

	void compensateBias(float *accBuf, float *gyrBuf)
	{
		for (int i = 0; i < 3; i++)
		{
			accBuf[i] -= bias_mean_acc[i];
			gyrBuf[i] -= bias_mean_gyr[i];
		}
	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSCReceiverUDP_Sensor)
};


