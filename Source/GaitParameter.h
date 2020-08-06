#pragma once
class GaitParameter
{
public:
	GaitParameter();
	~GaitParameter();

	//Member Variables
	float mean_overall_long;
	float mean_overall_short;
	float mean_left_long;
	float mean_left_short;
	float mean_right_long;
	float mean_right_short;
	float cov_long;
	float cov_short;
	float asymm_long;
	float asymm_short;

	float long_left[10000] = { 0.0 };
	float long_right[10000] = { 0.0 };

	float short_left[10] = { 0.0 };
	float short_right[10] = { 0.0 };

	int shortTermWindow = 2;
	int count_L;
	int count_R;

	//Member Functions
	void addNewValue(float value, bool side);
	void calculateBroadParameters();
};

