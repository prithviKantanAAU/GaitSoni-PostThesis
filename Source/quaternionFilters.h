# define M_PI           3.14159265358979323846  /* pi */
#define Kp 2.0f * 5.0f
#define Ki 0.0f

#include "../JuceLibraryCode/JuceHeader.h"

  //#include <Arduino.h>

class QuaternionFilter
{
public:
	QuaternionFilter() {};
	~QuaternionFilter() {};

	float GyroMeasError = M_PI * (40.0f / 180.0f);
	float beta = sqrt(3.0f / 4.0f) * GyroMeasError;
	float q[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
	float eInt[3] = { 0.0f, 0.0f, 0.0f };

	void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy,
		float gz, float mx, float my, float mz,
		float deltat);
	void MahonyQuaternionUpdate(float ax, float ay, float az, float gx, float gy,
		float gz, float mx, float my, float mz,	float deltat);
	float * getQ();
};
