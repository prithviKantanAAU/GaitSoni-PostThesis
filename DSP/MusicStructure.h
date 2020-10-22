#pragma once
class MusicStructure
{
public:
	MusicStructure();
	~MusicStructure();

	int numTrackMuteCombinations = 6;
	int melodicMeasuresPerFile = 24;
	int numSongStructures = 5;

	short trackMuteConfigurations[11][8] =
		{
			{0, 0, 0, 0, 0, 0, 0, 0},			// All										// 10	//M
			{0, 0, 0, 0, 0, 0, 0, 0},			// All										// 10	//M
			{0, 0, 0, 0, 0, 0, 0, 0},			// All										// 10	//M
			{0, 0, 0, 0, 0, 0, 0, 0},			// All										// 10	//M
			{0, 0, 0, 0, 0, 0, 0, 0},			// All										// 10	//M
			{0, 0, 0, 0, 0, 0, 0, 0},			// All										// 10	//M
			{0, 0, 0, 0, 0, 0, 0, 0},			// All										// 10	//M
			{0, 0, 0, 0, 0, 0, 0, 0},			// All										// 10	//M
			{0, 0, 0, 0, 0, 0, 0, 0},			// All										// 10	//M
			{0, 0, 0, 0, 0, 0, 0, 0},			// All										// 10	//M
			{0, 0, 0, 0, 0, 0, 0, 0}			// All										// 10	//M
		};

	short muteStructuresPerc_L1[5] = { 0, 1, 2, 3, 4};
	short muteStructuresPerc_L2[4] = { 5, 6, 7, 8 };
	short muteStructuresPerc_L3 = 9;
	short muteStructuresPerc_L4 = 10;

	short songPatterns[5][24]
	{
		{4, 5, 0, 1,   6, 7, 8, 8,   9, 9, 10, 10,   2, 2, 3, 3,   8, 8, 8, 8,   9, 9, 10, 10},
		{0, 0, 1, 1,   5, 5, 7, 7,   8, 9, 10, 10,   1, 2, 3, 3,   6, 7, 8, 4,   9, 9, 10, 10},
		{1, 1, 2, 3,   4, 4, 6, 7,   8, 8, 9, 9,   9, 9, 10, 10,   0, 1, 7, 7,   8, 9, 10, 10},
		{2, 2, 3, 3,   6, 6, 7, 7,   8, 8, 8, 8,   9, 9, 10, 10,   1, 1, 2, 3,   5, 9, 10, 10},
		{0, 1, 7, 8,   3, 3, 9, 10,  5, 5, 7, 7,   8, 8, 8, 8,	   9, 9, 9, 4,   10, 10, 10, 10}		
	};

	short arpMode[24] =
	{
		0, 0, 0, 0 ,
		0, 0, 0, 0 ,
		0, 0, 0, 0 ,
		0, 0, 0, 0 ,
		0, 0, 0, 0 ,
		0, 0, 0, 0 
	};

	int level_Perc[24] = 
	{	0, 0, 1, 1, 
		1, 1, 1, 1, 
		2, 2, 2, 2, 
		2, 2, 3, 3,
		3, 3, 3, 3,
		3, 3, 3, 3
	};
	int mute_Kick[24] = 
	{	1, 1,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0
	};
	int mute_Snare[24] = 
	{	1, 1,
		1, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0
	};
	int mute_HH[24] = 
	{	1, 1,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0
	};
	int mute_Chords[24] = 
	{	0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
	};
	int mute_Riff[24] = 
	{	1, 1,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0
		
	};
	int mute_Melody[24] = 
	{	0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0
	};
	int mute_ChordStabs[24] =
	{	1, 1,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0
	};
};
