#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "musicPerfRules.h"

class AccentCalculation
{
public:
	AccentCalculation() {};
	~AccentCalculation() {};

	float getFinalAccentValue(int trackIndex, int voiceIndex,
		float degreesMatrix[][8], double timeStamp, MusicPerfRules *ruleSet)
	{
		float contribution_MC = 0;
		float contribution_RA = 0;
		float total = 0;

		float degree_Mel = 0;
		float degree_Chord = degreesMatrix[0][3];
		int degree_Diff = 0;

		// MELODY - MELODIC ACCENTS
		if (trackIndex == 5 || trackIndex == 4)
		{
			// MELODIC CHARGE
			degree_Mel = degreesMatrix[0][5];
			degree_Diff = (int)abs(degree_Mel - degree_Chord);

			if (degree_Mel - (int)degree_Mel > 0.4)
				contribution_MC = 2 + ruleSet->melodicCharge_ByDegreeDiff[(int)degree_Diff] / 8.5;
			else
				contribution_MC = ruleSet->melodicCharge_ByDegreeDiff[(int)degree_Diff] / 8.5;
		}

		// METRICAL ACCENTS
		if (trackIndex == 5)
		{
			for (int i = 0; i < 4; i++)
			{
				if ((int)timeStamp % (int)ruleSet->rhythmicAccent_ByTickPosition[i][0] < 10)
				{
					contribution_RA = ruleSet->rhythmicAccent_ByTickPosition[i][1];
					break;
				}
			}
		}

		total = ruleSet->rules_k[1] * contribution_MC +
				ruleSet->rules_k[0] * contribution_RA;
			
		return fmin(2,total);
	}

	float addRhythmicAccent(int timeStamp, float &velValue, MusicPerfRules *ruleSet)
	{
		for (int i = 0; i < 4; i++)
		{
			if ((int)timeStamp % (int)ruleSet->rhythmicAccent_ByTickPosition[i][0] == 0)
			{
				velValue *= (1 + pow(ruleSet->rhythmicAccent_ByTickPosition[i][1],1.3) * 2) * 0.8;
				return ruleSet->rhythmicAccent_ByTickPosition[i][1];
			}
		}
		return 0;
	}

	float applyHighSharp(int midiKeyNum, float *vel, MusicPerfRules *ruleSet, int presentTrackIdx)
	{
		float pitch_Hz = MidiMessage::getMidiNoteInHertz(midiKeyNum);
		float centOffset = jlimit
		(0.0, (double)ruleSet->highSharp_maxIncreaseCents, 
		(midiKeyNum - ruleSet->highSharp_baseNoteNum) / 12.0 
			* ruleSet->highSharp_centsPerOctave * ruleSet->rules_k[2]);

		float centOffset_MULT = pow(2, centOffset / 1200);
		float final_pitch = (presentTrackIdx == 6) ? pitch_Hz * centOffset_MULT : pitch_Hz;

		if (*vel > 1)
		*vel += ruleSet->highLoud_velIncPerOctave * (midiKeyNum - ruleSet->highLoud_baseNoteNum) / 12.0;
		*vel *= ruleSet->highLoud_normFactor * ruleSet->rules_k[3];

		return final_pitch;
	}
};
