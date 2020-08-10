#pragma once
#define barsInMelMeasure 4

#include "../JuceLibraryCode/JuceHeader.h"
#include "DspFaust.h"
#include "MusicInfoRead.h"
#include "MidiTrack_Drum.h"
#include "MusicPhaseCalc.h"
#include "ScaleTonicTrans.h"
#include "mixerSettings.h"
#include "FaustStrings.h"

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
	DspFaust dspFaust;
	MixerSettings mixerSettings;
	FaustStrings faustStrings;
	
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
	float tapTempoCounter = 0;									
	bool isPlaying = false;										
	float timeElapsed_Song = 0;									
	float timeLeft_Song = 0;									
	float timeTotal_Song = 0;
	float tempo = 120;
	float timeElapsed_SONG = 0.0;								// PLAYBACK Time Elapsed
	
	double lastPulseTime = 0.0;									// Last 16th Pulse Time
	double nextPulseTime = 0.0;									// Next 16th Pulse Time
	double ticksPerMS = 0.0;									// MIDI Ticks per ms
	double midiTicksElapsed = 0.0;								// #MIDI Ticks elapsed
	int midiTickIncrement = 240;								// MIDI Tick Increment per 16th note
	double songProgress = 0;									// Song Process Fraction -> Sequencer

	//MIDI
	short numTracks = 8;
	float vels[4][8] = { 0.0 };
	bool note_isOn[4][8] = { false };
	int numVoices[8] = {1, 1, 1, 4, 1, 1, 4, 1};
	bool isPitched[8] = 
	{ 
		false,												// KICK
		false,												// SNARE
		false,												// HH
		true,												// CHORD MAIN
		true,												// BASSLINE
		true,												// MAIN MELODY
		true,												// CHORD HIGH
		false												// CRASH
	};
	int pitches[4][8] = { 0 };
	bool isVel_FromSongFile[8] =
	{
		false,
		false,
		false,
		false,
		false,
		true,
		false,
		false
	};
	int pitchesToMonitor[4][8] =
	{
		{36, 38, 42, 72, 60, 0, 84, 49},
		{0,  0,  0,  73, 0,  0, 85, 0 },
		{0,  0,  0,  74, 0,  0, 86, 0 },
		{0,  0,  0,  75, 0,  0, 87, 0 }
	};
	short trackIdx_to_midiTrack_map[8] = { -1, -1, -1, 1, 2, 0, 1, -1 };

	bool isNewEvents_ToHandle[8] = {false};
	void check_Handle_New_MIDIEvents()
	{
		for (int i = 1; i <= numTracks; i++)
			checkNew_MIDIEvents_SINGLE(i);
		mapNew_MIDIEvents();
	}
	void checkNew_MIDIEvents_SINGLE(int trackIndex);
	void mapNew_MIDIEvents();
	bool infoMapped_CurrentPulse_MIDI = false;

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
	short nextVoiceIndex[8] = { 0 };
	double nextEventTimeStamp[8] = { 0 };

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
	String cue_AP_Name = "";

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

		if (trackIdx_to_midiTrack_map[trackIndex] == 0 && APName == "Mel Degree")
			output *= sqrt(fmax(0.3,fmin(1, musicPhase.emphFunc_Present)));
		return output;
	};

	// ARRANGE PITCHES ASCENDING
	void arrangePitches_Asc(int trackIndex)
	{	
	}


	// DSPFAUST RELATED
	//Start DSPFaust, Initialize Gain -> Sequencer
	void onStartMusic()
	{
		dspFaust.start();
		initializeTrackGains();
	}
	void initializeTrackGains();
	void applyCurrentVariantGain(int trackIndex);
	void setTrackGains(int trackIndex, float value);
	void setTrackMutes(int trackIndex, int value);
	void applyCurrentVariantEQ(int trackIndex);
	void applyCurrentVariantComp(int trackIndex);
	//Set Master Gain -> Sequencer		
	void applyMasterGain(float value)
	{
		mixerSettings.masterGain = value;
		std::string address = faustStrings.baseName + faustStrings.MasterVol;
		dspFaust.setParamValue(address.c_str(), value);
	}
	void switchInstVariant(int trackIndex, int newVariant);
	void setFilename(String name);
	bool handleTapTempoPress();
	void togglePlayPause();
	void stopMusic();
	bool fetch_MusicInfo_Mode_MIDI();
	bool mapMusicInfo_Mode_MIDI();
	// Song Completion Fraction -> Sequencer
	bool getSongProgress()
	{
		switch (musicMode)										// Depends on Music Mode
		{
		case 1:
			songProgress = midiTicksElapsed / currentMusic.finalTimeStamp;
			timeLeft_Song = (currentMusic.finalTimeStamp - midiTicksElapsed) / (1000 * ticksPerMS);
			break;
		case 2:
			songProgress = pulsesElapsed / 1536.0;
			timeLeft_Song = timeTotal_Song * (1 - songProgress);
			break;
		case 3:
			songProgress = midiTicksElapsed / currentMusic.finalTimeStamp;
			timeLeft_Song = (currentMusic.finalTimeStamp - midiTicksElapsed) / (1000 * ticksPerMS);
			break;
		}
		return songProgress >= 1 ? true : false;
	}
};