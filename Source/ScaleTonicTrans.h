#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class ScaleTonicTrans
{
public:
	ScaleTonicTrans() {};
	~ScaleTonicTrans() {};

	short numScales = 6;
	short numTonics = 12;
	String scales_Names[10] =
	{
		"Major",
		"Minor",
		"Phrygian",
		"Dorian",
		"Mixolydian",
		"Lydian"
	};
	String tonics_Names[12] =
	{"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
	short tonics_Offsets[12] =
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

	int transform_T1S1_TO_T2S2(int noteNum, short tonic_Offset1,short scaleID1,
		short tonic_Offset2, short scaleID2, short trackIndex, String APName, float emphFunc)
	{
		/*if (scaleID1 == scaleID2)
		{
			return noteNum + (tonic_Offset2 - tonic_Offset1);
		}*/

		short octaveNum = noteNum / 12;
		short noteNum_NORM = noteNum - octaveNum * 12 - tonic_Offset1;

		short degreeDiff[8] = { 0 };
		short minDiff = 12;
		bool isDegreeFound = false;
		short degreeFound = 1;
		for (int i = 1; i <= 8; i++)
		{
			degreeDiff[i - 1] = noteNum_NORM - scales[scaleID1][i];
			if (degreeDiff[i - 1] == 0)
			{
				isDegreeFound = true;
				degreeFound = i;
				minDiff = 0;
				break;
			}
		}

		if (!isDegreeFound)
		{
			for (int i = 1; i <= 8; i++)
			{
				if (abs(degreeDiff[i - 1]) < minDiff)
				{
					minDiff = degreeDiff[i - 1];
					degreeFound = i;
				}
			}
		}

		if (APName == "Mel Degree" && trackIndex == 0)
		{
			octaveNum += (short)emphFunc;
			degreeFound = fmax(1, (int)degreeFound * emphFunc * 8);
			degreeFound %= 8;
		}

		return tonics_Offsets[tonic_Offset2] + 12 * octaveNum + scales[scaleID2][degreeFound] + minDiff;
	}

	short scales[10][10] =
	{
		{-1,0,2,4,5,7,9,11,12,14},		//Major
		{-2,0,2,3,5,7,8,10,12,14},		//Minor
		{-2,0,1,3,5,7,8,10,12,13},		//Phrygian
		{-2,0,2,3,5,7,9,10,12,14},		//Dorian
		{-2,0,2,4,5,7,9,10,12,14},		//Mixolydian
		{-1,0,2,4,6,7,9,11,12,14}		//Lydian
	};

};

