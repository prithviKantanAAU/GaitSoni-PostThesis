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
	float skew_mpMin = 0;
	float skew_mpMax = 1;
	float minVal_postSkew = 0;
	float maxVal_postSkew = 0;
	short calibrationType = 0;				// 0 = Maximum // 1 = STS // 2 = HS // 3 = Zones
	float filterFc_OPT = 50;
	int medianFilt_L_OPT = 1;
	void setCalibrationType(short val) { calibrationType = val; };
	
	void updateValue_Current(float val)											// Periodic Call 1
	{
		currentValue = val;
	};

	void initialize(float mini, float maxi, float mini_T, float maxi_T)			// Mandatory Call 2
	{
		minVal = mini;
		maxVal = maxi;
		minVal_postSkew = minVal;
		maxVal_postSkew = maxVal;
		target_MIN = mini_T;
		target_MAX = maxi_T;
	}
	
	void updateValue_Target(float val, bool isMax)								// UI Call 1
	{
		if (!isMax) target_MIN = val;
		else		target_MAX = val;
	}

	void storeFilterInfo(float fc_LPF, int medFilt_L)
	{
		filterFc_OPT = fc_LPF;
		medianFilt_L_OPT = medFilt_L;
	}

	void adjustMPBounds()
	{
		float range = maxVal - minVal;
		minVal_postSkew = minVal + skew_mpMin * range;
		maxVal_postSkew = minVal + skew_mpMax * range;
	}

	void updateNormTarget(float *targetMin, float *targetMax)
	{
		float range_skewed = maxVal_postSkew - minVal_postSkew;
		*targetMin = (target_MIN - minVal_postSkew) / range_skewed;
		*targetMax = (target_MAX - minVal_postSkew) / range_skewed;
		if (target_MIN <= minVal_postSkew)
			*targetMin = 0;
		if (target_MAX >= maxVal_postSkew)
			*targetMax = 1;
	}

	GaitParam_Single() {};
	~GaitParam_Single() {};
};

