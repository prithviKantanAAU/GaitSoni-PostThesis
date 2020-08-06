#include "Sequencer.h"

Sequencer::Sequencer()
{
	initialize();
}

Sequencer::~Sequencer()
{}

void Sequencer::initialize()
{
	resetCounters();
	songStructureIndex = 0;
}

void Sequencer::resetCounters()
{
	pulsesElapsed = 0;
	beatsElapsed = 0;
	barsElapsed = 0;
	currentMelodicMeasure = 1;
	beatsElapsed_withinBar = 0;
	barsElapsed_withinMeasure = 0;
	measuresElapsed_withinMovement = 0;
	sixteenthCounter_withinMeasure = 0;
	sixteenthCounter_withinBar = 0;
	currentMusic.baseBeats[index_baseBeat].Idx_nextEvent = 0;
}

void Sequencer::incrementPulseCounter()
{
	int beatsInBar = 4;
	switch (timingMode)
	{
	case 0:
	{
		beatsInBar = 4;
		pulsesElapsed++;
		break;
	}
	case 1:
	{
		beatsInBar = 4;
		pulsesElapsed += pulsesElapsed % 4 == 3 ? 2 : 1;
		break;
	}
	case 2:
	{
		beatsInBar = 3;
		pulsesElapsed += pulsesElapsed % 16 >= 11 ? 5 : 1;
		break;
	}
	}
	
	//Increment Global Counters
	if ((pulsesElapsed - 1) % 4 == 0 && pulsesElapsed > 4)
	beatsElapsed++;
	if ((pulsesElapsed - 1) % 16 == 0 && pulsesElapsed > 16)
	barsElapsed++;
	if ((pulsesElapsed - 1) % 64 == 0 && pulsesElapsed > 64)
	currentMelodicMeasure++;

	//Increment Positional Counters
	beatsElapsed_withinBar = beatsElapsed % beatsInBar;
	barsElapsed_withinMeasure = barsElapsed % barsInMelMeasure;
	lastBarCondition = (barsElapsed_withinMeasure == 3);

	//Update Local Musical Time Counters
	sixteenthCounter_withinMeasure = (pulsesElapsed - 1) % 64;
	sixteenthCounter_withinBar = (pulsesElapsed - 1) % 16;

	checkMeasureEnd();
	updateCurrentSelection();
}

void Sequencer::checkMeasureEnd()
{
	//Update Booleans
	barCompleted = ((pulsesElapsed - 1) % 16 == 0);
	melMeasureCompleted = ((pulsesElapsed - 1) % 64 == 0);
	percMeasureCompleted = ((pulsesElapsed - 1) % 128 == 0);
	//fileCompleted = (currentMelodicMeasure == 25);
	//currentMelodicMeasure = currentMelodicMeasure > 24 ? 1 : currentMelodicMeasure; // RESET
}

void Sequencer::updateCurrentSelection()
{
	if (barCompleted)
	{
		percMeasureCompleted = false;
		musicPhase.resetPhase();
		musicPhase.isEvenBar = (barsElapsed % 2 == 1);
	}

	if (melMeasureCompleted)
		melMeasureCompleted = false;
}

bool Sequencer::checkMIDIEventsDue(int trackIndex, short numVoices, bool freqOnly, 
	double ticksPerMs, double ticksElapsed, float *trackInfoArray, String APName)
{
	trackIndex--;
	bool isMelodyTrack = trackIndex <= 2;
	int numEvents_toHandle = 0;
	double timeStamp_IntervalStart = ticksElapsed - ticksPerMs;
	double timeStamp_IntervalEnd = ticksElapsed;
	ticksPerMeasure = timingMode != 2 ? 16 * currentMusic.midi_ticksPerBeat : 12 * currentMusic.midi_ticksPerBeat;
	if (!isMelodyTrack)
	{
		timeStamp_IntervalEnd -= (int)((int)timeStamp_IntervalEnd / ticksPerMeasure) * ticksPerMeasure;
		timeStamp_IntervalStart = timeStamp_IntervalEnd - ticksPerMs;
	}
	int nextEventIndex = 0;
	int finalEventIndex = 0;
	double eventTimeStamp = 0;
	float velUncooked = 0;
	short nextVoiceIndex_Current = 0;
	short voiceToTurnOff = 0;
	short percTrackIdx = 0;
	

	if (isMelodyTrack)
	{
		nextEventIndex = currentMusic.midiTracks[trackIndex].Idx_nextEvent;
		finalEventIndex = currentMusic.midiTracks[trackIndex].numEvents;

		for (int i = nextEventIndex; i < finalEventIndex; i++)
		{
			eventTimeStamp = currentMusic.midiTracks[trackIndex].infoMatrix[i][3];
			if (eventTimeStamp >= timeStamp_IntervalStart && eventTimeStamp < timeStamp_IntervalEnd)
				numEvents_toHandle++;
			else break;
		}
		if (numEvents_toHandle == 0)
			return false;

		velUncooked = 0;
		nextVoiceIndex_Current = nextVoiceIndex[trackIndex];
		voiceToTurnOff = 0;

		int originalKey = 0;
		int transformedKey = 0;

		for (int j = nextEventIndex; j < nextEventIndex + numEvents_toHandle; j++)
		{
			if (currentMusic.midiTracks[trackIndex].infoMatrix[j][0] == 1)				//NOTE ON
			{
				switch (musicMode)
				{
				case 1:
					trackInfoArray[nextVoiceIndex_Current] = currentMusic.midiTracks[trackIndex].infoMatrix[j][1]; //KEYNUM
					break;
				case 2:
					break;
				case 3:
					// ADD TRANSFORMATION
					originalKey = currentMusic.midiTracks[trackIndex].infoMatrix[j][1]; //KEYNUM
					transformedKey = scaleTonicTrans.transform_T1S1_TO_T2S2(originalKey, 
						tonicOffset_ORIG, scaleID_ORIG, tonicOffset_TRANS, scaleID_TRANS,
						trackIndex, APName, musicPhase.emphFunc_Present);
					trackInfoArray[nextVoiceIndex_Current] = transformedKey;
					break;
				}
				
				if (!freqOnly)
				{
					trackInfoArray[nextVoiceIndex_Current + numVoices] = 
						cookMIDIVel(currentMusic.midiTracks[trackIndex].infoMatrix[j][2], trackIndex, APName);
				}
				nextVoiceIndex[trackIndex] = (nextVoiceIndex[trackIndex] + 1) % numVoices;
				nextVoiceIndex_Current = nextVoiceIndex[trackIndex];
			}
			if (!freqOnly)		// HANDLE NOTE OFF IF NECESSARY
			{
				if (currentMusic.midiTracks[trackIndex].infoMatrix[j][0] == 2)			// NOTE OFF
				{
					for (int k = 0; k < numVoices; k++)
					{
						if (trackInfoArray[k] == currentMusic.midiTracks[trackIndex].infoMatrix[j][1])
							voiceToTurnOff = k;
					}
					trackInfoArray[voiceToTurnOff + numVoices] = 0;
				}
			}
			currentMusic.midiTracks[trackIndex].incrementEventsHandled();				// INCREMENT EVENT
		}
	}

	else                        //PERC
	{
		switch (trackIndex)
		{
		case 3:
			percObj = &currentMusic.baseBeats[index_baseBeat];
			break;
		case 4:
			break;
		case 5:
			percObj = &currentMusic.snareFlurry;
		}
		nextEventIndex = percObj->Idx_nextEvent;
		finalEventIndex = percObj->numEvents - 1;

		for (int i = nextEventIndex; i < finalEventIndex; i++)
		{
			eventTimeStamp = percObj->infoMatrix[i][3];
			if (eventTimeStamp >= timeStamp_IntervalStart && eventTimeStamp < timeStamp_IntervalEnd)
				numEvents_toHandle++;
			//else break;
		}
		if (numEvents_toHandle == 0)
			return false;

		velUncooked = 0;
		voiceToTurnOff = 0;

		for (int j = nextEventIndex; j < nextEventIndex + numEvents_toHandle; j++)
		{
			if (percObj->infoMatrix[j][0] == 1)				//NOTE ON
			{
				switch ((int)percObj->infoMatrix[j][1])
				{
				case 36: 
					nextVoiceIndex_Current = 0;
					break;
				case 38:
					nextVoiceIndex_Current = 1;
					break;
				case 42:
					nextVoiceIndex_Current = 2;
					break;
				case 49:
					nextVoiceIndex_Current = 3;
					break;
				}
				if (trackIndex == 5)
				{
					velUncooked = percObj->infoMatrix[j][2] * fmin(musicPhase.emphFunc_Present, 1) / 12.7;
					trackInfoArray[nextVoiceIndex_Current] = fmin(9,velUncooked);
				}
				else
				trackInfoArray[nextVoiceIndex_Current] = percObj->infoMatrix[j][2] / 12.7; //COOKED VEL
			}
			
			if (percObj->infoMatrix[j][0] == 2)			// NOTE OFF
			{
				switch ((int)percObj->infoMatrix[j][1])
				{
				case 36:
					nextVoiceIndex_Current = 0;
					break;
				case 38:
					nextVoiceIndex_Current = 1;
					break;
				case 42:
					nextVoiceIndex_Current = 2;
					break;
				case 49:
					nextVoiceIndex_Current = 3;
					break;
				}
				trackInfoArray[nextVoiceIndex_Current] = 0;
			}
			percObj->incrementEventsHandled();				// INCREMENT EVENT
			int a = percObj->Idx_nextEvent;
			int b = a * 1;
		}
	}
	return true;
}

int Sequencer::fetchNewRandomIndex(int range)
{
	return randGen.nextInt(range - 1);
}