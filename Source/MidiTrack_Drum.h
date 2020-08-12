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
	void incrementEventsHandled() 
	{ 
		//Idx_nextEvent++;
		Idx_nextEvent = (Idx_nextEvent + 1)%(numEvents - 1);
		while (infoMatrix[Idx_nextEvent][0] == 0)
			Idx_nextEvent++;
		//Idx_nextEvent += (Idx_nextEvent == 0) ? 1 : 0;
		//int a = 0;
	}
};