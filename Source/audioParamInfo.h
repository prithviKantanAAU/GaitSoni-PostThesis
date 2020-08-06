#pragma once
#include <string>
#include "AudioParam_Single.h"
#include "FaustStrings_AP.h"

class audioParamInfo
{
public:

	AudioParam_Single audioParam_ObjectArray[40];
	FaustStrings_AP faustStrings;
	short num_UseScenarios = 7;
	short indices_AP[10][30] = { 0 };
	short num_AP[10] = { 0 };
	short activeAudioParam = 0;
	short activeCueParam = 0;
	short numSoni_Musical = 16;

	audioParamInfo()
	{
		// 0					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray0[10] = { true, true, false, false, false, false, false };
		audioParam_ObjectArray[0].set_nameTypeAddress("Melody Detune",
			faustStrings.getSonificationStringAddress(0), 1);
		audioParam_ObjectArray[0].set_isIncluded_UseScenarios(useCaseArray0, num_UseScenarios);
		audioParam_ObjectArray[0].initializeSoniParams(0.5, 0, 1, 0, 0, 100);

		// 1					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray1[10] = { true, true, false, false, false, false, false };
		audioParam_ObjectArray[1].set_nameTypeAddress("Dissonance",
			faustStrings.getSonificationStringAddress(2), 1);
		audioParam_ObjectArray[1].set_isIncluded_UseScenarios(useCaseArray1, num_UseScenarios);
		audioParam_ObjectArray[1].initializeSoniParams(1, 0, 1, 0, 0, 100);

		// 2					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray2[10] = { true, false, true, false, false, false, false };
		audioParam_ObjectArray[2].set_nameTypeAddress("Instrumentation",
			faustStrings.getSonificationStringAddress(11), 1);
		audioParam_ObjectArray[2].set_isIncluded_UseScenarios(useCaseArray2, num_UseScenarios);
		audioParam_ObjectArray[2].initializeSoniParams(1, 0, 1, 0, 0, 100);

		// 3					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray3[10] = { true, false, true, false, false, false, true };
		audioParam_ObjectArray[3].set_nameTypeAddress("Complexity",
			faustStrings.getSonificationStringAddress(4), 2);
		audioParam_ObjectArray[3].set_isIncluded_UseScenarios(useCaseArray3, num_UseScenarios);
		audioParam_ObjectArray[3].initializeSoniParams(1, 0, 1, 0, 0, 100);

		// 4					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray4[10] = { true, false, true, false, false, false, false };
		audioParam_ObjectArray[4].set_nameTypeAddress("Brightness",
			faustStrings.getSonificationStringAddress(5), 1);
		audioParam_ObjectArray[4].set_isIncluded_UseScenarios(useCaseArray4, num_UseScenarios);
		audioParam_ObjectArray[4].initializeSoniParams(0.4, 0, 1, 0, 0, 100);

		// 5					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray5[10] = { true, false, false, true, false, false, true };
		audioParam_ObjectArray[5].set_nameTypeAddress("Tonic Pitch",
			faustStrings.getSonificationStringAddress(6), 1);
		audioParam_ObjectArray[5].set_isIncluded_UseScenarios(useCaseArray5, num_UseScenarios);
		audioParam_ObjectArray[5].initializeSoniParams(0.8, 0, 1, 0, 0, 100);

		// 6					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray6[10] = { true, false, false, true, false, true, false };
		audioParam_ObjectArray[6].set_nameTypeAddress("Saw Sweep",
			faustStrings.getSonificationStringAddress(7), 1);
		audioParam_ObjectArray[6].set_isIncluded_UseScenarios(useCaseArray6, num_UseScenarios);
		audioParam_ObjectArray[6].initializeSoniParams(0.4, 0, 1, 0, 0, 100);

		// 7					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray7[10] = { true, false, false, true, false, true, true };
		audioParam_ObjectArray[7].set_nameTypeAddress("Filtered Noise",
			faustStrings.getSonificationStringAddress(8), 1);
		audioParam_ObjectArray[7].set_isIncluded_UseScenarios(useCaseArray7, num_UseScenarios);
		audioParam_ObjectArray[7].initializeSoniParams(0.7, 0, 1, 0, 0, 100);

		// 8					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray8[10] = { true, false, false, false, false, true, false };
		audioParam_ObjectArray[8].set_nameTypeAddress("Foot Drum",
			faustStrings.getSonificationStringAddress(9), 1);
		audioParam_ObjectArray[8].set_isIncluded_UseScenarios(useCaseArray8, num_UseScenarios);
		audioParam_ObjectArray[8].initializeSoniParams(1, 0, 1, 0, 0, 100);

		// 9					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray9[10] = { true, true, false, false, false, false, false };
		audioParam_ObjectArray[9].set_nameTypeAddress("Ambulance",
			faustStrings.getSonificationStringAddress(10), 1);
		audioParam_ObjectArray[9].set_isIncluded_UseScenarios(useCaseArray9, num_UseScenarios);
		audioParam_ObjectArray[9].initializeSoniParams(1, 0, 1, 0, 0, 100);

		// 10					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray10[10] = { true, true, false, false, false, false, false };
		audioParam_ObjectArray[10].set_nameTypeAddress("Cartoon",
			faustStrings.getSonificationStringAddress(12), 1);
		audioParam_ObjectArray[10].set_isIncluded_UseScenarios(useCaseArray10, num_UseScenarios);
		audioParam_ObjectArray[10].initializeSoniParams(1, 0, 1, 0, 0, 100);

		// 11					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray11[10] = { true, false, false, false, true, false, false };
		audioParam_ObjectArray[11].set_nameTypeAddress("Bell Cue",
			faustStrings.getSonificationStringAddress(13), 1);
		audioParam_ObjectArray[11].set_isIncluded_UseScenarios(useCaseArray11, num_UseScenarios);
		audioParam_ObjectArray[11].initializeSoniParams(1, 0, 1, 0, 0, 100);

		// 12					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray12[10] = { true, false, false, false, true, false, false };
		audioParam_ObjectArray[12].set_nameTypeAddress("Music Stop",
			faustStrings.getSonificationStringAddress(14), 1);
		audioParam_ObjectArray[12].set_isIncluded_UseScenarios(useCaseArray12, num_UseScenarios);
		audioParam_ObjectArray[12].initializeSoniParams(1, 0, 1, 0, 0, 100);

		// 13					  TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray13[10] = { true, false, false, false, true, false, false };
		audioParam_ObjectArray[13].set_nameTypeAddress("Wah Cue",
			faustStrings.getSonificationStringAddress(15), 1);
		audioParam_ObjectArray[13].set_isIncluded_UseScenarios(useCaseArray13, num_UseScenarios);
		audioParam_ObjectArray[13].initializeSoniParams(0.5, 0, 1, 0, 0, 100);

		// 14					    TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray14[10] = { false, false, false, false, false, false, true };
		audioParam_ObjectArray[14].set_nameTypeAddress("Mel Degree",
			faustStrings.getSonificationStringAddress(16), 2);
		audioParam_ObjectArray[14].set_isIncluded_UseScenarios(useCaseArray14, num_UseScenarios);
		audioParam_ObjectArray[14].initializeSoniParams(1, 0, 1, 0, 0, 100);

		// 15					    TEST   SB     DB    JERK  ANGLE  GAIT   PHASE
		bool useCaseArray15[10] = { false, false, false, false, false, false, true };
		audioParam_ObjectArray[15].set_nameTypeAddress("Snare Flurry",
			faustStrings.getSonificationStringAddress(17), 2);
		audioParam_ObjectArray[15].set_isIncluded_UseScenarios(useCaseArray15, num_UseScenarios);
		audioParam_ObjectArray[15].initializeSoniParams(1, 0, 1, 0, 0, 100);

		populate_AP_Matrix();
	};

	void populate_AP_Matrix()
	{
		for (int i = 0; i < numSoni_Musical; i++)			// SONI
		{
			for (int j = 0; j < num_UseScenarios; j++)		// USE CASE SCENARIO
			{
				if (audioParam_ObjectArray[i].isIncluded_UseScenarios[j])
				{
					indices_AP[j][num_AP[j]] = i;
					num_AP[j] += 1;
				}
			}
		}
	};

	~audioParamInfo() {};
};

