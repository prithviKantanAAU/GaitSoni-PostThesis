#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class GaitParam_Single
{
public:

	String name = "";
	void setName_SensorReq(String MP_Name, bool *boolArray, short maxNum_SensorLocations) 
	{ 
		name = MP_Name;
		for (int i = 0; i < maxNum_SensorLocations; i++)
			isSensorRequired[i] = boolArray[i];
	}
	bool isSensorRequired[5] = { false };
	bool isIncluded_UseScenarios[10] = { false };
	void set_isIncluded_UseScenarios(bool* boolArray, short numExerciseModes)	// Mandatory Call 1
	{
		for (int i = 0; i < numExerciseModes; i++)
			isIncluded_UseScenarios[i] = boolArray[i];
	}
	
	float currentValue = 0;
	float target_MIN = 0;
	float target_MAX = 0;
	float minVal = 0;
	float maxVal = 0;
	
	void updateValue_Current(float val)											// Periodic Call 1
	{
		currentValue = val;
	};

	void initialize(float mini, float maxi, float mini_T, float maxi_T)			// Mandatory Call 2
	{
		minVal = mini;
		maxVal = maxi;
		target_MIN = mini_T;
		target_MAX = maxi_T;
	}
	
	void updateValue_Target(float val, bool isMax)								// UI Call 1
	{
		if (!isMax) target_MIN = val;
		else		target_MAX = val;
	}

	GaitParam_Single() {};
	~GaitParam_Single() {};
};

