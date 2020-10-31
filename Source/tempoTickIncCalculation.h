/*
  ==============================================================================

    tempoTickIncCalculation.h
    Created: 27 Oct 2020 8:30:48am
    Author:  Prithvi

  ==============================================================================
*/
#include "MidiTrack.h"

#pragma once

class TempoTickInc
{
public:
	TempoTickInc() {};
	~TempoTickInc() {};
	float percent_IncChange[10000] = {0.0};			 // CURVE VECTOR - RESOLUTION = 0.01% OF SONG

	// RULE PARAMETERS
	
	// TEMPO RITARDANDO
	float ritardando_percent_song_start = 90;
	float ritardando_percent_tempo_min = 0;
	float ritardando_shape_order = 5;

	// NOISE CONTROL
	Random noiseControl_Gen;
	int noiseControl_Range = 500;

	// FASTER UPHILL
	float fasterUphill_ORDER = 1.2;
	float fasterUphill_SCALEFACTOR = 0.7;

	void generateTempoCurve(MidiTrack *melody)		 // TEMPO RULES WILL BE APPLIED HERE BASED ON THE MELODY
	{
		float songPercent = 0;

		int noteOns[5000][2] = { 0 };
		int noteOns_Total = 0;
		int noteVal_PREV = 0;
		int noteVal_NEXT = 0;
		float noteVal_SLOPE = 0;
		double melodyTicks_CURRENT = 0;

		float contribution_TR = 0;
		float contribution_NC = 0;
		float contribution_FU = 0;

		// EXAMINE MELODY TRACK FOR NOTE ONS
		for (int i = 0; i < melody->numEvents; i++)
		{
			if (melody->infoMatrix[i][0] == 1)
			{
				noteOns[noteOns_Total][0] = melody->infoMatrix[i][1]; // STORE PITCH
				noteOns[noteOns_Total][1] = melody->infoMatrix[i][3]; // STORE TIMESTAMP
				noteOns_Total++;
			}
		}

		for (int i = 0; i < 10000; i++)
		{
			songPercent = i / 100.0;
			melodyTicks_CURRENT = i / 10000.0 * melody->infoMatrix[melody->numEvents - 2][3];

			// APPLY TEMPO RITARDANDO AT END
			contribution_TR = getPercentVal_fromFuncBounds(songPercent, 100,
				ritardando_percent_tempo_min, ritardando_percent_song_start, 100, ritardando_shape_order);
			contribution_TR = fmax(-100, contribution_TR);

			// FASTER UPHILL
			for (int j = 0; j < noteOns_Total - 1; j++)
			{
				if ((noteOns[j][1] <= melodyTicks_CURRENT) && ((noteOns[j + 1][1] > melodyTicks_CURRENT)))
				{
					noteVal_PREV = noteOns[j][0];
					noteVal_NEXT = noteOns[j + 1][0];
					contribution_FU = getPercentVals_fromFuncBounds_UphillONLY(melodyTicks_CURRENT, noteOns[j][1],
						noteOns[j + 1][1], noteVal_PREV, noteVal_NEXT, fasterUphill_ORDER);
				}
			}

			// NOISE CONTROL
			contribution_NC = fetchNewRandomPercent(noiseControl_Range);

			percent_IncChange[i] += contribution_TR + contribution_NC + contribution_FU;
		}
	}

	double getNewTickIncrement(double ticks_Elapsed, double ticks_Total, double tickIncrement_Base)
	{
		double percent_SongComplete = ticks_Elapsed / ticks_Total * 100.0;
		int percent_IncChange_ReadIdx = (int)(percent_SongComplete * 100);
		return tickIncrement_Base * (100.0 + percent_IncChange[percent_IncChange_ReadIdx]) / 100.0;
	};

	float getPercentVal_fromFuncBounds(float percentSong_Present, float percentTempo_Start,
		float percentTempo_End, float percentSong_Start, float percentSong_End, float order)
	{
		if (percentSong_Present > percentSong_Start && percentSong_Present < percentSong_End)
		{
			float percentTempo_changeRange = percentTempo_End - percentTempo_Start;
			float percentSong_changeRange = percentSong_End - percentSong_Start;
			float songPosition_Frac = pow((percentSong_Present - percentSong_Start) / percentSong_changeRange, order);
			return songPosition_Frac * percentTempo_changeRange;
		}
		else return 0;
	};

	float getPercentVals_fromFuncBounds_UphillONLY(float ticksPresent, float ticksStart, float ticksEnd,
		float keyPrev, float keyNext, float order)
	{
		if (keyNext > keyPrev)
		{
			float key_changeRange = keyNext - keyPrev;
			float ticks_changeRange = ticksEnd - ticksStart;
			float songPosition_Frac = pow((ticksPresent - ticksStart) / ticks_changeRange, order);
			float tempoContribution = songPosition_Frac * key_changeRange * fasterUphill_SCALEFACTOR;
			return tempoContribution;
		}
		else return 0;
	}

	void flushIncVector()
	{
		for (int i = 0; i < 10000; i++) percent_IncChange[i] = 0;
	}

	double fetchNewRandomPercent(int range)
	{
		int idx = noiseControl_Gen.nextInt(range - 1);
		return (idx - (range / 2)) / 100.0;
	}
};
