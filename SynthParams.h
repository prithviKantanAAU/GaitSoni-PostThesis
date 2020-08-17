#pragma once
class SynthParams
{
public:
	SynthParams() {};
	~SynthParams() {};

	short synthMethods[8][8] =
	{
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{1, 0, 0, 0},
		{4, 0, 0, 0},
		{3, 0, 0, 0},
		{2, 0, 0, 0},
		{4, 0, 0, 0},
		{0, 0, 0, 0},
	};

	float kick_Params[10][8] =			//Note: Order Depends on Synthesis Method!!
	{
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0}
	};

	float snare_Params[10][8] =			//Note: Order Depends on Synthesis Method!!
	{
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0}
	};

	float hh_Params[10][8] =			//Note: Order Depends on Synthesis Method!!
	{
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0}
	};

	float chords_Params[10][8] =			//Note: Order Depends on Synthesis Method!!
	{
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0}
	};

	float riff_Params[10][8] =			//Note: Order Depends on Synthesis Method!!
	{
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0}
	};

	float melody_Params[10][8] =			//Note: Order Depends on Synthesis Method!!
	{
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0}
	};

	float chordStabs_Params[10][8] =			//Note: Order Depends on Synthesis Method!!
	{
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0}
	};

	float crash_Params[10][8] =			//Note: Order Depends on Synthesis Method!!
	{
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0},
		{0,		0,		0,		0}
	};
};
