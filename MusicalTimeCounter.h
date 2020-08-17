#pragma once

#define beatsInBar 4
#define barsInMelMeasure 4
#define melMeasuresInMovement 8
#define barsInPercMeasure 1

class MusicalTimeCounter
{
public:
	MusicalTimeCounter()
	{
		initialize();
	};
	
	~MusicalTimeCounter()
	{};
	
	int pulsesElapsed;
	int beatsElapsed;
	int barsElapsed;
	int melodicMeasuresElapsed;
	int beatsElapsed_withinBar;
	int barsElapsed_withinMeasure;
	int measuresElapsed_withinMovement;
	int sixteenthCounter_withinMeasure;
	int sixteenthCounter_withinBar;

	bool barStarted = false;
	bool melodyMeasureStarted = false;
	bool percMeasureStarted = false;

	void initialize() 
	{
		pulsesElapsed = 0;
		beatsElapsed = 0;
		barsElapsed = 0;
		melodicMeasuresElapsed = 0;
		beatsElapsed_withinBar = 0;
		barsElapsed_withinMeasure = 0;
		measuresElapsed_withinMovement = 0;
		sixteenthCounter_withinMeasure = 0;
		sixteenthCounter_withinBar = 0;
	}

	void incrementPulseCounter()
	{
		pulsesElapsed += 1;
		
		//Increment Global Counters
		beatsElapsed = (pulsesElapsed - 1) / 4;
		barsElapsed = (beatsElapsed - 1) / beatsInBar;
		melodicMeasuresElapsed = barsElapsed / barsInMelMeasure;

		//Increment Positional Counters
		beatsElapsed_withinBar = beatsElapsed % beatsInBar;
		barsElapsed_withinMeasure = barsElapsed % barsInMelMeasure;
		measuresElapsed_withinMovement = melodicMeasuresElapsed % melMeasuresInMovement;

		//Update Booleans
		barStarted = ((pulsesElapsed-1)%16 == 0);
		melodyMeasureStarted = ((pulsesElapsed - 1) % 64 == 0);
		percMeasureStarted = ((pulsesElapsed - 1) % 128 == 0);

		//Update Local Musical Time Counters
		sixteenthCounter_withinMeasure = (pulsesElapsed - 1) % 64;
		sixteenthCounter_withinBar = (pulsesElapsed - 1) % 16;
	}

};