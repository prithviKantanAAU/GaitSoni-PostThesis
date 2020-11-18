#pragma once
class SensorInfo
{
public:
	SensorInfo() {};
	~SensorInfo() {};

	short numSensorsMax = 3;
	float IMU_SampleRate = 100;
	int UDP_Ports[5] = { 9999, 9998, 9997 };
	int UDP_Ports_REMOTE[5] = {8888, 8887, 8886};
	OSCSender connectionVerify_IP[3];
	std::string sensors_OSCAddress[3] = { "/S1","/S2","/S3" };
	short bodyLocation[5] = { 4, 4, 4 };
	bool isOnline[5] = { false, false, false };
	int batteryPercent[5] = { 0 };
	String remoteIP[5] = { "" };

	void updateStatus(int port, bool status)
	{
		short sensorIndex = -1;
		for (int i = 0; i < numSensorsMax; i++)
		{
			if (port == UDP_Ports[i])
				sensorIndex = i;
		}
		isOnline[sensorIndex] = status;
	};

	bool check_isAssignedAndOnline_TrunkSensor()
	{
		for (int i = 0; i < numSensorsMax; i++)
		{
			if (bodyLocation[i] == 1 && isOnline[i])
				return true;
		}
		return false;
	};
	bool check_isAssignedAndOnline_FootSensors()
	{
		bool isAssignedAndOnline_L = false;
		bool isAssignedAndOnline_R = false;

		for (int i = 0; i < numSensorsMax; i++)
		{
			isAssignedAndOnline_L = (bodyLocation[i] == 2 && isOnline[i]) ? true : isAssignedAndOnline_L;
			isAssignedAndOnline_R = (bodyLocation[i] == 3 && isOnline[i]) ? true : isAssignedAndOnline_R;
		}
		return (isAssignedAndOnline_L && isAssignedAndOnline_R) ? true : false;
	}

	bool check_isAssignedAndOnline_JointSensors()
	{
		bool isAssignedAndOnline_Upper = false;
		bool isAssignedAndOnline_Lower = false;

		for (int i = 0; i < numSensorsMax; i++)
		{
			isAssignedAndOnline_Upper = (bodyLocation[i] == 5 && isOnline[i]) ? true : isAssignedAndOnline_Upper;
			isAssignedAndOnline_Lower = (bodyLocation[i] == 6 && isOnline[i]) ? true : isAssignedAndOnline_Lower;
		}
		return (isAssignedAndOnline_Upper && isAssignedAndOnline_Lower) ? true : false;
	}
};

