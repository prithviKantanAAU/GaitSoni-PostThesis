#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class UI_MusicControls_Inbuilt
{
public:

	ComboBox inbuilt_ChooseMIDI;
	ComboBox inbuilt_Tonic;
	ComboBox inbuilt_Scale;
	TextButton melodyDraw[8][16];

	UI_MusicControls_Inbuilt()
	{
		configure();
	};
	~UI_MusicControls_Inbuilt() {};

	void configure()
	{
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 16; j++)
			{
				melodyDraw[i][j].setColour(melodyDraw[i][j].buttonColourId, Colours::black);
				if (j % 4 == 0)
					melodyDraw[i][j].setColour(melodyDraw[i][j].buttonColourId, Colours::yellow);
				if (i == 0)
					melodyDraw[i][j].setColour(melodyDraw[i][j].buttonColourId, Colours::white);
			}
		}
	}

	void melodyDraw_HandleButtonPress(int row, int col)
	{
		for (int i = 0; i < 8; i++)
		{
			melodyDraw[i][col].setColour(melodyDraw[i][col].buttonColourId, Colours::black);
			if (col % 4 == 0)
				melodyDraw[i][col].setColour(melodyDraw[i][col].buttonColourId, Colours::yellow);
			melodyDraw[row][col].setColour(melodyDraw[row][col].buttonColourId, Colours::white);
		}
	}

	void toggleVisible(bool on)
	{
		inbuilt_ChooseMIDI.setVisible(on);
		inbuilt_Tonic.setVisible(on);
		inbuilt_Scale.setVisible(on);
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 16; j++)
			{
				melodyDraw[i][j].setVisible(on);
			}
		}
	}

	void setLayout()
	{
		inbuilt_ChooseMIDI.setBounds(615, 235, 95, 25);
		inbuilt_Tonic.setBounds(710, 235, 95, 25);
		inbuilt_Scale.setBounds(805, 235, 95, 25);

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 16; j++)
			{
				melodyDraw[i][j].setBounds(50 + 35*j,490 - 25*i, 15, 15);
			}
		}
	}
};
