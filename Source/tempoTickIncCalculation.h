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

	float generateTempoCurve(MidiTrack *melody)		 // TEMPO RULES WILL BE APPLIED HERE BASED ON THE MELODY
	{
	}

	float getNewTickIncrement(double ticks_Elapsed, double ticks_Total, double tickIncrement_Base)
	{
		float percent_SongComplete = ticks_Elapsed / ticks_Total;
		int percent_IncChange_ReadIdx = (int)percent_SongComplete * 100;
		return tickIncrement_Base * (100 + percent_IncChange[percent_IncChange_ReadIdx]) / 100;
	};

};
