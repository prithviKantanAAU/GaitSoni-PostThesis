#pragma once
#include <string>
#include "GaitParam_Single.h"

class gaitParamInfo
{
public:

	short numMovementParams = 17;
	short activeGaitParam = 0;
	short num_UseScenarios = 6;
	short indices_MP[10][30] = { 0 };
	short num_MP[10] = { 0 };
	short numSensorLocations = 3;
	std::string names_DesiredBehavior[3] = { "Less Than", "Equal To", "Greater Than" };
	String exerciseModes[10] = { "Testing","Static Upright","Dynamic Trunk","Jerk Feedback","STS Angle Cue","Gait" };

	GaitParam_Single gaitParam_ObjectArray[30];

	gaitParamInfo() 
	{
		// 0					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray0[10] = { true, true, true, false, false, false, false };
		//							TRUNK FEET
		bool sensorReqArray0[5] = { true, false };
		gaitParam_ObjectArray[0].setName_SensorReq("Inclination (+-) - ML",
			sensorReqArray0,numSensorLocations);
		gaitParam_ObjectArray[0].set_isIncluded_UseScenarios(useCaseArray0, num_UseScenarios);
		gaitParam_ObjectArray[0].initialize(0, 0.01, -45, 45);

		// 1					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray1[10] = { true, true, true, false, false, false, false };
		//							TRUNK FEET
		bool sensorReqArray1[5] = { true, false };
		gaitParam_ObjectArray[1].setName_SensorReq("Inclination (+-) - AP",
			sensorReqArray1, numSensorLocations);
		gaitParam_ObjectArray[1].set_isIncluded_UseScenarios(useCaseArray1, num_UseScenarios);
		gaitParam_ObjectArray[1].initialize(0, 0.01, -45, 45);

		// 2					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray2[10] = { true, true, true, false, false, false, false };
		//							TRUNK FEET
		bool sensorReqArray2[5] = { true, false };
		gaitParam_ObjectArray[2].setName_SensorReq("Trunk Projection Zone",
			sensorReqArray2, numSensorLocations);
		gaitParam_ObjectArray[2].set_isIncluded_UseScenarios(useCaseArray2, num_UseScenarios);
		gaitParam_ObjectArray[2].initialize(0, 0.01, 1, 6);

		// 3					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray3[10] = { true, true, false, false, false, false, false };
		//							TRUNK FEET
		bool sensorReqArray3[5] = { true, false };
		gaitParam_ObjectArray[3].setName_SensorReq("RMS Acc ML",
			sensorReqArray3, numSensorLocations);
		gaitParam_ObjectArray[3].set_isIncluded_UseScenarios(useCaseArray3, num_UseScenarios);
		gaitParam_ObjectArray[3].initialize(0, 0.01, 0, 20);

		// 4					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray4[10] = { true, true, false, false, false, false, false };
		//							TRUNK FEET
		bool sensorReqArray4[5] = { true, false };
		gaitParam_ObjectArray[4].setName_SensorReq("RMS Acc AP",
			sensorReqArray4, numSensorLocations);
		gaitParam_ObjectArray[4].set_isIncluded_UseScenarios(useCaseArray4, num_UseScenarios);
		gaitParam_ObjectArray[4].initialize(0, 0.01, 0, 20);

		// 5					   TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray5[10] = { true, false, false, true, false, false, false };
		//							TRUNK FEET
		bool sensorReqArray5[5] = { true, false };
		gaitParam_ObjectArray[5].setName_SensorReq("Scalar Jerk",
			sensorReqArray5, numSensorLocations);
		gaitParam_ObjectArray[5].set_isIncluded_UseScenarios(useCaseArray5, num_UseScenarios);
		gaitParam_ObjectArray[5].initialize(0, 0.01, 0, 25);

		// 6					   TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray6[10] = { true, false, false, true, false, false, false };
		//							TRUNK FEET
		bool sensorReqArray6[5] = { true, false };
		gaitParam_ObjectArray[6].setName_SensorReq("Jerk - X",
			sensorReqArray6, numSensorLocations);
		gaitParam_ObjectArray[6].set_isIncluded_UseScenarios(useCaseArray6, num_UseScenarios);
		gaitParam_ObjectArray[6].initialize(0, 0.01, 0, 25);

		// 7					   TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray7[10] = { true, false, false, true, false, false, false };
		//							TRUNK FEET
		bool sensorReqArray7[5] = { true, false };
		gaitParam_ObjectArray[7].setName_SensorReq("Jerk - Y",
			sensorReqArray7, numSensorLocations);
		gaitParam_ObjectArray[7].set_isIncluded_UseScenarios(useCaseArray7, num_UseScenarios);
		gaitParam_ObjectArray[7].initialize(0, 0.01, 0, 25);

		// 8					   TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray8[10] = { true, false, false, true, false, false, false };
		//							TRUNK FEET
		bool sensorReqArray8[5] = { true, false };
		gaitParam_ObjectArray[8].setName_SensorReq("Jerk - Z",
			sensorReqArray8, numSensorLocations);
		gaitParam_ObjectArray[8].set_isIncluded_UseScenarios(useCaseArray8, num_UseScenarios);
		gaitParam_ObjectArray[8].initialize(0, 0.01, 0, 25);

		// 9					   TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray9[10] = { true, true, false, false, false, false, false };
		//							TRUNK FEET
		bool sensorReqArray9[5] = { true, false };
		gaitParam_ObjectArray[9].setName_SensorReq("Sway Vel - ML",
			sensorReqArray9, numSensorLocations);
		gaitParam_ObjectArray[9].set_isIncluded_UseScenarios(useCaseArray9, num_UseScenarios);
		gaitParam_ObjectArray[9].initialize(0, 0.01, 0, 200);

		// 10				   	   TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray10[10] = { true, true, false, false, false, false, false };
		//							TRUNK FEET
		bool sensorReqArray10[5] = { true, false };
		gaitParam_ObjectArray[10].setName_SensorReq("Sway Vel - AP",
			sensorReqArray10, numSensorLocations);
		gaitParam_ObjectArray[10].set_isIncluded_UseScenarios(useCaseArray10, num_UseScenarios);
		gaitParam_ObjectArray[10].initialize(0, 0.01, 0, 200);

		// 11				   	    TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray11[10] = { true, false, false, false, false, true, false };
		//							TRUNK FEET
		bool sensorReqArray11[5] = { true, false };
		gaitParam_ObjectArray[11].setName_SensorReq("HS Timing",
			sensorReqArray11, numSensorLocations);
		gaitParam_ObjectArray[11].set_isIncluded_UseScenarios(useCaseArray11, num_UseScenarios);
		gaitParam_ObjectArray[11].initialize(0, 0.01, 0, 1);

		// 12				   	    TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray12[10] = { true, false, false, false, false, true, false };
		//							TRUNK FEET
		bool sensorReqArray12[5] = { true, false };
		gaitParam_ObjectArray[12].setName_SensorReq("HS Trigger",
			sensorReqArray12, numSensorLocations);
		gaitParam_ObjectArray[12].set_isIncluded_UseScenarios(useCaseArray12, num_UseScenarios);
		gaitParam_ObjectArray[12].initialize(0, 0.01, 0, 1);

		// 13				   	    TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray13[10] = { true, false, false, false, true, false, false };
		//							TRUNK FEET
		bool sensorReqArray13[5] = { true, false };
		gaitParam_ObjectArray[13].setName_SensorReq("STS Cue",
			sensorReqArray13, numSensorLocations);
		gaitParam_ObjectArray[13].set_isIncluded_UseScenarios(useCaseArray13, num_UseScenarios);
		gaitParam_ObjectArray[13].initialize(0, 0.01, 0, 1);

		populate_MP_Matrix();
	};

	void populate_MP_Matrix()
	{
		for (int i = 0; i < numMovementParams; i++)			// SONI
		{
			for (int j = 0; j < num_UseScenarios; j++)		// USE CASE SCENARIO
			{
				if (gaitParam_ObjectArray[i].isIncluded_UseScenarios[j])
				{
					indices_MP[j][num_MP[j]] = i;
					num_MP[j] += 1;
				}
			}
		}
	};

	void setTargetValue(float val)
	{
		gaitParam_ObjectArray[activeGaitParam].target = gaitParam_ObjectArray[activeGaitParam].minVal
			+ val * (gaitParam_ObjectArray[activeGaitParam].maxVal 	- gaitParam_ObjectArray[activeGaitParam].minVal);
	};

	void setDesiredBehavior(short val) { gaitParam_ObjectArray[activeGaitParam].desiredBehavior = val;	};

	void updateGaitParameter(int index) {	activeGaitParam = index - 1;   };
	
	~gaitParamInfo() {};
};

