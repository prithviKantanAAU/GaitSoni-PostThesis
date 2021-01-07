#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "BiQuad.h"
#include "medianFilter.h"

class OSCReceiverUDP_Sensor : public Component,
	private OSCReceiver,
	private OSCReceiver::ListenerWithOSCAddress<OSCReceiver::MessageLoopCallback>
{
public:
	bool connectionStatus = false;
	int portNumber = 0;
	MedianFilter medFilter_ACC[3];
	MedianFilter medFilter_GYR[3];
	MedianFilter medFilter_MAG[3];
	
	// DATA BUFFERS
	float oscDataArray[10] = { 0.0 };
	short messageSize = 10;
	float acc[3] = { 0.0 }; float acc_Buf[3] = { 0.0 };
	float gyr[3] = { 0.0 }; float gyr_Buf[3] = { 0.0 };
	float mag[3] = { 0.0 }; float mag_Buf[3] = { 0.0 };
	
	// BATTERY LEVEL
	float level_Battery = 0.0;
	
	// MESSAGE COUNT FOR ONLINE STATUS
	int messageCount_Recvd = 0;
	int messageCount_Recvd_z1 = 0;
	short messageCount_Recvd_smpl_z0 = 0;
	short messageCount_Recvd_smpl_z1 = 0;
	float isMessageRecvd_smpl_z0 = 0;

	// FILTERING
	float fs = 100;
	BiQuad LPF_Acc[3];
	BiQuad LPF_Gyr[3];
	BiQuad LPF_Mag[3];

	BiQuad LPF_Acc_2[3];
	BiQuad LPF_Gyr_2[3];
	BiQuad LPF_Mag_2[3];

	BiQuad LPF_Acc_3[3];
	BiQuad LPF_Gyr_3[3];
	BiQuad LPF_Mag_3[3];

	float filterFc = 20;
	short oscSampleReceived_BUFFER[200] = { 0 };
	int oscSampleReceived_WriteIdx = 0;
	float oscSampleReceived_Percent = 0;

	OSCReceiverUDP_Sensor()
	{
		initializeFilters();
	}

	~OSCReceiverUDP_Sensor()
	{
		disconnect();
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
			LPF_Mag[i].flushDelays();
			LPF_Mag[i].calculateLPFCoeffs(filterFc, 0.7, fs);

			LPF_Acc_2[i].flushDelays();
			LPF_Acc_2[i].calculateLPFCoeffs(filterFc, 0.7, fs);
			LPF_Gyr_2[i].flushDelays();
			LPF_Gyr_2[i].calculateLPFCoeffs(filterFc, 0.7, fs);
			LPF_Mag_2[i].flushDelays();
			LPF_Mag_2[i].calculateLPFCoeffs(filterFc, 0.7, fs);

			LPF_Acc_3[i].flushDelays();
			LPF_Acc_3[i].calculateLPFCoeffs(filterFc, 0.7, fs);
			LPF_Gyr_3[i].flushDelays();
			LPF_Gyr_3[i].calculateLPFCoeffs(filterFc, 0.7, fs);
			LPF_Mag_3[i].flushDelays();
			LPF_Mag_3[i].calculateLPFCoeffs(filterFc, 0.7, fs);
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

	void set_medianFilter_N(int n)
	{
		for (int i = 0; i < 3; i++)
		{
			medFilter_ACC[i].filterLength = n;
			medFilter_GYR[i].filterLength = n;
			medFilter_MAG[i].filterLength = n;
		}
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

	void updateBuffers()
	{
		for (int i = 0; i < 3; i++)
		{
			acc[i] = medFilter_ACC[i].doFiltering(acc[i]);
			gyr[i] = medFilter_GYR[i].doFiltering(gyr[i]);
			mag[i] = medFilter_MAG[i].doFiltering(mag[i]);

			// 6th ORDER FILTERING
			// First 2 degrees
			acc_Buf[i] = LPF_Acc[i].doBiQuad(acc[i], 0);
			gyr_Buf[i] = LPF_Gyr[i].doBiQuad(gyr[i], 0);
			mag_Buf[i] = LPF_Mag[i].doBiQuad(mag[i], 0);

			// Next 2 degrees
			acc_Buf[i] = LPF_Acc_2[i].doBiQuad(acc_Buf[i], 0);
			gyr_Buf[i] = LPF_Gyr_2[i].doBiQuad(gyr_Buf[i], 0);
			mag_Buf[i] = LPF_Mag_2[i].doBiQuad(mag_Buf[i], 0);

			// Last 2 degrees
			acc_Buf[i] = LPF_Acc_3[i].doBiQuad(acc_Buf[i], 0);
			gyr_Buf[i] = LPF_Gyr_3[i].doBiQuad(gyr_Buf[i], 0);
			mag_Buf[i] = LPF_Mag_3[i].doBiQuad(mag_Buf[i], 0);
		}
		updateBias(acc_Buf, gyr_Buf);
		compensateBias(acc_Buf, gyr_Buf, mag_Buf);

		messageCount_Recvd_smpl_z0 = messageCount_Recvd;
		isMessageRecvd_smpl_z0 = (messageCount_Recvd_smpl_z0 > messageCount_Recvd_smpl_z1) ? 1.0 : 0.0;
		messageCount_Recvd_smpl_z1 = messageCount_Recvd_smpl_z0;

		// Calculate Short Term (2 Sec) Average Packet Received Percent
		oscSampleReceived_BUFFER[oscSampleReceived_WriteIdx] = (short)isMessageRecvd_smpl_z0;
		oscSampleReceived_WriteIdx = (oscSampleReceived_WriteIdx + 1) % 200;
		oscSampleReceived_Percent = 0;
		for (int i = 0; i < 200; i++)
			oscSampleReceived_Percent += oscSampleReceived_BUFFER[i];
		oscSampleReceived_Percent /= 2.0;
	}

	void oscMessageReceived(const OSCMessage& message) override
	{
		messageCount_Recvd++;
		if (message.size() == messageSize)
		{
			for (int i = 0; i < message.size(); i++)
			{
				if (message[i].isFloat32())
				{
					oscDataArray[i] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
					switch (i)
					{
					case 0:
						acc[0] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
						break;
					case 1:
						acc[1] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
						break;
					case 2:
						acc[2] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
						break;
					case 3:
						gyr[0] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
						break;
					case 4:
						gyr[1] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
						break;
					case 5:
						gyr[2] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
						break;
					case 6:
						level_Battery = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
						break;
					case 7:
						mag[0] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
						break;
					case 8:
						mag[1] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
						break;
					case 9:
						mag[2] = jlimit(-10000.0f, 10000.0f, message[i].getFloat32());
						break;
					default:
						break;
					}
				}
			}
		}
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
	double maxs_mag[3] = { 0.0, 0.0, 0.0 };
	double mins_mag[3] = { 0.0, 0.0, 0.0 };
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

	void compensateBias(float *accBuf, float *gyrBuf, float *magBuf)
	{
		float mag_offset[3] = { 0.0, 0.0, 0.0 };
		float mag_delta[3] = { 0.0, 0.0, 0.0 };
		float scale[3] = { 0.0, 0.0, 0.0 };

		mag_offset[0] = (maxs_mag[0] + mins_mag[0]) / 2;
		mag_offset[1] = (maxs_mag[1] + mins_mag[1]) / 2;
		mag_offset[2] = (maxs_mag[2] + mins_mag[2]) / 2;

		mag_delta[0] = (maxs_mag[0] - mins_mag[0]) / 2;
		mag_delta[1] = (maxs_mag[1] - mins_mag[1]) / 2;
		mag_delta[2] = (maxs_mag[2] - mins_mag[2]) / 2;
		
		float mag_delta_avg = (mag_delta[0] + mag_delta[1] + mag_delta[2])/3.0;

		for (int i = 0; i < 3; i++)
		{
			accBuf[i] -= bias_mean_acc[i];
			gyrBuf[i] -= bias_mean_gyr[i];

			if (magBuf[i] > maxs_mag[i])	maxs_mag[i] = magBuf[i];
			if (magBuf[i] < mins_mag[i])	mins_mag[i] = magBuf[i];

			magBuf[i] -= mag_offset[i];

			scale[i] = mag_delta_avg / mag_delta[i];

			magBuf[i] *= scale[i];
		}
	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSCReceiverUDP_Sensor)
};


