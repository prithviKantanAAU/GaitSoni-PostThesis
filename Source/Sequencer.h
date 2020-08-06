#pragma once

#define barsInMelMeasure 4
#define melMeasuresInMovement 8
#define barsInPercMeasure 1

#include "../JuceLibraryCode/JuceHeader.h"
#include "MusicInfoRead.h"
#include "MidiTrack_Drum.h"
#include "MusicPhaseCalc.h"
#include "ScaleTonicTrans.h"

class Sequencer
{
public:

	Sequencer();
	~Sequencer();
	void initialize();
	void resetCounters();
	MusicPhaseCalc musicPhase;
	Random randGen;
	ScaleTonicTrans scaleTonicTrans;
	short musicMode = 1;
	bool isFileLoaded = false;
	int currentScale = 1;
	short currentMotif = 0;

	void checkMeasureEnd();
	void updateCurrentSelection();

	int fetchNewRandomIndex(int range);

	MusicInfoRead currentMusic;
	MidiTrack_Drum* percObj;

	int numActiveFiles = 1;
	
	void loadNewFile_MIDI(String name)
	{
		currentMusic.loadMidiFile(name);
		if (!isFileLoaded)
			isFileLoaded = true;
	}

	//Counters
	int pulsesElapsed = 0;	int beatsElapsed; 	int barsElapsed;
	int pulsesElapsedWithinBeat = 0;
	int currentMelodicMeasure; 
	int beatsElapsed_withinBar; int beatsElapsed_withinBar_Z1;
	int barsElapsed_withinMeasure; int measuresElapsed_withinMovement;
	int sixteenthCounter_withinMeasure; int sixteenthCounter_withinBar;
	bool barJustCompleted = false;

	void setTimingMode(String path)
	{
		if (path.contains("3by4"))
		{
			timingMode = 2;
			isTripletMode = false;
			is3by4Mode = true;
		}
		else if (path.contains("Triplet"))
		{
			timingMode = 1;
			isTripletMode = true;
			is3by4Mode = false;
		}
		else
		{
			timingMode = 0;
			isTripletMode = false;
			is3by4Mode = false;
		}
		for (int i = 0; i < currentMusic.drum_numBase; i++)
		{
			if (currentMusic.drum_beatTypes[i] == timingMode)
			{
				index_baseBeat = i;
				break;
			}
		}
	}
	short timingMode = 0;
	int ticksPerMeasure = 15360;
	bool is3by4Mode = false;
	bool isTripletMode = false;
	bool barCompleted = false;
	bool percMeasureCompleted = false;
	bool melMeasureCompleted = false;
	bool fileCompleted = false;

	//Clock Increment
	void incrementPulseCounter();
	short nextVoiceIndex[4] = { 0 };
	void resetMidiPlayback()
	{
		for (int i = 0; i < 4; i++)
		{
			nextVoiceIndex[i] = 0;
			currentMusic.midiTracks[i].resetPlayback();
		}
	};
	
	double nextEventTimeStamp[8] = { 0 };
	bool checkMIDIEventsDue(int trackIndex, short numVoices, bool freqOnly,
		double ticksPerMs, double ticksElapsed, float *trackInfoArray, String APName);

	//File Switch
	void openNextFile()
	{
		resetCounters();
	}

	bool lastBarCondition = false;
	//Randomized Indices
	short songStructureIndex = 0;
	short index_baseBeat = 0;
	short index_layer_Fixed = 0;
	short index_layer_Random = 0;
	short hh_Index_Fixed = 0;
	short hh_Index_Random = 0;
	short hhIndex;
	short riffVelIndex;
	short riffOctIndex;
	short stabsIndex;
	short arpVelIndex;
	short arpDegIndex;
	short crashIndex;
	void nextRhythm(short musicMode) 
	{
		switch (musicMode)
		{
		case 1:
			index_baseBeat = currentMusic.getNextBeat(index_baseBeat, timingMode);
			break;
		case 3:
			index_baseBeat = currentMusic.getNextBeat(index_baseBeat, timingMode);
			break;
		}
	};

	short scales[10][10] =
	{
		{-1,0,2,4,5,7,9,11,12,14},		//Major
		{-2,0,2,3,5,7,8,10,12,14},		//Minor
		{-2,0,1,3,5,7,8,10,12,13},		//Phrygian
		{-2,0,2,3,5,7,9,10,12,14},		//Dorian
		{-2,0,2,4,5,7,9,10,12,14},		//Mixolydian
		{-1,0,2,4,6,7,9,11,12,14}		//Lydian
	};

	short chord_2nds[8] = {4,	3,	4,	3,	4,	4,	3,	7};
	short chord_3rds[8] = {7,	7,	7,	7,	7,	7,	7,	12};
	short chord_4ths[8] = {12,	12,	11,	10,	10,	14,	14,	19};

	short scaleID_ORIG = 0;
	short scaleID_TRANS = 0;
	short tonicOffset_ORIG = 0;
	short tonicOffset_TRANS = 0;

	int getMelodyMidiValue(short activeScale, short tonic, short octave, short activeNoteDegree, short lowerlim_MIDI, short upperlim_MIDI)
	{
		int activeInterval = scales[activeScale][activeNoteDegree];
		int preliminaryMidiValue = tonic + 12 * octave + activeInterval;
		return midiNoteLimit(preliminaryMidiValue,lowerlim_MIDI,upperlim_MIDI);
	};

	int midiNoteLimit(short prelimValue, short lowerlim_MIDI, short upperlim_MIDI)
	{
		bool tooLow = lowerlim_MIDI > prelimValue; short tooLow_num = tooLow ? 1 : 0;
		bool tooHigh = upperlim_MIDI < prelimValue; short tooHigh_num = tooHigh ? 1 : 0;
		short octavesUnder = ((lowerlim_MIDI - prelimValue)*tooLow_num / 12) + tooLow_num;
		short octavesOver = ((prelimValue - upperlim_MIDI)*tooHigh_num / 12) + tooHigh_num;
		return prelimValue + 12 * octavesUnder - 12 * octavesOver;
	};

	void fetchChordFreqs(int *chordMIDIArray, short tonic, short activeScale, short chordDegree, short chordType, short chordsOctave, short *lowerlims_MIDI, short *upperlims_MIDI)
	{
		int activeDegree = scales[activeScale][chordDegree];
		short incs[3] = { 0 };
		incs[0] = chord_2nds[chordType];
		incs[1] = chord_3rds[chordType];
		incs[2] = chord_4ths[chordType];

		chordMIDIArray[0] = midiNoteLimit(tonic + 12 * chordsOctave + activeDegree, lowerlims_MIDI[0], upperlims_MIDI[0]);
		for (int i = 1; i <=3; i++)
			chordMIDIArray[i] = midiNoteLimit(chordMIDIArray[0] + incs[i - 1], lowerlims_MIDI[i], upperlims_MIDI[i]);
		
	};

	void resetPercMIDIOnChange(double midiTicksElapsed)
	{
		//int ticksPerMeasure = 16 * currentMusic.midi_ticksPerBeat;
		midiTicksElapsed -= (int)((int)midiTicksElapsed / ticksPerMeasure) * ticksPerMeasure;
		currentMusic.baseBeats[index_baseBeat].Idx_nextEvent = 0;

		for (int i = 0; i < currentMusic.baseBeats[index_baseBeat].numEvents; i++)
		{
			if (midiTicksElapsed < currentMusic.baseBeats[index_baseBeat].infoMatrix[i][3])
			{
				currentMusic.baseBeats[index_baseBeat].Idx_nextEvent = i;
				break;
			}
		}
	};

	float cookMIDIVel(float midiVel, short trackIndex, String APName)
	{
		float output = 0;
		float humanizeAmount = (-30 + fetchNewRandomIndex(60)) / 30.0;
		if (midiVel < 64)
			output = fmin(0 + humanizeAmount + 3 * midiVel / 64.0, 2.999);
		else if (midiVel < 96)
			output = fmin(3 + humanizeAmount + 3 * (midiVel - 64) / 32.0, 5.999);
		else
			output = fmin(6 + humanizeAmount + 3 * (midiVel - 96) / 32.0,9);

		if (trackIndex == 0 && APName == "Mel Degree")
			output *= sqrt(fmax(0.3,fmin(1, musicPhase.emphFunc_Present)));
		return output;
	};
};