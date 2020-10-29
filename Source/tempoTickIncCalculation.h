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
	float ritardando_percent_tempo_min = 60;
	float ritardando_shape_order = 1.2;

	void generateTempoCurve(MidiTrack *melody)		 // TEMPO RULES WILL BE APPLIED HERE BASED ON THE MELODY
	{
		float songPercent = 0;
		float contribution_TR = 0;
		for (int i = 0; i < 10000; i++)
		{
			songPercent = i / 100.0;

			// APPLY TEMPO RITARDANDO AT END
			contribution_TR = getPercentVal_fromFuncBounds(songPercent, 100,
				ritardando_percent_tempo_min, ritardando_percent_song_start, 100, ritardando_shape_order);

			percent_IncChange[i] += contribution_TR;
		}
	}

	float getNewTickIncrement(double ticks_Elapsed, double ticks_Total, double tickIncrement_Base)
	{
		float percent_SongComplete = ticks_Elapsed / ticks_Total * 100.0;
		int percent_IncChange_ReadIdx = (int)(percent_SongComplete * 100);
		return tickIncrement_Base * (100 + percent_IncChange[percent_IncChange_ReadIdx]) / 100.0;
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

	void flushIncVector()
	{
		for (int i = 0; i < 10000; i++) percent_IncChange[i] = 0;
	}
};
