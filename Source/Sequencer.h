#pragma once
#define barsInMelMeasure 4

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
	MusicPhaseCalc musicPhase;
	ScaleTonicTrans scaleTonicTrans;
	MusicInfoRead currentMusic;
	MidiTrack_Drum* percObj;
	Random randGen;
	
	void resetCounters();
	short musicMode = 1;
	bool isFileLoaded = false;
	short index_baseBeat = 0;
	
	// FETCH RANDOM INDEX IN INTEGER RANGE SPECIFIED
	int fetchNewRandomIndex(int range)
	{
		return randGen.nextInt(range - 1);
	}

	// LOAD NEW MIDI SONG FILE
	void loadNewFile_MIDI(String name)
	{
		currentMusic.loadMidiFile(name);
		if (!isFileLoaded)
			isFileLoaded = true;
	}

	// BEAT AND BAR COUNTERS
	int pulsesElapsed = -1;	
	int beatsElapsed = -1; 	
	int barsElapsed = -1;
	int beatsElapsed_withinBar = 0;

	// SET RHYTHM TIMING MODE DEPENDING ON SONG NAME
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
		// SET SELECTED RHYTHM TO FIRST CHOICE WITH APPROPRIATE TIMING MODE
		for (int i = 0; i < currentMusic.drum_numBase; i++)
		{
			if (currentMusic.drum_beatTypes[i] == timingMode)
			{
				index_baseBeat = i;
				break;
			}
		}
	}

	// TIMING MODE
	int ticksPerMeasure = 15360;
	short beatsInBar_TimingMode[10] = { 4, 4, 3 };
	short timingMode = 0;
	bool is3by4Mode = false;
	bool isTripletMode = false;

	// INCREMENT BAR AND BEAT COUNTERS
	void incrementPulseCounter();
	
	// RESET MIDI PLAYBACK (EVENT COUNTERS TO ZERO)
	void resetMidiPlayback()
	{
		for (int i = 0; i < 4; i++)
		{
			nextVoiceIndex[i] = 0;
			currentMusic.midiTracks[i].resetPlayback();
		}
	};
	
	// MIDI EVENT HANDLING
	short nextVoiceIndex[4] = { 0 };
	double nextEventTimeStamp[8] = { 0 };
	bool checkMIDIEventsDue(int trackIndex, short numVoices, bool freqOnly,
		double ticksPerMs, double ticksElapsed, float *trackInfoArray, String APName);

	// UPDATE CHOSEN RHYTHM WITH NEXT
	void nextRhythm(short musicMode) 
	{
		index_baseBeat = currentMusic.getNextBeat(index_baseBeat, timingMode);
	};

	// SCALE AND TONIC TRANSFORMATIONS
	short scaleID_ORIG = 0;
	short scaleID_TRANS = 0;
	short tonicOffset_ORIG = 0;
	short tonicOffset_TRANS = 0;

	// OCTAVE LIMIT MIDI NOTE WITHIN TWO BOUNDS
	int midiNoteLimit(short prelimValue, short lowerlim_MIDI, short upperlim_MIDI)
	{
		bool tooLow = lowerlim_MIDI > prelimValue; short tooLow_num = tooLow ? 1 : 0;
		bool tooHigh = upperlim_MIDI < prelimValue; short tooHigh_num = tooHigh ? 1 : 0;
		short octavesUnder = ((lowerlim_MIDI - prelimValue)*tooLow_num / 12) + tooLow_num;
		short octavesOver = ((prelimValue - upperlim_MIDI)*tooHigh_num / 12) + tooHigh_num;
		return prelimValue + 12 * octavesUnder - 12 * octavesOver;
	};

	// RESET PERC MIDI OBJECTS WHEN RHYTHM CHANGED
	void resetPercMIDIOnChange(double midiTicksElapsed)
	{
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

	// COOK LOADED MIDI VELOCITY TO DSPFAUST COMPATIBLE
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