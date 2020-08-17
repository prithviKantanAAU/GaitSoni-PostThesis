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
	void updateValue_Current(float val)											// Periodic Call 1
	{
		currentValue = val;
	};
	float target = 0;
	void updateValue_Target(float val)											// UI Call 1
	{
		target = val;
	}

	short desiredBehavior = 0;
	float tolerance_Percent = 0.01;
	float minVal = 0;
	float maxVal = 0;
	void initialize(short desired, float tol, float mini, float maxi)			// Mandatory Call 2
	{
		desiredBehavior = desired;
		tolerance_Percent = tol;
		minVal = mini;
		maxVal = maxi;
	}

	GaitParam_Single() {};
	~GaitParam_Single() {};

};

