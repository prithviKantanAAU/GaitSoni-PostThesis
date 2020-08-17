#include "GaitParameter.h"

GaitParameter::GaitParameter()
{
	mean_overall_long = 0.0;
	mean_overall_short = 0.0;
	mean_left_long = 0.0;
	mean_left_short = 0.0;
	mean_right_long = 0.0;
	mean_right_short = 0.0;
	cov_long = 0.0;
	cov_short = 0.0;
	asymm_long = 0.0;
	asymm_short = 0.0;
	count_L = 0;
	count_R = 0;
}

GaitParameter::~GaitParameter()
{
}

void GaitParameter::addNewValue(float value, bool side)
{
	// L = false & R = true
	if (side == false)
	{
		long_left[count_L] = value;
		short_left[count_L%shortTermWindow] = value;
	}
	else
	{
		long_right[count_R] = value;
		short_right[count_R%shortTermWindow] = value;
	}
}

void GaitParameter::calculateBroadParameters()
{
}
