#include "Sequencer.h"

// CONSTRUCTOR - INITIALIZE
Sequencer::Sequencer()
{
	resetCounters();
}

// DESTRUCTOR
Sequencer::~Sequencer()
{}

// RESET TIMING COUNTERS AND EVENT COUNTERS
void Sequencer::resetCounters()
{
	pulsesElapsed = 0;
	beatsElapsed = 0;
	barsElapsed = 0;
	beatsElapsed_withinBar = 0;
	currentMusic.baseBeats[index_baseBeat].Idx_nextEvent = 0;
}

// INCREMENT SIXTEENTH PULSE COUNTER, CHECK BEAT AND BAR COMPLETION
void Sequencer::incrementPulseCounter()
{
	// CHECK BEATS IN BAR, INCREMENT SIXTEENTH PULSE COUNTER
	int beatsInBar = beatsInBar_TimingMode[timingMode];
	pulsesElapsed++;
	
	// CHECK IF BEAT COMPLETED
	if ((pulsesElapsed - 1) % 4 == 0)
	{
		beatsElapsed++;
		beatsElapsed_withinBar = beatsElapsed % beatsInBar;
	}

	// CHECK IF BAR COMPLETED
	if ((pulsesElapsed - 1) % 16 == 0)
	{
		barsElapsed++;
		musicPhase.resetPhase();
		musicPhase.isEvenBar = (barsElapsed % 2 == 1);
	}
}

// FIRST TIME INITIALIZE MUTES, VARIANTS, MASTER GAIN, MAP-----------------MOVE TO SEQUENCER
void Sequencer::initializeTrackGains()
{
	for (int i = 0; i < 8; i++)
	{
		setTrackMutes(i, 1);
		switchInstVariant(i, mixerSettings.currentVariant[index_baseBeat][i]);
	}
	applyMasterGain(mixerSettings.masterGain);
	dspFaust.setParamValue(faustStrings.MasterEQ_1_Q.c_str(), 0.7);
	dspFaust.setParamValue(faustStrings.MasterEQ_2_Q.c_str(), 0.7);
}

// MAP CURRENT VARIANT GAIN - SINGLE TRACK---------------------------------MOVE TO SEQUENCER
void Sequencer::applyCurrentVariantGain(int trackIndex)
{
	std::string address = "";
	float gain = 0;
	short variantNum = mixerSettings.currentVariant[index_baseBeat][trackIndex] - 1;
	address = faustStrings.getTrackGainString(trackIndex);
	gain = mixerSettings.trackGains[trackIndex][variantNum]
		+ mixerSettings.trackGain_Offsets[index_baseBeat][trackIndex];
	dspFaust.setParamValue(address.c_str(), gain);
}

// APPLY AND MAP CURRENT VARIANT GAIN - SINGLE TRACK (on Variant Change)---MOVE TO SEQUENCER
void Sequencer::setTrackGains(int trackIndex, float value)
{
	mixerSettings.trackGain_Offsets[index_baseBeat][trackIndex] = value;
	applyCurrentVariantGain(trackIndex);
}

// APPLY AND MAP MUTE VALUE - SINGLE TRACK---------------------------------MOVE TO SEQUENCER
void Sequencer::setTrackMutes(int trackIndex, int value)
{
	std::string address = faustStrings.baseName + faustStrings.trackMutes[trackIndex];
	dspFaust.setParamValue(address.c_str(), value);
}

// APPLY AND MAP VARIANT EQ - SINGLE TRACK (on Variant Change)-------------MOVE TO SEQUENCER
void Sequencer::applyCurrentVariantEQ(int trackIndex)
{
	short variantNum = mixerSettings.currentVariant[index_baseBeat][trackIndex] - 1;
	float value = 0;
	bool toMap = true;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			std::string address = faustStrings.FetchEQ_String(trackIndex, i, j);
			value = mixerSettings.fetchEQValue(variantNum, trackIndex, i, j);
			dspFaust.setParamValue(address.c_str(), value);
		}
	}
}

// APPLY AND MAP VARIANT COMP - SINGLE TRACK (on Variant Change)-----------MOVE TO SEQUENCER
void Sequencer::applyCurrentVariantComp(int trackIndex)
{
	std::string address = "";
	float value = 0;
	int currentRhythm = index_baseBeat;
	short variantNum = mixerSettings.currentVariant[currentRhythm][trackIndex] - 1;
	for (int j = 0; j < 4; j++)		//Param ID
	{
		address = faustStrings.FetchComp_String(trackIndex, j);
		value = mixerSettings.fetchCompValue(variantNum, trackIndex, j);
		dspFaust.setParamValue(address.c_str(), value);
	}
}

// SWITCH INSTRUMENT VARIANT - SINGLE TRACK--------------------------------MOVE TO SEQUENCER
void Sequencer::switchInstVariant(int trackIndex, int newVariant)
{
	// UPDATE MIXER SETTINGS WITH NEW SETTING
	int currentRhythm = index_baseBeat;
	mixerSettings.currentVariant[currentRhythm][trackIndex] = newVariant;
	//MAP VARIANT TO FAUST
	// CHANNEL SETTINGS
	applyCurrentVariantGain(trackIndex);
	applyCurrentVariantComp(trackIndex);
	applyCurrentVariantEQ(trackIndex);
	// VARIANT NUMBER
	std::string address = faustStrings.baseName + faustStrings.trackVariant_Base + std::to_string(trackIndex + 1);
	dspFaust.setParamValue(address.c_str(), mixerSettings.currentVariant[index_baseBeat][trackIndex]);
}

// LOAD MIDI SONG FILE-----------------------------------------------------MOVE TO SEQUENCER
void Sequencer::setFilename(String name)
{
	stopMusic();
	loadNewFile_MIDI(name);									// Load MIDI Song File from PATH
	togglePlayPause();
}

// HANDLE TAP TEMPO BUTTON PRESS ------------------------------------------MOVE TO SEQUENCER
bool Sequencer::handleTapTempoPress()
{
	float newTempo = 0;
	if (tapTempoCounter >= 1.0)
	{
		tapTempoCounter = 0;
		return false;
	}
	else
	{
		newTempo = 60 / tapTempoCounter;
		tempo = newTempo;
		tapTempoCounter = 0;
		return true;
	}
}

// PAUSE OR PLAY IF FILE HAS BEEN LOADED-----------------------------------MOVE TO SEQUENCER
void Sequencer::togglePlayPause()
{
	if (isPlaying)
	{
		dspFaust.stop();
		isPlaying = false;
	}
	else
	{
		if (isFileLoaded)
		{
			onStartMusic();
			isPlaying = true;
		}
	}
}

// STOP MUSIC, RESET COUNTERS AND TIME-------------------------------------MOVE TO SEQUENCER
void Sequencer::stopMusic()
{
	if (isPlaying)
	{
		isPlaying = false;
		dspFaust.stop();
		resetCounters();
		resetMidiPlayback();
		midiTicksElapsed = 0;
		timeElapsed_SONG = 0;
		nextPulseTime = 0;
		musicPhase.resetPhase();
	}
}

// CHECK FOR NEW MIDI EVENTS ON ALL TRACKS, RETURN TRUE IF NEW EVENT-------MOVE TO SEQUENCER
bool Sequencer::fetch_MusicInfo_Mode_MIDI()
{
	//// CHECK NEW - Tracks 1, 2, 3 ,8 - PERC
	//bool isNew_MIDI_PERC_BASE =
	//	checkMIDIEventsDue(4, 4, false, ticksPerMS, midiTicksElapsed, info_PERC_V_COMMON
	//		, audioParams.audioParam_ObjectArray[audioParams.activeCueParam].name);

	//// CHECK NEW - Track 4 - Chords
	//bool isNew_MIDI_C =
	//	sequencer.checkMIDIEventsDue(2, 4, true, ticksPerMS, midiTicksElapsed, info_C_MIDI
	//		, audioParams.audioParam_ObjectArray[audioParams.activeCueParam].name);
	//if (isNew_MIDI_C) arrangeChordNotes_Asc(info_C_MIDI, 8);					// Arrange in ASC Order

	//// CHECK NEW - Track 5 - Bassline
	//bool isNew_MIDI_R =
	//	sequencer.checkMIDIEventsDue(3, 1, true, ticksPerMS, midiTicksElapsed, info_R_MIDI
	//		, audioParams.audioParam_ObjectArray[audioParams.activeCueParam].name);
	//if (isNew_MIDI_R)															// Limit Note Range
	//	info_R_MIDI[0] = sequencer.midiNoteLimit(info_R_MIDI[0],
	//		mixerSettings.var_noteMin[4][mixerSettings.currentVariant[sequencer.index_baseBeat][4] - 1],
	//		mixerSettings.var_noteMax[4][mixerSettings.currentVariant[sequencer.index_baseBeat][4] - 1]);

	//// CHECK NEW - Track 6 - Main Melody
	//bool isNew_MIDI_M =
	//	sequencer.checkMIDIEventsDue(1, 1, false, ticksPerMS, midiTicksElapsed, info_M_MIDI
	//		, audioParams.audioParam_ObjectArray[audioParams.activeCueParam].name);

	//// SNARE FLURRY - HANDLE IF ACTIVE
	//bool isNew_MIDI_PERC_SnareFlurry = false;
	//if (audioParams.audioParam_ObjectArray[audioParams.activeCueParam].name == "Snare Flurry")
	//{
	//	info_PERC_V_COMMON[1] = 0;
	//	isNew_MIDI_PERC_SnareFlurry = sequencer.checkMIDIEventsDue(6, 4, false, ticksPerMS, midiTicksElapsed, info_PERC_V_COMMON
	//		, audioParams.audioParam_ObjectArray[audioParams.activeCueParam].name);
	//}

	//return isNew_MIDI_M
	//	|| isNew_MIDI_C
	//	|| isNew_MIDI_R
	//	|| isNew_MIDI_PERC_BASE
	//	|| isNew_MIDI_PERC_SnareFlurry;

	return false;
}

// MAP MIDI INFO - MODIFY--------------------------------------------------MOVE TO SEQUENCER
bool Sequencer::mapMusicInfo_Mode_MIDI()
{
	// Will be rewritten in SEQUENCER
	//if (audioParams.audioParam_ObjectArray[audioParams.activeAudioParam].name == "Foot Drum")
	//{
	//	info_PERC_V_COMMON[0] = 9;
	//	info_PERC_V_COMMON[1] = 9;
	//}
	//else {
	//	dspFaust.setParamValue(faustStrings.KickV.c_str(), info_PERC_V_COMMON[0]); //REMOVE
	//	dspFaust.setParamValue(faustStrings.SnareV.c_str(), info_PERC_V_COMMON[1]); //REMOVE
	//}
	//dspFaust.setParamValue(faustStrings.hhV.c_str(), info_PERC_V_COMMON[2]); //REMOVE
	//dspFaust.setParamValue(faustStrings.crashV.c_str(), info_PERC_V_COMMON[3]); //REMOVE

	//dspFaust.setParamValue(faustStrings.CF1.c_str(), sequencer.midiNoteLimit(info_C_MIDI[0], mixerSettings.chordNote_Mins[0], mixerSettings.chordNote_Maxs[0]));
	//dspFaust.setParamValue(faustStrings.CF2.c_str(), sequencer.midiNoteLimit(info_C_MIDI[1], mixerSettings.chordNote_Mins[1], mixerSettings.chordNote_Maxs[1]));
	//dspFaust.setParamValue(faustStrings.CF3.c_str(), sequencer.midiNoteLimit(info_C_MIDI[2], mixerSettings.chordNote_Mins[2], mixerSettings.chordNote_Maxs[2]));
	//dspFaust.setParamValue(faustStrings.CF4.c_str(), sequencer.midiNoteLimit(info_C_MIDI[3], mixerSettings.chordNote_Mins[3], mixerSettings.chordNote_Maxs[3]));

	//dspFaust.setParamValue(faustStrings.CSF1.c_str(), sequencer.midiNoteLimit(info_C_MIDI[0], mixerSettings.chordStab_Mins[0], mixerSettings.chordStab_Mins[0]));
	//dspFaust.setParamValue(faustStrings.CSF2.c_str(), sequencer.midiNoteLimit(info_C_MIDI[1], mixerSettings.chordStab_Mins[1], mixerSettings.chordStab_Mins[1]));
	//dspFaust.setParamValue(faustStrings.CSF3.c_str(), sequencer.midiNoteLimit(info_C_MIDI[2], mixerSettings.chordStab_Mins[2], mixerSettings.chordStab_Mins[2]));
	//dspFaust.setParamValue(faustStrings.CSF4.c_str(), sequencer.midiNoteLimit(info_C_MIDI[3], mixerSettings.chordStab_Mins[3], mixerSettings.chordStab_Mins[3]));

	//dspFaust.setParamValue(faustStrings.RF.c_str(), info_R_MIDI[0]);
	//dspFaust.setParamValue(faustStrings.MV.c_str(), info_M_MIDI[1]);
	//dspFaust.setParamValue(faustStrings.MF.c_str(), info_M_MIDI[0]);

	return true;
}

// CHECK AND HANDLE NEW MIDI EVENTS FOR ALL TRACKS
void Sequencer::check_Handle_New_MIDIEvents()
{
	for (int i = 1; i <= numTracks; i++)
		checkNew_MIDIEvents_SINGLE(i);
	mapNew_MIDIEvents();
}

// CHECK NEW MIDI EVENTS FOR SINGLE TRACK
void Sequencer::checkNew_MIDIEvents_SINGLE(int trackIndex)
{
	// DECREMENT TRACK INDEX AND INITIALIZE EVENT COUNT
	trackIndex--;
	int numEvents_toHandle = 0;

	// DEFINE TICK INTERVAL TO CHECK FOR NEW EVENTS
	double timeStamp_IntervalStart = midiTicksElapsed - ticksPerMS;
	double timeStamp_IntervalEnd = midiTicksElapsed;

	// DEFINE TICK INTERVAL FOR NEW LOOPED EVENTS
	double timeStamp_IntervalStart_MOD = timeStamp_IntervalStart;
	double timeStamp_IntervalEnd_MOD = timeStamp_IntervalEnd;

	// SET TICKS PER BAR DEPENDING ON TIME SIGNATURE 4/4 OR 3/4
	ticksPerMeasure = timingMode != 2 ? 16 * currentMusic.midi_ticksPerBeat : 12 * currentMusic.midi_ticksPerBeat;

	// IF VEL LOOPED, ADJUST TICK INTERVALS TO BE WITHIN BAR LENGTH (SUBTRACT ELAPSED BARS FROM TOTAL TICKS)
	if (!isVel_FromSongFile[trackIndex])
	{
		timeStamp_IntervalEnd_MOD -= (int)((int)timeStamp_IntervalEnd / ticksPerMeasure) * ticksPerMeasure;
		timeStamp_IntervalStart_MOD = timeStamp_IntervalEnd - ticksPerMS;
	}

	// INITIALIZE EVENT INDICES
	int nextEventIndex = 0;
	int finalEventIndex = 0;
	double eventTimeStamp = 0;

	// INITIALIZE VOICE VALUES
	short voiceToTurnOff = 0;

	// INITIALIZE ORIGINAL AND TRANSFORMED MIDI KEY NUMBER
	int originalKey = 0;
	int transformedKey = 0;

	// INITIALIZE MISC
	float velUncooked = 0;
	percObj = &currentMusic.baseBeats[index_baseBeat];

	// PITCHED TRACKS - TAKE CARE OF PITCH SETTING, NOTE ON AND NOTE OFF WHERE NEEDED
	if (isPitched[trackIndex])
	{
		// FETCH NEXT AND FINAL INDICES - MAP TRACK IDX TO MIDI TRACK IDX
		nextEventIndex = currentMusic.midiTracks[trackIdx_to_midiTrack_map[trackIndex]].Idx_nextEvent;
		finalEventIndex = currentMusic.midiTracks[trackIdx_to_midiTrack_map[trackIndex]].numEvents;

		// CHECK ALL EVENTS - DO THEY FIT WITHIN BOUNDS? ANYTHING TO HANDLE?
		for (int i = nextEventIndex; i < finalEventIndex; i++)
		{
			// FETCH NEXT EVENT TIME STAMP
			eventTimeStamp = currentMusic.midiTracks[trackIdx_to_midiTrack_map[trackIndex]].infoMatrix[i][3];

			// CHECK IF IT LIES WITHIN BOUNDS
			if (eventTimeStamp >= timeStamp_IntervalStart && eventTimeStamp < timeStamp_IntervalEnd)
				numEvents_toHandle++;

			// IF IT EXCEEDS INTERVAL END, EXIT FOR LOOP
			if (eventTimeStamp >= timeStamp_IntervalEnd) break;
		}

		// IF NOTHING TO HANDLE, LEAVE FIRST PART. IF SOMETHING, THEN SET FLAG TRUE
		if (numEvents_toHandle == 0)
		{
			isNewEvents_ToHandle[trackIndex] = false;
		}
		else isNewEvents_ToHandle[trackIndex] = true;

		// IF THERE ARE PITCH / FILE VEL EVENTS TO HANDLE
		if (isNewEvents_ToHandle[trackIndex] == true)
		{
			// INITIALIZE VELOCITY, VOICE TO TURN OFF, NEXT VOICE INDEX FOR PRESENT TRACK
			velUncooked = 0;
			voiceToTurnOff = 0;

			// HANDLE ALL EVENTS THAT ARE TO BE HANDLED
			for (int j = nextEventIndex; j < nextEventIndex + numEvents_toHandle; j++)
			{
				// IF EVENT IS A NOTE ON EVENT
				if (currentMusic.midiTracks[trackIdx_to_midiTrack_map[trackIndex]].infoMatrix[j][0] == 1)
				{
					// HANDLE PITCH INFO
					switch (musicMode)
					{
					case 1:
						pitches[nextVoiceIndex[trackIndex]][trackIndex] = 
							currentMusic.midiTracks[trackIdx_to_midiTrack_map[trackIndex]].infoMatrix[j][1];
						break;
					case 2:
						break;
					case 3:
						// ADD TRANSFORMATION
						originalKey = currentMusic.midiTracks
							[trackIdx_to_midiTrack_map[trackIndex]].infoMatrix[j][1];
						transformedKey = scaleTonicTrans.transform_T1S1_TO_T2S2(originalKey,
							tonicOffset_ORIG, scaleID_ORIG, tonicOffset_TRANS, scaleID_TRANS,
							trackIndex, cue_AP_Name, musicPhase.emphFunc_Present);
						pitches[nextVoiceIndex[trackIndex]][trackIndex] = transformedKey;
						break;
					}

					// IF VELOCITY IS FROM SONG FILE, TAKE IT FROM HERE 
					if (isVel_FromSongFile[trackIndex])
					{
						vels[nextVoiceIndex[trackIndex]][trackIndex] =
							cookMIDIVel(currentMusic.midiTracks[trackIdx_to_midiTrack_map[trackIndex]].
								infoMatrix[j][2], trackIndex, cue_AP_Name);
					}
					nextVoiceIndex[trackIndex] = (nextVoiceIndex[trackIndex] + 1) % numVoices[trackIndex];
				}

				// IF EVENT IS A NOTE OFF EVENT
				if (currentMusic.midiTracks[trackIdx_to_midiTrack_map[trackIndex]].infoMatrix[j][0] == 2)
				{
					// IF VELOCITY IS FROM SONG FILE, TAKE IT FROM HERE
					if (isVel_FromSongFile[trackIndex])
					{
						for (int k = 0; k < numVoices[trackIndex]; k++)
						{
							if (pitches[k][trackIndex] == currentMusic.
								midiTracks[trackIdx_to_midiTrack_map[trackIndex]].infoMatrix[j][1])
								voiceToTurnOff = k;
							vels[voiceToTurnOff][trackIndex] = 0;
						}
					}
				}

				currentMusic.midiTracks[trackIdx_to_midiTrack_map[trackIndex]].incrementEventsHandled();				// INCREMENT EVENT

				// ARRANGE VOICES ASCENDING IF GREATER THAN 1 VOICE
				if (numVoices[trackIndex] > 1) arrangePitches_Asc(trackIndex); // !!!!!!!!!!!!!!!!
			}
		}
	}

	// ALL LOOPED TRACKS - TAKE CARE OF VELOCITY
	if (!isVel_FromSongFile[trackIndex])
	{
		nextEventIndex = percObj->Idx_nextEvent;
		finalEventIndex = percObj->numEvents - 1;

		// FIND NUMBER OF EVENTS TO HANDLE
		for (int i = nextEventIndex; i < finalEventIndex; i++)
		{
			eventTimeStamp = percObj->infoMatrix[i][3];
			if (eventTimeStamp >= timeStamp_IntervalStart_MOD && eventTimeStamp < timeStamp_IntervalEnd_MOD)
				numEvents_toHandle++;
		}

		// RETURN IF ZERO EVENTS TO HANDLE - EXIT FUNCTION
		if (numEvents_toHandle == 0)
		{
			isNewEvents_ToHandle[trackIndex] = false;
			return;
		}
		else isNewEvents_ToHandle[trackIndex] = true;

		// IF PERC TRACK EVENTS TO HANDLE, THEN HANDLE THEM
		if (isNewEvents_ToHandle[trackIndex] == true)
		{
			velUncooked = 0;
			voiceToTurnOff = 0;

			for (int j = nextEventIndex; j < nextEventIndex + numEvents_toHandle; j++)
			{
				//HANDLE NOTE ON - CHECK DESIGNATED PITCH ROWS
				if (percObj->infoMatrix[j][0] == 1)
				{
					for (int i = 0; i < numVoices[trackIndex]; i++)
					{
						if (pitchesToMonitor[i][trackIndex] != 0)
						{
							if (pitchesToMonitor[i][trackIndex] == (int)percObj->infoMatrix[j][1])
								vels[i][trackIndex] = cookMIDIVel(percObj->infoMatrix[j][2], trackIndex, cue_AP_Name);
						}
					}
				}

				//HANDLE NOTE OFF - CHECK DESIGNATED PITCH ROWS
				if (percObj->infoMatrix[j][0] == 2)
				{
					for (int i = 0; i < numVoices[trackIndex]; i++)
					{
						if (pitchesToMonitor[i][trackIndex] != 0)
						{
							if (pitchesToMonitor[i][trackIndex] == (int)percObj->infoMatrix[j][1])
								vels[i][trackIndex] = 0;
						}
					}
				}

				percObj->incrementEventsHandled();				// INCREMENT EVENT COUNT
			}
		}
	}
}
// MAP NEW MIDI EVENT FOR REQUIRED TRACKS
void Sequencer::mapNew_MIDIEvents()
{
	for (int presentTrack = 1; presentTrack <= numTracks; presentTrack++)
	{
		if (isNewEvents_ToHandle[presentTrack - 1])
		{
			for (int currentVoice = 1; currentVoice < numVoices[presentTrack - 1]; currentVoice++)
			{
				// WRITE DSPFAUST MAPPING CODE
				// VELOCITY
				if (!note_isOn[currentVoice - 1][presentTrack - 1])
				{
					// MAP VELOCITY AS ZERO
				}
				else
				{
					// MAP VELOCITY AS VALUE IN VELOCITY ARRAY
				}
				
				// PITCH
				if (isPitched[presentTrack - 1])
				{
				}
			}
		}
	}
}