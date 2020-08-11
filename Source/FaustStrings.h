#pragma once
#include <string>

class FaustStrings
{
public:

	FaustStrings()
	{
		initializeMusicInfoNames();
	};

	~FaustStrings()
	{

	};

	std::string baseName = "/GaitSoni/";

	std::string MasterClock = "Master_Clock";

	// Music Info

	void initializeMusicInfoNames()
	{
		ActiveScale = baseName + ActiveScale;
		Tempo = baseName + Tempo;
		Tonic = baseName + Tonic;
		CurrentStyle = baseName + CurrentStyle;
		SwingMs = baseName + SwingMs;
		KickV = baseName + KickV;
		SnareV = baseName + SnareV;
		hhV = baseName + hhV;
		crashV = baseName + crashV;
		CV1 = baseName + CV1;
		CV2 = baseName + CV2;
		CV3 = baseName + CV3;
		CV4 = baseName + CV4;
		CT = baseName + CT;
		CF1 = baseName + CF1;
		CF2 = baseName + CF2;
		CF3 = baseName + CF3;
		CF4 = baseName + CF4;
		CSV = baseName + CSV;
		CSF1 = baseName + CSF1;
		CSF2 = baseName + CSF2;
		CSF3 = baseName + CSF3;
		CSF4 = baseName + CSF4;
		CS_ArpMode = baseName + CS_ArpMode;
		RV = baseName + RV;
		RF = baseName + RF;
		RO = baseName + RO;
		MV = baseName + MV;
		MF = baseName + MF;
		MO = baseName + MO;
	}

	std::string ActiveScale = "Mapping_Tabs/Music_Info/Active_Scale";

	std::string Tempo = "Mapping_Tabs/Music_Info/Tempo";

	std::string Tonic = "Mapping_Tabs/Music_Info/Tonic";

	std::string CurrentStyle = "Mapping_Tabs/Music_Info/Current_Style";

	std::string SwingMs = "Mapping_Tabs/Music_Info/Drums/Swing_Ms";
	std::string KickV = "Mapping_Tabs/Music_Info/Drums/Kick_Velocity";
	std::string SnareV = "Mapping_Tabs/Music_Info/Drums/Snare_Velocity";
	std::string hhV = "Mapping_Tabs/Music_Info/Drums/HH_Velocity";
	std::string crashV = "Mapping_Tabs/Music_Info/Drums/Crash_Velocity";

	std::string CV1 = "Mapping_Tabs/Music_Info/Chords/CV1_Current";
	std::string CV2 = "Mapping_Tabs/Music_Info/Chords/CV2_Current";
	std::string CV3 = "Mapping_Tabs/Music_Info/Chords/CV3_Current";
	std::string CV4 = "Mapping_Tabs/Music_Info/Chords/CV4_Current";
	std::string CF1 = "Mapping_Tabs/Music_Info/Chords/CF1_Current";
	std::string CF2 = "Mapping_Tabs/Music_Info/Chords/CF2_Current";
	std::string CF3 = "Mapping_Tabs/Music_Info/Chords/CF3_Current";
	std::string CF4 = "Mapping_Tabs/Music_Info/Chords/CF4_Current";
	std::string CT = "Mapping_Tabs/Music_Info/Chords/CT_Current";

	std::string CSV = "Mapping_Tabs/Music_Info/Chords/CSV_Current";
	std::string CSF1 = "Mapping_Tabs/Music_Info/Chords/CSF1_Current";
	std::string CSF2 = "Mapping_Tabs/Music_Info/Chords/CSF2_Current";
	std::string CSF3 = "Mapping_Tabs/Music_Info/Chords/CSF3_Current";
	std::string CSF4 = "Mapping_Tabs/Music_Info/Chords/CSF4_Current";
	std::string CS_ArpMode = "Mapping_Tabs/Music_Info/Chords/Arp_Mode";

	std::string RV = "Mapping_Tabs/Music_Info/Melody/RV_Current";
	std::string RF = "Mapping_Tabs/Music_Info/Melody/RF_Current";
	std::string RO = "Mapping_Tabs/Music_Info/Melody/RF_Octave";

	std::string MV = "Mapping_Tabs/Music_Info/Melody/MV_Current";
	std::string MF = "Mapping_Tabs/Music_Info/Melody/MF_Current";
	std::string MO = "Mapping_Tabs/Music_Info/Melody/MF_Octave";

	// Master Volume and Track Mute
	std::string MasterVol = "Mapping_Tabs/Mixer_And_Master/Master_Gain/Master_Gain";
	std::string trackVariant_Base = "Mapping_Tabs/Mixer_And_Master/Track_Variant/Variant_";

	std::string trackGain_Base = "Mapping_Tabs/Mixer_And_Master/Track_Gain/";
	std::string trackGains[8] =
	{
		"Track_1","Track_2","Track_3","Track_4","Track_5","Track_6","Track_7","Track_8"
	};

	std::string getMusicAddress(int trackNum, String pOrV, int voiceNum)
	{
		std::string attribute = "";
		if (pOrV == "P") attribute = "Pitch";
		if (pOrV == "V") attribute = "Velocity";
		std::string address = baseName + "Mapping_Tabs/Music_Info/" + attribute + "/T"
			+ std::to_string(trackNum) + "_" + pOrV.toStdString() + "_" + std::to_string(voiceNum);

		return address;
	}

	std::string getMusicAddress_Pitch(int presentTrack)
	{
		std::string address = "";

		return address;
	}

	std::string getTrackGainString(int trackIndex)
	{
		return baseName + trackGain_Base + trackGains[trackIndex];
	}

	std::string getTrackMuteString(int trackIndex)
	{
		return baseName + trackMutes_Base + trackMutes[trackIndex];
	}

	std::string trackMutes_Base = "Mapping_Tabs/Mixer_And_Master/Track_Mute/";
	std::string trackMutes[8] = { "1","2","3","4","5","6","7","8" };

	std::string trackIDs[8] 
	= { "Track_1/","Track_2/","Track_3/","Track_4/","Track_5/","Track_6/","Track_7/","Track_8/" };

	std::string MasterEQ_1_F = baseName + "Mapping_Tabs/Mixer_And_Master/Master_EQ/2_Parametric_1/Freq";
	std::string MasterEQ_2_F = baseName + "Mapping_Tabs/Mixer_And_Master/Master_EQ/3_Parametric_2/Freq";

	std::string MasterEQ_1_Q = baseName + "Mapping_Tabs/Mixer_And_Master/Master_EQ/2_Parametric_1/Q";
	std::string MasterEQ_2_Q = baseName + "Mapping_Tabs/Mixer_And_Master/Master_EQ/3_Parametric_2/Q";

	std::string MasterEQ_1_Gain = baseName + "Mapping_Tabs/Mixer_And_Master/Master_EQ/2_Parametric_1/Gain";
	std::string MasterEQ_2_Gain = baseName + "Mapping_Tabs/Mixer_And_Master/Master_EQ/3_Parametric_2/Gain";

	//EQ Setters
	std::string EQ_base = "Mapping_Tabs/EQ_Section/";
	std::string EQ_filterIDs[4] = { "1_HPF/", "2_Parametric_1/", "3_Parametric_2/", "4_LPF/" };
	std::string EQ_filterParamIDs[3] = { "Freq", "Gain", "Q" };
	std::string FetchEQ_String(int trackID, int filterID, int paramID)
	{
		return baseName + EQ_base + trackIDs[trackID] + EQ_filterIDs[filterID] + EQ_filterParamIDs[paramID];
	};

	//Comp Setters
	std::string Comp_base = "Mapping_Tabs/Comp_Section/";
	std::string Comp_ParamIDs[4] = { "Threshold","Ratio","Attack","Release" };
	std::string FetchComp_String(int trackID, int paramID)
	{
		return baseName + Comp_base + trackIDs[trackID] + Comp_ParamIDs[paramID];
	};

};