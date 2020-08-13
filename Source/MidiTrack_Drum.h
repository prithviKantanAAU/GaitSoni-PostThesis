#pragma once
class MidiTrack_Drum
{
public:
	MidiTrack_Drum() {};
	~MidiTrack_Drum() {};
	String name = "";
	void resetPlayback()
	{
		Idx_nextEvent = 0;
	};
	int numEvents = 0;
	bool isEventNoteOn[500] = { false };
	int infoMatrix[500][4] = { 0 };	// Type - Val - Vel - TS
	int eventCount_ByTrack[8] = { 0 };
	int eventIdx_ByTrack_NEXT[8] = { 0 };
	void flush_nextEventIndices()
	{
		for (int i = 0; i < 8; i++)
			eventIdx_ByTrack_NEXT[i] = 0;
	}
	int eventIdx_ByTrack_ALL[200][8] = { 0 };
	int eventsHandled_ByTrack[8] = { 0 };

	void populateTrackwiseEvents(int numVoices[], int pitchesToMonitor[][8])
	{
		for (int i = 0; i < 8; i++)							// EACH TRACK
		{
			for (int k = 0; k < 500; k++)				// EACH EVENT
				{
				for (int j = 0; j < numVoices[i]; j++)			// EACH VOICE
				{
				if ((pitchesToMonitor[j][i] != 0) && (infoMatrix[k][1] == pitchesToMonitor[j][i]))
					{
						eventIdx_ByTrack_ALL[eventCount_ByTrack[i]][i] = k;
						eventCount_ByTrack[i]++;
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
	int Idx_nextEvent = 0;			
	void incrementEventsHandled(int trackIndex) 
	{ 
		eventIdx_ByTrack_NEXT[trackIndex] = 
			(eventIdx_ByTrack_NEXT[trackIndex] + 1) % (eventCount_ByTrack[trackIndex] - 1);
		int nextEventIndex_TRACK = eventIdx_ByTrack_ALL[eventIdx_ByTrack_NEXT[trackIndex]][trackIndex];
		while (infoMatrix[nextEventIndex_TRACK][0] == 0)
			eventIdx_ByTrack_NEXT[trackIndex]++;
	}
};