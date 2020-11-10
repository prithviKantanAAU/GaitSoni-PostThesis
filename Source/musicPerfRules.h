/*
  ==============================================================================

    musicPerfRules.h
    Created: 10 Nov 2020 8:54:48am
    Author:  Prithvi

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class MusicPerfRules
{
public:

	MusicPerfRules() {};
	~MusicPerfRules() {};

	String names[20] =
	{
		"Rhythmic Accent",
		"Melodic Charge",
		"High Sharp",
		"High Loud",
		"Tempo Ritardando",
		"Noise Control",
		"Faster Uphill"
	};

	float rules_k[20] =
	{
		0.5,					// RHYTHMIC ACCENT
		1.6,					// MELODIC CHARGE
		1,						// HIGH SHARP
		1,						// HIGH LOUD
		1,						// TEMPO RITARDANDO
		1,						// NOISE CONTROL
		1						// FASTER UPHILL
	};

	// RHYTHMIC ACCENTUATION
	float rhythmicAccent_ByTickPosition[4][2] =
	{
		{3840, 0.4},						// BAR START
		{960, 0.35},						// BEAT
		{480, 0.2},							// 8TH NOTE
		{240, 0}							// 16TH NOTE
	};

	// MELODIC CHARGE
	float melodicCharge_ByDegreeDiff[12] =
	{
		0,
		2,
		4,
		2.5,
		1,
		3,
		5
	};

	// HIGH SHARP
	float highSharp_centsPerOctave = 5;
	int highSharp_baseNoteNum = 24;
	float highSharp_maxIncreaseCents = 20;

	// HIGH LOUD
	int highLoud_baseNoteNum = 48;
	float highLoud_velIncPerOctave = 0.7;
	float highLoud_normFactor = 0.7;

	// TEMPO RITARDANDO
	float ritardando_percent_song_start = 90;
	float ritardando_percent_tempo_min = 0;
	float ritardando_shape_order = 5;

	// TEMPO NOISE CONTROL
	int noiseControl_Range = 500;

	// FASTER UPHILL
	float fasterUphill_ORDER = 1.2;
	float fasterUphill_SCALEFACTOR = 0.7;
};
