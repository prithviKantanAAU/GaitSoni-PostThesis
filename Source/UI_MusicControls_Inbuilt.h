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
	short melodyDraw_selectedBox[16] = { 0 };
	int melodyDraw_Vels[16] = 
	{ 127 ,127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };
	float melodyDraw_colourVals[16] =
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
	Label melodyDraw_TimeMarker;

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
		for (int j = 0; j < 16; j++)
		{
			for (int i = 0; i < 8; i++)
			{
				melodyDraw[i][j].setVisible(on);
				melodyDraw_Vel[j].setVisible(on);
			}
		}
		melodyDraw_TimeMarker.setVisible(on);
	}

	void melodyDraw_updateSelectedBoxColour(int col)
	{
		float colourVal = melodyDraw_Vels[col] / 127.0;
		melodyDraw_colourVals[col] = pow(colourVal,5);
		int selectedDeg = melodyDraw_selectedBox[col];
		melodyDraw[selectedDeg][col].setColour(melodyDraw[selectedDeg][col].buttonColourId,
			Colour::fromFloatRGBA(1, 1, 1, melodyDraw_colourVals[col]));
	}

	void setLayout()
	{
		//inbuilt_ChooseMIDI.setBounds(615, 235, 95, 25);
		//inbuilt_Tonic.setBounds(710, 235, 95, 25);
		inbuilt_Scale.setBounds(50, 265, 100, 25);

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 16; j++)
			{
				melodyDraw[i][j].setBounds(50 + 35*j,490 - 25*i, 15, 15);
				melodyDraw_Vel[j].setBounds(50 + 35 * j, 510, 15, 70);
			}
		}
		melodyDraw_TimeMarker.setBounds(50, 585, 15, 15);
	}
};
