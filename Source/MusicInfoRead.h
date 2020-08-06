#pragma once

#include "MidiTrack.h"
#include "MidiTrack_Drum.h"

class MusicInfoRead
{
public:
	MusicInfoRead() 
	{
		populateDrumBeatLib();
	};
	~MusicInfoRead() {};
	String songName = "";
	
	// Drum Rhythms
	Random randNum;
	String drumLibPath_Base = "D:\\GaitSonification\\MIDI Drum Library\\Base\\";		//MAKE RELATIVE
	String drumLibPath_Cue = "D:\\GaitSonification\\MIDI Drum Library\\Cues\\32nd.mid";	//MAKE RELATIVE
	String MelLibPath = "D:\\GaitSonification\\MIDI Inbuilt Library\\";					//MAKE RELATIVE
	String MelLibFiles[5] = { "1.mid", "2.mid", "3.mid", "4.mid", "5.mid" };
	String nameCode_BaseBeat = "B_";
	MidiTrack_Drum baseBeats[20];
	MidiTrack_Drum snareFlurry;
	short drum_numBase = 0;
	short num_Inbuilt = 5;
	short num_beats_Straight = 0;
	short num_beats_Triplet = 0;
	short num_beats_3by4 = 0;

	// INBUILT FILES - MELODY EMPHASIS
	short inbuilt_TonicOffset1 = 0;
	short inbuilt_Scale1 = 0;
	short inbuilt_TonicOffset2 = 0;
	short inbuilt_Scale2 = 0;
	
	short drum_beatTypes[50] = { 0 };

	void populateDrumBeatLib()
	{
		auto dir_Base = File(drumLibPath_Base);
		auto file_cue_32nd = File(drumLibPath_Cue);
		drum_numBase = dir_Base.getNumberOfChildFiles(2, "*.mid");
		auto drumFiles_Base = dir_Base.findChildFiles(2, false, "*.mid");
		drumFiles_Base.sort();
		File currentFile;
		
		for (int i = 0; i < drum_numBase; i++)						// Load Base Beats From Directory
		{
			currentFile = drumFiles_Base.getUnchecked(i);
			baseBeats[i].name = currentFile.getFileNameWithoutExtension().fromLastOccurrenceOf("_", false, false);
			if (currentFile.getFileName().contains("Triplet"))
			{
				num_beats_Triplet++;
				drum_beatTypes[i] = 1;
			}
			else if (currentFile.getFileName().contains("3by4"))
			{
				num_beats_3by4++;
				drum_beatTypes[i] = 2;
			}
			else
			{
				num_beats_Straight++;
				drum_beatTypes[i] = 0;
			}

			loadMIDIFile_Drum(&baseBeats[i], currentFile);
		}
		loadMIDIFile_Drum(&snareFlurry, file_cue_32nd);
	};

	MidiFile midiObj;
	File midiFile;
	MidiTrack midiTracks[4];
	float finalTimeStamp = 0;
	float midi_ticksPerBeat;
	int numTracksMidi = 0;
	int currentTrack = 0;
	const MidiMessageSequence* midiInfo;
	MidiMessageSequence::MidiEventHolder* midiEventHolder;

	void loadMIDIFile_Drum(MidiTrack_Drum* beatContainer, File currentFile)
	{
		midiFile = currentFile;
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

		beatContainer->flush_infoMatrix();
		midiInfo = midiObj.getTrack(1);
		finalTimeStamp = midiObj.getLastTimestamp();
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

		};
	}

	short getNextBeat(int previousBeat, short timingMode)		// GET next beat for present timing mode
	{
		bool nextHigher = false;
		short nextIndex = 0;
		for (int i = 0; i < drum_numBase; i++)
		{
			if (i > previousBeat && drum_beatTypes[i] == timingMode)
			{
				nextHigher = true;
				nextIndex = i;
				break;
			}
		}
		if (nextHigher == false)
		{
			for (int i = 0; i <= previousBeat; i++)
			{
				if (drum_beatTypes[i] == timingMode)
				{
					nextIndex = i;
					break;
				}
			}
		}
		return nextIndex;
	};			

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
	};
	
};

