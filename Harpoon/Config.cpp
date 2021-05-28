#include <fstream>
#include <ShlObj.h>
#include "Config.h"
#include "Helpers.h"
#include "Hacks/OTHER/Debug.h"
#include <iomanip>


#include "nlohmann/json.hpp"

using json = nlohmann::json;

int g_nMode = 0;

#define ReadValue(VAL, TYPE) try { VAL = cfgFile[#VAL].get<TYPE>(); } catch(std::exception &e){Debug::QuickWarning(#VAL); Debug::QuickWarning(e.what());}
#define WriteValue(VAL, TYPE) try { cfgFile[#VAL] = VAL; } catch(std::exception &e){Debug::QuickWarning(#VAL); Debug::QuickWarning(e.what());}
#define ConfigValue(VAL, TYPE) try { if(Mode){ cfgFile[#VAL] = VAL; } else { VAL = cfgFile[#VAL].get<TYPE>(); }} catch (std::exception &e){Debug::QuickWarning(e.what());}


#include <fstream>
#include <sstream>
std::fstream g_CurrentConfigFile;
// thanks stackoverflow!
int char2int(char input)
{
	if (input >= '0' && input <= '9')
		return input - '0';
	if (input >= 'A' && input <= 'F')
		return input - 'A' + 10;
	if (input >= 'a' && input <= 'f')
		return input - 'a' + 10;
	throw std::invalid_argument("Invalid input string");
}

void hex2bin(const char* src, char* target)
{
	while (*src && src[1])
	{
		*(target++) = char2int(*src) * 16 + char2int(src[1]);
		src += 2;
	}
}

template<class T>
void FindConfigValue(T* val, const char* szValueName) {
	std::string line;
	while (std::getline(g_CurrentConfigFile, line, '\n'))
	{
		if (strstr(line.c_str(), szValueName)) {
			break;
		}
	}
	int pos = line.find("[/CF]");
	line.erase(pos, line.size());

	pos = line.find("|");
	line.erase(0, pos);
	line.erase(std::remove(line.begin(), line.end(), '|'),
		line.end());
	Debug::QuickPrint(szValueName);
	Debug::QuickPrint(line.c_str());
	char* value = (char*)malloc(sizeof(T));
	hex2bin(line.c_str(), value);
	*val = *(T*)value;
	Debug::QuickPrint(std::to_string(*val).c_str());
	free(value);
}

template<class T>
void CreateConfigValue(T* val, const char* szValueName) {
	std::stringstream stream;
	stream << std::hex << *val;
	std::string hex(stream.str());
	char buffer[4096];
	snprintf(buffer, 4096, "[CF]:%s|%s[/CF]\n", szValueName, hex.c_str());
	g_CurrentConfigFile << buffer;
}

template<class T>
void ConfigValueFunc(T* val, const char* szValueName) {

	if (g_nMode == 0) {
		FindConfigValue(val, szValueName);
	}
	else {
		CreateConfigValue(val, szValueName);
	}
}
/*
namespace ns {
	void to_json(json& j, const person& p) {
		j = json{ {"name", p.name}, {"address", p.address}, {"age", p.age} };
	}

	void from_json(const json& j, person& p) {
		j.at("name").get_to(p.name);
		j.at("address").get_to(p.address);
		j.at("age").get_to(p.age);
	}
} // namespace ns
*/

void Config::RunConfig(bool Mode, const char* szFile) {
	json cfgFile;

	if (!Mode) {
		g_CurrentConfigFile.open(szFile, std::ios::in);
		std::string str((std::istreambuf_iterator<char>(g_CurrentConfigFile)),
			std::istreambuf_iterator<char>());

		json cfgFile = json::parse(str);

		Debug::QuickPrint(cfgFile.dump(4).c_str());

		Debug::QuickPrint(std::to_string(cfgFile["misc.autoAccept"].get<bool>()).c_str());
		ReadValue(misc.autoAccept, bool)
		Debug::QuickPrint(std::to_string(misc.autoAccept).c_str());
	}
	else {
		g_CurrentConfigFile.open(std::string(szFile), std::fstream::out);
		g_CurrentConfigFile.clear();
	}

	if (!g_CurrentConfigFile.is_open()) {
		Debug::QuickPrint("FILE FAILED TO OPEN");
	}

	try {
		ConfigValue(misc.bunnyHop, bool)
		ConfigValue(misc.autoAccept, bool)
		ConfigValue(misc.ragestrafe, bool)
	}
	catch (std::exception& e) {
		Debug::QuickWarning(e.what());
		Debug::QuickPrint(("Mode Was" + std::to_string(Mode)).c_str());
	}

	if (Mode) {
		std::string seralizedStr = cfgFile.dump(4);
		g_CurrentConfigFile.write(seralizedStr.c_str(), seralizedStr.size());
	}

	g_CurrentConfigFile.close();
}
#include <streambuf>

#define VALUE ReadValue


//std::vector<std::wstring> ChamsCatagories2 = { L"Allies", L"Enemies", L"Planting", L"Defusing", L"Local player", L"Weapons", L"DroppedWeapons", L"Backtrack", L"LP Latency", L"Hands",  L"Tickbase BT", L"Sleeves", L"Desync", L"Targeted", L"ShotAt", L"BreadCrumbs", L"ResolverMatrices", L"TargetedResolverMatrix", L"Original Matrix", L"Animations Matrix", L"Props", L"Dynamic Props", L"Ragdolls", L"Chickens", L"Fish", L"NULL" };
//std::vector<std::wstring> ChamsMaterials2 = { L"Normal",L"Flat",L"Animated",L"Platinum",L"Glass",L"Chrome",L"Crystal",L"Silver",L"Gold",L"Plastic",L"Glow",L"Pearlescent",L"Metallic",L"MetalSnow",L"GlassWindow",L"C4Gift",L"UrbanPuddle",L"CrystalCubeVertigo",L"Seagull...",L"Zombie",L"Searchlight",L"BrokenGlass",L"CrystalBlue",L"Velvet",L"Water1",L"de_vetigo/tv_news_02",L"Whiteboard01",L"Whiteboard04" };
std::vector<std::string> ChamsCatagories2 = { "Enemies", "Teammates", "Backtrack", "LocalPlayer", "Fake", "Weapons", "Sleeves", "DroppedWeapons","Chickens","Ragdoll" };
std::vector<std::string> ChamsMaterials2 = { "Normal","Flat","Aftershock","Animated","Platinum","Glass","Chrome","Crystal","Silver","Gold","Plastic","Glow","Pearlescent","Metallic","MetalSnow","GlassWindow","C4Gift","UrbanPuddle","CrystalCubeVertigo","Seagull...","Zombie","Searchlight","BrokenGlass","CrystalBlue","Velvet","Water1","de_vetigo/tv_news_02","Whiteboard01","Whiteboard04" };



void WriteColorToggle(Config::ColorToggle& col, const char* szName, json& cfgFile) {
	try {
		cfgFile[szName]["enabled"] = col.enabled;
		cfgFile[szName]["color"]["r"] = col.color.at(0);
		cfgFile[szName]["color"]["g"] = col.color.at(1);
		cfgFile[szName]["color"]["b"] = col.color.at(2);
		float* valcol = reinterpret_cast<float*>(col.color.data());
		cfgFile[szName]["color"]["a"] = valcol[3];
	}
	catch (std::exception& e) {
		Debug::QuickPrint("Missing Value, Please Recreate Config");
	}
}

void ReadColorToggle(Config::ColorToggle& col, const char* szName, json cfgFile) {
	try {
		col.enabled = cfgFile[szName]["enabled"];
		col.color.at(0) = cfgFile[szName]["color"]["r"];
		col.color.at(1) = cfgFile[szName]["color"]["g"];
		col.color.at(2) = cfgFile[szName]["color"]["b"];
		float* valcol = reinterpret_cast<float*>(col.color.data());
		valcol[3] = cfgFile[szName]["color"]["a"];
	}
	catch (std::exception& e) {
		Debug::QuickPrint("Missing Value, Please Recreate Config");
	}
}



#define ValColorToggle(VAR) try{ ReadColorToggle(VAR, #VAR, cfgFile); } catch(std::exception &e){Debug::QuickWarning(#VAR); Debug::QuickWarning(e.what());}

void Config::ReadFromConfigFile(const char* szFile) {
	try {
		g_CurrentConfigFile.open(szFile, std::ios::in);
		std::string str((std::istreambuf_iterator<char>(g_CurrentConfigFile)),
			std::istreambuf_iterator<char>());

		json cfgFile = json::parse(str);
		//Debug::QuickPrint(cfgFile.dump(4).c_str());
		if (!g_CurrentConfigFile.is_open()) {
			Debug::QuickWarning("Unable to Open Config File!!!!");
			return;
		}


		VALUE(misc.autoAccept, bool);
		VALUE(misc.bunnyHop, bool);
		VALUE(misc.ragestrafe, bool);
		VALUE(misc.autoStrafe, bool);

		VALUE(grief.teamDamageOverlay, bool);
		VALUE(grief.BlockBot, int);
		VALUE(grief.grenadeAIM, int);
		VALUE(grief.triggername, bool);

		VALUE(debug.AutoQueue, bool);



		ValColorToggle(visuals.bulletTracers);
		ValColorToggle(visuals.bulletTracersEnemy);
		ValColorToggle(visuals.dlight);
		VALUE(visuals.dlightExponent, int);
		VALUE(visuals.dlightRadius, float);

		VALUE(visuals.FogControl.Fog.iStartDistance, int);
		VALUE(visuals.FogControl.Fog.iEndDistance, int);
		VALUE(visuals.FogControl.Fog.flHdrColorScale, float);
		ValColorToggle(visuals.FogControl.Fog.Color);

		VALUE(visuals.FogControl.Sky.iStartDistance, int);
		VALUE(visuals.FogControl.Sky.iEndDistance, int);
		VALUE(visuals.FogControl.Sky.flHdrColorScale, float);
		ValColorToggle(visuals.FogControl.Sky.Color);


		ValColorToggle(visuals.world);
		ValColorToggle(visuals.NonCachedWorld);
		ValColorToggle(visuals.PrecacheWorld);
		ValColorToggle(visuals.NonCached);
		ValColorToggle(visuals.PrecacheWorld);
		ValColorToggle(visuals.NightMode);
		ValColorToggle(visuals.AllDrawNavs);
		ValColorToggle(visuals.DrawNavs);
		ValColorToggle(visuals.sky);
		ValColorToggle(misc.watermark);
		ValColorToggle(visuals.DrawLights);

		VALUE(visuals.DrawNavSettings.CalcAllConnections, bool);
		VALUE(visuals.DrawNavSettings.DrawCenterLines, bool);
		VALUE(visuals.DrawNavSettings.DrawNavsRect, bool);
		VALUE(visuals.DrawNavSettings.DrawNavDepth, int);
		VALUE(visuals.DrawNavSettings.CalculateAllNavVectors, bool);
		VALUE(visuals.disablePostProcessing, bool);
		VALUE(visuals.thirdperson, bool);
		VALUE(visuals.thirdpersonKey, int);
		VALUE(visuals.thirdpersonDistance, int);
		VALUE(visuals.fullHLTV, bool);
		VALUE(visuals.matgrey, bool);
		VALUE(visuals.skybox, int);
		VALUE(visuals.no3dSky, bool);


		VALUE(visuals.colorCorrection.blue, float);
		VALUE(visuals.colorCorrection.red, float);
		VALUE(visuals.colorCorrection.green, float);
		VALUE(visuals.colorCorrection.enabled, bool);
		VALUE(visuals.colorCorrection.saturation, float);
		VALUE(visuals.colorCorrection.ghost, float);
		VALUE(visuals.colorCorrection.mono, float);
		VALUE(visuals.colorCorrection.yellow, float);


		ValColorToggle(esp.Box);
		ValColorToggle(esp.Text);
		VALUE(esp.HealthBar, bool);



		ValColorToggle(misc.spectatorList);
		VALUE(visuals.disablePostProcessing, bool);
		VALUE(misc.revealRanks, bool);
		VALUE(misc.radarHack, bool);
		VALUE(visuals.noBlur, bool);
		VALUE(backtrack.enabled, bool);
		VALUE(backtrack.backtrackAll, bool);
		VALUE(backtrack.backtrackx88, bool);
		VALUE(backtrack.fakeLatency, bool);
		VALUE(backtrack.timeLimit, int);
		VALUE(backtrack.breadcrumbtime, float);
		VALUE(backtrack.breadexisttime, float);
		VALUE(config->backtrack.extendedrecords, bool);
		ValColorToggle(backtrack.x88);
#ifdef DEV_SECRET_BUILD
		VALUE(lagger.PacketSize, int);
		VALUE(lagger.SubChannelPacketSize, int);
		VALUE(lagger.skipOnCount, bool);
		VALUE(lagger.reliableOnly, bool);
		VALUE(lagger.OnVoice, bool);
		VALUE(lagger.ResetOnTransmit, bool);
		VALUE(lagger.waitTime, int);
		VALUE(lagger.activeTime, int);
		VALUE(lagger.FULLONBOGANFUCKERY, bool);
		VALUE(mmlagger.packets, int);
		VALUE(mmlagger.blip, int);
		VALUE(mmlagger.blippackets, int);
		VALUE(mmlagger.bNewPrediction, bool);
		VALUE(mmlagger.ticksToSimulate, int);
		VALUE(mmlagger.constant, int);
		VALUE(mmcrasher.key, int);
		VALUE(mmcrasher.toggleKey, bool);
		VALUE(mmcrasher.antiVoteKick, int);
		VALUE(TestShit.fuckKey9, int);
#endif



		VALUE(debug.bRenderChamsOverViewModel, bool);
#define CHAMSVAL(VAR, TYPE) try{VAR = cfgFile[AccessStr][#VAR].get<TYPE>();}catch(std::exception& e){Debug::QuickWarning(e.what());}

		for (std::string Catagorey : ChamsCatagories2) {
			try {
				m_mapChams[Catagorey].bEnabled = cfgFile["m_mapChams"][Catagorey]["bEnabled"].get <bool>();
				m_mapChams[Catagorey].bCallOriginal = cfgFile["m_mapChams"][Catagorey]["bCallOriginal"].get <bool>();
			}
			catch (std::exception& e) {};
			for (int b = 0; b < 10; b++) {
				std::string AccessStr = { "m_mapChams[" + Catagorey + "].Materials[" + std::to_string(b) + "]" };
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].bEnabled, bool);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].bCallOriginalInSceneEnd, bool);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].bRenderInSceneEnd, bool);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].Opts.bIgnorez, bool);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].Opts.bEnabled, bool);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].Opts.bWireframe, bool);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].Opts.bCover, bool);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].Opts.bBlinking, bool);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].Opts.bHealthBased, bool);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].Opts.nMaterial, int);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].MaterialName, std::string);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].color.at(0), float);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].color.at(1), float);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].color.at(2), float);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].color.at(3), float);
			}
		}


		try {
			for (int i = 0; i < glow.size(); i++) {
				std::string iden("Glow" + std::to_string(i));
				glow.at(i).enabled = cfgFile[iden]["enabled"].get<bool>();
				glow.at(i).color.at(0) = cfgFile[iden]["colorr"].get<float>();
				glow.at(i).color.at(1) = cfgFile[iden]["colorg"].get<float>();
				glow.at(i).color.at(2) = cfgFile[iden]["colorb"].get<float>();
				glow.at(i).color.at(3) = cfgFile[iden]["colora"].get<float>();
			}
#if 0
			for (int i = 0; i < ChamsCatagories2.size(); i++) {
				for (int b = 0; b < 10; b++) {
					std::string catastr(ChamsCatagories2.at(i).begin(), ChamsCatagories2.at(i).end());
					std::string matstr(std::to_string(b));

					chams[ChamsCatagories2.at(i)].materials.at(b).enabled = cfgFile["Chams"][catastr][matstr]["enabled"].get<bool>();
					chams[ChamsCatagories2.at(i)].materials.at(b).healthBased = cfgFile["Chams"][catastr][matstr]["healthBased"].get<bool>();
					chams[ChamsCatagories2.at(i)].materials.at(b).blinking = cfgFile["Chams"][catastr][matstr]["blinking"].get<bool>();
					chams[ChamsCatagories2.at(i)].materials.at(b).material = cfgFile["Chams"][catastr][matstr]["material"].get<int>();
					chams[ChamsCatagories2.at(i)].materials.at(b).ignorez = cfgFile["Chams"][catastr][matstr]["ignorez"].get<bool>();
					chams[ChamsCatagories2.at(i)].materials.at(b).cover = cfgFile["Chams"][catastr][matstr]["cover"].get<bool>();
					chams[ChamsCatagories2.at(i)].materials.at(b).pulse.enabled = cfgFile["Chams"][catastr][matstr]["pulse"].get<bool>();
					chams[ChamsCatagories2.at(i)].materials.at(b).wireframe = cfgFile["Chams"][catastr][matstr]["wireframe"].get<bool>();
					chams[ChamsCatagories2.at(i)].materials.at(b).color.at(0) = cfgFile["Chams"][catastr][matstr]["colorr"].get<float>();
					chams[ChamsCatagories2.at(i)].materials.at(b).color.at(1) = cfgFile["Chams"][catastr][matstr]["colorg"].get<float>();
					chams[ChamsCatagories2.at(i)].materials.at(b).color.at(2) = cfgFile["Chams"][catastr][matstr]["colorb"].get<float>();
					float* valcol = reinterpret_cast<float*>(chams[ChamsCatagories2.at(i)].materials.at(b).color.data());
					(valcol[3]) = cfgFile["Chams"][catastr][matstr]["colora"].get<float>();

				}
			}
#endif
		}
		catch (std::exception& e) {
			Debug::QuickPrint("Missing Value, Please Recreate Config");
		}


		g_CurrentConfigFile.close();
	}
	catch (std::exception& e)
	{
			Debug::QuickWarning(e.what());
	}
}

#undef VALUE
#undef ValColorToggle
#define ValColorToggle(VAR) try{ WriteColorToggle(VAR, #VAR, cfgFile); } catch(std::exception &e){Debug::QuickWarning(#VAR); Debug::QuickWarning(e.what());}
#define VALUE WriteValue

void Config::WriteToConfigFile(const char* szFile) {
	try {
		Debug::QuickPrint("Writing Config File");
		g_CurrentConfigFile.open(std::string(szFile), std::fstream::out);

		if (!g_CurrentConfigFile.is_open()) {
			Debug::QuickWarning("Unable to Open Config File!!!!");
			return;
		}

		g_CurrentConfigFile.clear();
		json cfgFile;

		VALUE(misc.autoAccept, bool);
		VALUE(misc.bunnyHop, bool);
		VALUE(misc.ragestrafe, bool);
		VALUE(misc.autoStrafe, bool);

		VALUE(grief.teamDamageOverlay, bool);
		VALUE(grief.BlockBot, int);
		VALUE(grief.grenadeAIM, int);
		VALUE(grief.triggername, bool);

		VALUE(debug.AutoQueue, bool);



		ValColorToggle(visuals.bulletTracers);
		ValColorToggle(visuals.bulletTracersEnemy);
		ValColorToggle(visuals.dlight);
		VALUE(visuals.dlightExponent, int);
		VALUE(visuals.dlightRadius, float);

		VALUE(visuals.FogControl.Fog.iStartDistance, int);
		VALUE(visuals.FogControl.Fog.iEndDistance, int);
		VALUE(visuals.FogControl.Fog.flHdrColorScale, float);
		ValColorToggle(visuals.FogControl.Fog.Color);

		VALUE(visuals.FogControl.Sky.iStartDistance, int);
		VALUE(visuals.FogControl.Sky.iEndDistance, int);
		VALUE(visuals.FogControl.Sky.flHdrColorScale, float);
		ValColorToggle(visuals.FogControl.Sky.Color);


		ValColorToggle(visuals.world);
		ValColorToggle(visuals.NonCachedWorld);
		ValColorToggle(visuals.PrecacheWorld);
		ValColorToggle(visuals.NonCached);
		ValColorToggle(visuals.PrecacheWorld);
		ValColorToggle(visuals.NightMode);
		ValColorToggle(visuals.AllDrawNavs);
		ValColorToggle(visuals.DrawNavs);
		ValColorToggle(visuals.sky);
		ValColorToggle(visuals.DrawLights);
		ValColorToggle(misc.watermark);

		VALUE(visuals.DrawNavSettings.CalcAllConnections, bool);
		VALUE(visuals.DrawNavSettings.DrawCenterLines, bool);
		VALUE(visuals.DrawNavSettings.DrawNavsRect, bool);
		VALUE(visuals.DrawNavSettings.DrawNavDepth, int);
		VALUE(visuals.DrawNavSettings.CalculateAllNavVectors, bool);
		VALUE(visuals.disablePostProcessing, bool);
		VALUE(visuals.thirdperson, bool);
		VALUE(visuals.thirdpersonKey, int);
		VALUE(visuals.thirdpersonDistance, int);
		VALUE(visuals.fullHLTV, bool);
		VALUE(visuals.matgrey, bool);

		VALUE(visuals.colorCorrection.blue, float);
		VALUE(visuals.colorCorrection.red, float);
		VALUE(visuals.colorCorrection.green, float);
		VALUE(visuals.colorCorrection.enabled, bool);
		VALUE(visuals.colorCorrection.saturation, float);
		VALUE(visuals.colorCorrection.ghost, float);
		VALUE(visuals.colorCorrection.mono, float);
		VALUE(visuals.colorCorrection.yellow, float);


		VALUE(visuals.skybox, int);
		VALUE(visuals.no3dSky, bool);

		ValColorToggle(esp.Box);
		ValColorToggle(esp.Text);
		VALUE(esp.HealthBar, bool);

		ValColorToggle(misc.spectatorList);
		VALUE(visuals.disablePostProcessing, bool);
		VALUE(misc.revealRanks, bool);
		VALUE(misc.radarHack, bool);
		VALUE(visuals.noBlur, bool);
		VALUE(backtrack.enabled, bool);
		VALUE(backtrack.backtrackAll, bool);
		VALUE(backtrack.backtrackx88, bool);
		VALUE(backtrack.fakeLatency, bool);
		VALUE(backtrack.timeLimit, int);

		VALUE(backtrack.breadcrumbtime, float);
		VALUE(backtrack.breadexisttime, float);
		VALUE(config->backtrack.extendedrecords, bool);

		ValColorToggle(backtrack.x88);
#ifdef DEV_SECRET_BUILD
		VALUE(lagger.PacketSize, int);
		VALUE(lagger.SubChannelPacketSize, int);
		VALUE(lagger.skipOnCount, bool);
		VALUE(lagger.reliableOnly, bool);
		VALUE(lagger.OnVoice, bool);
		VALUE(lagger.ResetOnTransmit, bool);
		VALUE(lagger.waitTime, int);
		VALUE(lagger.activeTime, int);
		VALUE(lagger.FULLONBOGANFUCKERY, bool);
		VALUE(mmlagger.packets, int);
		VALUE(mmlagger.blip, int);
		VALUE(mmlagger.blippackets, int);
		VALUE(mmlagger.bNewPrediction, bool);
		VALUE(mmlagger.ticksToSimulate, int);
		VALUE(mmlagger.constant, int);
		VALUE(mmcrasher.key, int);
		VALUE(mmcrasher.toggleKey, bool);
		VALUE(mmcrasher.antiVoteKick, int);
		VALUE(TestShit.fuckKey9, int);





#endif








		for (int i = 0; i < glow.size(); i++) {
			std::string iden("Glow" + std::to_string(i));
			cfgFile[iden]["enabled"] = glow.at(i).enabled;
			cfgFile[iden]["colorr"] = glow.at(i).color.at(0);
			cfgFile[iden]["colorg"] = glow.at(i).color.at(1);
			cfgFile[iden]["colorb"] = glow.at(i).color.at(2);
			cfgFile[iden]["colora"] = glow.at(i).color.at(3);
		}

		VALUE(debug.bRenderChamsOverViewModel, bool);
#define CHAMSVAL(VAR, TYPE)  try{cfgFile[AccessStr][#VAR] = VAR;}catch(std::exception& e){Debug::QuickWarning(e.what());}

		for (std::string Catagorey : ChamsCatagories2) {
			try {
				cfgFile["m_mapChams"][Catagorey]["bEnabled"] = m_mapChams[Catagorey].bEnabled;
				cfgFile["m_mapChams"][Catagorey]["bCallOriginal"] = m_mapChams[Catagorey].bCallOriginal;
			}
			catch (std::exception& e) {};

			for (int b = 0; b < 10; b++) {
				std::string AccessStr = { "m_mapChams[" + Catagorey + "].Materials[" + std::to_string(b) + "]" };
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].bEnabled, bool);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].bCallOriginalInSceneEnd, bool);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].bRenderInSceneEnd, bool);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].Opts.bIgnorez, bool);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].Opts.bEnabled, bool);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].Opts.bWireframe, bool);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].Opts.bCover, bool);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].Opts.bBlinking, bool);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].Opts.bHealthBased, bool);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].Opts.nMaterial, int);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].MaterialName, std::string);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].color.at(0), float);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].color.at(1), float);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].color.at(2), float);
				CHAMSVAL(m_mapChams[Catagorey].Materials[b].color.at(3), float);
			}
		}
#if 0
		for (int i = 0; i < ChamsCatagories2.size(); i++) {
			for (int b = 0; b < 10; b++) {





				cfgFile["Chams"][catastr][matstr]["enabled"] = chams[ChamsCatagories2.at(i)].materials.at(b).enabled;
				cfgFile["Chams"][catastr][matstr]["healthBased"] = chams[ChamsCatagories2.at(i)].materials.at(b).healthBased;
				cfgFile["Chams"][catastr][matstr]["blinking"] = chams[ChamsCatagories2.at(i)].materials.at(b).blinking;
				cfgFile["Chams"][catastr][matstr]["material"] = chams[ChamsCatagories2.at(i)].materials.at(b).material;
				cfgFile["Chams"][catastr][matstr]["ignorez"] = chams[ChamsCatagories2.at(i)].materials.at(b).ignorez;
				cfgFile["Chams"][catastr][matstr]["cover"] = chams[ChamsCatagories2.at(i)].materials.at(b).cover;
				cfgFile["Chams"][catastr][matstr]["pulse"] = chams[ChamsCatagories2.at(i)].materials.at(b).pulse.enabled;
				cfgFile["Chams"][catastr][matstr]["wireframe"] = chams[ChamsCatagories2.at(i)].materials.at(b).wireframe;
				cfgFile["Chams"][catastr][matstr]["colorr"] = chams[ChamsCatagories2.at(i)].materials.at(b).color.at(0);
				cfgFile["Chams"][catastr][matstr]["colorg"] = chams[ChamsCatagories2.at(i)].materials.at(b).color.at(1);
				cfgFile["Chams"][catastr][matstr]["colorb"] = chams[ChamsCatagories2.at(i)].materials.at(b).color.at(2);
				float* valcol = reinterpret_cast<float*>(chams[ChamsCatagories2.at(i)].materials.at(b).color.data());
				cfgFile["Chams"][catastr][matstr]["colora"] = (valcol[3]);

			}
		}
#endif

		std::string seralizedStr = cfgFile.dump(4);
		g_CurrentConfigFile.write(seralizedStr.c_str(), seralizedStr.size());
		g_CurrentConfigFile.close();
	}
	catch (std::exception& e)
	{
		Debug::QuickWarning(e.what());
	}
}