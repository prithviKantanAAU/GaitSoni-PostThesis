#pragma once
class MidiTrack_Drum
{
public:
	MidiTrack_Drum() {};
	~MidiTrack_Drum() {};
	String name = "";
	int numEvents = 0;
	bool isEventNoteOn[500] = { false };
	int infoMatrix[500][4] = { 0 };	// Type - Val - Vel - TS
	int eventCount_ByTrack[8] = { 0 };
	int eventIdx_ByTrack_NEXT[8] = { 0 };
	void flush_allEventIndices()
	{
		for (int i = 0; i < 200; i++)
			for (int j = 0; j < 8; j++)
				eventIdx_ByTrack_ALL[i][j] = 0;
	}
	void flush_nextEventIndices()
	{
		for (int i = 0; i < 8; i++)
			eventIdx_ByTrack_NEXT[i] = 0;
	}
	int eventIdx_ByTrack_ALL[200][8] = { 0 };
	int eventsHandled_ByTrack[8] = { 0 };

	void populateTrackwiseEvents(int numVoices[], int pitchesToMonitor[][8], int ticksPerMeasure)
	{
		flush_nextEventIndices();
		int ticksPerBar = ticksPerMeasure / 4;
		int eventTimeStamp = 0;
		double eventTimeStamp_MOD = 0;
		int barNum = 0;

		for (int i = 0; i < 8; i++)							// EACH TRACK
		{
			eventCount_ByTrack[i] = 0;
			eventIdx_ByTrack_NEXT[i] = 0;
			for (int k = 0; k < 500; k++)				// EACH EVENT
				{
				eventTimeStamp = infoMatrix[k][3];
				barNum = (int)eventTimeStamp / 3840;
				eventTimeStamp_MOD = eventTimeStamp - 3840 * barNum;
				if (eventTimeStamp_MOD < ticksPerBar)
				{
					for (int j = 0; j < numVoices[i]; j++)			// EACH VOICE
					{
						if ((pitchesToMonitor[j][i] != 0) && (infoMatrix[k][1] == pitchesToMonitor[j][i]))
						{
							if (infoMatrix[k][0] != 0)
							{
								eventIdx_ByTrack_ALL[eventCount_ByTrack[i]][i] = k;
								eventCount_ByTrack[i]++;
							}
						}
					}
				}
			}
		}
	}

	void flush_infoMatrix()
	{
		for (int i = 0; i < 500; i++)
		{
			isEventNoteOn[i] = false;
			for (int j = 0; j < 4; j++)
				infoMatrix[i][j] = 0;
		}
	}	
	void incrementEventsHandled(int trackIndex, int ticksPerMeasure) 
	{ 
		if (eventCount_ByTrack[trackIndex] != 0)
		eventIdx_ByTrack_NEXT[trackIndex] = 
			(eventIdx_ByTrack_NEXT[trackIndex] + 1) % (eventCount_ByTrack[trackIndex]);

		int nextIdx = eventIdx_ByTrack_NEXT[trackIndex];

		// HANDLE TIME SIGNATURE

		int timeStamp_nextEvent = infoMatrix[nextIdx][3];
		int barNum_nextEvent = timeStamp_nextEvent / 3840 + 1;
		double timeStamp_nextEvent_MOD = timeStamp_nextEvent - barNum_nextEvent * 3840;
		while (timeStamp_nextEvent_MOD > ticksPerMeasure / 4)
		{
			timeStamp_nextEvent = infoMatrix[nextIdx][3];
			barNum_nextEvent = timeStamp_nextEvent / 3840;
			timeStamp_nextEvent_MOD = timeStamp_nextEvent - barNum_nextEvent * 3840;
			nextIdx++;
		}
		eventIdx_ByTrack_NEXT[trackIndex] = nextIdx % (eventCount_ByTrack[trackIndex]);
	}
};