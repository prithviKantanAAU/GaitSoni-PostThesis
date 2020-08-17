#pragma once
#include "BiQuad.h"

class SoniMappingCompute
{
public:
	SoniMappingCompute()
	{
		paramSmoothing.flushDelays();
	};

	bool isSoniSource_Slider = true;							// Flag - Soni Source Slider?
	void setSonificationMode(bool isSlider)						// Set Soni Mode - Slider OR Sensor
	{
		isSoniSource_Slider = isSlider;
	};

	float soniVal_Slider = 0.0;									// Soni Slider Value
	void setSoniVal_Slider(float value)							// Soni Mode Slider - Set Value
	{
		soniVal_Slider = value;
	};

	short complexityTable_Perc[8][16] =
	{
		{1,1,1,1,	1,1,1,1,	1,1,1,1,	1,1,1,1},
		{1,1,1,1,	1,1,1,1,	1,1,1,1,	1,1,1,1},
		{1,1,1,0,	1,1,1,0,	1,1,1,0,	1,1,1,0},
		{1,1,1,0,	1,1,1,0,	1,1,1,0,	1,1,1,0},
		{1,1,0,0,	1,1,0,0,	1,1,0,0,	1,1,0,0},
		{1,1,0,0,	1,1,0,0,	1,1,0,0,	1,1,0,0},
		{1,0,0,0,	1,0,0,0,	1,0,0,0,	1,0,0,0},		
		{1,0,0,0,	0,0,0,0,	1,0,0,0,	0,0,0,0}		
	};

	short complexityTable_Mel[8][16] =
	{
		{1,1,1,1,	1,1,1,1,	1,1,1,1,	1,1,1,1},
		{1,0,1,1,	1,0,1,1,	1,0,1,1,	1,0,1,1},
		{1,0,1,0,	1,0,1,0,	1,0,1,0,	1,0,1,0},
		{1,0,0,0,	1,0,0,0,	1,0,0,0,	1,0,0,0},
		{1,0,0,0,	1,0,0,0,	1,0,0,0,	1,0,0,0},
		{1,0,0,0,	0,0,0,0,	1,0,0,0,	0,0,0,0},
		{0,0,0,0,	0,0,0,0,	0,0,0,0,	0,0,0,0},
		{0,0,0,0,	0,0,0,0,	0,0,0,0,	0,0,0,0}
	};

	float dyn_desired_Amplitude = 1;
	short movementFreqSetting = 1;
	float movementFunc_Order = 1;
	float movement_ErrorTolerance = 0;

	~SoniMappingCompute() {};
	BiQuad paramSmoothing;
	float computeParamValue(float currentVal, float targetVal, float *functionParamSet)
	{
			float order_mappingFunc = functionParamSet[0];
			float toleranceBW = functionParamSet[1];
			int numQuantLevels = functionParamSet[2];
			float smoothingFc = functionParamSet[3];
			float originalRange = functionParamSet[4];
			short desiredBehavior = functionParamSet[5];
			float currentError = 0;
			float adaptiveRange = 0;
			float output = 0.0;

			switch (desiredBehavior)
			{
			case 0:
				adaptiveRange = fmax(originalRange - targetVal, originalRange / 10);
				currentError = fabs(currentVal - targetVal) / adaptiveRange;
				if (currentVal < targetVal)
					currentError = 0;
				output = pow(fabs(currentError), order_mappingFunc);
				break;
			case 1:
				adaptiveRange = fmax(targetVal, originalRange / 4);
				currentError = fabs(currentVal - targetVal) / adaptiveRange;
				if (fabs(currentError) > toleranceBW)
					output = pow(fabs(currentError), order_mappingFunc);
				break;
			case 2:
				adaptiveRange = fmax(targetVal, originalRange / 4);
				currentError = fabs(currentVal - targetVal) / adaptiveRange;
				if (currentVal > targetVal)
					currentError = 0;
				if (fabs(currentError) > toleranceBW)
					output = pow(fabs(currentError), order_mappingFunc);
				break;
			}

			if (!isFilterFcSet)
				setFilterFc(smoothingFc);
			if (smoothingFc != 100)
				output = paramSmoothing.doBiQuad(output, 0);

			output = quantizeParam(output, numQuantLevels);
			return output;	
	};

	float computeParamValue_SliderSource(float distance, float targetVal, float *functionParamSet)
	{
		float order_mappingFunc = functionParamSet[0];
		float toleranceBW = functionParamSet[1];
		int numQuantLevels = functionParamSet[2];
		float smoothingFc = functionParamSet[3];
		float originalRange = functionParamSet[4];
		short desiredBehavior = functionParamSet[5];

		float currentVal = distance * originalRange;
		float currentError = 0;
		float adaptiveRange = 0;
		float output = 0.0;

		switch (desiredBehavior)
		{
		case 0:
			adaptiveRange = fmax(originalRange - targetVal,originalRange/10);
			currentError = fabs(currentVal - targetVal) / adaptiveRange;
			if (currentVal < targetVal)
				currentError = 0;
				output = pow(fabs(currentError), order_mappingFunc);
			break;
		case 1:
			adaptiveRange = fmax(targetVal,originalRange/4);
			currentError = fabs(currentVal - targetVal) / adaptiveRange;
			if (fabs(currentError) > toleranceBW)
				output = pow(fabs(currentError), order_mappingFunc);
			break;
		case 2:
			adaptiveRange = fmax(targetVal, originalRange / 4);
			currentError = fabs(currentVal - targetVal) / adaptiveRange;
			if (currentVal > targetVal)
				currentError = 0;
			if (fabs(currentError) > toleranceBW)
				output = pow(fabs(currentError), order_mappingFunc);
			break;
		}

		if (!isFilterFcSet)
			setFilterFc(smoothingFc);
		if (smoothingFc != 100)
			output = paramSmoothing.doBiQuad(output, 0);

		output = quantizeParam(output, numQuantLevels);

		return output;
	};

	float getDynamicTarget(float staticTarget, float order, float tempo, float timeElapsed)
	{
		float freq = tempo / 60;
		switch (movementFreqSetting)
		{
		case 1:
			freq /= 8;
			break;
		case 2:
			freq /= 16;
			break;
		}
		float sineSource = sin(2 * M_PI * freq * timeElapsed);
		sineSource = pow(jlimit((float)0.0, (float)1.0, sineSource), order) * dyn_desired_Amplitude;
		
		return staticTarget * sineSource;
	}
	
	float quantizeParam(float currentParamValue, int numQuantizationSteps)
	{
		float quantizedParam = 0;
		if (numQuantizationSteps == 0)
			return currentParamValue;

		else
		{
			float quantizationStepSize = 1 / (float)numQuantizationSteps;

			float diff = 100;
			for (int i = 0; i < numQuantizationSteps + 1; i++)
			{
				float currentStepForTest = 0;
				currentStepForTest += i * quantizationStepSize;
				if (diff > fabs(currentParamValue - currentStepForTest))
				{
					diff = currentParamValue - currentStepForTest;
				}
			}
			quantizedParam = currentParamValue - diff;
			return quantizedParam;
		}
	};
	void setSampleRate(float fs) { 	sampleRate = fs;  }	;
	void setFilterFc(float fc)
	{
		paramSmoothing.calculateLPFCoeffs(fc, 0.7, sampleRate);
		isFilterFcSet = true;
	};

	float sampleRate = 100;
	bool isFilterFcSet = false;
};
