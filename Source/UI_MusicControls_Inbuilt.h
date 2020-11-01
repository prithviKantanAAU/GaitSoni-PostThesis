#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class UI_MusicControls_Inbuilt
{
public:

	ComboBox inbuilt_ChooseMIDI;
	ComboBox inbuilt_Tonic;
	ComboBox inbuilt_Scale;
	TextButton melodyDraw[8][16];
	Slider melodyDraw_Vel[16];
	ComboBox chordDegree[4];
	Slider cycleBeatsSixteenth;
	short melodyDraw_selectedBox[16] = { 0 };
	int melodyDraw_Vels[16] = 
	{ 127 ,127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };
	float melodyDraw_colourVals[16] =
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
	Label melodyDraw_TimeMarker;
	ComboBox chordType;

	UI_MusicControls_Inbuilt()
	{
		configure();
	};
	~UI_MusicControls_Inbuilt() {};

	void configure()
	{
		for (int j = 0; j < 16; j++)
		{
			for (int i = 0; i < 8; i++)
			{
				melodyDraw[i][j].setColour(melodyDraw[i][j].buttonColourId, Colours::black);
				if (j % 4 == 0)
					melodyDraw[i][j].setColour(melodyDraw[i][j].buttonColourId, Colours::yellow);
				if (i == 0)
					melodyDraw[i][j].setColour(melodyDraw[i][j].buttonColourId, 
					Colour::fromFloatRGBA(1, 1, 1, melodyDraw_colourVals[j]));
			}

			melodyDraw_Vel[j].setSliderStyle(Slider::SliderStyle::LinearVertical);
			melodyDraw_Vel[j].setSkewFactor(3);
			melodyDraw_Vel[j].setRange(72, 127);
			melodyDraw_Vel[j].setValue(127);
			melodyDraw_Vel[j].setColour(melodyDraw_Vel[j].trackColourId, Colours::yellow);
			melodyDraw_Vel[j].setColour(melodyDraw_Vel[j].backgroundColourId, Colours::blue);
		}

		for (int i = 0; i < 4; i++)
		{
			chordDegree[i].addItem("I", 1);
			chordDegree[i].addItem("II", 2);
			chordDegree[i].addItem("III", 3);
			chordDegree[i].addItem("IV", 4);
			chordDegree[i].addItem("V", 5);
			chordDegree[i].addItem("VI", 6);
			chordDegree[i].addItem("VII", 7);
		}
		chordDegree[0].setSelectedId(1);
		chordDegree[1].setSelectedId(6);
		chordDegree[2].setSelectedId(4);
		chordDegree[3].setSelectedId(5);

		cycleBeatsSixteenth.setRange(1, 16);
		cycleBeatsSixteenth.setValue(16);
		cycleBeatsSixteenth.setColour(cycleBeatsSixteenth.trackColourId, Colours::yellow);
		cycleBeatsSixteenth.setColour(cycleBeatsSixteenth.backgroundColourId, Colours::grey);
		cycleBeatsSixteenth.setTextBoxStyle(Slider::NoTextBox, false, 10, 10);

		melodyDraw_TimeMarker.setColour(melodyDraw_TimeMarker.backgroundColourId, Colours::blue);
	}

	void melodyDraw_updateTimeMarkerBounds(float ticksElapsed)
	{
		int col = (int)(ticksElapsed / 240) % 16;
		melodyDraw_TimeMarker.setBounds(50 + 35 * col, 585, 15, 15);
	}

	void melodyDraw_HandleButtonPress(int row, int col)
	{
		for (int i = 0; i < 8; i++)
		{
			melodyDraw[i][col].setColour(melodyDraw[i][col].buttonColourId, Colours::black);
			if (col % 4 == 0)
				melodyDraw[i][col].setColour(melodyDraw[i][col].buttonColourId, Colours::yellow);
			melodyDraw[row][col].setColour(melodyDraw[row][col].buttonColourId, 
					Colour::fromFloatRGBA(1, 1, 1, melodyDraw_colourVals[col]));
		}

		melodyDraw_selectedBox[col] = row;
	}

	void toggleVisible(bool on)
	{
		inbuilt_ChooseMIDI.setVisible(on);
		inbuilt_Tonic.setVisible(on);
		inbuilt_Scale.setVisible(on);
		if (!on)
		{
			for (int j = 0; j < 16; j++)
			{
				for (int i = 0; i < 8; i++)
				{
					melodyDraw[i][j].setVisible(on);
					melodyDraw_Vel[j].setVisible(on);
				}
			}
		}
		else
		{
			for (int j = 0; j < (int)cycleBeatsSixteenth.getValue() + 1; j++)
			{
				for (int i = 0; i < 8; i++)
				{
					melodyDraw[i][j].setVisible(on);
					melodyDraw_Vel[j].setVisible(on);
				}
			}
		}
		melodyDraw_TimeMarker.setVisible(on);
		for (int i = 0; i < 4; i++) chordDegree[i].setVisible(on);
		cycleBeatsSixteenth.setVisible(on);
		chordType.setVisible(on);
	}

	void melodyDraw_updateSelectedBoxColour(int col)
	{
		float colourVal = melodyDraw_Vels[col] / 127.0;
		melodyDraw_colourVals[col] = pow(colourVal,5);
		int selectedDeg = melodyDraw_selectedBox[col];
		melodyDraw[selectedDeg][col].setColour(melodyDraw[selectedDeg][col].buttonColourId,
			Colour::fromFloatRGBA(1, 1, 1, melodyDraw_colourVals[col]));
	}

	void adjustMelDrawBoxVisibility(int numCols, int currentTab)
	{
		if (currentTab == 1)
		{
			for (int j = 0; j < 16; j++)
			{
				for (int i = 0; i < 8; i++)
				{
					if (j < numCols)
					{
						melodyDraw[i][j].setVisible(true);
						melodyDraw_Vel[j].setVisible(true);
					}
					else
					{
						melodyDraw[i][j].setVisible(false);
						melodyDraw_Vel[j].setVisible(false);
					}
				}
			}
		}
	}

	void setLayout()
	{
		inbuilt_Scale.setBounds(50, 265, 100, 25);
		chordType.setBounds(195, 265, 100, 25);

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 16; j++)
			{
				melodyDraw[i][j].setBounds(50 + 35*j,490 - 25*i, 15, 15);
				melodyDraw_Vel[j].setBounds(50 + 35 * j, 510, 15, 70);
			}
		}
		for (int k = 0; k < 4; k++)
			chordDegree[k].setBounds(50 + 140 * k, 630, 130, 25);
		cycleBeatsSixteenth.setBounds(50, 605, 560, 15);
		melodyDraw_TimeMarker.setBounds(50, 585, 15, 15);
	}
};
