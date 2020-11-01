#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "MidiTrack.h"
#include "MidiTrack_Drum.h"
#include "MidiTrack_Style.h"

class MusicInfoRead
{
public:
	MusicInfoRead() {};
	~MusicInfoRead() {};

	// MIDI Song Files

	String songName = "";
	MidiFile midiObj;
	File midiFile;
	MidiTrack midiTracks[4];
	float finalTimeStamp = 0;
	float midi_ticksPerBeat;
	int numTracksMidi = 0;
	int currentTrack = 0;
	const MidiMessageSequence* midiInfo;
	MidiMessageSequence::MidiEventHolder* midiEventHolder;

	void loadSongMetadata(String path)
	{
		auto metadataFile = File(path);
		short numLines_Meta = 1;
		short numEntries_perLine[100] = { 2 };

		if (!metadataFile.existsAsFile())
			return;  // file doesn't exist

		juce::FileInputStream inputStream(metadataFile); // [2]

		if (!inputStream.openedOk())
			return;  // failed to open

		for (int i = 0; i < numLines_Meta; i++)
		{
			auto line = inputStream.readNextLine();
			auto line_Rem = line;

			for (int j = 0; j < numEntries_perLine[i]; j++)
			{
				line = line_Rem.upToFirstOccurrenceOf(",", false, true);
				line_Rem = line_Rem.fromFirstOccurrenceOf(",", false, true);

				switch (i)
				{
				case 0:
					if (j == 0) currentKey = line;
					if (j == 1) currentScale = line;
					break;
				}
			}
		}
	}

	void loadMidiFile(String path)						// Load MIDI Song File into JUCE Program Memory
	{
		midiFile = File(path);
		FileInputStream midiFile_inputStream(midiFile);
		if (midiFile_inputStream.openedOk())
		{
			midiObj.readFrom(midiFile_inputStream);
		}
		numTracksMidi = midiObj.getNumTracks();
		midi_ticksPerBeat = midiObj.getTimeFormat();
		MidiMessage currentMidiMessage;
		int noteVal = 0; int noteVel = 0; int timeStamp = 0; int noteType = 1;
		int currentTrack_numMidiEvents = 0;

		for (int i = 1; i < numTracksMidi; i++)					//Each Track
		{
			midiTracks[i - 1].flush_infoMatrix();
			midiInfo = midiObj.getTrack(i);
			finalTimeStamp = midiObj.getLastTimestamp();
			midiTracks[i - 1].numEvents = midiInfo->getNumEvents();
			currentTrack_numMidiEvents = midiInfo->getNumEvents();
			for (int j = 0; j < currentTrack_numMidiEvents; j++)	//Each Event
			{
				midiEventHolder = midiInfo->getEventPointer(j);
				currentMidiMessage = midiEventHolder->message;
				if (currentMidiMessage.isNoteOn())
				{
					midiTracks[i - 1].infoMatrix[j][0] = 1;
					midiTracks[i - 1].isEventNoteOn[j] = true;
					midiTracks[i - 1].infoMatrix[j][1] = currentMidiMessage.getNoteNumber();
					midiTracks[i - 1].infoMatrix[j][2] = currentMidiMessage.getVelocity();
					midiTracks[i - 1].infoMatrix[j][3] = currentMidiMessage.getTimeStamp();
				}
				if (currentMidiMessage.isNoteOff())
				{
					midiTracks[i - 1].infoMatrix[j][0] = 2;
					midiTracks[i - 1].isEventNoteOn[j] = false;
					midiTracks[i - 1].infoMatrix[j][1] = currentMidiMessage.getNoteNumber();
					midiTracks[i - 1].infoMatrix[j][2] = currentMidiMessage.getVelocity();
					midiTracks[i - 1].infoMatrix[j][3] = currentMidiMessage.getTimeStamp();
				}
			}
		}

		String fileName = path.fromLastOccurrenceOf("\\", false, false)
						  .upToFirstOccurrenceOf(".", false, false);
		String path2 = path.upToLastOccurrenceOf("\\", true, true) + fileName + ".csv";
		loadSongMetadata(path2);
	};
	
	// Inbuilt MIDI Song Files

	String MelLibFiles[5] = { "1.mid", "2.mid", "3.mid", "4.mid", "5.mid" };
	short num_Inbuilt = 5;

	// MELODY MODIFY
	short presentDegrees[16] = { 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1 };
	short presentChords[4] = { 1, 6, 4, 5 };
	String currentScale = "";
	String currentKey = "";

	// Drum Rhythms
	
	MidiTrack_Style styles[15];
	short numStyles = 0;
	short style_current = 0;

	// RHYTHM FILES - NOTE ROWS AND NUMBER OF VOICES

	int pitchesToMonitor[4][8] = 	{
		{36, 38, 42, 72, 60, 0, 84, 49},
		{0,  0,  0,  73, 0,  0, 85, 0 },
		{0,  0,  0,  74, 0,  0, 86, 0 },
		{0,  0,  0,  75, 0,  0, 87, 0 }
	};
	int numVoices[8] = { 1, 1, 1, 4, 1, 1, 4, 1 };
	
	bool isStylesPopulated = false;
	void populateStyles(String path)
	{
		auto dir_Base = File(path);
		numStyles = dir_Base.getNumberOfChildFiles(2, "*.mid");
		auto styleFiles_Base = dir_Base.findChildFiles(2, false, "*.mid");
		styleFiles_Base.sort();
		File currentFile;

		for (int i = 0; i < numStyles; i++)						// Load Styles from Defined Directory
		{
			currentFile = styleFiles_Base.getUnchecked(i);
			styles[i].name = currentFile.getFileNameWithoutExtension();
			load_Style_Single(&styles[i], currentFile, path);
		}

		isStylesPopulated = true;
	}

	void load_Style_Single(MidiTrack_Style* styleContainer, File currentFile, String path)
	{
		midiFile = currentFile;
		FileInputStream midiFile_inputStream(midiFile);
		if (midiFile_inputStream.openedOk())
		{
			midiObj.readFrom(midiFile_inputStream);
		}
		numTracksMidi = midiObj.getNumTracks();
		styleContainer->grooves_total = numTracksMidi - 1;
		midi_ticksPerBeat = midiObj.getTimeFormat();
		MidiMessage currentMidiMessage;

		for (int i = 1; i < numTracksMidi; i++)
		{
			midiInfo = midiObj.getTrack(i);
			finalTimeStamp = midiObj.getLastTimestamp();
			load_Groove_Single(&styleContainer->grooves[i - 1], currentFile, path);
			loadMIDIFile_Style_Metadata(path, i - 1);
		}
		
	}

	void load_Groove_Single(MidiTrack_Drum* beatContainer, File currentFile, String path)
	{
		MidiMessage currentMidiMessage;
		int noteVal = 0; int noteVel = 0; int timeStamp = 0; int noteType = 1;
		int currentTrack_numMidiEvents = 0;
		beatContainer->flush_infoMatrix();
		beatContainer->numEvents = midiInfo->getNumEvents();
		currentTrack_numMidiEvents = midiInfo->getNumEvents();

		for (int j = 0; j < currentTrack_numMidiEvents; j++)	//Each Event
		{
			midiEventHolder = midiInfo->getEventPointer(j);
			currentMidiMessage = midiEventHolder->message;
			if (currentMidiMessage.isNoteOn())
			{
				beatContainer->infoMatrix[j][0] = 1;
				beatContainer->isEventNoteOn[j] = true;
				beatContainer->infoMatrix[j][1] = currentMidiMessage.getNoteNumber();
				beatContainer->infoMatrix[j][2] = currentMidiMessage.getVelocity();
				beatContainer->infoMatrix[j][3] = currentMidiMessage.getTimeStamp();
			}
			if (currentMidiMessage.isNoteOff())
			{
				beatContainer->infoMatrix[j][0] = 2;
				beatContainer->isEventNoteOn[j] = false;
				beatContainer->infoMatrix[j][1] = currentMidiMessage.getNoteNumber();
				beatContainer->infoMatrix[j][2] = currentMidiMessage.getVelocity();
				beatContainer->infoMatrix[j][3] = currentMidiMessage.getTimeStamp();
			}
		}
		beatContainer->populateTrackwiseEvents(numVoices, pitchesToMonitor,15360);
	}

	void resetMelodyTrackEventIndices()
	{
		for (int i = 0; i < 3; i++)
		{
			midiTracks[i].resetPlayback();
		}
	}

	void repopulatePercTrackEvents(int ticksPerMeasure)
	{
		styles[style_current].grooves[styles->groove_current].
			populateTrackwiseEvents(numVoices, pitchesToMonitor, ticksPerMeasure);
	}

	// LOAD VARIANT AND GAIN OFFSET METADATA								// MODIFY !!!!!!!!!!!!!!!!!!!
	void loadMIDIFile_Style_Metadata(String path, int styleNum)
	{
		auto metadataFile = File(path + styles[styleNum].name + ".csv");
		short numLines_Meta = 3;
		short numGrooves_Style = styles[styleNum].grooves_total;
		short numEntries_perLine[100] = { 8, 8, numGrooves_Style };
		
		if (!metadataFile.existsAsFile())
			return;  // file doesn't exist

		juce::FileInputStream inputStream(metadataFile); // [2]

		if (!inputStream.openedOk())
			return;  // failed to open

		for (int i = 0; i < numLines_Meta; i++)
		{
			auto line = inputStream.readNextLine();
			auto line_Rem = line;
			short *varConfig = styles[styleNum].variantConfig;
			float *varConfig_GAINS = styles[styleNum].variantConfig_GAINS;
			
			for (int j = 0; j < numEntries_perLine[i]; j++)
			{
				line = line_Rem.upToFirstOccurrenceOf(",", false, true);
				line_Rem = line_Rem.fromFirstOccurrenceOf(",", false, true);

				switch (i)
				{
				case 0:
					varConfig[j] = line.getIntValue();
					break;
				case 1:
					varConfig_GAINS[j] = line.getFloatValue();
					break;
				case 2:
					styles[styleNum].grooves[j].name = line;
					break;
				}
			}
		}
	}
};

