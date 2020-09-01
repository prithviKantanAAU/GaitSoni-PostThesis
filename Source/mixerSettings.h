#pragma once
class MixerSettings
{
public:
	MixerSettings() {};
	~MixerSettings() {};

	String varNames[8][3] =
	{
		{"Acoustic Kick", "Electro Kick", "Alt Kick"},
		{"Acoustic Snare", "Clap", "Alt Snare"},
		{"Acoustic Hats", "Acoustic Ride", "Marimba"},
		{"Acoustic Piano", "Xylophone", "Guitar"},
		{"Deep Bass", "Electro Bass", "Dist Bass"},
		{"Basic Lead", "Warm Lead", "Trumpet"},
		{"Synth Pluck", "Warm Pad", "Bright Pad"},
		{"Crash 1", "Crash 2", "Crash 3"}
	};

	short var_noteMins[3][4][8] =
	{
		// VARIANT 1
		{
			{0,0,0,50,24,10,63,0},
			{0,0,0,53,0 ,0 ,65,0},
			{0,0,0,56,0 ,0 ,68,0},
			{0,0,0,62,0 ,0 ,74,0}
		},

		{
			{0,0,0,50,36,10,63,0},
			{0,0,0,53,0 ,0 ,65,0},
			{0,0,0,56,0 ,0 ,68,0},
			{0,0,0,62,0 ,0 ,74,0}
		},

		{
			{0,0,0,50,30,10,63,0},
			{0,0,0,53,0 ,0 ,65,0},
			{0,0,0,56,0 ,0 ,68,0},
			{0,0,0,62,0 ,0 ,74,0}
		}
	};

	short var_noteMaxs[3][4][8] =
	{
		// VARIANT 1
		{
			{0,0,0,61,35,80,74,0},
			{0,0,0,64,0 ,0 ,77,0},
			{0,0,0,67,0 ,0 ,80,0},
			{0,0,0,73,0 ,0 ,86,0}
		},

		{
			{0,0,0,61,48,80,74,0},
			{0,0,0,64,0 ,0 ,77,0},
			{0,0,0,67,0 ,0 ,80,0},
			{0,0,0,73,0 ,0 ,86,0}
		},

		{
			{0,0,0,61,41,80,74,0},
			{0,0,0,64,0 ,0 ,77,0},
			{0,0,0,67,0 ,0 ,80,0},
			{0,0,0,73,0 ,0 ,86,0}
		}
	};

	float masterGain = -16;

	float trackGains[8][3] =
	{
		{12,		5,		16},
		{8,		2,		11},
		{10,		17,		13},
		{0,		8,		33},
		{7,		10,		13},
		{11,	13,		11},
		{13,		2,		-5},
		{5,		5,		5}
	};

	float eqSettings[3][8][12] =
	{
		{										//VARIANT 1
			{54, 0, 3.0,	87, 4, 0.7,			2321, 9.4, 0.56,	20000, 0, 0.7},		//DT
			{77, 0, 2.52,	200, 3.19, 0.7,		4927, 4.94, 0.7,	20000, 0, 0.7},		//DT
			{3140, 0, 2.07,	200, 0, 0.7,		12721, 8.1, 0.7,	20000, 0, 0.7},		//DT
			{80, 0, 0.7,	341, 5.2, 0.7,		2201, -.63, 0.7,	10000, 0, 0.7},		//DT
			{120, 0, 3.19,	110, 7.44, 0.3,		1154, -1.81, 0.7,	1666, 1.69, 4},		//DT
			{150, 0, 0.7,	341, 11.5 , 0.33,	2709, 5.7, 0.7,		5138, 0, 2.7},		//DT
			{200, 0, 0.7,	660, 10, 0.7,		4523, 0, 3.07,		4161, 0, 4},		//DT
			{9000, 0, 1.7,	800, -5, 0.7,		9000, 5, 0.7,		20000, 0, 0.7}		//DT
		},

		{										//VARIANT 2
			{60, 0, 3.0,	123, -5, 5,			500, 3.09, 0.7,		1000, 0, 0.7},		//DT
			{211, 0, 3.42,	5137, 5.2, 0.7,		1000, 7, 0.7,		16000, 0, 0.7},		//DT
			{2575, 0, 2.8,	200, 0, 0.7,		9000, 3, 0.7,		20000, 0, 0.7},		//DT
			{138, 0, 2.6,	261, 8.13, 0.7,		2848, -6.81, 0.49,	3787, 0, 4.48},		//DT
			{117, 0, 3,		138, 8.2, 0.3,		400, -4, 0.7,		901, 0, 3.5},		//DT
			{211, 0, 2.41,	311, 6 , 0.56,		1706, 7.82, 0.7,	3106, 0, 3.11},		//DT
			{522, 0, 1.6,	800, -4, 0.4,		883, 1.94, 0.7,		4000, 0, 0.7},		//DT
			{9000, 0, 1.7,	800, -5, 0.7,		9000, 5, 0.7,		20000, 0, 0.7}		//DT
		},

		{										//VARIANT 3
			{61, 0, 3.5,	800, 6.2, 0.7,		4000, -5.06, 0.7,	20000, 0, 0.7},		//DT
			{50, 0, 0.7,	250, 0, 0.7,		1000, 0, 0.7,		16000, 0, 0.7},
			{283, 0, 2.9,	200, 0, 0.7,		2000, -5, 0.7,		20000, 0, 0.7},		//DT
			{171, 0, 2.7,	412, 13.4, 0.7,		1000, 2, 0.7,		2848, 0, 2.05},		//DT
			{87, 0, 3.35,	171, 8.5, 0.3,		2446, -8, 0.6,		4000, 0, 2.7},		//DT
			{171, 0, 2.41,	520, 4.2 , 0.37,	3713, 6.5, 0.4,		5000, 0, 1.62},		//DT
			{200, 0, 2.7,	1394, -6, 1.2,		2575, 4.9, 0.4,		6288, 0, 2.7},		//DT
			{9000, 0, 1.7,	800, -5, 0.7,		9000, 5, 0.7,		20000, 0, 0.7}		//DT
		}
	};

	float compSettings[3][8][4] =
	{
		{									//Variant 1
			{-17, 6.4, 0.05, 0.36},		//DT
			{-7, 5.3, 0.05, 0.19},	//DT
			{-30, 6.8, 0.03, 0.09},		//DT
			{-12, 1, 0.10, 1.00},		//DT
			{-12, 1, 10, 100},			//DT
			{-29, 6.4, 0.04, 0.37},		//DT
			{-7.25, 6, 0.05, 0.1},		//DT
			{-12, 1, 10, 100}			//DT
		},

		{									//Variant 2
			{-9, 6.42, 0.05, 0.39},		//DT
			{-23.5, 5.6, 0.05, 0.41},		//DT
			{-26, 6.4, 0.02, 0.22},			//DT
			{-19, 4, 0.04, 0.22},			//DT
			{0, 1, 10, 100},				//DT
			{-9.58, 6, 0.1, 1.0},			//DT
			{-10, 6, 5, 400},				//DT
			{0, 1, 10, 100}					//DT
		},

		{									//Variant 3
			{-25.25, 5.74, 0.05, 0.47},		//DT
			{-6.21, 6.3, 0.05, 0.31},
			{-12.92, 10, 0.03, 0.42},		//DT
			{-8, 6, 0.03, 0.2},	//DT
			{0, 1, 0.1, 1},			//DT
			{0, 1, 10, 100},		//DT
			{-10, 6, 5, 400},
			{0, 1, 10, 100}		//DT
		}
	};


	float fetchEQValue(int styleID, int trackID, int filterID, int paramID)
	{
		return eqSettings[styleID][trackID][3 * filterID + paramID];
	};

	float fetchCompValue(int styleID, int trackID, int paramID)
	{
		return compSettings[styleID][trackID][paramID];
	};

};