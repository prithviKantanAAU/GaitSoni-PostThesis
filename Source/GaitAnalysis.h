#pragma once

#include "OSC_Class.h"
#include "BiQuad.h"
#include <math.h>
#include "gaitParamInfo.h"
#include "SensorInfo.h"
#include "HFEN_HPF.h"
#include "../JuceLibraryCode/JuceHeader.h"
# define M_PI           3.14159265358979323846  /* pi */

class GaitAnalysis
{
public:
	GaitAnalysis();
	~GaitAnalysis();

	gaitParamInfo gaitParams;
	SensorInfo sensorInfo;
	OSCReceiverUDP_Sensor sensors_OSCReceivers[3]; //L    R   Trunk 
	HFEN_HPF hfen_trunk;
	HFEN_HPF hfen_foot_L;
	HFEN_HPF hfen_foot_R;
	bool areRequiredSensorsOnline()
	{
		if
			(
				gaitParams.gaitParam_ObjectArray[gaitParams.activeGaitParam].isSensorRequired[0]
				== sensorInfo.check_isAssignedAndOnline_TrunkSensor()
				&&
				gaitParams.gaitParam_ObjectArray[gaitParams.activeGaitParam].isSensorRequired[1]
				== sensorInfo.check_isAssignedAndOnline_FootSensors()
				)
			return true;
		return false;
	};
	void updateSensorStatus()
	{
		for (int i = 0; i < sensorInfo.numSensorsMax; i++)
			sensorInfo.isOnline[i] = sensors_OSCReceivers[i].isSensorActive();
		sensorInfo.check_isAssignedAndOnline_FootSensors();
		sensorInfo.check_isAssignedAndOnline_TrunkSensor();
	};
	short idx_Sensor_Trunk = 0;
	short idx_Sensor_Foot_L = 0;
	short idx_Sensor_Foot_R = 0;
	void setupReceivers();
	int numMsgReceived_Sensor[2] = { 0 };
	void boundValuesAndStore(short gaitParamIndex, float value)
	{
		float minVal = gaitParams.gaitParam_ObjectArray[gaitParamIndex].minVal;
		float maxVal = gaitParams.gaitParam_ObjectArray[gaitParamIndex].maxVal;
		gaitParams.gaitParam_ObjectArray[gaitParamIndex].currentValue = jlimit(minVal, maxVal, value);
	};

	int fs = 100;
	bool fsChecked = false;
	std::string paramName = "";
	void setParamName(std::string name) { paramName = name; };
	int rms_Length = 5;
	float rmsWindow_ML[10] = { 0.0 };
	float rmsWindow_AP[10] = { 0.0 };
	float accX_z1 = 0.0;
	float accY_z1 = 0.0;
	float accZ_z1 = 0.0;
	
	float R_acc_est_z1[3] = { 0.0 };
	float R_acc_est[3] = { 0.0 };
	float R_gyr_z1[3] = { 0.0 };
	bool isCalibrated_Orientation = false;
	float wt_Gyr = 5;
	float staticBalance_Div_Roll = 2;
	float staticBalance_Div_Pitch = 2;
	short staticBalance_ZoneMap_Current = 1;
	float staticBalance_ZoneMaps[3][6] = 
	{
		{0.005,0.38,0.73,0.85,0.93,0.98},
		{0.005,0.73,0.73,0.85,0.93,0.98},
		{0.005,0.85,0.85,0.85,0.93,0.98}
	};

	float staticBalance_BoundsCoordinates[2][2] = { 0.0 }; // R1 - ML AP (rest) R2 - ML AP (extreme)
	short staticBalance_calibrationMode = 0;
	float staticBalance_calibrationLength = 0;
	void staticBalance_calibrateCoordinates()
	{
		float ML_mean_OLD = staticBalance_BoundsCoordinates[staticBalance_calibrationMode][0];
		float AP_mean_OLD = staticBalance_BoundsCoordinates[staticBalance_calibrationMode][1];
		float ML_newVal = gaitParams.gaitParam_ObjectArray[0].currentValue;
		float AP_newVal = gaitParams.gaitParam_ObjectArray[1].currentValue;
		staticBalance_BoundsCoordinates[staticBalance_calibrationMode][0] = updateMean(ML_mean_OLD, staticBalance_calibrationLength, ML_newVal);
		staticBalance_BoundsCoordinates[staticBalance_calibrationMode][1] = updateMean(AP_mean_OLD, staticBalance_calibrationLength, AP_newVal);
		staticBalance_calibrationLength++;
	};

	//HeelStrike
	BiQuad LPF_Gyr_Smooth;
	BiQuad LPF_Gyr_Turn_Smooth;
	float beatInterval = 0.5;
	float strideInterval = 1.0;
	bool isHalfTime = false;
	bool isSimulation = false;
	float HS_IntervalTolerance = 0;
	float HS_Sim_Freq = 2;
	float timeElapsed = 0;
	float timeStamp_HS_Last = 0;
	float timeStamp_HS_Next_Exp = 0;
	float timeStamp_HS_Last_Exp = 0;
	float timeExpected_NextStep = 0;
	bool sim_HS = false;
	bool isEarly = false;
	bool isLate = false;
	float HSdelayOffset_ms = 0; 
	void set_HSdelayOffset_ms(float value) { HSdelayOffset_ms = value; tempoDependentDelay_Setup(); };
	float triggerBuffer[20] = { 0.0 };
	float delayBuffer[100] = { 0.0 };
	int delayLength = 100;
	int readIndex = 0;
	int writeIndex = 0;
	int readWriteDiff_Samples = 0;

	//HS Accelerometer
	float HS_thresh_pos = 1.84;
	float HS_thresh_neg = 0.04;
	float HS_timeOut = 0.1;
	float HS_timeDone = 0;
	bool HS_isExpected_L = true;
	bool HS_isExpected_R = true;
	bool detectHS_acc(float *accBuf, bool isLeft);
	void drumTrigger(float *accBuf_L, float *accBuf_R, bool isCalibrating);
	float norm_acc_z1 = 0.0;
	bool HS_accNorm_crossed_pos = false;
	bool HS_accNorm_crossed_neg = true;
	bool isThreshCross(float input, float thresh, float input_z1, bool isUp)
	{
		bool output = false;
		if (isUp)
		{
			if (input > thresh && input_z1 <= thresh)
				output = true;
		}
		else
		{
			if (input < thresh && input_z1 >= thresh)
				output = true;
		}
		return output;
	};
	void calibrateRest(float *accBuf);
	void setSampleRate(int fs_ext);
	float applyHFEN_PreProcess(float inputSample, char axis);
	void getOrientation_Fused(float *accBuf, float *gyrBuf);
	void getProjection_ML_AP();
	void getRMS_Acc(float *accBuf);

	void getJerkParams(float *accBuf);
	void getSwayVelocity(float *gyrBuf);
	void getSitStandCueFeature();
	bool sitStand_feedbackMode = true;
	BiQuad smooth_Angle_AP;
	bool sitStand_isStanding = false;
	bool sitStand_isStabilized = true;
	float sitStand_Thresh_Stand = 15;
	float sitStand_Thresh_Sit = 10;
	float sitStand_Thresh_Current = 15;
	void sitStand_FlipState()
	{
		gaitParams.gaitParam_ObjectArray[13].currentValue = 
			gaitParams.gaitParam_ObjectArray[13].currentValue > 0.5 ? 0 : 1;
		sitStand_isStanding = sitStand_isStanding ? false : true;
		sitStand_isStabilized = true;
	};

	void calcTimingPerformanceFeature(float *gyrBuf, bool isCalibrating);
	void calcTimingPerformanceFeature_2(float *accBuf_L, float *accBuf_R, float *gyrBuf, bool isCalibrating);
	void detectHS_Instants(float *gyrBuf, bool isCalibrating);
	float tempoDependentDelay(float input);
	void tempoDependentDelay_Setup();
	void compute(short currentGaitParam, bool isCalibrating);

	//Calibration
	float calibrationValues[30] = { 0.0 };
	float calibrationValues_Temp[30] = { 0.0 };
	float param_Z1 = 0.0;
	short calibration_stepCount = 0;
	float hs_Intervals[5] = { 0.0 };
	void updateHS_Calibration(float strideDuration_Latest);
	float strideDur_Mean = 0;
	float strideDur_COV = 0;
	bool isParamCalibrated[30] = { false };
	void calibrateMaximum(short activeGaitParam)
	{
		if (gaitParams.gaitParam_ObjectArray[activeGaitParam].currentValue > calibrationValues_Temp[activeGaitParam])
			calibrationValues_Temp[activeGaitParam] = gaitParams.gaitParam_ObjectArray[activeGaitParam].currentValue;
		param_Z1 = gaitParams.gaitParam_ObjectArray[activeGaitParam].currentValue;
	};

	void saveCalibration(short activeGaitParam)
	{
		calibrationValues[activeGaitParam] = calibrationValues_Temp[activeGaitParam];
		isParamCalibrated[activeGaitParam] = true;
	}

	void discardCalibration(short activeGaitParam)
	{
		calibrationValues_Temp[activeGaitParam] = 0.0;
		param_Z1 = 0.0;
	}

	void flush_HSArray()
	{
		for (int i = 0; i < 5; i++)
		{
			updateHS_Calibration(0);
		}
	}

	double updateMean(double oldMean, int count, float newVal)
	{
		return (oldMean*count + newVal) / (count + 1);
	};

	float calcArraySTD(float *arr, float *mean, int count)
	{
		float standDev = 0.0; float variance = 0.0;
		for (int i = 0; i < count; i++)
		{
			variance += pow(arr[i] - *mean, 2);
		}
		variance = variance / float(count - 1);

		standDev = sqrt(variance);

		return standDev;
	}
};