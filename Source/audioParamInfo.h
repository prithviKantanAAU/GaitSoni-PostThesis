#pragma once
#include <string>
#include "AudioParam_Single.h"

class audioParamInfo
{
public:

	AudioParam_Single audioParam_ObjectArray[40];
	short num_UseScenarios = 7;
	short indices_AP_OVERALL[30] = { 0 };
	short indices_AP[10][30] = { 0 };
	short num_AP[10] = { 0 };
	short activeAudioParam = 0;
	short activeCueParam = 0;
	short activeAudioParam_DynTarget_X = 0;
	short activeAudioParam_DynTarget_Y = 0;
	short numSoni_Musical = 0;

	audioParamInfo()
	{
		// 1					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray1[10] = { true, true, true, false, false, false, false };
		audioParam_ObjectArray[0].set_nameTypeAddress("Dissonance", 7, 1);
		audioParam_ObjectArray[0].set_isIncluded_UseScenarios(useCaseArray1, num_UseScenarios);
		audioParam_ObjectArray[0].initializeSoniParams(1, 0, 1, 0, 0, 100, 0);
		indices_AP_OVERALL[numSoni_Musical] = 0;
		numSoni_Musical++;

		// 2					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray2[10] = { true, true, true, false, false, false, false };
		audioParam_ObjectArray[1].set_nameTypeAddress("Instrumentation", 2, 1);
		audioParam_ObjectArray[1].set_isIncluded_UseScenarios(useCaseArray2, num_UseScenarios);
		audioParam_ObjectArray[1].initializeSoniParams(1, 0, 1, 0, 0, 100, 0);
		indices_AP_OVERALL[numSoni_Musical] = 1;
		numSoni_Musical++;

		// 3					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray4[10] = { true, false, true, false, true, false, true };
		audioParam_ObjectArray[2].set_nameTypeAddress("Brightness",	8, 1);
		audioParam_ObjectArray[2].set_isIncluded_UseScenarios(useCaseArray4, num_UseScenarios);
		audioParam_ObjectArray[2].initializeSoniParams(0.4, 0, 1, 0, 0, 100, 0);
		indices_AP_OVERALL[numSoni_Musical] = 2;
		numSoni_Musical++;

		// 4					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray5[10] = { true, false, false, true, false, false, true };
		audioParam_ObjectArray[3].set_nameTypeAddress("Tonic Pitch", 9, 1);
		audioParam_ObjectArray[3].set_isIncluded_UseScenarios(useCaseArray5, num_UseScenarios);
		audioParam_ObjectArray[3].initializeSoniParams(0.8, 0, 1, 0, 0, 100, 0.5);
		indices_AP_OVERALL[numSoni_Musical] = 3;
		numSoni_Musical++;

		// 5					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray6[10] = { true, false, false, true, false, true, false };
		audioParam_ObjectArray[4].set_nameTypeAddress("Saw Sweep", 10, 1);
		audioParam_ObjectArray[4].set_isIncluded_UseScenarios(useCaseArray6, num_UseScenarios);
		audioParam_ObjectArray[4].initializeSoniParams(0.4, 0, 1, 0, 0, 100, 0);
		indices_AP_OVERALL[numSoni_Musical] = 4;
		numSoni_Musical++;

		// 6					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray7[10] = { true, false, true, true, false, true, false };
		audioParam_ObjectArray[5].set_nameTypeAddress("Filtered Noise", 11, 1);
		audioParam_ObjectArray[5].set_isIncluded_UseScenarios(useCaseArray7, num_UseScenarios);
		audioParam_ObjectArray[5].initializeSoniParams(0.7, 0, 1, 0, 0, 100, 0);
		indices_AP_OVERALL[numSoni_Musical] = 5;
		numSoni_Musical++;

		// 7					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray8[10] = { true, false, false, false, false, true, false };
		audioParam_ObjectArray[6].set_nameTypeAddress("Foot Drum", 6, 1);
		audioParam_ObjectArray[6].set_isIncluded_UseScenarios(useCaseArray8, num_UseScenarios);
		audioParam_ObjectArray[6].initializeSoniParams(1, 0, 1, 0, 0, 100, 0);
		indices_AP_OVERALL[numSoni_Musical] = 6;
		numSoni_Musical++;

		// 8					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray9[10] = { true, true, false, false, false, false, false };
		audioParam_ObjectArray[7].set_nameTypeAddress("Ambulance", 1, 1);
		audioParam_ObjectArray[7].set_isIncluded_UseScenarios(useCaseArray9, num_UseScenarios);
		audioParam_ObjectArray[7].initializeSoniParams(1, 0, 1, 0, 0, 100, 0);
		indices_AP_OVERALL[numSoni_Musical] = 7;
		numSoni_Musical++;

		// 9					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray11[10] = { true, true, false, false, true, false, false };
		audioParam_ObjectArray[8].set_nameTypeAddress("Bell Cue", 3, 1);
		audioParam_ObjectArray[8].set_isIncluded_UseScenarios(useCaseArray11, num_UseScenarios);
		audioParam_ObjectArray[8].initializeSoniParams(1, 0, 1, 0, 0, 100, 0);
		indices_AP_OVERALL[numSoni_Musical] = 8;
		numSoni_Musical++;

		// 10					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray12[10] = { true, true, true, false, true, false, false };
		audioParam_ObjectArray[9].set_nameTypeAddress("Music Stop", 4, 1);
		audioParam_ObjectArray[9].set_isIncluded_UseScenarios(useCaseArray12, num_UseScenarios);
		audioParam_ObjectArray[9].initializeSoniParams(1, 0, 1, 0, 0, 100, 0);
		indices_AP_OVERALL[numSoni_Musical] = 9;
		numSoni_Musical++;

		// 11					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray13[10] = { true, true, false, false, true, false, true };
		audioParam_ObjectArray[10].set_nameTypeAddress("Wah Cue", 5, 1);
		audioParam_ObjectArray[10].set_isIncluded_UseScenarios(useCaseArray13, num_UseScenarios);
		audioParam_ObjectArray[10].initializeSoniParams(0.5, 0, 1, 0, 0, 100, 0);
		indices_AP_OVERALL[numSoni_Musical] = 10;
		numSoni_Musical++;

		// 12					    TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray14[10] = { true, false, false, false, false, false, true };
		audioParam_ObjectArray[11].set_nameTypeAddress("Mel Degree", 0, 2);
		audioParam_ObjectArray[11].set_isIncluded_UseScenarios(useCaseArray14, num_UseScenarios);
		audioParam_ObjectArray[11].initializeSoniParams(1, 0, 1, 0, 0, 100, 0);
		indices_AP_OVERALL[numSoni_Musical] = 11;
		numSoni_Musical++;

		// 13					    TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray15[10] = { true, false, false, false, false, false, true };
		audioParam_ObjectArray[12].set_nameTypeAddress("Spatialization", 12, 1);
		audioParam_ObjectArray[12].set_isIncluded_UseScenarios(useCaseArray15, num_UseScenarios);
		audioParam_ObjectArray[12].initializeSoniParams(1, 0, 1, 0, 0, 100, 0.5);
		indices_AP_OVERALL[numSoni_Musical] = 12;
		numSoni_Musical++;

		// 14					    TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray16[10] = { true, false, false, false, false, false, true };
		audioParam_ObjectArray[13].set_nameTypeAddress("Tempo", 0, 2);
		audioParam_ObjectArray[13].set_isIncluded_UseScenarios(useCaseArray16, num_UseScenarios);
		audioParam_ObjectArray[13].initializeSoniParams(1, 0, 1, 0, 0, 100, 0.5);
		indices_AP_OVERALL[numSoni_Musical] = 13;
		numSoni_Musical++;

		// 15					    TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray17[10] = { true, false, false, false, false, false, false };
		audioParam_ObjectArray[14].set_nameTypeAddress("Tempo 1D", 0, 2);
		audioParam_ObjectArray[14].set_isIncluded_UseScenarios(useCaseArray17, num_UseScenarios);
		audioParam_ObjectArray[14].initializeSoniParams(1, 0, 1, 0, 0, 100, 1);
		indices_AP_OVERALL[numSoni_Musical] = 14;
		numSoni_Musical++;

		// 40 - NONE
		// 40					    TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray42[10] = { true, true, true, true, true, true, true };
		audioParam_ObjectArray[39].set_nameTypeAddress("NONE", 0, 2);
		audioParam_ObjectArray[39].set_isIncluded_UseScenarios(useCaseArray42, num_UseScenarios);
		audioParam_ObjectArray[39].initializeSoniParams(1, 0, 1, 0, 0, 100, 0);
		indices_AP_OVERALL[numSoni_Musical] = 39;
		numSoni_Musical++;

		populate_AP_Matrix();
	};

	void populate_AP_Matrix()
	{
		for (int i = 0; i < numSoni_Musical; i++)			// SONI
		{
			for (int j = 0; j < num_UseScenarios; j++)		// USE CASE SCENARIO
			{
				if (audioParam_ObjectArray[indices_AP_OVERALL[i]].isIncluded_UseScenarios[j])
				{
					indices_AP[j][num_AP[j]] = indices_AP_OVERALL[i];
					num_AP[j] += 1;
				}
			}
		}
	};

	~audioParamInfo() {};
};

