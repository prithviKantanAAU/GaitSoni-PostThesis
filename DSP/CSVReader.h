#pragma once

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class CSVReader
{
public:

	CSVReader() {};
	~CSVReader() {};

	void readMelCSV_Metadata(std::string path,string *songName, int *tonic, short *orderArray, int lineNum)
	{
		ifstream ip(path);
		if (!ip.is_open()) std::cout << "ERROR: File Open" << '\n';

		string name;
		string tonicArray[5] = { "" };
		string order[24] = { "" };

		if (lineNum == 2)
		{
			getline(ip, name, '\n');
			name.erase(std::remove(name.begin(), name.end(), ','), name.end());
			*songName = name;

			for (int j = 0; j < 4; j++)
			{
				getline(ip, tonicArray[j], ',');
				tonic[j] = std::stoi(tonicArray[j]);
			}
			getline(ip, tonicArray[4], '\n');
			tonic[4] = std::stoi(tonicArray[4]);

			for (int k = 0; k < 23; k++)
			{
				getline(ip, order[k], ',');
				orderArray[k] = std::stoi(order[k]);
			}
			getline(ip, order[23], '\n');
			orderArray[23] = std::stoi(order[23]);
		}
	};

	void readMelCSV_Custom(std::string path, int *melMatrix, int *scaleVar, int lineNum, int startLine)
	{
		ifstream ip(path);
		if (!ip.is_open()) std::cout << "ERROR: File Open" << '\n';
		
		string scale; 
		string codes[8] = { "" };

		for (int j = 0; j < startLine; j++)
		{
			getline(ip, scale);
		}

		for (int i = startLine; i <= lineNum; i++)
		{
			getline(ip, codes[0], ',');
			getline(ip, codes[1], ',');
			getline(ip, codes[2], ',');
			getline(ip, codes[3], ',');
			getline(ip, codes[4], ',');
			getline(ip, codes[5], ',');
			getline(ip, codes[6], ',');
			if (lineNum >= 8)
			{
				getline(ip, codes[7], '\n');
			}
			else
			{
				getline(ip, codes[7], ',');
				getline(ip, scale, '\n');
				*scaleVar = std::stoi(scale);
			}
		}

		for (int i = 0; i < 8; i++)
		{
			melMatrix[i] = std::stoi(codes[i]);
		}
		
	}
};

