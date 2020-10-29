#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "MidiTrack_Drum.h"

class MidiTrack_Style
{
public:
	MidiTrack_Style() {};
	~MidiTrack_Style() {};

	// BASIC STYLE PROPERTIES
	String nameCode_BaseBeat = "B_";
	String name = "";
	short variantConfig[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };
	float variantConfig_GAINS[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	short grooves_total = 0;
	short groove_current = 0;
	short groove_types[15] = { 0 };

	// NUMBER OF BEATS OF EACH TYPE WITHIN STYLE
	short num_beats_Straight = 0;
	short num_beats_Triplet = 0;
	short num_beats_3by4 = 0;

	// GROOVE INFORMATION EXTRACTED FROM MIDI TRACKS
	MidiTrack_Drum grooves[15];
};
