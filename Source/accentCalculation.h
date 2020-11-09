/*
  ==============================================================================

    accentCalculation.h
    Created: 27 Oct 2020 8:30:02am
    Author:  Prithvi

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class AccentCalculation
{
public:
	AccentCalculation() {};
	~AccentCalculation() {};

	float mel_Degree = 0;
	float mel_Degree_Z1 = 0;
	float mel_Degree_Z2 = 0;
	float chord_Degree_Z1 = 0;

	float rhythmicAccentByPosition[5][2] =
	{
		{3840, 0.25},
		{1920, 0.25},
		{960, 0.18},
		{480, 0.1},
		{240, 0}
	};

	float harmonicTensionByDegree[10] = 
	{
		0.5,
		0.2,
		0.6,
		0.3,
		0.4,
		0.3,
		0.6,
		0.5,
		0.2
	};

	float melodicTensionByDegreeDiff[10] = 
	{
		0,
		0.5,
		0.2,
		0.3,
		0.2,
		0.4,
		0.3,
		0
	};

	float getFinalAccentValue(int trackIndex, int voiceIndex, float degreesMatrix[][8], double timeStamp)
	{
		float contribution_MC = 0;
		float contribution_HC = 0;
		float contribution_A_Metrical = 0;
		float contribution_A_Melodic = 0;
		float contribution_HA = 0;
		float total = 0;

		float degree_Mel = 0;
		float degree_Chord = degreesMatrix[0][3];
		int degree_Diff = 0;

		// CHORDS - HARMONIC ACCENTS
		if (trackIndex == 3)
		{
			// HARMONIC CHARGE
			if (degree_Chord - (int)degree_Chord > 0.4)
				contribution_HC = 0.3;
			else
			contribution_HC = harmonicTensionByDegree[(int)degree_Chord];

			// HARMONIC ACCENT - CHORD CHANGES
			if (degree_Chord != chord_Degree_Z1)
			{
				degree_Diff = abs(degree_Chord - chord_Degree_Z1);
				contribution_HA = 0.5 * pow(degree_Diff / 7, 0.5);
			}

			chord_Degree_Z1 = degree_Chord;
		}

		// MELODY - MELODIC ACCENTS
		if (trackIndex == 5 || trackIndex == 4)
		{
			// MELODIC CHARGE
			degree_Mel = degreesMatrix[0][5];
			if (degree_Mel - (int)degree_Mel > 0.4)
				contribution_A_Melodic = 0.3;
			else
			{
				degree_Diff = (int)abs(degree_Mel - degree_Chord);
				contribution_A_Melodic = pow(melodicTensionByDegreeDiff[degree_Diff],2) * 2;
			}
		}

		// METRICAL ACCENTS
		if (trackIndex == 5)
		{
			for (int i = 0; i < 5; i++)
			{
				if ((int)timeStamp % (int)rhythmicAccentByPosition[i][0] < 10)
				{
					contribution_A_Metrical = rhythmicAccentByPosition[i][1];
					break;
				}
			}
		}

		total = contribution_MC
			//+ contribution_HC 
			+ contribution_A_Metrical
			+ contribution_A_Melodic;
			//+ contribution_HA;
		return fmin(1,total);
	}

	float addRhythmicAccent(int timeStamp, float &velValue)
	{
		for (int i = 0; i < 5; i++)
		{
			if ((int)timeStamp % (int)rhythmicAccentByPosition[i][0] == 0)
			{
				velValue *= (1 + pow(rhythmicAccentByPosition[i][1],1.3) * 2) * 0.8;
				return rhythmicAccentByPosition[i][1];
			}
		}
		return 0;
	}

	float applyHighSharp(int midiKeyNum, float *vel)
	{
		float pitch_Hz = MidiMessage::getMidiNoteInHertz(midiKeyNum);
		float centOffset = jlimit(0, 15, (midiKeyNum - 24) / 2);
		float centOffset_MULT = pow(2, centOffset / 1200);
		float final_pitch = pitch_Hz * centOffset_MULT;

		if (*vel > 1)
		*vel += (midiKeyNum - 48) / 12.0;
		*vel *= 0.7;

		return final_pitch;
	}
};
