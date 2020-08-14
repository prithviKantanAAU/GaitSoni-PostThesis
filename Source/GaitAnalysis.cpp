#include "GaitAnalysis.h"

GaitAnalysis::GaitAnalysis()
{
	LPF_Gyr_Turn_Smooth.flushDelays();
	LPF_Gyr_Turn_Smooth.calculateLPFCoeffs(1, 0.7, fs);
	smooth_Angle_AP.flushDelays();
	smooth_Angle_AP.calculateLPFCoeffs(3, 0.7, fs);
}

GaitAnalysis::~GaitAnalysis()
{
}

void GaitAnalysis::setupReceivers()
{
	for (int i = 0; i < sensorInfo.numSensorsMax; i++)
	{
		sensors_OSCReceivers[i].setupPortAndListener(sensorInfo.UDP_Ports[i], sensorInfo.sensors_OSCAddress[i]);
		sensors_OSCReceivers[i].setSampleRate(sensorInfo.IMU_SampleRate);
	}
	setSampleRate(sensorInfo.IMU_SampleRate);
}

void GaitAnalysis::setSampleRate(int fs_ext)
{
	fs = fs_ext;
	hfen_trunk.setSampleRate(fs_ext);
	hfen_foot_L.setSampleRate(fs_ext);
	hfen_foot_R.setSampleRate(fs_ext);
}

void GaitAnalysis::compute(short currentGaitParam, bool isCalibrating)
{
	if (!areRequiredSensorsOnline()) return;					// ONLY PROCEED IF REQD SENSORS ON
	
	for (int j = 0; j < sensorInfo.numSensorsMax; j++)
	{
		switch (sensorInfo.bodyLocation[j])
		{
		case 1:
			idx_Sensor_Trunk = j;
			break;
		case 2:
			idx_Sensor_Foot_L = j;
			break;
		case 3:
			idx_Sensor_Foot_R = j;
			break;
		}	
	} 
	switch (gaitParams.activeGaitParam)
		{
		case 0:				// ML +/-
			getOrientation_Fused(sensors_OSCReceivers[idx_Sensor_Trunk].acc_Buf,
				sensors_OSCReceivers[idx_Sensor_Trunk].gyr_Buf);
			if (isCalibrating) calibrateMaximum(3);
			break;
		case 1:				// AP +/-
			getOrientation_Fused(sensors_OSCReceivers[idx_Sensor_Trunk].acc_Buf,
				sensors_OSCReceivers[idx_Sensor_Trunk].gyr_Buf);
			if (isCalibrating) calibrateMaximum(4);
			break;
		case 2:				// ML/AP Projection
			getOrientation_Fused(sensors_OSCReceivers[idx_Sensor_Trunk].acc_Buf,
				sensors_OSCReceivers[idx_Sensor_Trunk].gyr_Buf);
			getProjection_ML_AP();
			if (isCalibrating) staticBalance_calibrateCoordinates();
			break;
		case 3:				// RMS ACC ML
			getRMS_Acc(sensors_OSCReceivers[idx_Sensor_Trunk].acc_Buf);
			if (isCalibrating) calibrateMaximum(7);
			break;
		case 4:				// RMS ACC AP
			getRMS_Acc(sensors_OSCReceivers[idx_Sensor_Trunk].acc_Buf);
			if (isCalibrating) calibrateMaximum(8);
			break;
		case 5:				// JERK SCALAR
			getJerkParams(sensors_OSCReceivers[idx_Sensor_Trunk].acc_Buf);
			if (isCalibrating) calibrateMaximum(9);
			break;
		case 6:			// JERK X
			getJerkParams(sensors_OSCReceivers[idx_Sensor_Trunk].acc_Buf);
			if (isCalibrating) calibrateMaximum(10);
			break;
		case 7:			// JERK Y
			getJerkParams(sensors_OSCReceivers[idx_Sensor_Trunk].acc_Buf);
			if (isCalibrating) calibrateMaximum(11);
			break;
		case 8:			// JERK Z
			getJerkParams(sensors_OSCReceivers[idx_Sensor_Trunk].acc_Buf);
			if (isCalibrating) calibrateMaximum(12);
			break;
		case 9:			// SWAY VEL ML
			getSwayVelocity(sensors_OSCReceivers[idx_Sensor_Trunk].gyr_Buf);
			if (isCalibrating) calibrateMaximum(13);
			break;
		case 10:			// SWAY VEL AP
			getSwayVelocity(sensors_OSCReceivers[idx_Sensor_Trunk].gyr_Buf);
			if (isCalibrating) calibrateMaximum(14);
			break;
		case 11:			// GAIT PERIODICITY	
			calcTimingPerformanceFeature_2(sensors_OSCReceivers[idx_Sensor_Foot_L].acc_Buf,
				sensors_OSCReceivers[idx_Sensor_Foot_R].acc_Buf,
				sensors_OSCReceivers[idx_Sensor_Foot_L].gyr_Buf,
				isCalibrating);
			break;
		case 12:			// HEEL STRIKE FEATURE
			drumTrigger(sensors_OSCReceivers[idx_Sensor_Foot_L].acc_Buf,
				sensors_OSCReceivers[idx_Sensor_Foot_R].acc_Buf, isCalibrating);
			break;
		case 13:			// SST CUE
			getOrientation_Fused(sensors_OSCReceivers[idx_Sensor_Trunk].acc_Buf, 
				sensors_OSCReceivers[idx_Sensor_Trunk].gyr_Buf);
			getSitStandCueFeature();
			break;
		}
}

void GaitAnalysis::trunk_CalibrateRest(float *accBuf)
{
	float magnitude = sqrt(pow(accBuf[0],2) + pow(accBuf[1], 2) + pow(accBuf[2], 2));

	R_acc_est[0] = accBuf[0] / magnitude;
	R_acc_est[1] = accBuf[2] / magnitude;
	R_acc_est[2] = accBuf[1] / magnitude;
}

void GaitAnalysis::getOrientation_Fused(float *accBuf, float *gyrBuf)
{
	//Adjust Coordinates and Units
	float R_acc_X = accBuf[0];	float gyrReading_X = gyrBuf[0] * M_PI / 180;
	float R_acc_Y = accBuf[2];	float gyrReading_Y = gyrBuf[2] * M_PI / 180;
	float R_acc_Z = accBuf[1];	float gyrReading_Z = gyrBuf[1] * M_PI / 180;
	float R_gyro_Inter[3] = { 0.0 };
	float R_acc_Vector[3] = { R_acc_X,R_acc_Y,R_acc_Z };

	if (!isCalibrated_Orientation)
	{
		trunk_CalibrateRest(accBuf);
		isCalibrated_Orientation = true;
	}

	//Normalize
	float R_acc = sqrt(R_acc_X*R_acc_X + R_acc_Y * R_acc_Y + R_acc_Z * R_acc_Z);
	R_acc_X /= R_acc;	R_acc_Y /= R_acc;	R_acc_Z /= R_acc;

	//Get Angles
	R_acc_est[2] = fmax(0.0001, R_acc_est[2]);
	float angle_xz_z1 = atan2(R_acc_est[0], R_acc_est[2]);
	float angle_yz_z1 = atan2(R_acc_est[1], R_acc_est[2]);

	//Average Gyr Reading and store buffer
	float gyrReading_Y_avg = 0.5*(gyrReading_Y + R_gyr_z1[1]);
	float gyrReading_X_avg = 0.5*(gyrReading_X + R_gyr_z1[0]);
	R_gyr_z1[0] = gyrReading_X;
	R_gyr_z1[1] = gyrReading_Y;
	R_gyr_z1[2] = gyrReading_Z;

	float angle_xz = angle_xz_z1 + gyrReading_Y_avg * 1 / fs;
	float angle_yz = angle_yz_z1 + gyrReading_X_avg * 1 / fs;
	boundValuesAndStore(0, isnan(angle_xz) ? 0 : angle_xz * 180 / M_PI);
	boundValuesAndStore(1, isnan(angle_yz) ? 0 : angle_yz * 180 / M_PI);

	//Calculate R_Gyro_Inter
	R_gyro_Inter[0] = sin(angle_xz) / sqrt(1 + pow(cos(angle_xz), 2)*pow(tan(angle_yz), 2));
	R_gyro_Inter[1] = sin(angle_yz) / sqrt(1 + pow(cos(angle_yz), 2)*pow(tan(angle_xz), 2));
	R_gyro_Inter[2] = sqrt(1 - R_gyro_Inter[0] * R_gyro_Inter[0] - R_gyro_Inter[1] * R_gyro_Inter[1]);

	for (int i = 0; i < 3; i++)
	{
		R_acc_est_z1[i] = R_acc_est[i];
		R_acc_est[i] = (R_acc_Vector[i] + wt_Gyr * R_gyro_Inter[i]) / (1 + wt_Gyr);
	}
}

void GaitAnalysis::getProjection_ML_AP()
{
	float roll_deg = (gaitParams.gaitParam_ObjectArray[0].currentValue + staticBalance_BoundsCoordinates[0][0]);
	float pitch_deg =(gaitParams.gaitParam_ObjectArray[1].currentValue + staticBalance_BoundsCoordinates[0][1]);
	float area = pow(staticBalance_Div_Roll, 2) + pow(staticBalance_Div_Pitch, 2);

	if (roll_deg < - 1 * staticBalance_Div_Roll)
	{
		boundValuesAndStore(2, 6);
		return;
	}

	else if (roll_deg > 1 * staticBalance_Div_Roll)
	{
		boundValuesAndStore(2, 5);
		return;
	}

	else if (pow(roll_deg, 2) + pow(pitch_deg, 2) <= area)
	{
		boundValuesAndStore(2, 1);
		return;
	}

	else if (pow(((pitch_deg - 0.5) / 2.25), 2) + pow((roll_deg / 1.5), 2) <= area)
	{
		boundValuesAndStore(2, 2);
		return;
	}

	else if (pow(((pitch_deg - 0.5) / 3), 2) + pow((roll_deg / 2), 2) <= area)
	{
		boundValuesAndStore(2, 3);
		return;
	}

	else
	{
		boundValuesAndStore(2, 4);
		return;
	}
}

void GaitAnalysis::getSitStandCueFeature()
{
	float angle_AP_signed = smooth_Angle_AP.doBiQuad(gaitParams.gaitParam_ObjectArray[1].currentValue, sitStand_Thresh_Current);
	if (smooth_Angle_AP.isThreshCrossing_Pos && sitStand_isStabilized)
	{
		sitStand_isStanding = !sitStand_isStanding;
		sitStand_isStabilized = false;
	}
	if (smooth_Angle_AP.isThreshCrossing_Neg)
	{
		sitStand_isStabilized = true;
		sitStand_Thresh_Current = sitStand_isStanding ? sitStand_Thresh_Sit : sitStand_Thresh_Stand;
	}
	bool feedbackCondition = sitStand_feedbackMode ? sitStand_isStabilized : sitStand_isStanding;
	boundValuesAndStore(13, feedbackCondition ? 0 : 1);
}

void GaitAnalysis::getRMS_Acc(float *accBuf)
{
	float accX = accBuf[0];
	float accZ = accBuf[2];
	
	//RAW

	accX = hfen_trunk.applyHFEN_PreProcess(accX,'x');
	accZ = hfen_trunk.applyHFEN_PreProcess(accZ,'z');

	//HPF APPLIED

	float horizComponent_ML = accX;
	float horizComponent_AP = accZ;

	float rmsSum_ML = 0;
	float rmsSum_AP = 0;

	rmsWindow_AP[1] = rmsWindow_AP[0]; 	rmsWindow_ML[1] = rmsWindow_ML[0];
	rmsWindow_ML[0] = accX;				rmsWindow_AP[0] = accZ;

	rmsSum_ML += pow(rmsWindow_ML[0], 2);
	rmsSum_AP += pow(rmsWindow_AP[0], 2);

	for (int i = 1; i < rms_Length - 1; i++)
	{
		rmsWindow_ML[i + 1] = rmsWindow_ML[i];
		rmsWindow_AP[i + 1] = rmsWindow_AP[i];
		rmsSum_ML += pow(rmsWindow_ML[i], 2);
		rmsSum_AP += pow(rmsWindow_AP[i], 2);
	}

	boundValuesAndStore(3, rmsSum_ML / rms_Length * 100);
	boundValuesAndStore(4, rmsSum_AP / rms_Length * 100);
}

void GaitAnalysis::getJerkParams(float *accBuf)
{
	float accX = accBuf[0];
	float accY = accBuf[1];
	float accZ = accBuf[2];

	//Apply HFEN
	accX = hfen_trunk.applyHFEN_PreProcess(accX,'x');
	accY = hfen_trunk.applyHFEN_PreProcess(accY,'y');
	accZ = hfen_trunk.applyHFEN_PreProcess(accZ,'z');

	float jerkX = fabs(accX - accX_z1);
	float jerkY = fabs(accY - accY_z1);
	float jerkZ = fabs(accZ - accZ_z1);

	float scalarJerk = sqrt(jerkX*jerkX + jerkY * jerkY + jerkZ * jerkZ);	//Euclidean Norm

	boundValuesAndStore(5, scalarJerk * 100);
	boundValuesAndStore(6, jerkX * 100);
	boundValuesAndStore(7, jerkY * 100);
	boundValuesAndStore(8, jerkZ * 100);

	accX_z1 = accX;		accY_z1 = accY;		accZ_z1 = accZ;
}

void GaitAnalysis::getSwayVelocity(float *gyrBuf)
{
	boundValuesAndStore(9, fabs(gyrBuf[0]));
	boundValuesAndStore(10, fabs(gyrBuf[2]));
}

void GaitAnalysis::updateHS_Calibration(float strideDuration_Latest)
{
	calibration_stepCount++;
	hs_Intervals[(calibration_stepCount - 1) % 2] = strideDuration_Latest;
	strideDur_Mean = 0;
	for (int i = 0; i < 2; i++)
	{	strideDur_Mean += hs_Intervals[i];	}
	strideDur_Mean /= 2.0;
	strideInterval = strideDur_Mean / 2.0;
	strideDur_COV = calcArraySTD(hs_Intervals, &strideDur_Mean, 2);
	calibrationValues_Temp[11] = strideDur_Mean;
	calibrationValues_Temp[12] = strideDur_Mean;
}

void GaitAnalysis::calcTimingPerformanceFeature_2(float *accBuf_L, float *accBuf_R, float *gyrBuf, bool isCalibrating)
{
	float featureValue = 0;
	float stepDuration_Exp = isHalfTime ? 2 * beatInterval : beatInterval;
	float timeTolerance = stepDuration_Exp * HS_IntervalTolerance;
	bool isTurning = abs(LPF_Gyr_Turn_Smooth.doBiQuad(gyrBuf[1], 0.001)) > 60 ? true : false;

	bool toTrigger = detectHS_acc(accBuf_L, true) || detectHS_acc(accBuf_R, false);								//MODIFY
	if (toTrigger)
	{
		updateHS_Calibration(timeElapsed - timeStamp_HS_Last);
		timeStamp_HS_Last = timeElapsed;

		if (timeStamp_HS_Last < (timeStamp_HS_Next_Exp - timeTolerance))// Check If Early
			isEarly = true;

		isLate = false;													// Reset isLate

		if (isEarly)
			timeStamp_HS_Last_Exp = timeStamp_HS_Next_Exp;				// Save Last Expected Time

		timeStamp_HS_Next_Exp = timeStamp_HS_Last + stepDuration_Exp; // Update Next Expected Time
	}

	float time_toNext_HS = timeStamp_HS_Next_Exp - timeElapsed;			// Time to Next Expected HS
	if (timeElapsed > timeStamp_HS_Last_Exp)
		isEarly = false;

	if (time_toNext_HS < -1 * timeTolerance)
	{
		isLate = true;		isEarly = false;
	}

	if (!isTurning)
		if (isEarly || isLate)		featureValue = 1;
	boundValuesAndStore(11, featureValue);
};

bool GaitAnalysis::detectHS_acc(float *accBuf, bool isLeft)
{
	float accX_filt = isLeft? hfen_foot_L.applyHFEN_PreProcess(accBuf[0], 'x') 
							: hfen_foot_R.applyHFEN_PreProcess(accBuf[0], 'x');
	float accY_filt = isLeft ? hfen_foot_L.applyHFEN_PreProcess(accBuf[1], 'y')
							 : hfen_foot_R.applyHFEN_PreProcess(accBuf[1], 'y');
	float accZ_filt = isLeft ? hfen_foot_L.applyHFEN_PreProcess(accBuf[2], 'z')
							 : hfen_foot_R.applyHFEN_PreProcess(accBuf[2], 'z');
	float norm = pow(accX_filt, 2) + pow(accY_filt, 2) + pow(accZ_filt, 2);
	bool stepDetected = false;
	HS_timeOut = 0.8 * beatInterval;

	if (HS_timeDone > HS_timeOut)
	{
		if (isThreshCross(norm, HS_thresh_pos, norm_acc_z1, true))
		{
			if (HS_isExpected_L) 
			{
				if (isLeft)
				{
					stepDetected = true;
					HS_isExpected_L = false;
					HS_isExpected_R = true;
					HS_timeDone = 0;
				}
			}

			if (HS_isExpected_R)
			{
				if (!isLeft)
				{
					stepDetected = true;
					HS_isExpected_L = true;
					HS_isExpected_R = false;
					HS_timeDone = 0;
				}
			}
		}
	}
	HS_timeDone += 1.0 / (float)fs;
	return stepDetected;
}

void GaitAnalysis::drumTrigger(float *accBuf_L, float *accBuf_R, bool isCalibrating)
{
	bool toTrigger_L = detectHS_acc(accBuf_L, true);
	bool toTrigger_R = detectHS_acc(accBuf_R, false);
	if (toTrigger_L || toTrigger_R)
	{
		updateHS_Calibration(timeElapsed - timeStamp_HS_Last);
		timeStamp_HS_Last = timeElapsed;
	}
	float featVal = 0;
	if (toTrigger_L)
		featVal = 0.7;
	if (toTrigger_R)
		featVal = 0.8;
	float mapVal = 0.5;

	for (int i = 4; i > 0; i--)
	{
		triggerBuffer[i] = triggerBuffer[i - 1];
	}
	triggerBuffer[0] = featVal;
	for (int i = 0; i < 4; i++)
	{
		if (triggerBuffer[i] > 0.5)
			mapVal = triggerBuffer[i];
	}

	boundValuesAndStore(12, mapVal);
}

