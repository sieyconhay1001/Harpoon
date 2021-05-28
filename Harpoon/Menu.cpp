#include "Menu.h"
#include "Interfaces.h"
#include "SDK/OsirisSDK/Surface.h"
#include "Config.h"
#include "Memory.h"
#include "SDK/OsirisSDK/GlobalVars.h"
#include "SDK/OsirisSDK/InputSystem.h"
#include "Hacks/Renderables.h"

#include "COMPILERDEFINITIONS.h"

#include <iomanip>
#include <sstream>
template< typename T >
std::wstring to_hex_wstring(T val)
{
    std::wostringstream wstr;  // note the 'w'
    wstr << std::setfill(L'0') << std::setw(2 * sizeof(T)) << std::hex << (uint_least8_t)val;
    return wstr.str();
}


template<class T>
std::wstring VarToWString(std::wstring varName, T& var, bool set = false) {
    if (!set) {
        return std::wstring{ varName + L"(" + to_hex_wstring(&var) + L"): " + to_hex_wstring(var) + L" | " + std::to_wstring(var) };
    }

}

template<class T>
std::wstring VarToWStringCast(std::wstring varName, T& var, bool set = false) {
    if (!set) {
        return std::wstring{ varName + L"(" + to_hex_wstring(&var) + L"): " + to_hex_wstring(var) + L" | " + std::to_wstring((char)var) };
    }

}
#include "Other/CrossCheatTalk.h"
#include "Hacks/SteamWorks/SteamWorks.h"
#include <steam/isteamuser.h>
#include <steam/isteamfriends.h>
void Menu::WaterMark(const wchar_t* text) {
#ifdef DEV_SECRET_BUILD
    if (config->misc.watermark.enabled) {

        //g_pSteamUser->GetHSteamUser()
        //g_pSteamUser->Get

        static bool init{ false };
        static std::wstring str{L""};
        const char* szName = steamFriends->GetPersonaName();
        const char* szLastName  = "";

        if (strcmp(szName, szLastName)) {
            init = false;
            szLastName = szName;
        }

        if (!init) {   

            const char* szBuild = "Harpoon";
            if (strstr(szName, "Enron") || strstr(szName, "Fly")) {
                szBuild = "Harpoon Development Build";
                szName = "Enron";
            }

            if (strstr(szName, "DENZIL") || strstr(szName, "Ryan") || strstr(szName, "Iron") || strstr(szName, "Metal") || strstr(szName, "Demon")) {
                szBuild = "Harpoon Badass Build";
                szName = "DENZIL";
            }

            static char buffer[256];
            if (strlen(szName) < 16) {
                snprintf(buffer, 256, "%s | %s", szBuild, szName);
            }
            else {
                snprintf(buffer, 256, "%s", szBuild);
            }

            str = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(buffer);
            init = true;
        }


        interfaces->surface->setTextFont(5);
        interfaces->surface->setTextColor(config->misc.watermark.color); // config->misc.watermark.color
        interfaces->surface->setTextPosition(10, 10);
        interfaces->surface->setDrawColor(1, 1, 1, 230);
        auto [w, h] = interfaces->surface->getTextSize(5, str.c_str());
        interfaces->surface->drawFilledRect(5, 5, 10 + w + 5, 10 + h + 5);
       // interfaces->surface->drawLine(10, 10, 10, 10 + h + 5);
        interfaces->surface->printText(str.c_str());
    }



#else 
    if (config->misc.watermark.enabled) {
        interfaces->surface->setTextFont(5);

        interfaces->surface->setTextColor(config->misc.watermark.color); // config->misc.watermark.color

        interfaces->surface->setTextPosition(5, 0);
        interfaces->surface->printText(text);

        static auto frameRate = 1.0f;
        frameRate = 0.9f * frameRate + 0.1f * memory->globalVars->absoluteFrameTime;
        const auto [screenWidth, screenHeight] = interfaces->surface->getScreenSize();
        std::wstring fps{ std::to_wstring(static_cast<int>(1 / frameRate)) + L" fps" };
        const auto [fpsWidth, fpsHeight] = interfaces->surface->getTextSize(Surface::font, fps.c_str());
        interfaces->surface->setTextPosition(screenWidth - fpsWidth - 5, 1);
        interfaces->surface->printText(fps.c_str());
        interfaces->surface->setTextFont(73);
        const auto [textWidth, textHeight] = interfaces->surface->getTextSize(Surface::font, fps.c_str());
        interfaces->surface->setTextPosition(15, 0);
        interfaces->surface->printText(L"e");
        //float latency = 0.0f;
        //if (auto networkChannel = interfaces->engine->getNetworkChannel(); networkChannel && networkChannel->getLatency(0) > 0.0f)
        //     latency = networkChannel->getLatency(0);

        //std::wstring ping{ L"PING: " + std::to_wstring(static_cast<int>(latency * 1000)) + L" ms" };
        //const auto pingWidth = interfaces->surface->getTextSize(Surface::font, ping.c_str()).first;
        //interfaces->surface->setTextPosition(screenWidth - pingWidth - 5, fpsHeight);
        //interfaces->surface->printText(ping.c_str());
    }
#endif
}

#include "Pendelum/Pendelum.h"
#include "Config.h"


/* To Do: Make not Global!*/
Pendelum::AsyncKey menu_key{ false, VK_INSERT, 0.0f, .1f };
Pendelum::AsyncKey playerlist_key{ false, VK_HOME, 0.0f, .1f };
Pendelum::AsyncKey walkbot_key{ false, 0, 0.0f, .1f };
PendelumTimer opentimer;
PendelumTimer closetimer;


bool Value = false;
bool Defined = false;
bool GlowMenu = false;
#ifdef _DEBUG
Pendelum::Window* FallBackWindow = Pendelum::CreateNewWindow();
#endif
Pendelum::Window* MainWindow = Pendelum::CreateNewWindow();
Pendelum::ScreenVec screen;
bool MDebug = false;

Config::Aimbot* aimbot_weapon_ptr = &(config->aimbot[0]);
Config::Aimbot aimbot_weapon;
Config::Aimbot* RageBot_weapon_ptr = &(config->RageBot[0]);
Config::Aimbot RageBot_weapon;
int weapon_class = 0;
int weapon = 0;
int last_weap;
int last_class;
int* joinSnipe = new int(0);
bool WChamUpdate = false;
bool lolMutinyGONE = false;
bool disableMutiny = false;
bool saveMutiny = false;
int current_cham_catagory = 0;
int current_cham_material = 1;
int current_cham_map_index = 0;

bool BacktrackCham = false;
bool* CMSave = nullptr;

bool SendIdle = false;
bool SendActiveStandby = false;


int last_mat = 0;
int last_cat = 0;
#include "Other/CrossCheatTalk.h"

std::vector<std::wstring> Bones = {L"Find Nearest", L"Best Damage"};

std::vector<std::wstring> WeaponClass = { L"All",L"Pistols",L"Heavy",L"SMG",L"Rifles"};
std::vector<std::wstring> All = { L"--" };
std::vector<std::wstring> Pistols = { L"All", L"Glock-18", L"P2000", L"USP-S", L"Dual Berettas", L"P250", L"Tec-9", L"Five-Seven", L"CZ-75", L"Desert Eagle", L"Revolver" };
std::vector<std::wstring> Heavy = { L"All", L"Nova", L"XM1014", L"Sawed-off", L"MAG-7", L"M249", L"Negev" };
std::vector<std::wstring> SMG = { L"All", L"Mac-10", L"MP9", L"MP7", L"MP5-SD", L"UMP-45", L"P90", L"PP-Bizon" };
std::vector<std::wstring> Rifle = { L"All", L"Galil AR", L"Famas", L"AK-47", L"M4A4", L"M4A1-S", L"SSG-08", L"SG-553", L"AUG", L"AWP", L"G3SG1", L"SCAR-20" };

//std::vector<std::string> ChamsCatagories = { "Allies", "Enemies", "Planting", "Defusing", "Local player", "Weapons", "DroppedWeapons", "Backtrack", "LP Latency", "Hands",  "Tickbase BT", "Sleeves", "Desync", "Targeted", "ShotAt", "BreadCrumbs", "ResolverMatrices","TargetedResolverMatrix", "Original Matrix", "Animations Matrix", "Props", "Dynamic Props","Ragdolls","Chickens", "Fish", "NULL" };
std::vector<std::string> ChamsCatagories = { "Enemies", "Teammates", "Backtrack", "LocalPlayer", "Fake", "Weapons", "Sleeves", "DroppedWeapons", "TargetedMatrix", "Chickens","Ragdoll" };
std::vector<std::string> ChamsMaterials = { "Normal","Flat","Aftershock", "Animated","Platinum","Glass","Chrome","Crystal","Silver","Gold","Plastic","Glow","Pearlescent","Metallic","MetalSnow","GlassWindow","C4Gift","UrbanPuddle","CrystalCubeVertigo","Seagull...","Zombie","Searchlight","BrokenGlass","CrystalBlue","Velvet","Water1","de_vetigo/tv_news_02","Whiteboard01","Whiteboard04" };

std::vector<std::wstring> Skyboxes = { L"Default", L"cs_baggage_skybox_", L"cs_tibet", L"embassy", L"italy", L"jungle", L"nukeblank", L"office", L"sky_cs15_daylight01_hdr", L"sky_cs15_daylight02_hdr", L"sky_cs15_daylight03_hdr", L"sky_cs15_daylight04_hdr", L"sky_csgo_cloudy01", L"sky_csgo_night_flat", L"sky_csgo_night02", L"sky_day02_05_hdr", L"sky_day02_05", L"sky_dust", L"sky_l4d_rural02_ldr", L"sky_venice", L"vertigo_hdr", L"vertigo", L"vertigoblue_hdr", L"vietnam", L"sky_lunacy", L"sky_borealis01", L"cliff", L"blue", L"city1", L"neb1", L"xen8", L"sky2mh_" };

std::vector<std::wstring> Regions = { L"Sydney (Australia South-East)",L"Beijing (Asia North-East)",L"Bombay (India West)",L"Cangzhou (Asia North-East)",L"Chennai (India South-East)",L"Dongcheng Qu (Asia Central)",L"Dubai (Middle-East)",L"Guangzhou 1 (Asia East)",L"Guangzhou 2 (Asia East)",L"Guangzhou 3 (Asia East)",L"Guangzhou 4 (Asua East)",L"Hong Kong (Asia East)",L"Hongshan Qu (Asia South-East)",L"Huangpu Qu (Asia East)",L"Manilla (Asia South-East)",L"Qingcheng (Asia East)",L"Shanghai 1 (Asia East)",L"Shanghai 2 (Asia East)",L"Singapore (Asia South-East)",L"Tianjin 1 (Asia North-East)",L"Tianjin 2 (Asia North-East)",L"Tokyo 1 (Asia East)",L"Tokyo 2 (Asia East)",L"Xianghe Xian (Asia North-East)",L"Amsterdam (EU West)",L"Frankfurt (EU West)",L"Hamina (EU North)",L"London (EU North)",L"Luxembourg (EU West)",L"Madrid (EU South)",L"Paris (EU West)",L"Stockholm (Bromma) (EU North)",L"Stockholm (Kista) (EU North)",L"Vienna (EU Central)",L"Warsaw (EU Central)",L"Atlanta (US East)",L"Chicago (US Central)",L"Los Angeles (US West)",L"Moses Lake 1 (US Central)",L"Moses Lake 2 (US Central)",L"Oklahoma City (US Central)",L"Seattle (US North-West)",L"Sterling (US East)",L"Lima (SA West)",L"Sao Paulo 1 (SA East)",L"Sao Paulo 2 (SA East)",L"Santiago (SA South-West)",L"Johannesburg, South Africa (ZA)",L"Jiaxing, China (CN)", L"Argentinia", L"dfw", L"eze", L"Unknown" };


std::vector<std::wstring>* weaponv = &All;

#include "Hacks/Walkbot/Walkbot.h"
#include "Hacks/OTHER/Debug.h"
namespace OnScreenIndicator{


    


    void Run() {
#ifndef WALKBOT_BUILD
        if (!config->walkbot.enabled)
            return;
#endif

        interfaces->surface->setTextFont(5);
        static const std::vector<std::wstring> Targets{L"Follow Path", L"Hide", L"Defuse", L"Plant", L"Head To Networked Point", L"PlayerLink", L"FindingPlayerToLink"};
        Debug::coords coords;
        auto [x,y] = interfaces->surface->getScreenSize();
        coords.x = x;
        coords.y = 5;
        std::wstring stri{ L"Current Walkbot Target : " + Targets.at(std::clamp(Walkbot::WBot.CurrentAct, (Walkbot::WalkbotActs)0, (Walkbot::WalkbotActs)(Targets.size()-1)))};
        auto [textw, texth] = interfaces->surface->getTextSize(5, stri.c_str());
        coords.x /= 2;
        coords.x /= 2;
        coords.x /= 2;
        interfaces->surface->setTextPosition(coords.x, coords.y);
        interfaces->surface->setTextColor(255, 0, 0, 255);
        interfaces->surface->printText(stri.c_str());
        coords.y += texth + 3;

        static const std::vector<std::wstring> PathStatus{ L"Calculating Path", L"In Path", L"Out Of Nav Bounds", L"No Path" };
        std::wstring stri3{ L"Current Pathing Status : " + PathStatus.at(std::clamp(Walkbot::WBot.PathingStatus, (Walkbot::WalkbotPathFindingStatus)0, (Walkbot::WalkbotPathFindingStatus)(PathStatus.size()-1))) };
        auto [textw3, texth3] = interfaces->surface->getTextSize(5, stri3.c_str());
        interfaces->surface->setTextPosition(coords.x, coords.y);
        interfaces->surface->setTextColor(255, 0, 0, 255);
        interfaces->surface->printText(stri3.c_str());
        coords.y += texth3 + 3;

        static const std::vector<std::wstring> Status{ L"In Engagement", L"Completing Objective", L"Active Standby", L"Idle StandBy", L"Idle", L"To Networked Position" };

        std::wstring stri2{ L"Current Status : " + Status.at(std::clamp(Walkbot::WBot.Status, (Walkbot::WalkbotStatus)0, (Walkbot::WalkbotStatus)(Status.size()-1))) };
        auto [textwn, texthn] = interfaces->surface->getTextSize(5, stri2.c_str());
        interfaces->surface->setTextPosition(coords.x, coords.y);
        interfaces->surface->setTextColor(255, 0, 0, 255);
        interfaces->surface->printText(stri2.c_str());
        coords.y += texthn + 3;
        std::string Name = "No Valid";
        if (Walkbot::WBot.CurrentAct == Walkbot::WalkbotActs::PlayerLink) {
            Entity* ent = interfaces->entityList->getEntity(Walkbot::WBot.LinkEntity);
            if (ent) {
                Name = ent->getPlayerName(true);
            }
                

        }      
        std::wstring stri4{ L"Entity Link " + std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(Name) }; 
        auto [textwnkn, texthnkn] = interfaces->surface->getTextSize(5, stri3.c_str());
        interfaces->surface->setTextPosition(coords.x, coords.y);
        interfaces->surface->setTextColor(255, 0, 0, 255);
        interfaces->surface->printText(stri4.c_str());
    }










}







namespace EasyCreate {
    static bool* createNewSubMenu(const wchar_t* text, Pendelum::Window* window, bool* wrapper = false, int index = 0) {
        bool* boolptr = new bool(false);
        Pendelum::MenuItem* MenuController = Pendelum::CreateCheckBox(text, boolptr);
        if (index > 0) {
            window->Add_Wrapped_Item(MenuController, wrapper, index);
        }
        else {
            window->Add_Item(MenuController);
        }
        return boolptr;
    }

    static void createCheckBox(const wchar_t* text, Pendelum::Window* window, bool* val, bool* wrapper = false, int index = 0) {
        Pendelum::MenuItem* MenuController = Pendelum::CreateCheckBox(text, val);
        if (index > 0) {
            window->Add_Wrapped_Item(MenuController, wrapper, index);
        }
        else {
            window->Add_Item(MenuController);
        }
    }

    static void createHotKey(const wchar_t* text, Pendelum::Window* window, int* val, bool* wrapper = false, int index = 0) {
        Pendelum::MenuItem* MenuController = Pendelum::CreateHotKey(text, val);
        if (index > 0) {
            window->Add_Wrapped_Item(MenuController, wrapper, index);
        }
        else {
            window->Add_Item(MenuController);
        }
    }

    static void createColorToggle(const wchar_t* text, Pendelum::Window* window, Config::ColorToggle* color, bool* wrapper = false, int index = 0) {
        Pendelum::MenuItem* MenuController = Pendelum::CreateColorSettings(text, color, window);
        if (index > 0) {
            window->Add_Wrapped_Item(MenuController, wrapper, index);
        }
        else {
            window->Add_Item(MenuController);
        }
    }

    static void createColorToggle(const wchar_t* text, Pendelum::Window* window, std::array<float,4>* color, bool* wrapper = false, int index = 0) {
        Pendelum::MenuItem* MenuController = Pendelum::CreateColorSettings(text, color, window);
        if (index > 0) {
            window->Add_Wrapped_Item(MenuController, wrapper, index);
        }
        else {
            window->Add_Item(MenuController);
        }
    }

    static void createFloatSlider(const wchar_t* text, Pendelum::Window* window, float* val, float min, float max, float step = .05f, bool* wrapper = false, int index = 0) {
        Pendelum::MenuItem* MenuController = Pendelum::CreateFloatSlider(text, val, min, max, step);
        if (index > 0) {
            window->Add_Wrapped_Item(MenuController, wrapper, index);
        }
        else {
            window->Add_Item(MenuController);
        }
    }

    static void createIntSlider(const wchar_t* text, Pendelum::Window* window, int* val, int min, int max, int step = .05f, bool* wrapper = false, int index = 0) {
        Pendelum::MenuItem* MenuController = Pendelum::CreateIntSlider(text, val, min, max, step);
        if (index > 0) {
            window->Add_Wrapped_Item(MenuController, wrapper, index);
        }
        else {
            window->Add_Item(MenuController);
        }
    }

    static void createCombo(const wchar_t* text, Pendelum::Window* window, int* val, std::vector<std::wstring>* ComboText, bool* wrapper = false, int index = 0) {
        Pendelum::MenuItem* MenuController = Pendelum::CreateCombo(text, val, ComboText);
        if (index > 0) {
            window->Add_Wrapped_Item(MenuController, wrapper, index);
        }
        else {
            window->Add_Item(MenuController);
        }
    }

    static void createCStrCombo(const char* text, Pendelum::Window* window, int* val, std::vector<std::string>* ComboText, bool* wrapper = false, int index = 0) {
        Pendelum::MenuItem* MenuController = Pendelum::CreateCStrCombo(text, val, ComboText);
        if (index > 0) {
            window->Add_Wrapped_Item(MenuController, wrapper, index);
        }
        else {
            window->Add_Item(MenuController);
        }
    }


    static void createMapCombo(const char* text, Pendelum::Window* window, std::string* val, std::vector<std::string>* ComboText, bool* wrapper = false, int index = 0) {
        Pendelum::MenuItem* MenuController = Pendelum::CreateMapCombo(text, val, ComboText);
        if (index > 0) {
            window->Add_Wrapped_Item(MenuController, wrapper, index);
        }
        else {
            window->Add_Item(MenuController);
        }
    }

    static void createHitboxCombo(const wchar_t* text, Pendelum::Window* window, std::array<bool,19>* hitboxes, bool* wrapper = false, int index = 0) {
        Pendelum::MenuItem* MenuController = Pendelum::CreateHitBoxCombo(text, hitboxes, window);
        if (index > 0) {
            window->Add_Wrapped_Item(MenuController, wrapper, index);
        }
        else {
            window->Add_Item(MenuController);
        }
    }

}

bool bForceChamSave = false;
static void setupPointers() {


    int val = last_weap;
    if (last_class != weapon_class) {
        weapon = 0;
        last_class = weapon_class;
    }

    switch (weapon_class) {
        case 0:
            *weaponv = All;
            aimbot_weapon_ptr = &(config->aimbot[0]);

            RageBot_weapon_ptr = &(config->RageBot[0]);
            last_weap = 0;
            break;
        case 1: 
            *weaponv = Pistols;
            aimbot_weapon_ptr = &(config->aimbot[weapon]);
            RageBot_weapon_ptr = &(config->RageBot[weapon]);
            last_weap = weapon + 1;
            break;
        
        case 2: 
            *weaponv = Heavy;
            aimbot_weapon_ptr = &(config->aimbot[weapon + 10]);
            RageBot_weapon_ptr = &(config->RageBot[weapon+ 10]);
            last_weap = weapon+10;
            break;
        
        case 3: 
            *weaponv = SMG;
            aimbot_weapon_ptr = &(config->aimbot[weapon + 16]);
            RageBot_weapon_ptr = &(config->RageBot[weapon + 16]);
            last_weap = weapon+16;
            break;
        
        case 4: 
            *weaponv = Rifle;
            aimbot_weapon_ptr = &(config->aimbot[weapon + 23]);
            RageBot_weapon_ptr = &(config->RageBot[weapon + 23]);
            last_weap = weapon + 23;
            break;
        
    }
    if (last_weap != val) {
        aimbot_weapon = *aimbot_weapon_ptr;
        RageBot_weapon = *RageBot_weapon_ptr;
    }
    *aimbot_weapon_ptr = aimbot_weapon;
    *RageBot_weapon_ptr = RageBot_weapon;

    if ((last_cat != current_cham_catagory) || bForceChamSave) {
        if (config->m_mapChams.find(ChamsCatagories[current_cham_catagory]) == config->m_mapChams.end()) {
            Config::Cham_s NewCham;
            config->m_mapChams[ChamsCatagories[current_cham_catagory]] = NewCham;
            return;
        }
        config->backupChamMaterial = config->m_mapChams[ChamsCatagories[current_cham_catagory]].Materials[current_cham_material - 1];
        config->backupChamCatagorey = config->m_mapChams[ChamsCatagories[current_cham_catagory]];
        last_cat = current_cham_catagory;
    } if (last_mat != current_cham_material || bForceChamSave) {
        config->backupChamMaterial = config->m_mapChams[ChamsCatagories[current_cham_catagory]].Materials[current_cham_material - 1];
        last_mat = current_cham_material;
    }


    if (bForceChamSave) {
        bForceChamSave = false;
    }

    if ((current_cham_catagory == 7) && *CMSave )
        BacktrackCham = true;
    else
        BacktrackCham = false;

    if (current_cham_material > 0 && current_cham_material < 10)
    {
        config->backupChamCatagorey.Materials[current_cham_material - 1] = config->backupChamMaterial;
        config->m_mapChams[ChamsCatagories[current_cham_catagory]] = config->backupChamCatagorey;
       // config->m_mapChams[ChamsCatagories[current_cham_catagory]].Materials[current_cham_material - 1] = config->backupChamMaterial;
    }
    else
    {
        current_cham_material = 1;
    }
}

bool MG = false;


#ifdef WALKBOT_BUILD
int WBOTKey = VK_NUMLOCK;
#endif
#ifndef WALKBOT_BUILD
int WBOTKey = 0;
#endif

int* ForceFullUpdate = new int(false);
bool* commLaggerOn = new bool(false);
bool* justgo = new bool(false);
bool* lagOnEnemy = new bool(false);
bool* streamerMode = new bool(false);
int* communityLagger = new int(false);
int* steamWorks = new int(0);
bool* LaggerMenu = new bool(false);
bool* mmCrash = new bool(false);
bool* WriteConfig = new bool(false);
bool* ReadConfig = new bool(false);
Pendelum::AsyncKey LaggerToggle_Key{ false, 0, 0.0f, .1f };


#include "PlayerList.h"

static void DefineWindows() {
    if (Defined)
        return;
    Defined = true;
    
    Pendelum::Set_Focus(*MainWindow);
    MainWindow->SetColor(60, 60, 60, 150);
    MainWindow->SetWindowInfo(NULL, NULL, NULL, NULL, 10, 10, true);


    EasyCreate::createCheckBox(L"ANTI-IP", MainWindow, &config->debug.allowcalloriginal);

    EasyCreate::createHotKey(L"Force Full Update In Client State", MainWindow, ForceFullUpdate);
    EasyCreate::createCheckBox(L"Insta-Reconnect", MainWindow, &config->debug.instareconnect);
#ifdef DEV_SECRET_BUILD
    // This is the funniest shit dude lmaoooooooo
#if 1
    bool* mutiny = new bool(false);
    EasyCreate::createCheckBox(L"Grief Mutiny Teammates", MainWindow, mutiny);
    {
        EasyCreate::createCheckBox(L"Disable Mutiny Player", MainWindow, &disableMutiny, mutiny, 1);
        EasyCreate::createCheckBox(L"Save Mutiny Player", MainWindow, &saveMutiny, mutiny, 1);
        EasyCreate::createCheckBox(L"Crash Mutiny Player", MainWindow, &lolMutinyGONE, mutiny, 1);
    }
#endif
#endif
    

    EasyCreate::createCheckBox(L"Write Config File", MainWindow, WriteConfig);
    EasyCreate::createCheckBox(L"Read Config File", MainWindow, ReadConfig);
    EasyCreate::createCheckBox(L"Streamer Mode", MainWindow, streamerMode);
    bool* StreamerModeQuickKeys = new bool(false);
    EasyCreate::createCheckBox(L"Streamer Mode Quick Access", MainWindow, StreamerModeQuickKeys, streamerMode, 0);
    {
        EasyCreate::createHotKey(L"Server Crasher Key", MainWindow, &config->mmcrasher.key, StreamerModeQuickKeys, 1);
        EasyCreate::createCheckBox(L"Server Crasher Key Toggle Mode", MainWindow, &config->mmcrasher.toggleKey, StreamerModeQuickKeys, 1);
        EasyCreate::createCheckBox(L"Cooldown Cooker", MainWindow, &config->mmcrasher.cooldownMaker, StreamerModeQuickKeys, 1);
        EasyCreate::createCheckBox(L"Pure Annoyance", MainWindow, &config->mmcrasher.annoyance, StreamerModeQuickKeys, 1);
        EasyCreate::createHotKey(L"Pure Annoyance Key", MainWindow, &config->mmcrasher.annoyanceKey, StreamerModeQuickKeys, 1);
        EasyCreate::createHotKey(L"Server Lagger Key", MainWindow, &LaggerToggle_Key.Key, StreamerModeQuickKeys, 1);
    }
#ifdef DEV_SECRET_BUILD
    bool* FuckKeys = new bool(false);
    EasyCreate::createCheckBox(L"Fuck Around Keys", MainWindow, FuckKeys); {
        EasyCreate::createHotKey(L"Fuck Around Key 5", MainWindow, &config->TestShit.fuckKey5, FuckKeys, 1);
        EasyCreate::createHotKey(L"Fuck Around Key 8", MainWindow, &config->TestShit.fuckKey8, FuckKeys, 1);
        EasyCreate::createHotKey(L"Fuck Around Key 10", MainWindow, &config->TestShit.fuckKey10, FuckKeys, 1);
        EasyCreate::createHotKey(L"Fuck Around Key 11", MainWindow, &config->TestShit.fuckKey11, FuckKeys, 1);
        EasyCreate::createHotKey(L"Fuck Around Key 12", MainWindow, &config->TestShit.fuckKey12, FuckKeys, 1);
    }
#endif
   EasyCreate::createHotKey(L"joinSnipe", MainWindow, joinSnipe);

   
   // EasyCreate::createHotKey(L"Dumb MM Lagger lol (Make sure to fuck SDR at same time)", MainWindow, &config->TestShit.fuckKey7);
    //EasyCreate::createHotKey(L"Fuck The SDR", MainWindow, steamWorks);
   bool* matchmaking = new bool(false);
   EasyCreate::createCheckBox(L"MatchMaking", MainWindow, matchmaking);
   {
       EasyCreate::createCheckBox(L"Auto-Accept", MainWindow, &config->misc.autoAccept, matchmaking, 1);
       EasyCreate::createCheckBox(L"Auto-Queue", MainWindow, &config->debug.AutoQueue, matchmaking, 1);
       EasyCreate::createCheckBox(L"Auto-Win", MainWindow, &config->autowin.enabled, matchmaking, 1);

   }

   bool* AllLagger = new bool(false);
   EasyCreate::createCheckBox(L"Lag/Crash/Freeze Exploits", MainWindow, AllLagger);

    EasyCreate::createCheckBox(L"Server Stutterer", MainWindow, LaggerMenu, AllLagger, 1);
    {

        EasyCreate::createCheckBox(L"Enable", MainWindow, commLaggerOn, LaggerMenu, 2);
        EasyCreate::createHotKey(L"On Key", MainWindow, &LaggerToggle_Key.Key, LaggerMenu, 2);
        EasyCreate::createCheckBox(L"Reset On Transmit", MainWindow, &config->lagger.ResetOnTransmit, LaggerMenu, 2);
        EasyCreate::createCheckBox(L"Reliable Only", MainWindow, &config->lagger.reliableOnly, LaggerMenu,2);
        EasyCreate::createIntSlider(L"Packet Size", MainWindow, &config->lagger.PacketSize, 1, 20, 1, LaggerMenu, 2);
        EasyCreate::createIntSlider(L"SubChannel Packet Size", MainWindow, &config->lagger.SubChannelPacketSize, 1, 40, 1, LaggerMenu, 2);
        EasyCreate::createIntSlider(L"Wait Time", MainWindow, &config->lagger.waitTime, 0, 100, 1, LaggerMenu, 2);
        EasyCreate::createCheckBox(L"Skip On Count", MainWindow, &config->lagger.skipOnCount, LaggerMenu, 2);
        EasyCreate::createIntSlider(L"Count To Skip On", MainWindow, &config->lagger.countToSkip, 0,20, 1, LaggerMenu, 2);
        EasyCreate::createCheckBox(L"Lag On Enemy Vis", MainWindow, lagOnEnemy, LaggerMenu, 2);
        EasyCreate::createCheckBox(L"Hard Mode (Don't Edit This Value)", MainWindow, &config->debug.Lagger, LaggerMenu, 2);
        EasyCreate::createCheckBox(L"Community Server Enable (ALWAYS ON, USE WITH CAUTION)", MainWindow, &config->lagger.FULLONBOGANFUCKERY, LaggerMenu, 2);
        EasyCreate::createHotKey(L"Community Server Enable Key", MainWindow, communityLagger, LaggerMenu, 2);

    }

    bool* Stutterer2Menu = new bool(false);
    EasyCreate::createCheckBox(L"Server Stutterer 2", MainWindow, Stutterer2Menu, AllLagger, 1);
    {
        EasyCreate::createCheckBox(L"Enable", MainWindow, &config->stutterer.enabled, Stutterer2Menu, 2);
        EasyCreate::createHotKey(L"On Key", MainWindow, &config->stutterer.key, Stutterer2Menu, 2);
        EasyCreate::createIntSlider(L"Count To Send", MainWindow, &config->stutterer.CountToSend, 10, 5000, 100, Stutterer2Menu, 2);
        EasyCreate::createFloatSlider(L"Time To Sleep", MainWindow, &config->stutterer.SleepTime, 10.f, 5000.f, 10.f, Stutterer2Menu, 2);
    }



#ifndef DYLANS_WORRIED
#ifdef DEV_SECRET_BUILD
#ifdef DUMBEST_LAGGER
    bool* mmLagger = new bool(false);
    EasyCreate::createCheckBox(L"MM Server Lagger", MainWindow, mmLagger, AllLagger, 1);
    {
        EasyCreate::createHotKey(L"Freeze Everyone But Yourself", MainWindow, &config->TestShit.fuckKey9, mmLagger, 2);
       // EasyCreate::createHotKey(L"Constant Key", MainWindow, &config->mmlagger.key, mmLagger, 1);
        EasyCreate::createCheckBox(L"Toggle Constant Key Mode", MainWindow, &config->mmlagger.toggleMode, mmLagger, 2);
        EasyCreate::createHotKey(L"Keep Alive Key", MainWindow, &config->mmlagger.KeepAlive, mmLagger, 2);
        EasyCreate::createCheckBox(L"Keep Alive Sleep", MainWindow, &config->mmlagger.KeepAliveSleep, mmLagger, 2);
        EasyCreate::createHotKey(L"1 Tick Nlip", MainWindow, &config->mmlagger.blip, mmLagger, 2);
        EasyCreate::createHotKey(L"Constant", MainWindow, &config->mmlagger.constant, mmLagger, 2);
        EasyCreate::createIntSlider(L"Count", MainWindow, &config->mmlagger.packets, 0, 100, 1, mmLagger, 2);
        EasyCreate::createIntSlider(L"Count2 Percise", MainWindow, &config->mmlagger.blippackets, 0, 1000, 1, mmLagger, 2);
        EasyCreate::createIntSlider(L"Count2", MainWindow, &config->mmlagger.blippackets, 0, 10000, 1, mmLagger, 2);
        EasyCreate::createIntSlider(L"Count2 Extended", MainWindow, &config->mmlagger.blippackets, 0, 30000, 1, mmLagger, 2);
        EasyCreate::createIntSlider(L"Skip", MainWindow, &config->mmlagger.skip, 0, 1000, 1, mmLagger, 2);
        EasyCreate::createIntSlider(L"Speed", MainWindow, &config->mmlagger.ticksToSimulate, 0, 10, 1, mmLagger, 2);
        EasyCreate::createFloatSlider(L"Desired Ping", MainWindow, &config->mmlagger.desiredPing, 0.f, 3000.f, 1.f, mmLagger, 2);
        EasyCreate::createCheckBox(L"Stutter", MainWindow, &config->mmlagger.stutter, mmLagger, 2);
        EasyCreate::createCheckBox(L"Stutter2", MainWindow, &config->mmlagger.stutter2, mmLagger, 2);
        EasyCreate::createCheckBox(L"Crim", MainWindow, &config->mmlagger.crimWalk, mmLagger, 2);
        EasyCreate::createCheckBox(L"New Prediction Method", MainWindow, &config->mmlagger.bNewPrediction, mmLagger, 2);
        EasyCreate::createCheckBox(L"Cleanup Prediction Comeback", MainWindow, &config->mmlagger.cleanup, mmLagger, 2);
        EasyCreate::createCheckBox(L"Disable Desync", MainWindow, &config->mmlagger.turnoffDesync, mmLagger, 2);
        EasyCreate::createIntSlider(L"Desync Disabler Amount", MainWindow, &config->mmlagger.desyncTimer, 0,120, 1, mmLagger, 2);
       // EasyCreate::createIntSlider(L"Desync TickOut Amount", MainWindow, &config->mmlagger.tickOut, 0, 40, 1, mmLagger, 2);
        EasyCreate::createCheckBox(L"Teleport", MainWindow, &config->mmlagger.teleport, mmLagger, 2);
        //EasyCreate::createCheckBox(L"Enable", MainWindow, &config->mmlagger.enabled, mmLagger, 1);
        //EasyCreate::createCheckBox(L"asTCP", MainWindow, &config->mmlagger.asTcp, mmLagger, 1);
        //EasyCreate::createCheckBox(L"isReplayDemo", MainWindow, &config->mmlagger.isReplayDemo, mmLagger, 1);
        //EasyCreate::createIntSlider(L"Channel To Send On", MainWindow, &config->mmlagger.chan, 0, 8, 1, mmLagger, 1);
        //EasyCreate::createCheckBox(L"Reliable Only", MainWindow, &config->mmlagger.reliableOnly, mmLagger, 1);
    }
#endif
#endif
    

    EasyCreate::createCheckBox(L"MM Server Freezer", MainWindow, mmCrash, AllLagger, 1);
    {
        // config->mmcrasher.annoyance
        EasyCreate::createCheckBox(L"Anti Vote Kick", MainWindow, &config->mmcrasher.antiVoteKick, mmCrash, 2);
        EasyCreate::createCheckBox(L"Turn On", MainWindow, &config->mmcrasher.enabled, mmCrash, 2);
        EasyCreate::createHotKey(L"Key", MainWindow, &config->mmcrasher.key, mmCrash, 2);
        EasyCreate::createCheckBox(L"Key Toggle Mode", MainWindow, &config->mmcrasher.toggleKey, mmCrash, 2);
        EasyCreate::createCheckBox(L"No Normal Data", MainWindow, &config->mmcrasher.breturn, mmCrash, 2);
        EasyCreate::createIntSlider(L"Times to Send", MainWindow, &config->mmcrasher.timesToSend, 0, 1000, 5, mmCrash, 2);
        EasyCreate::createIntSlider(L"Times to PostPone", MainWindow, &config->mmcrasher.timesToPostpone, 0, 100, 1, mmCrash, 2);
        EasyCreate::createIntSlider(L"BufferSize", MainWindow, &config->mmcrasher.bufferSize, 0, INT_MAX, 3000, mmCrash, 2);
        EasyCreate::createIntSlider(L"MTU", MainWindow, &config->mmcrasher.MTU, 0, INT_MAX, 10, mmCrash, 2);
        EasyCreate::createIntSlider(L"Nag On 'Er", MainWindow, &config->mmcrasher.nagleTime, 0, 30, 1, mmCrash, 2);
        EasyCreate::createIntSlider(L"Duplicate Percentage", MainWindow, &config->mmcrasher.DupPercent, 0, 100, 1, mmCrash, 2);
        EasyCreate::createFloatSlider(L"Annoyance On Time", MainWindow, &config->mmcrasher.annoyanceOn, 0.f, 1.f, .1f, mmCrash, 2);
        EasyCreate::createFloatSlider(L"Annoyance Off Time", MainWindow, &config->mmcrasher.annoyanceOff, 0.f, 18.f, .1f, mmCrash, 2);
    }
#endif
#ifdef DEV_SECRET_BUILD
    bool* shark = new bool(false);
    EasyCreate::createCheckBox(L"Shark's Crasher", MainWindow, shark, AllLagger, 1);
    {
        EasyCreate::createHotKey(L"Key", MainWindow, &config->sharkcrasher.key, shark, 2);
    }
#endif


    Pendelum::MenuItem* GlowCheckbox = Pendelum::CreateCheckBox(L"Glow Settings", &GlowMenu);
    MainWindow->Add_Item(GlowCheckbox);

    {
        bool* EnemyGlow = new bool(false);
        Pendelum::MenuItem* EnemyGlowAllCheckbox = Pendelum::CreateCheckBox(L"Enemy", EnemyGlow);
        MainWindow->Add_Wrapped_Item(EnemyGlowAllCheckbox, &GlowMenu,1);
        {
            Pendelum::MenuItem* AColorSettings = Pendelum::CreateColorSettings(L"All", &config->glow[3], MainWindow);
            MainWindow->Add_Wrapped_Item(AColorSettings, EnemyGlow, 2);

            Pendelum::MenuItem* VColorSettings = Pendelum::CreateColorSettings(L"Visible", &config->glow[4], MainWindow);
            MainWindow->Add_Wrapped_Item(VColorSettings, EnemyGlow, 2);

            Pendelum::MenuItem* OColorSettings = Pendelum::CreateColorSettings(L"Occluded", &config->glow[5], MainWindow);
            MainWindow->Add_Wrapped_Item(OColorSettings, EnemyGlow, 2);

        }


        bool* TeamGlow = new bool(false);
        Pendelum::MenuItem* TeamGlowAllCheckbox = Pendelum::CreateCheckBox(L"Teammates", TeamGlow);
        MainWindow->Add_Wrapped_Item(TeamGlowAllCheckbox, &GlowMenu, 1);
        {
            Pendelum::MenuItem* AColorSettings = Pendelum::CreateColorSettings(L"All", &config->glow[0], MainWindow);
            MainWindow->Add_Wrapped_Item(AColorSettings, TeamGlow, 2);

            Pendelum::MenuItem* VColorSettings = Pendelum::CreateColorSettings(L"Visible", &config->glow[1], MainWindow);
            MainWindow->Add_Wrapped_Item(VColorSettings, TeamGlow, 2);

            Pendelum::MenuItem* OColorSettings = Pendelum::CreateColorSettings(L"Occluded", &config->glow[2], MainWindow);
            MainWindow->Add_Wrapped_Item(OColorSettings, TeamGlow, 2);

        }



        bool* LocalPlayerGlow = new bool(false);
        Pendelum::MenuItem* LPGlowAllCheckbox = Pendelum::CreateCheckBox(L"Local Player", LocalPlayerGlow);
        MainWindow->Add_Wrapped_Item(LPGlowAllCheckbox, &GlowMenu, 1);
        {
            Pendelum::MenuItem* AColorSettings = Pendelum::CreateColorSettings(L"All", &config->glow[12], MainWindow);
            MainWindow->Add_Wrapped_Item(AColorSettings, LocalPlayerGlow, 2);
        }

        bool* DefusePlayerGlow = new bool(false);
        Pendelum::MenuItem* DefuseGlowAllCheckbox = Pendelum::CreateCheckBox(L"Defusing Player", DefusePlayerGlow);
        MainWindow->Add_Wrapped_Item(DefuseGlowAllCheckbox, &GlowMenu, 1);
        {
            Pendelum::MenuItem* AColorSettings = Pendelum::CreateColorSettings(L"All", &config->glow[9], MainWindow);
            MainWindow->Add_Wrapped_Item(AColorSettings, DefusePlayerGlow, 2);

            Pendelum::MenuItem* VColorSettings = Pendelum::CreateColorSettings(L"Visible", &config->glow[10], MainWindow);
            MainWindow->Add_Wrapped_Item(VColorSettings, DefusePlayerGlow, 2);

            Pendelum::MenuItem* OColorSettings = Pendelum::CreateColorSettings(L"Occluded", &config->glow[11], MainWindow);
            MainWindow->Add_Wrapped_Item(OColorSettings, DefusePlayerGlow, 2);
        }
    }


    bool* NetworkMenu = new bool(false);

    Pendelum::MenuItem* NetworkCheckbox = Pendelum::CreateCheckBox(L"Network", NetworkMenu);
    MainWindow->Add_Item(NetworkCheckbox);

    {
        bool* Backtrack = new bool(false);
        Pendelum::MenuItem* EnemyGlowAllCheckbox = Pendelum::CreateCheckBox(L"Backtrack", Backtrack);
        MainWindow->Add_Wrapped_Item(EnemyGlowAllCheckbox, NetworkMenu, 1);
        {
            Pendelum::MenuItem* AColorSettings = Pendelum::CreateCheckBox(L"Enabled", &config->backtrack.enabled);
            MainWindow->Add_Wrapped_Item(AColorSettings, Backtrack, 2);

            Pendelum::MenuItem* VColorSettings = Pendelum::CreateCheckBox(L"Extended FakeLag +200ms", &config->backtrack.fakeLatency);
            MainWindow->Add_Wrapped_Item(VColorSettings, Backtrack, 2);

            Pendelum::MenuItem* TColorSettings = Pendelum::CreateCheckBox(L"Extended With TickShift +200ms (Experimental)", &config->backtrack.tickShift);
            MainWindow->Add_Wrapped_Item(TColorSettings, Backtrack, 2);

            Pendelum::MenuItem* OColorSettings = Pendelum::CreateIntSlider(L"Backtrack Time", &config->backtrack.timeLimit, 0, 600, 10);
            MainWindow->Add_Wrapped_Item(OColorSettings, Backtrack, 2);




        }
    }

    bool* VisualsMenu = new bool(false);

    Pendelum::MenuItem* VisualsCheckbox = Pendelum::CreateCheckBox(L"Visuals", VisualsMenu);
    MainWindow->Add_Item(VisualsCheckbox);

    {
        bool* dlights = new bool(false);
        Pendelum::MenuItem* DlightsMenu = Pendelum::CreateCheckBox(L"Player Dlights", dlights);
        MainWindow->Add_Wrapped_Item(DlightsMenu, VisualsMenu, 1);
        {
            Pendelum::MenuItem* DLightColorSettings = Pendelum::CreateColorSettings(L"Dlights", &config->visuals.dlight, MainWindow);
            MainWindow->Add_Wrapped_Item(DLightColorSettings, dlights, 2);
            {
                Pendelum::MenuItem* DlightRadius = Pendelum::CreateFloatSlider(L"Radius", &config->visuals.dlightRadius, 100, 400,10.f);
                MainWindow->Add_Wrapped_Item(DlightRadius, dlights, 2);

                Pendelum::MenuItem* DlightExponent = Pendelum::CreateIntSlider(L"Exponent", &config->visuals.dlightExponent, 0, 126,1);
                MainWindow->Add_Wrapped_Item(DlightExponent, dlights, 2);
            }

        }

        bool* lpdlights = new bool(false);
        Pendelum::MenuItem* lpDlightsMenu = Pendelum::CreateCheckBox(L"LocalPlayer Dlights", lpdlights);
        MainWindow->Add_Wrapped_Item(lpDlightsMenu, VisualsMenu, 1);
        {
            Pendelum::MenuItem* DLightColorSettings = Pendelum::CreateColorSettings(L"Dlights", &config->visuals.lpdlight, MainWindow);
            MainWindow->Add_Wrapped_Item(DLightColorSettings, lpdlights, 2);
            {
                Pendelum::MenuItem* DlightRadius = Pendelum::CreateFloatSlider(L"Radius", &config->visuals.lpdlightRadius, 100, 400, 10.f);
                MainWindow->Add_Wrapped_Item(DlightRadius, lpdlights, 2);

                Pendelum::MenuItem* DlightExponent = Pendelum::CreateIntSlider(L"Exponent", &config->visuals.lpdlightExponent, 0, 126, 1);
                MainWindow->Add_Wrapped_Item(DlightExponent, lpdlights, 2);
            }

        }

        bool*  ColorCorrection = EasyCreate::createNewSubMenu(L"Color Correction", MainWindow, VisualsMenu, 1);

        {
            EasyCreate::createCheckBox(L"Enable", MainWindow, &config->visuals.colorCorrection.enabled, ColorCorrection, 2);
            EasyCreate::createFloatSlider(L"Red", MainWindow, &config->visuals.colorCorrection.red, 0.f, 1.f, .1f, ColorCorrection, 2);
            EasyCreate::createFloatSlider(L"Green", MainWindow, &config->visuals.colorCorrection.green, 0.f, 1.f, .1f, ColorCorrection, 2);
            EasyCreate::createFloatSlider(L"Blue", MainWindow, &config->visuals.colorCorrection.blue, 0.f, 1.f, .1f, ColorCorrection, 2);
            EasyCreate::createFloatSlider(L"Yellow", MainWindow, &config->visuals.colorCorrection.yellow, 0.f, 1.f, .1f, ColorCorrection, 2);
            EasyCreate::createFloatSlider(L"Ghost", MainWindow, &config->visuals.colorCorrection.ghost, 0.f, 1.f, .1f, ColorCorrection, 2);
            EasyCreate::createFloatSlider(L"Mono", MainWindow, &config->visuals.colorCorrection.mono, 0.f, 1.f, .1f, ColorCorrection, 2);
            EasyCreate::createFloatSlider(L"Saturation", MainWindow, &config->visuals.colorCorrection.saturation, 0.f, 1.f, .1f, ColorCorrection, 2);
        }



        bool* Fog = EasyCreate::createNewSubMenu(L"Fog", MainWindow, VisualsMenu, 1);
        {
            bool* FogM = EasyCreate::createNewSubMenu(L"Main Fog", MainWindow, Fog, 2);
            EasyCreate::createColorToggle(L"Color", MainWindow, &config->visuals.FogControl.Fog.Color, FogM, 3);
            EasyCreate::createFloatSlider(L"Hdr", MainWindow, &config->visuals.FogControl.Fog.flHdrColorScale, 0.f, 1.f, .1f, FogM, 3);
            EasyCreate::createIntSlider(L"Start Distance", MainWindow, &config->visuals.FogControl.Fog.iStartDistance, 0, 8000, 100, FogM, 3);
            EasyCreate::createIntSlider(L"End Distance", MainWindow, &config->visuals.FogControl.Fog.iEndDistance, 0, 8000, 100, FogM, 3);

            bool* SkyFog = EasyCreate::createNewSubMenu(L"Sky Fog", MainWindow, Fog, 2);
            EasyCreate::createColorToggle(L"Color", MainWindow, &config->visuals.FogControl.Sky.Color, SkyFog, 3);
            EasyCreate::createFloatSlider(L"Hdr", MainWindow, &config->visuals.FogControl.Sky.flHdrColorScale, 0.f, 1.f, .1f, SkyFog, 3);
            EasyCreate::createIntSlider(L"Start Distance", MainWindow, &config->visuals.FogControl.Sky.iStartDistance, 0, 8000, 100, SkyFog, 3);
            EasyCreate::createIntSlider(L"End Distance", MainWindow, &config->visuals.FogControl.Sky.iEndDistance, 0, 8000, 100, SkyFog, 3);
        }

        bool* bulletTracers = EasyCreate::createNewSubMenu(L"Bullet Tracers", MainWindow, VisualsMenu, 1);
        {
            EasyCreate::createColorToggle(L"LocalPlayer", MainWindow, &config->visuals.bulletTracers, bulletTracers, 2);
            EasyCreate::createColorToggle(L"Enemies", MainWindow, &config->visuals.bulletTracersEnemy, bulletTracers, 2);
        }

        bool* skyboxmenu = EasyCreate::createNewSubMenu(L"Skybox", MainWindow, VisualsMenu, 1);
        {
            EasyCreate::createCheckBox(L"Disable 3D Skyboxes", MainWindow, &config->visuals.no3dSky, skyboxmenu, 2);
            EasyCreate::createCombo(L"Skybox", MainWindow, &config->visuals.skybox, &Skyboxes, skyboxmenu, 2);
        }


        bool* modulation = new bool(false);
        Pendelum::MenuItem* ModulationMenu = Pendelum::CreateCheckBox(L"Color Modulation", modulation);
        MainWindow->Add_Wrapped_Item(ModulationMenu, VisualsMenu, 1);

        EasyCreate::createColorToggle(L"Sky Color", MainWindow, &config->visuals.sky, modulation, 2);

        Pendelum::MenuItem* WorldColorSettings = Pendelum::CreateColorSettings(L"World", &config->visuals.world, MainWindow);
        MainWindow->Add_Wrapped_Item(WorldColorSettings, modulation, 2);

        bool* exmodulation = new bool(false);
        Pendelum::MenuItem* ExWorldModulationMenu = Pendelum::CreateCheckBox(L"Extra Options (World)", exmodulation);
        MainWindow->Add_Wrapped_Item(ExWorldModulationMenu, modulation, 2);
        {
            Pendelum::MenuItem* PreCacheWColorSettings = Pendelum::CreateColorSettings(L"World (Pre-Cache)", &config->visuals.PrecacheWorld, MainWindow);
            MainWindow->Add_Wrapped_Item(PreCacheWColorSettings, exmodulation, 3);

            Pendelum::MenuItem* NonCacheWColorSettings = Pendelum::CreateColorSettings(L"World (Not Cached)", &config->visuals.NonCachedWorld, MainWindow);
            MainWindow->Add_Wrapped_Item(NonCacheWColorSettings, exmodulation, 3);
        }


        Pendelum::MenuItem* PropColorSettings = Pendelum::CreateColorSettings(L"Props", &config->visuals.props, MainWindow);
        MainWindow->Add_Wrapped_Item(PropColorSettings, modulation, 2);

        bool* expmodulation = EasyCreate::createNewSubMenu(L"Extra Options (Props)", MainWindow, modulation, 2);
        {
            Pendelum::MenuItem* PreCacheWColorSettings = Pendelum::CreateColorSettings(L"Props (Pre-Cache)", &config->visuals.PrecacheProps, MainWindow);
            MainWindow->Add_Wrapped_Item(PreCacheWColorSettings, expmodulation, 3);

            Pendelum::MenuItem* NonCacheWColorSettings = Pendelum::CreateColorSettings(L"Prop (Not Cached)", &config->visuals.NonCached, MainWindow);
            MainWindow->Add_Wrapped_Item(NonCacheWColorSettings, expmodulation, 3);
        }
        
        Pendelum::MenuItem* NonCacheWColorSettings = Pendelum::CreateColorSettings(L"Ambient Light", &config->visuals.NightMode, MainWindow);
        MainWindow->Add_Wrapped_Item(NonCacheWColorSettings, modulation, 2);

        EasyCreate::createColorToggle(L"Moltov Color", MainWindow, &config->visuals.moltovcolor, VisualsMenu, 1);
        EasyCreate::createColorToggle(L"World Lights", MainWindow, &config->visuals.DrawLights, VisualsMenu, 1);
        bool* NavDraw = EasyCreate::createNewSubMenu(L"Nav Color", MainWindow, VisualsMenu, 1);
        {
            EasyCreate::createColorToggle(L"Enemy Nav", MainWindow, &config->visuals.DrawNavs, NavDraw, 2);
            EasyCreate::createColorToggle(L"Entire Nav Map", MainWindow, &config->visuals.AllDrawNavs, NavDraw, 2);


            EasyCreate::createCheckBox(L"Draw Filled", MainWindow, &config->visuals.DrawNavSettings.DrawNavsRect, NavDraw, 2);
            /* TODO: Create this */
            EasyCreate::createIntSlider(L"Nav Depth Level", MainWindow, &config->visuals.DrawNavSettings.DrawNavDepth, 0, 10, 1, NavDraw, 2);
            EasyCreate::createCheckBox(L"Draw Center Line", MainWindow, &config->visuals.DrawNavSettings.DrawCenterLines, NavDraw, 2);
            EasyCreate::createCheckBox(L"PreCalculate Connections", MainWindow, &config->visuals.DrawNavSettings.CalcAllConnections, NavDraw, 2);
            EasyCreate::createCheckBox(L"PreCalculate Nav Vector", MainWindow, &config->visuals.DrawNavSettings.CalculateAllNavVectors, NavDraw, 2);
        }

        EasyCreate::createColorToggle(L"WaterMark", MainWindow, &config->misc.watermark, VisualsMenu, 1);
        EasyCreate::createCheckBox(L"Third-Person", MainWindow, &config->visuals.thirdperson, VisualsMenu, 1);
        EasyCreate::createHotKey(L"Third-Person Key", MainWindow, &config->visuals.thirdpersonKey, VisualsMenu, 1);
        EasyCreate::createIntSlider(L"Third-Person Offset", MainWindow, &config->visuals.thirdpersonDistance, 0, 500, 10, VisualsMenu, 1);
        EasyCreate::createCheckBox(L"Radar Hack", MainWindow, &config->misc.radarHack, VisualsMenu, 1);
        EasyCreate::createCheckBox(L"Show Rank", MainWindow, &config->misc.revealRanks, VisualsMenu, 1);
        EasyCreate::createColorToggle(L"Show Spectators", MainWindow, &config->misc.spectatorList, VisualsMenu, 1);
        EasyCreate::createCheckBox(L"No Post-Processing", MainWindow, &config->visuals.disablePostProcessing, VisualsMenu, 1);
        EasyCreate::createCheckBox(L"No Blur", MainWindow, &config->visuals.noBlur, VisualsMenu, 1);
        EasyCreate::createCheckBox(L"No Recoil", MainWindow, &config->visuals.noAimPunch, VisualsMenu, 1);
        EasyCreate::createCheckBox(L"No Scope", MainWindow, &config->visuals.noScopeOverlay, VisualsMenu, 1);
        EasyCreate::createCheckBox(L"Full HLTV Experience On Spectate", MainWindow, &config->visuals.fullHLTV, VisualsMenu, 1);
    }


    bool* ChamsMenu = EasyCreate::createNewSubMenu(L"Chams", MainWindow);
    CMSave = ChamsMenu;
    {

        //EasyCreate::createCombo(L"Material", MainWindow, &config->backupChamMaterial.material, &ChamsMaterials, ChamsMenu, 1);
        EasyCreate::createCStrCombo("Chams Target", MainWindow, &current_cham_catagory, &ChamsCatagories, ChamsMenu, 1);
        EasyCreate::createCheckBox(L"Enabled", MainWindow, &(config->backupChamCatagorey).bEnabled, ChamsMenu, 1);
        EasyCreate::createCheckBox(L"Call Original", MainWindow, &(config->backupChamCatagorey).bCallOriginal, ChamsMenu, 1);
        EasyCreate::createIntSlider(L"Cham Number", MainWindow, &current_cham_material, 1, 10, 1, ChamsMenu, 1);
        EasyCreate::createMapCombo("Cham Material", MainWindow, &(config->backupChamMaterial).MaterialName, &ChamsMaterials, ChamsMenu, 1);
        EasyCreate::createCheckBox(L"Enable Layer", MainWindow, &(config->backupChamMaterial).bEnabled, ChamsMenu, 1);

        EasyCreate::createColorToggle(L"Color", MainWindow, &(config->backupChamMaterial.color), ChamsMenu, 1);

        EasyCreate::createCheckBox(L"Health Based", MainWindow, &(config->backupChamMaterial).Opts.bHealthBased, ChamsMenu, 1);
        EasyCreate::createCheckBox(L"Pulse", MainWindow, &(config->backupChamMaterial).Opts.bBlinking, ChamsMenu, 1);
        bool* FlagsMenu = EasyCreate::createNewSubMenu(L"Flags", MainWindow, ChamsMenu, 1);
        {
            EasyCreate::createCheckBox(L"WireFrame", MainWindow, &(config->backupChamMaterial).Opts.bWireframe, FlagsMenu, 2);
            EasyCreate::createCheckBox(L"Ignore Occlusion", MainWindow, &(config->backupChamMaterial).Opts.bIgnorez, FlagsMenu, 2);
            EasyCreate::createCheckBox(L"Call Original In SceneEnd", MainWindow, &(config->backupChamMaterial).bCallOriginalInSceneEnd, FlagsMenu, 2);
            EasyCreate::createCheckBox(L"Draw In SceneEnd", MainWindow, &(config->backupChamMaterial).bRenderInSceneEnd, FlagsMenu, 2);
        }
        EasyCreate::createCheckBox(L"Render SceneEndChams Above ViewModel", MainWindow, &config->debug.bRenderChamsOverViewModel, ChamsMenu, 1);


#if 0
        EasyCreate::createCombo(L"Chams Target", MainWindow, &current_cham_catagory, &ChamsCatagories, ChamsMenu, 1);
        EasyCreate::createIntSlider(L"Cham Number", MainWindow, &current_cham_material, 1,10, 1, ChamsMenu, 1);

        EasyCreate::createCheckBox(L"Enabled", MainWindow, &(config->backupChamMaterial).enabled, ChamsMenu,1);

        EasyCreate::createCheckBox(L"Draw All Backtrack Ticks", MainWindow, &config->backtrack.backtrackAll, &BacktrackCham, 1);
        EasyCreate::createIntSlider(L"StepSize", MainWindow, &config->backtrack.step, 1, 20, 1, &BacktrackCham, 1);
        EasyCreate::createCheckBox(L"Steal x88 Fade", MainWindow, &config->backtrack.backtrackx88, &BacktrackCham, 1);
        EasyCreate::createColorToggle(L"x88 Fade Color", MainWindow, &(config->backtrack.x88), &BacktrackCham, 1);
        EasyCreate::createFloatSlider(L"Breadcrumb Save Time", MainWindow, &(config->backtrack.breadcrumbtime), 0, 2000, 10, &BacktrackCham, 1);
        EasyCreate::createFloatSlider(L"Breadcrumb Exist Time", MainWindow, &(config->backtrack.breadexisttime), 0, 2000, 10, &BacktrackCham, 1);
        EasyCreate::createCheckBox(L"Enable Breadcrumbs", MainWindow, &(config->backtrack.extendedrecords), &BacktrackCham, 1);

        EasyCreate::createColorToggle(L"Color", MainWindow, &(config->backupChamMaterial.color), ChamsMenu, 1);

        EasyCreate::createCheckBox(L"Health Based", MainWindow, &(config->backupChamMaterial).healthBased, ChamsMenu, 1);
        EasyCreate::createCheckBox(L"Pulse", MainWindow, &(config->backupChamMaterial).blinking, ChamsMenu, 1);
        bool* FlagsMenu = EasyCreate::createNewSubMenu(L"Flags", MainWindow, ChamsMenu, 1);
        {
            EasyCreate::createCheckBox(L"WireFrame", MainWindow, &(config->backupChamMaterial).wireframe, FlagsMenu, 2);
            EasyCreate::createCheckBox(L"Ignore Occlusion", MainWindow, &(config->backupChamMaterial).ignorez, FlagsMenu, 2);
            EasyCreate::createCheckBox(L"Cover Other Chams", MainWindow, &(config->backupChamMaterial).cover, FlagsMenu, 2);
        }
#endif
        //config->backupCham;
    }

#if 0
    bool* WChamsMenu = EasyCreate::createNewSubMenu(L"World Chams", MainWindow);
    {     

        EasyCreate::createCombo(L"Material", MainWindow, &config->World.material, &ChamsMaterials, WChamsMenu, 1);
        EasyCreate::createCheckBox(L"Enabled", MainWindow, &config->World.enabled, WChamsMenu, 1);

        EasyCreate::createColorToggle(L"Color", MainWindow, &(config->World.color), WChamsMenu, 1);

        EasyCreate::createCheckBox(L"Pulse", MainWindow, &config->World.blinking, WChamsMenu, 1);
        bool* FlagsMenu = EasyCreate::createNewSubMenu(L"Flags", MainWindow, WChamsMenu, 1);
        {
            EasyCreate::createCheckBox(L"WireFrame", MainWindow, &config->World.wireframe, FlagsMenu, 2);
            EasyCreate::createCheckBox(L"Ignore Occlusion", MainWindow, &config->World.ignorez, FlagsMenu, 2);
        }

        EasyCreate::createCheckBox(L"Update", MainWindow, &WChamUpdate, WChamsMenu, 1);

    }
#endif
#if 0
    bool* SPChamsMenu = EasyCreate::createNewSubMenu(L"Static Prop Chams", MainWindow);
    {
        EasyCreate::createCombo(L"Material", MainWindow, &config->StaticProps.material, &ChamsMaterials, SPChamsMenu, 1);
        EasyCreate::createCheckBox(L"Enabled", MainWindow, &config->StaticProps.enabled, SPChamsMenu, 1);

        EasyCreate::createColorToggle(L"Color", MainWindow, &(config->StaticProps.color), SPChamsMenu, 1);

        EasyCreate::createCheckBox(L"Pulse", MainWindow, &config->StaticProps.blinking, SPChamsMenu, 1);
        bool* FlagsMenu = EasyCreate::createNewSubMenu(L"Flags", MainWindow, SPChamsMenu, 1);
        {
            EasyCreate::createCheckBox(L"WireFrame", MainWindow, &config->StaticProps.wireframe, FlagsMenu, 2);
            EasyCreate::createCheckBox(L"Ignore Occlusion", MainWindow, &config->StaticProps.ignorez, FlagsMenu, 2);
        }
        EasyCreate::createCheckBox(L"Update", MainWindow, &WChamUpdate, SPChamsMenu, 1);
    }
#endif

    bool* AimbotMenu = EasyCreate::createNewSubMenu(L"Legit Aimbot", MainWindow);

    {
        EasyCreate::createCombo(L"Weapon Class", MainWindow, &weapon_class, &WeaponClass, AimbotMenu, 1);
        EasyCreate::createCombo(L"Weapon", MainWindow, &weapon, weaponv, AimbotMenu, 1);

        EasyCreate::createCheckBox(L"Enabled", MainWindow, &(aimbot_weapon.enabled), AimbotMenu, 1);
        EasyCreate::createHotKey(L"On Key", MainWindow, &(aimbot_weapon.key), AimbotMenu, 1);

        EasyCreate::createFloatSlider(L"FOV", MainWindow, &(aimbot_weapon.fov), 1.f, 255.f, 1.f, AimbotMenu, 1);
        EasyCreate::createFloatSlider(L"Smoothing", MainWindow, &(aimbot_weapon.smooth), 1.f, 100.f, 1.f, AimbotMenu, 1);
        EasyCreate::createFloatSlider(L"Hitchance", MainWindow, &(aimbot_weapon.hitChance), 0.f, 100.f, 1.f, AimbotMenu, 1);

        EasyCreate::createCheckBox(L"Aimlock", MainWindow, &(aimbot_weapon.aimlock), AimbotMenu, 1);
        EasyCreate::createCheckBox(L"Silent", MainWindow, &(aimbot_weapon.silent), AimbotMenu, 1);
        EasyCreate::createCheckBox(L"Auto Shoot", MainWindow, &(aimbot_weapon.autoShot), AimbotMenu, 1);
        EasyCreate::createCheckBox(L"Auto Scope", MainWindow, &(aimbot_weapon.autoScope), AimbotMenu, 1);
        EasyCreate::createCheckBox(L"Delay User Shot For HC", MainWindow, &(aimbot_weapon.shootonhconly), AimbotMenu, 1);
        EasyCreate::createCheckBox(L"AutoWall", MainWindow, &(aimbot_weapon.autowall), AimbotMenu, 1);
        EasyCreate::createIntSlider(L"Min Damage", MainWindow, &(aimbot_weapon.minDamage), 1, 200, 5, AimbotMenu, 1);
        EasyCreate::createCombo(L"Scan Option", MainWindow, &(aimbot_weapon.bone), &Bones,  AimbotMenu, 1);
        /*
        std::vector<std::wstring> hitboxstr = {
                L"Head",
                L"Neck",
                L"Pelvis",
                L"Abdomen",
                L"Kidneys",
                L"Sternum",
                L"Clavicles",
                L"Left Thigh",
                L"Right Thigh",
                L"Left Shin",
                L"Right Shin",
                L"Left Ankle",
                L"Right Ankle",
                L"Left Hand",
                L"Right Hand",
                L"Left Arm",
                L"Left Forearm",
                L"Right Arm",
                L"Right Forearm",
                L"ALL"
        };

        bool* RageHit = EasyCreate::createNewSubMenu(L"HitBoxes", MainWindow, AimbotMenu, 1);
        {
            for (int i = 0; i < 18; i++) {
                EasyCreate::createCheckBox(hitboxstr.at(i).c_str(), MainWindow, &(aimbot_weapon.hitboxes[i]), RageHit, 2);
            }

        }
        */


        EasyCreate::createCheckBox(L"Ignore Flash", MainWindow, &(aimbot_weapon.ignoreFlash), AimbotMenu, 1);
        EasyCreate::createCheckBox(L"Ignore Smoke", MainWindow, &(aimbot_weapon.ignoreSmoke), AimbotMenu, 1);
    }

    bool* RageBotMenu = EasyCreate::createNewSubMenu(L"RageBot", MainWindow);

    {
        EasyCreate::createCheckBox(L"Enable Rage", MainWindow, &config->rageEnabled, RageBotMenu, 1);
        EasyCreate::createCombo(L"Weapon Class", MainWindow, &weapon_class, &WeaponClass, RageBotMenu, 1);
        EasyCreate::createCombo(L"Weapon", MainWindow, &weapon, weaponv, RageBotMenu, 1);

        EasyCreate::createCheckBox(L"Enabled", MainWindow, &(RageBot_weapon.enabled), RageBotMenu, 1);

        EasyCreate::createFloatSlider(L"FOV", MainWindow, &(RageBot_weapon.fov), 1.f, 255.f, 1.f, RageBotMenu, 1);
        EasyCreate::createFloatSlider(L"Smoothing", MainWindow, &(RageBot_weapon.smooth), 1.f, 100.f, 1.f, RageBotMenu, 1);
        EasyCreate::createFloatSlider(L"Hitchance", MainWindow, &(RageBot_weapon.hitChance), 0.f, 100.f, 1.f, RageBotMenu, 1);

        bool* Hitc = EasyCreate::createNewSubMenu(L"Hitchance Options", MainWindow, RageBotMenu, 1);
        {
            EasyCreate::createCheckBox(L"Use ClipRayToEntity", MainWindow, &(RageBot_weapon.ClipRay), Hitc, 2);
            EasyCreate::createCheckBox(L"Parse Target Hitbox Only", MainWindow, &(RageBot_weapon.ParseHitboxOnly), Hitc, 2);
            EasyCreate::createCheckBox(L"std::async (Experimental & Slow)", MainWindow, &(RageBot_weapon.Async), Hitc, 2);
        }


        EasyCreate::createCheckBox(L"Aimlock", MainWindow, &(RageBot_weapon.aimlock), RageBotMenu, 1);
        EasyCreate::createCheckBox(L"Silent", MainWindow, &(RageBot_weapon.silent), RageBotMenu, 1);
        EasyCreate::createCheckBox(L"Auto Shoot", MainWindow, &(RageBot_weapon.autoShot), RageBotMenu, 1);
        EasyCreate::createCheckBox(L"Auto Scope", MainWindow, &(RageBot_weapon.autoScope), RageBotMenu, 1);
        EasyCreate::createCombo(L"Scan Option", MainWindow, &(RageBot_weapon.bone), &Bones, RageBotMenu, 1);
        
        std::vector<std::wstring> hitboxstr = {
                L"Head",
                L"Neck",
                L"Pelvis",
                L"Abdomen",
                L"Kidneys",
                L"Sternum",
                L"Clavicles",
                L"Left Thigh",
                L"Right Thigh",
                L"Left Shin",
                L"Right Shin",
                L"Left Ankle",
                L"Right Ankle",
                L"Left Hand",
                L"Right Hand",
                L"Left Arm",
                L"Left Forearm",
                L"Right Arm",
                L"Right Forearm",
                L"ALL"
        };

        bool* RageHit = EasyCreate::createNewSubMenu(L"HitBoxes", MainWindow, RageBotMenu, 1);
        {
            for (int i = 0; i < 18; i++) {
                EasyCreate::createCheckBox(hitboxstr.at(i).c_str(), MainWindow, &(RageBot_weapon.hitboxes[i]), RageHit, 2);
            }
            EasyCreate::createCheckBox(L"Respect Hitbox Select (Never Override)", MainWindow, &(RageBot_weapon.respectHitbox), RageHit, 2);
            EasyCreate::createCheckBox(L"Safe Point", MainWindow, &(RageBot_weapon.safepoint), RageHit, 2);
        }
        
        bool* RageBT = EasyCreate::createNewSubMenu(L"Backtrack Integration", MainWindow, RageBotMenu, 1);
        {

            EasyCreate::createCheckBox(L"On-Shot", MainWindow, &(RageBot_weapon.onshot), RageBT, 2);
            EasyCreate::createCheckBox(L"LBY Update", MainWindow, &(RageBot_weapon.ShootForLBY), RageBT, 2);
            EasyCreate::createCheckBox(L"Pelvis Aim On LBY Update", MainWindow, &(RageBot_weapon.pelvisAimOnLBYUpdate), RageBT, 2);
        }

        bool* RageResolv = EasyCreate::createNewSubMenu(L"Backtrack Integration", MainWindow, RageBotMenu, 1);
        {

            EasyCreate::createCheckBox(L"On-Shot Or Desyncless Only", MainWindow, &(RageBot_weapon.OnshotOrDesyncless), RageResolv, 2);
            EasyCreate::createCheckBox(L"Prioritize Head Aim On No-Desync", MainWindow, &(RageBot_weapon.PrioritizeHeadOnNoDesync), RageResolv, 2);
        }

        bool* MinDamages = EasyCreate::createNewSubMenu(L"MinDamages", MainWindow, RageBotMenu, 1);
        {

            EasyCreate::createIntSlider(L"Min Damage Autowall", MainWindow, &(RageBot_weapon.minDamage), 0, 200, 2, MinDamages, 2);
            EasyCreate::createFloatSlider(L"Min Damage Visible", MainWindow, &(RageBot_weapon.minDamageVis), 0, 200, 2, MinDamages, 2);
        }

        EasyCreate::createCheckBox(L"Ignore Smoke", MainWindow, &(RageBot_weapon.ignoreSmoke), RageBotMenu, 1);
        EasyCreate::createCheckBox(L"Force Enemies To Peak (BETA)", MainWindow, &(RageBot_weapon.m_bForcePeak), RageBotMenu, 1);


    }

    bool* AntiAimMenu = EasyCreate::createNewSubMenu(L"AntiAim", MainWindow);

    {
        EasyCreate::createCheckBox(L"Enable", MainWindow, &config->antiAim.enabled, AntiAimMenu, 1);
        EasyCreate::createCheckBox(L"LegitAA", MainWindow, &config->antiAim.legitaa, AntiAimMenu, 1);

        EasyCreate::createCheckBox(L"Pitch", MainWindow, &config->antiAim.pitch, AntiAimMenu, 1);
        EasyCreate::createFloatSlider(L"Pitch Val", MainWindow, &config->antiAim.pitchAngle, -89.f, 89.f, 1.f, AntiAimMenu, 1);

        EasyCreate::createCheckBox(L"Yaw", MainWindow, &config->antiAim.yaw, AntiAimMenu, 1);
        EasyCreate::createFloatSlider(L"Pitch Val", MainWindow, &config->antiAim.manYaw, -180.f, 180.f, 1.f, AntiAimMenu, 1);

        EasyCreate::createCheckBox(L"Jitter", MainWindow, &config->antiAim.bJitter, AntiAimMenu, 1);
        EasyCreate::createIntSlider(L"Jitter Amount", MainWindow, &config->antiAim.JitterRange, -120, 120, 1, AntiAimMenu, 1);
        EasyCreate::createIntSlider(L"Jitter Chance", MainWindow, &config->antiAim.JitterRange, 0, 100, 1, AntiAimMenu, 1);

        EasyCreate::createIntSlider(L"v1", MainWindow, &config->antiAim.v1, -120, 120, 5, AntiAimMenu, 1);
        EasyCreate::createIntSlider(L"v2", MainWindow, &config->antiAim.v2, -58, 58, 1, AntiAimMenu, 1);
        EasyCreate::createIntSlider(L"v4", MainWindow, &config->antiAim.v4, -120, 120, 5, AntiAimMenu, 1);

        EasyCreate::createCheckBox(L"Pre-Break LBY", MainWindow, &config->antiAim.preBreak, AntiAimMenu, 1);
        EasyCreate::createCheckBox(L"To Wall", MainWindow, &config->antiAim.toWall, AntiAimMenu, 1);

        EasyCreate::createCheckBox(L"Move Fix", MainWindow, &config->misc.fixMovement, AntiAimMenu, 1);

    }

    bool* MovementMenu = EasyCreate::createNewSubMenu(L"Movement", MainWindow);

    {
        EasyCreate::createCheckBox(L"Auto Bunny Hop", MainWindow, &config->misc.bunnyHop, MovementMenu, 1);
        EasyCreate::createIntSlider(L"Bunny Hop Chance", MainWindow, &config->misc.bunnyHopChance, 0, 100, 1, MovementMenu, 1);
        EasyCreate::createIntSlider(L"Bunny Hop Max", MainWindow, &config->misc.bunnymax, 0, 20, 1, MovementMenu, 1);
        EasyCreate::createCheckBox(L"Auto-Strafe", MainWindow, &config->misc.autoStrafe, MovementMenu, 1);
        EasyCreate::createCheckBox(L"Rage Auto-Strafe", MainWindow, &config->misc.ragestrafe, MovementMenu, 1);
    }

    bool* AimAssistMenu = EasyCreate::createNewSubMenu(L"Aim Assist (Advanced Aim)", MainWindow);

    {
        EasyCreate::createCheckBox(L"Enabled", MainWindow, &config->aimassist_single.enabled, AimAssistMenu, 1);
        EasyCreate::createCheckBox(L"Only After Hit", MainWindow, &config->aimassist_single.onlyAfterHit, AimAssistMenu, 1);
        EasyCreate::createFloatSlider(L"FOV", MainWindow, &config->aimassist_single.FOV, 0.f, 200.f, 1.f, AimAssistMenu, 1);
        //EasyCreate::createFloatSlider(L"Lock FOV", MainWindow, &config->aimassist_single.lockFOV, 0.f, 100.f, 1.f, AimAssistMenu, 1);
        EasyCreate::createFloatSlider(L"Penalty %", MainWindow, &config->aimassist_single.Penalty, 0.f, 100.f, 1.f, AimAssistMenu, 1);
        EasyCreate::createFloatSlider(L"Away Pentalty %", MainWindow, &config->aimassist_single.AwayPenalty, 0.f, 100.f, 1.f, AimAssistMenu, 1);
        EasyCreate::createFloatSlider(L"Max Speed Increase %", MainWindow, &config->aimassist_single.maxSpeed, 0.f, 200.f, 5.f, AimAssistMenu, 1);
    }




    bool* GriMenu = EasyCreate::createNewSubMenu(L"Grief", MainWindow);

    {
        EasyCreate::createCheckBox(L"Team Damage Counter", MainWindow, &config->grief.teamDamageOverlay, GriMenu, 1);
        EasyCreate::createCheckBox(L"Trigger Bot Name Stealer", MainWindow, &config->grief.triggername, GriMenu, 1);
        EasyCreate::createCheckBox(L"Chat Spam", MainWindow, &config->grief.spam.enabled, GriMenu, 1);
        EasyCreate::createCheckBox(L"Location", MainWindow, &config->grief.spam.position, GriMenu, 1);
        EasyCreate::createCheckBox(L"Kill Message", MainWindow, &config->misc.killMessage, GriMenu, 1);
        EasyCreate::createHotKey(L"Grenade Aim", MainWindow, &config->grief.grenadeAIM, GriMenu, 1);
        EasyCreate::createHotKey(L"Block Bot", MainWindow, &config->grief.BlockBot, GriMenu, 1);
    }


    
#ifdef _MENU_DEBUG
    MDebug = true;
    EasyCreate::createCheckBox(L"Menu Debug", MainWindow, &MDebug);
#endif



#ifdef _EXTRA_DEBUG
    bool* debugMenu = EasyCreate::createNewSubMenu(L"Additional Debug", MainWindow);
    
    {
#ifdef DEV_SECRET_BUILD
        EasyCreate::createCheckBox(L"Turn Off All Rendering", MainWindow, &config->debug.bNoRender, debugMenu, 1);
        EasyCreate::createCheckBox(L"Local Timing", MainWindow, &config->debug.LocalTiming, debugMenu, 1);
        EasyCreate::createColorToggle(L"Client State", MainWindow, &config->debug.clientstate, debugMenu, 1);
        EasyCreate::createColorToggle(L"Netchannel", MainWindow, &config->debug.networkchannel, debugMenu, 1);
        EasyCreate::createColorToggle(L"Backtrack", MainWindow, &config->debug.backtrack.color, debugMenu, 1);
        EasyCreate::createCheckBox(L"Backtrack Mon", MainWindow, &config->debug.backtrack.enabled, debugMenu, 1);
        EasyCreate::createCheckBox(L"Backtrack Find Active", MainWindow, &config->debug.backtrack.findactive, debugMenu, 1);
        EasyCreate::createIntSlider(L"Entity ID", MainWindow, &config->debug.backtrack.entityid, 0, 64, 1, debugMenu, 1);
        EasyCreate::createCheckBox(L"Dump Net Out", MainWindow, &config->debug.dumpNetInAndOut, debugMenu, 1);
        EasyCreate::createCheckBox(L"Infinite Prediction", MainWindow, &config->debug.InfinitePred, debugMenu, 1);
#endif
        EasyCreate::createCheckBox(L"Slow-walk", MainWindow, &config->misc.slowwalk, debugMenu, 1);
        EasyCreate::createHotKey(L"Slow-walk Key", MainWindow, &config->misc.slowwalkKey, debugMenu, 1);

        EasyCreate::createCheckBox(L"Timing Stats", MainWindow, &config->debug.TimingStats, debugMenu, 1);
        //EasyCreate::createCheckBox(L"Auto-Accept", MainWindow, &config->misc.autoAccept, debugMenu, 1);
       // EasyCreate::createCheckBox(L"Auto-Queue", MainWindow, &config->debug.AutoQueue, debugMenu, 1);
        EasyCreate::createCheckBox(L"Damage Done", MainWindow, &config->misc.showDamagedone, debugMenu, 1);
        EasyCreate::createCheckBox(L"Animation Fix", MainWindow, &config->debug.Animfix, debugMenu, 1);
        EasyCreate::createCheckBox(L"Cham Matrix On Shot", MainWindow, &config->debug.showshots, debugMenu, 1);
        EasyCreate::createCheckBox(L"Draw Multipoints", MainWindow, &config->debug.drawMultipoints, debugMenu, 1);
        EasyCreate::createCheckBox(L"Draw New Multipoints", MainWindow, &config->debug.drawHitPoints, debugMenu, 1);
        EasyCreate::createCheckBox(L"Draw Original Multipoints", MainWindow, &config->debug.drawOrgMultipoints, debugMenu, 1);
        EasyCreate::createCheckBox(L"Draw Dynam Multipoints", MainWindow, &config->debug.drawDynamicPoints, debugMenu, 1);
        EasyCreate::createCheckBox(L"New Rage Bot", MainWindow, &config->debug.newRage, debugMenu, 1);
        EasyCreate::createCheckBox(L"Clan Tag", MainWindow, &config->misc.customClanTag, debugMenu, 1);
        EasyCreate::createCheckBox(L"Resolver", MainWindow, &config->debug.animstatedebug.resolver.enabled, debugMenu, 1);
        EasyCreate::createCheckBox(L"Go 'Til Kill", MainWindow, &config->debug.animstatedebug.resolver.goforkill, debugMenu, 1);
        EasyCreate::createCheckBox(L"Parse all hitboxes in Resolver detection", MainWindow, &config->debug.parseALL, debugMenu, 1);
        EasyCreate::createIntSlider(L"Weird Shit", MainWindow, &config->debug.weird, 0, 128, 1, debugMenu, 1);
        EasyCreate::createCheckBox(L"Spam Vest Buy", MainWindow, &config->debug.Vest, debugMenu, 1);
        EasyCreate::createCheckBox(L"Force Spectator With Additional Thread", MainWindow, &config->debug.spectateForce, debugMenu, 1);
        EasyCreate::createCheckBox(L"Fuck W/ Voice Buffer", MainWindow, &config->debug.voiceBufferFuck, debugMenu, 1);
        EasyCreate::createCheckBox(L"Always AA (Bot Mimic Fix)", MainWindow, &config->debug.AAALWAYS, debugMenu, 1);
        EasyCreate::createCheckBox(L"Dump Voice Data", MainWindow, &config->debug.dumpVoiceData, debugMenu, 1);
        EasyCreate::createCheckBox(L"Dump Voice Data of Format", MainWindow, &config->debug.dumpVoiceDataFormat, debugMenu, 1);
#ifdef DEV_SECRET_BUILD
        EasyCreate::createCheckBox(L"Hard Mode", MainWindow, &config->debug.HardMode, debugMenu, 1);
        EasyCreate::createCheckBox(L"Lagger", MainWindow, &config->debug.Lagger, debugMenu, 1);
#endif
        EasyCreate::createCheckBox(L"Return During CLCMOVE", MainWindow, &config->debug.returnonCLCMOVE, debugMenu, 1);
        EasyCreate::createCheckBox(L"Return on NET_disconnect", MainWindow, &config->debug.dontsenddisconnect, debugMenu, 1);
        EasyCreate::createCheckBox(L"Always True for isHlTV", MainWindow, &config->debug.alwaystrueforhltv, debugMenu, 1);
        
        bool* console = EasyCreate::createNewSubMenu(L"Console Debug", MainWindow, debugMenu, 1);
        {
#ifndef DYLANS_WORRIED
#ifdef DEV_SECRET_BUILD
            EasyCreate::createCheckBox(L"SteamNetworkingSockets Info", MainWindow, &config->debug.SteamNetworkingDebug, console, 2);
            EasyCreate::createCheckBox(L"Connectionless Packet Info", MainWindow, &config->debug.PrintConnectionless, console, 2);
            EasyCreate::createCheckBox(L"Protection", MainWindow, &config->debug.DontRecieveVoice, console, 2);
#endif
#endif
            EasyCreate::createCheckBox(L"Tickbase Info", MainWindow, &config->debug.tickbasedebug, console, 2);
            EasyCreate::createCheckBox(L"Aimbot Info", MainWindow, &config->debug.aimbotcoutdebug, console, 2);
            EasyCreate::createCheckBox(L"Resolver Info", MainWindow, &config->debug.resolverDebug, console, 2);
            EasyCreate::createCheckBox(L"Backtrack Info", MainWindow, &config->debug.backtrackCount, console, 2);
            EasyCreate::createCheckBox(L"SendNetMessage Debug", MainWindow, &config->debug.printNetMSG, console, 2);
        }
        bool* cout = EasyCreate::createNewSubMenu(L"COUT Debug", MainWindow, debugMenu, 1);
        {
            EasyCreate::createColorToggle(L"Damage Log", MainWindow, &config->debug.DamageLog, cout, 2);
        }
        EasyCreate::createCheckBox(L"Transmit Matrix", MainWindow, &config->debug.TransmitMatrix, debugMenu, 1);
        
    }

    bool* TickBaseMenu = EasyCreate::createNewSubMenu(L"TickBase", MainWindow);

    {
        EasyCreate::createCheckBox(L"Double Tap", MainWindow, &config->m_Exploits.m_TickBase.m_DoubleTap.bEnabled, TickBaseMenu, 1);
        EasyCreate::createIntSlider(L"TickBase Shift Amount", MainWindow, &config->m_Exploits.m_TickBase.m_DoubleTap.nTicksToShift, 0, 16, 1, TickBaseMenu, 1);
        EasyCreate::createCheckBox(L"CL_Move DT", MainWindow, &config->m_Exploits.m_TickBase.m_DoubleTap.bClMoveDT, TickBaseMenu, 1);
        EasyCreate::createCheckBox(L"HostRunFrameInput DT", MainWindow, &config->m_Exploits.m_TickBase.m_DoubleTap.bHostRunFrameInputDT, TickBaseMenu, 1);
    }


    bool* RegionMenu = EasyCreate::createNewSubMenu(L"Region Changer", MainWindow);
    {
        EasyCreate::createCheckBox(L"Enabled", MainWindow, &config->RegionChanger.enabled, RegionMenu, 1);

        bool* RegionsO = EasyCreate::createNewSubMenu(L"Regions", MainWindow, RegionMenu, 1);
        {
            for (int i = 0; i < Regions.size(); i++) {
                EasyCreate::createCheckBox(Regions.at(i).c_str(), MainWindow, &(config->RegionChanger.Regions[i]), RegionsO, 2);
            }



        }

    }

    bool* WalkbotMenu = EasyCreate::createNewSubMenu(L"Walkbot", MainWindow);

    {
        EasyCreate::createCheckBox(L"Walkbot", MainWindow, &config->walkbot.enabled, WalkbotMenu, 1);
        EasyCreate::createHotKey(L"Walkbot On Key", MainWindow, &walkbot_key.Key, WalkbotMenu, 1);
        EasyCreate::createIntSlider(L"Look Smoothing", MainWindow, &config->walkbot.lookSmoothing, 1, 100, 1, WalkbotMenu, 1);
        EasyCreate::createFloatSlider(L"Walk Smoothing", MainWindow, &config->walkbot.walkSmoothing, 1.f, 100.f, 1.f, WalkbotMenu, 1);
        EasyCreate::createCheckBox(L"Bunny Hop", MainWindow, &config->walkbot.bunnyhop, WalkbotMenu, 1);
        EasyCreate::createCheckBox(L"Use Bounding Box Detection", MainWindow, &config->walkbot.boundingbox, WalkbotMenu, 1);
        EasyCreate::createCheckBox(L"Smooth Path (Experimental)", MainWindow, &config->walkbot.smoothPath, WalkbotMenu, 1);
        EasyCreate::createCheckBox(L"Draw Path", MainWindow, &config->walkbot.DrawPath, WalkbotMenu, 1);

        EasyCreate::createFloatSlider(L"Aimbot FOV", MainWindow, &config->walkbot.AimFov, 1.f, 255.f, 1.f, WalkbotMenu, 1);
        EasyCreate::createFloatSlider(L"Aimbot Hitchance", MainWindow, &config->walkbot.hitchance, 1.f, 100.f, 1.f, WalkbotMenu, 1);
        EasyCreate::createFloatSlider(L"Aimbot Smoothing", MainWindow, &config->walkbot.AimSmoothing, 1.f, 100.f, 1.f, WalkbotMenu, 1);
        EasyCreate::createCheckBox(L"Variable Smoothing", MainWindow, &config->walkbot.variableSmooth, WalkbotMenu, 1);
        EasyCreate::createCheckBox(L"Ignore Head", MainWindow, &config->walkbot.ignoreHead, WalkbotMenu, 1);

        EasyCreate::createCheckBox(L"No Render", MainWindow, &config->walkbot.NoRender, WalkbotMenu, 1);
        EasyCreate::createCheckBox(L"Talk", MainWindow, &config->walkbot.talk, WalkbotMenu, 1);


    }

    bool* ESPMenu = EasyCreate::createNewSubMenu(L"ESP", MainWindow);
    {
        EasyCreate::createColorToggle(L"Text", MainWindow, &config->esp.Text, ESPMenu, 1);
        EasyCreate::createColorToggle(L"Bounding Box", MainWindow, &config->esp.Box, ESPMenu, 1);
        EasyCreate::createCheckBox(L"Health Bar", MainWindow, &config->esp.HealthBar, ESPMenu, 1);
        EasyCreate::createCheckBox(L"Shared ESP", MainWindow, &config->esp.shared_esp.bEnabled);
    }





#ifdef MASTER_CONTROLLER

    bool* WalkbotControllerMenu = EasyCreate::createNewSubMenu(L"Walkbot Controls", MainWindow);

    {
        EasyCreate::createCheckBox(L"enable", MainWindow, &config->walkbotcontroller.enabled, WalkbotControllerMenu, 1);
        EasyCreate::createCheckBox(L"Swap To T", MainWindow, &config->walkbotcontroller.SwapToT, WalkbotControllerMenu, 1);
        EasyCreate::createCheckBox(L"Swap To CT", MainWindow, &config->walkbotcontroller.SwapToCT, WalkbotControllerMenu, 1);
        EasyCreate::createCheckBox(L"Turn On Voice", MainWindow, &config->walkbotcontroller.VoiceOn, WalkbotControllerMenu, 1);
        EasyCreate::createCheckBox(L"Turn Off Voice", MainWindow, &config->walkbotcontroller.VoiceOff, WalkbotControllerMenu, 1);
        EasyCreate::createHotKey(L"Enter Pos Mode Key", MainWindow, &config->walkbotcontroller.SetPosMode, WalkbotControllerMenu, 1);
        EasyCreate::createHotKey(L"ViewPoint Set Pos Key", MainWindow, &config->walkbotcontroller.SetPos, WalkbotControllerMenu, 1);
        EasyCreate::createCheckBox(L"Send Bots To Idle", MainWindow, &SendIdle, WalkbotControllerMenu, 1);
        EasyCreate::createCheckBox(L"Send Bots To Active Standby", MainWindow, &SendActiveStandby, WalkbotControllerMenu, 1);
    }


#endif

#endif

#ifdef _GRAPHS
    bool* graphsMenu = EasyCreate::createNewSubMenu(L"Graphs", MainWindow);

    {
        EasyCreate::createCheckBox(L"Enabled Graph", MainWindow, &config->debug.graph.enabled, graphsMenu, 1);
        EasyCreate::createCheckBox(L"Fps Graph", MainWindow, &config->debug.graph.FPSBar, graphsMenu, 1);
        EasyCreate::createCheckBox(L"Ping Graph", MainWindow, &config->debug.graph.Ping, graphsMenu, 1);
    }
#endif

    
    PlayerList::PlayerList* pList = new PlayerList::PlayerList(const_cast<wchar_t*>(L"Player List"), MainWindow);
    MainWindow->Add_Item(reinterpret_cast<Pendelum::MenuItem*>(pList));





}

#include "SDK/OsirisSDK/Engine.h"
#include "SDK/OsirisSDK/NetworkChannel.h"

static void printStatus(bool bStatus, const wchar_t* basestr, int &x, int &y) {
    
        interfaces->surface->setTextColor(150, 150, 250, 255);
        const wchar_t* status = (bStatus ? L"ON" : L"OFF");
        std::wstring currtime = { std::wstring(status) + L" - " + basestr };
        auto [w, h] = interfaces->surface->getTextSize(5, currtime.c_str());

        currtime = {L" - " + std::wstring(basestr) };
        auto [w2, h2] = interfaces->surface->getTextSize(5, currtime.c_str());

        interfaces->surface->setTextPosition(x - w2, y);
        interfaces->surface->printText(currtime.c_str());
        if (bStatus) {
            interfaces->surface->setTextColor(150,255,150,255);
        }
        else {
            interfaces->surface->setTextColor(250, 150, 150, 255);
        }
        interfaces->surface->setTextPosition(x - w, y);
        y += h + 2;
        interfaces->surface->printText(status);
    
}

#include "SDK/OsirisSDK/LocalPlayer.h"
#include "SDK/OsirisSDK/Entity.h"

#include <locale>
#include <codecvt>
#include <string>

inline std::wstring ConstCharToWString(const char* string) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wide = converter.from_bytes(string);
    return wide;
}

#include "Hacks/TickbaseManager.h"
#include "Timing.h"
static void DrawMenuDebug() {
    
    int x, y;
    x = screen.width;
    y = screen.y;
    x -= 2;

    interfaces->surface->setTextFont(5);
    
    interfaces->surface->setTextColor(255, 0, 0, 255);
#ifdef _INTERNAL_COPY
    {
        std::wstring currtime = L"WARNING: INTERNAL COPY, NOT FOR DISTRIBUTION";
        auto [w, h] = interfaces->surface->getTextSize(5, currtime.c_str());
        interfaces->surface->setTextPosition(x - w, y);
        y += h + 2;
        interfaces->surface->printText(currtime.c_str());
    }
#endif
#ifdef DEV_SECRET_BUILD
    {
        std::wstring currtime = L"WARNING: DEVELOPMENTAL BUILD WITH SECRET CODE - DO NOT DISTRIBUTE!!!";
        auto [w, h] = interfaces->surface->getTextSize(5, currtime.c_str());
        interfaces->surface->setTextPosition(x - w, y);
        y += h + 2;
        interfaces->surface->printText(currtime.c_str());
    }
#endif
#ifdef _DEBUG
    {
        std::wstring currtime = L"WARNING: DEBUG BUILD, NOT FOR DISTRIBUTION";
        auto [w, h] = interfaces->surface->getTextSize(5, currtime.c_str());
        interfaces->surface->setTextPosition(x - w, y);
        y += h + 2;
        interfaces->surface->printText(currtime.c_str());
    }
#endif
#ifdef _GRAPHS
    {
        std::wstring currtime = L"GRAPHS INCLUDED";
        auto [w, h] = interfaces->surface->getTextSize(5, currtime.c_str());
        interfaces->surface->setTextPosition(x - w, y);
        y += h + 2;
        interfaces->surface->printText(currtime.c_str());
    }
#endif


    interfaces->surface->setTextColor(0, 250, 0, 255);
    {
        std::wstring currtime = { std::to_wstring(memory->globalVars->currenttime) + L" - Current Time" };
        auto [w, h] = interfaces->surface->getTextSize(5, currtime.c_str());
        interfaces->surface->setTextPosition(x-w, y);
        y += h+2;
        interfaces->surface->printText(currtime.c_str());
    }

    {
        std::wstring currtime = { std::to_wstring(memory->globalVars->tickCount) + L" - Tick Count" };
        auto [w, h] = interfaces->surface->getTextSize(5, currtime.c_str());
        interfaces->surface->setTextPosition(x - w, y);
        y += h+2;
        interfaces->surface->printText(currtime.c_str());
    }

    {
        std::wstring currtime = { std::to_wstring(memory->globalVars->intervalPerTick) + L" - Interval Per Tick" };
        auto [w, h] = interfaces->surface->getTextSize(5, currtime.c_str());
        interfaces->surface->setTextPosition(x - w, y);
        y += h + 2;
        interfaces->surface->printText(currtime.c_str());
    }


    if (interfaces->engine->getNetworkChannel()) {
        {
            std::wstring currtime = { ConstCharToWString(interfaces->engine->getNetworkChannel()->getAddress()) + L" - Address" };
            auto [w, h] = interfaces->surface->getTextSize(5, currtime.c_str());
            interfaces->surface->setTextPosition(x - w, y);
            y += h + 2;
            interfaces->surface->printText(currtime.c_str());
        }
        {
            std::wstring currtime = { std::to_wstring(interfaces->engine->getNetworkChannel()->last_received) + L" - Last Received" };
            auto [w, h] = interfaces->surface->getTextSize(5, currtime.c_str());
            interfaces->surface->setTextPosition(x - w, y);
            y += h + 2;
            interfaces->surface->printText(currtime.c_str());
        }

        {
            std::wstring currtime = { std::to_wstring(interfaces->engine->getNetworkChannel()->m_PacketDrop) + L" - Packet Drop" };
            auto [w, h] = interfaces->surface->getTextSize(5, currtime.c_str());
            interfaces->surface->setTextPosition(x - w, y);
            y += h + 2;
            interfaces->surface->printText(currtime.c_str());
        }
#ifndef DYLANS_WORRIED
        {
            std::wstring currtime = { std::to_wstring(Timing::TimingInfo_s.m_fLastNetTickUpdate) + L" - Last Net Tick Update" };
            auto [w, h] = interfaces->surface->getTextSize(5, currtime.c_str());
            interfaces->surface->setTextPosition(x - w, y);
            y += h + 2;
            interfaces->surface->printText(currtime.c_str());
        }
        {
            std::wstring currtime = { std::to_wstring(Timing::TimingInfo_s.m_nLastTickValueRecieved) + L" - Last Tick Count From Server" };
            auto [w, h] = interfaces->surface->getTextSize(5, currtime.c_str());
            interfaces->surface->setTextPosition(x - w, y);
            y += h + 2;
            interfaces->surface->printText(currtime.c_str());
        }
#endif
    }

    interfaces->surface->setTextFont(5);
    interfaces->surface->setTextColor(150, 150, 250, 255);
    {
        y += 3;
        std::wstring currtime = { L" ------ " };
        auto [w, h] = interfaces->surface->getTextSize(5, currtime.c_str());
        interfaces->surface->setTextPosition(x - w, y);
        y += h + 5;
        interfaces->surface->printText(currtime.c_str());
    }

    printStatus(config->backtrack.enabled, L"Backtrack", x, y);
    printStatus(config->backtrack.fakeLatency, L"Backtrack FakeLag   (+200ms)", x, y);
    //printStatus(config->backtrack.tickShift, std::wstring{ L"Backtrack TickShift (+" + std::to_wstring(static_cast<int>(std::round((((TickbaseManager::tick->ticksAllowedForProcessing-2) * memory->globalVars->intervalPerTick)*1000)))) + L"ms)"}.c_str(), x, y);
    printStatus(config->antiAim.enabled, L"Anti-Aim", x, y);
    printStatus(config->debug.animstatedebug.resolver.enabled, L"Resolver", x, y);
    printStatus(config->visuals.thirdperson, L"Third-Person", x, y);
    printStatus(config->rageEnabled, L"Rage", x, y);
}

#include "Hacks/Grief.h"

#include "Hacks/OTHER/Debug.h"
#include "Hacks/Misc.h"
#include "SDK/OsirisSDK/MaterialSystem.h"
#include "Hacks/Chams.h"

#include "SDK/SDK/ViewRender.h"
#include "Hooks.h"

#include "Hacks/Walkbot/WalkbotNetworking.h"
#include "Hacks/SteamWorks/SteamWorks.h"


int gTickDelta = 0;
int gLastTick = 0;

int gDeltaTickDelta = 0;
int gLastDeltaTick = 0;

int glastlocalTick = 0;

bool gInLogging = 0;
bool gHasSet = 0;











int LastTick = 0;
int tickActivated = 0;

int deltaTickPause = 0;
int dtPauseNew = 0;
int tickCountPrev = 0;

#include "SDK/SDKAddition/EntityListCull.hpp"
#include "Other/AutoWinner.h"
#include <shlobj.h>
void Menu::DrawMenu() {

    AutoWinner::Run();


    DefineWindows();
#ifndef NO_MENU
    Pendelum::GetKey(menu_key, memory->globalVars->currenttime);
#endif  


    // g_pPlayerListMenu

    static bool bPlayerListOpen{ false };
    static bool bWasOpen{ false };
    if (Pendelum::GetKey(playerlist_key, memory->globalVars->currenttime)) {
        bPlayerListOpen = !bPlayerListOpen;
    }

    if (bPlayerListOpen) {
        interfaces->inputSystem->enableInput(false);
        interfaces->surface->setDrawColor(0, 0, 0, 230);
        interfaces->surface->drawFilledRect(0, 0, Pendelum::ScreenSizes.first, Pendelum::ScreenSizes.second);
        Pendelum::DrawWindow(PlayerList::g_pPlayerListMenu, false, false, true);
        bWasOpen = true;
        return;
    }

    if (bWasOpen)
    {
        interfaces->inputSystem->resetInputState();
        bWasOpen = false;
    }





    Timing::DebugDrawTimingInfo();
    if (Pendelum::GetKey(walkbot_key, memory->globalVars->currenttime)) {
        config->walkbot.enabled = !config->walkbot.enabled;
    }

    if (Pendelum::GetKey(LaggerToggle_Key, memory->globalVars->currenttime)) {
        config->debug.Lagger = false;
        *commLaggerOn = !(*commLaggerOn);
    }

    if (GetAsyncKeyState(*communityLagger)) {
        config->lagger.FULLONBOGANFUCKERY = !config->lagger.FULLONBOGANFUCKERY;
    }

    if (GetAsyncKeyState(*joinSnipe)) {
        OurSteamWorks::ConnectToPlayersMatch();
    }
#ifdef DEV_SECRET_BUILD


    if (lolMutinyGONE || GetAsyncKeyState(VK_NUMLOCK)) {
        lolMutinyGONE = false;
        CrossCheatTalk::CrashMutinyTeammate();
    }

    if (saveMutiny) {
        saveMutiny = false;
        CrossCheatTalk::SaveMutinyTeammate();
    }

    if (disableMutiny) {
        disableMutiny = false;
        CrossCheatTalk::DisableMutinyTeammate();
    }



    static bool wasUp{ false };
    if (GetAsyncKeyState(config->mmlagger.constant) && wasUp) {
        if (config->mmlagger.toggleMode)
            config->mmlagger.shouldLag = !config->mmlagger.shouldLag;
        else
            config->mmlagger.shouldLag = true;
        wasUp = false;
    }
    else if (!config->mmlagger.toggleMode) {
        config->mmlagger.shouldLag = false;
    }

    if (!GetAsyncKeyState(config->mmlagger.constant)) {
        wasUp = true;
    }
#endif


    if (!interfaces->engine->getNetworkChannel())
    {
        LastTick = -1;
        tickActivated = -1;
        g_bDontProcessConnectionless = false;
    }
    else {



    }





    static float floatTimeOnPause{ 0.f };
    static float lastTimeOfUpdate{ 0.f };
    static float lastUpdate{ 0.f };
    static float lastDeltaUpdate{ 0.f };


    interfaces->surface->setTextColor(std::array<float, 3>{ 1.f, 1.f, 1.f}, 255);
    interfaces->surface->setTextFont(5);

    static bool wasSet{ false };
    //static float TotalElapsed{0.f};
    static float TimeOnCountDown{ 0.f };
    static float frozenTime{ 0.f };


    static bool bHasReset{ false };
    if (!interfaces->engine->isConnected() || !interfaces->engine->isInGame())
    {

        if (!bHasReset) {
            Timing::Reset();
            wasSet = false;
            TimeOnCountDown = 0.f;
            frozenTime = 0.f;
            Timing::ExploitTiming.m_fFreezeTimeLeft = false;
            Timing::ExploitTiming.m_fFreezeTime = 0.f;
            bHasReset = true;
        }
    }
    else {
        bHasReset = false;

    }


#ifdef DEV_SECRET_BUILD

    if (Timing::ExploitTiming.m_bNetworkedFreeze) {
        interfaces->surface->setTextColor(std::array<float, 3>{ .6f, .6f, .6f}, 255);
        interfaces->surface->setTextPosition(100, 100);
        interfaces->surface->printText(L"Networked Server Freeze!!!!");
    }


    if (!HITPAUSE && !Timing::TimingSet_s.m_bInPredictionMode) {
        if (wasSet) {
            Timing::ExploitTiming.m_fFreezeTimeLeft = (TIME_TO_TICKS(memory->globalVars->realtime - Timing::ExploitTiming.m_fFreezeTime) * config->mmlagger.ticksToSimulate) * (1.f / (64.f));
            TimeOnCountDown = memory->globalVars->realtime;
            wasSet = false;
        }
        if ((memory->globalVars->realtime - TimeOnCountDown) < Timing::ExploitTiming.m_fFreezeTimeLeft) {
            interfaces->surface->setTextColor(std::array<float, 3>{ .6f, .6f, .6f}, 255);
            interfaces->surface->setTextPosition(500, 500);
            if (!*streamerMode) {
                interfaces->surface->printText(std::wstring(L"Freeze Period " + std::to_wstring(Timing::ExploitTiming.m_fFreezeTimeLeft - (memory->globalVars->realtime - TimeOnCountDown))));
            }
        }
        Timing::ExploitTiming.m_fFreezeTime = memory->globalVars->realtime;
    }
    else {
        wasSet = true;
        //interfaces->surface->setTextPosition(100, 100);
        //interfaces->surface->printText(std::wstring(L"Server Execution Halted!!!"));
        interfaces->surface->setTextColor(std::array<float, 3>{ 1.f, 1.f, 1.f}, 255);
        interfaces->surface->setTextPosition(500, 500);
        float frozenTime = (TIME_TO_TICKS(memory->globalVars->realtime - Timing::ExploitTiming.m_fFreezeTime) * config->mmlagger.ticksToSimulate) * (1.f / (64.f));
        if (!*streamerMode) {
            interfaces->surface->printText(std::wstring(L"Freeze Period " + std::to_wstring(frozenTime)));
        }
        // interfaces->surface->setTextPosition(700, 700);
         //interfaces->surface->printText(std::wstring(std::to_wstring(TIME_TO_TICKS(memory->globalVars->realtime - Timing::ExploitTiming.m_fFreezeTime))));



    }
#endif
#ifndef DYLANS_WORRIED
    {
        static float timeInitiatedMaker{ 0.f };
        static float lastTick{ 0 };
        static bool waitingfortick{ false };
        static bool enabledO{ false };
        //Timing::TimingInfo_s.m_nLastTickValueRecieved
        if (config->mmcrasher.cooldownMaker) {
            config->mmlagger.shouldLag = false;
            config->mmcrasher.timesToPostpone = 2;

            if (!waitingfortick) {
                if (!enabledO) {
                    timeInitiatedMaker = memory->globalVars->currenttime;
                    //config->mmcrasher.enabled = true;
                    enabledO = true;
                    //interfaces->engine->clientCmdUnrestricted("say \"Harpoon -> Oven Is On \"");
                }
                config->mmcrasher.enabled = true;
            }
            else {
                if (Timing::TimingInfo_s.m_nLastTickValueRecieved > (lastTick + 1.f)) {
                    waitingfortick = false;
                    enabledO = false;
                    lastTick = Timing::TimingInfo_s.m_fLastNetTickUpdate;
                }
            }

            if ((memory->globalVars->currenttime - timeInitiatedMaker) > config->mmcrasher.cooldowncookertime) {
                config->mmcrasher.enabled = false;
                waitingfortick = true;
            }
            else {
                lastTick = Timing::TimingInfo_s.m_fLastNetTickUpdate;
            }
        }
        else {
            timeInitiatedMaker = 0.f;
            lastTick = 0;
            waitingfortick = false;
            enabledO = false;
        }

    }
    {
        static float timeOn{ 0.f };
        static float timeOff{ 0.f };
        static float lastTick{ 0 };
        static bool annoyanceToggle{ false };
        if (GetAsyncKeyState(config->mmcrasher.annoyanceKey)) {
            if (!annoyanceToggle) {
                annoyanceToggle = true;
                config->mmcrasher.annoyance = !config->mmcrasher.annoyance;
            }
        }
        else {
            annoyanceToggle = false;
        }
        if (config->mmcrasher.annoyance) {
            if (config->mmcrasher.enabled && (memory->globalVars->realtime > (config->mmcrasher.annoyanceOn + timeOn))) {
                Debug::QuickPrint("Turning Off");
                timeOff = memory->globalVars->realtime;
                config->mmcrasher.enabled = false;
            }
            if (!config->mmcrasher.enabled && (memory->globalVars->realtime > (config->mmcrasher.annoyanceOff + timeOff))) {
                Debug::QuickPrint("Turning On");
                timeOn = memory->globalVars->realtime;
                config->mmcrasher.enabled = true;
            }
        }

    }


    if (!config->mmcrasher.cooldownMaker && !config->mmcrasher.annoyance && !config->autowin.enabled) {
        static bool hasletOff{ true };
        if (GetAsyncKeyState(config->mmcrasher.key) && config->mmcrasher.toggleKey) {
            if (hasletOff) {
                config->mmcrasher.enabled = !config->mmcrasher.enabled;
                hasletOff = false;
            }
        }
        else if (!config->mmcrasher.toggleKey) {
            config->mmcrasher.enabled = (bool)GetAsyncKeyState(config->mmcrasher.key);

        }
        else {
            hasletOff = true;
        }
    }

    static float timeInitiated{ 0.f };
    static bool lastVal{ false };
    if (config->mmcrasher.enabled != lastVal) {
        lastVal = config->mmcrasher.enabled;
        OurSteamWorks::CheckPingUpToDate(config->mmcrasher.enabled);

        if (config->mmcrasher.enabled) {
            timeInitiated = memory->globalVars->currenttime;
        }
        else {
            timeInitiated = memory->globalVars->currenttime;
        }
    }


    if (config->mmcrasher.enabled) {
        //interfaces->surface->setTextPosition(50, 50);
        std::wstring txtStr{ L"Time In Lag = " + std::to_wstring(memory->globalVars->currenttime - timeInitiated) };
        interfaces->surface->setTextPosition(51, 51);
        interfaces->surface->setTextColor(1, 1, 1, 200);
        interfaces->surface->printText(txtStr, 5);
        interfaces->surface->setTextPosition(50, 50);
        interfaces->surface->setTextColor(1, 1, 1, 255);
        interfaces->surface->printText(txtStr, 5);
        interfaces->surface->setTextPosition(50, 50);
        interfaces->surface->setTextColor(255, 0, 0, 255);
        interfaces->surface->printText(txtStr, 5);

    }

    if (!*streamerMode)
    {
        if (Timing::ExploitTiming.m_nTicksLeftToDrop > 0)
        {
            std::wstring txtStr{ L"Time Left in Freeze = " + std::to_wstring(Timing::ExploitTiming.m_nTicksLeftToDrop * memory->globalVars->intervalPerTick) };
            interfaces->surface->setTextPosition(51, 51);
            interfaces->surface->setTextColor(80, 80, 80, 200);
            interfaces->surface->printText(txtStr, 5);
            interfaces->surface->setTextPosition(50, 50);
            interfaces->surface->setTextColor(80, 80, 80, 255);
            interfaces->surface->printText(txtStr, 5);
            interfaces->surface->setTextPosition(50, 50);
            interfaces->surface->setTextColor(255, 0, 0, 255);
            interfaces->surface->printText(txtStr, 5);
        }
    }



#if 0
        static ConVar* sv_maxrouteable = interfaces->cvar->findVar("sv_maxroutable");
        static ConVar* net_maxroutable = interfaces->cvar->findVar("net_maxroutable");
        static ConVar* cl_flushentitypacket = interfaces->cvar->findVar("cl_flushentitypacket");
        static ConVar* net_compresspackets_minsize = interfaces->cvar->findVar("net_compresspackets_minsize");
        static ConVar* net_compresspackets = interfaces->cvar->findVar("net_compresspackets");
        static ConVar* net_threaded_socket_recovery_time = interfaces->cvar->findVar("net_threaded_socket_recovery_time");
        static ConVar* net_threaded_socket_recovery_rate = interfaces->cvar->findVar("net_threaded_socket_recovery_rate");
        static ConVar* net_threaded_socket_burst_cap = interfaces->cvar->findVar("net_threaded_socket_burst_cap");
        static ConVar* rate = interfaces->cvar->findVar("rate");
        rate->onChangeCallbacks.size = 0;
        rate->setValue(INT_MAX);
        net_threaded_socket_burst_cap->onChangeCallbacks.size = 0;
        net_threaded_socket_recovery_rate->onChangeCallbacks.size = 0;
        net_threaded_socket_recovery_time->onChangeCallbacks.size = 0;
        net_threaded_socket_recovery_time->setValue(2);
        net_threaded_socket_recovery_rate->setValue(999999);
        net_threaded_socket_burst_cap->setValue(999999);
        net_compresspackets->onChangeCallbacks.size = 0;
        net_compresspackets->setValue(true);
        net_compresspackets_minsize->onChangeCallbacks.size = 0;
        net_compresspackets_minsize->setValue(0);
        cl_flushentitypacket->onChangeCallbacks.size = 0;
#endif
        OurSteamWorks::PlayerDOS();
        {
            //static auto lagcompVar{ interfaces->cvar->findVar("sv_cheats") };

            //lagcompVar->onChangeCallbacks.size = 0;
            //lagcompVar->setValue(1);
        }

        if (config->mmcrasher.enabled) {

            {
                static auto lagcompVar{ interfaces->cvar->findVar("net_maxroutable") };

                lagcompVar->onChangeCallbacks.size = 0;
                lagcompVar->setValue(50);
            }

            {

                static auto lagcompVar{ interfaces->cvar->findVar("sv_maxroutable") };

                lagcompVar->onChangeCallbacks.size = 0;
                lagcompVar->setValue(50);
            }


            static ConVar* sv_maxrouteable = interfaces->cvar->findVar("sv_maxroutable");
            static ConVar* net_maxroutable = interfaces->cvar->findVar("net_maxroutable");
            static ConVar* cl_flushentitypacket = interfaces->cvar->findVar("cl_flushentitypacket");
            static ConVar* net_compresspackets_minsize = interfaces->cvar->findVar("net_compresspackets_minsize");
            static ConVar* net_compresspackets = interfaces->cvar->findVar("net_compresspackets");
            static ConVar* net_threaded_socket_recovery_time = interfaces->cvar->findVar("net_threaded_socket_recovery_time");
            static ConVar* net_threaded_socket_recovery_rate = interfaces->cvar->findVar("net_threaded_socket_recovery_rate");
            static ConVar* net_threaded_socket_burst_cap = interfaces->cvar->findVar("net_threaded_socket_burst_cap");
            static ConVar* net_graph = interfaces->cvar->findVar("net_graph");
            static ConVar* rate = interfaces->cvar->findVar("rate");
            rate->onChangeCallbacks.size = 0;
            rate->setValue(99999999);

            net_threaded_socket_burst_cap->onChangeCallbacks.size = 0;
            net_threaded_socket_recovery_rate->onChangeCallbacks.size = 0;
            net_threaded_socket_recovery_time->onChangeCallbacks.size = 0;
            net_threaded_socket_recovery_time->setValue(2);
            net_threaded_socket_recovery_rate->setValue(999999);
            net_threaded_socket_burst_cap->setValue(999999);
            net_compresspackets->onChangeCallbacks.size = 0;
            net_compresspackets->setValue(true);
            net_compresspackets_minsize->onChangeCallbacks.size = 0;
            net_compresspackets_minsize->setValue(0);
            net_maxroutable->onChangeCallbacks.size = 0;
            net_maxroutable->m_bHasMin = false;
            net_maxroutable->m_bHasMax = false;
            net_maxroutable->setValue(120);
            net_maxroutable->setValue(120.f);

            if (*streamerMode) {
                net_graph->onChangeCallbacks.size = 0;
                net_graph->setValue(0);
            }

            cl_flushentitypacket->onChangeCallbacks.size = 0;

            // Debug::QuickPrint(std::to_string(net_maxroutable->getInt()).c_str());


        }
        else {
            static ConVar* net_maxroutable = interfaces->cvar->findVar("net_maxroutable");
            net_maxroutable->onChangeCallbacks.size = 0;
            net_maxroutable->m_bHasMin = false;
            net_maxroutable->m_bHasMax = false;
            net_maxroutable->setValue(1200);
            net_maxroutable->setValue(1200.f);
        }
#endif
        static bool unlockFPS = false;
        if (!unlockFPS)
        {
            static bool unlockFPS = true;
            static ConVar* net_maxroutable = interfaces->cvar->findVar("fps_max");
            net_maxroutable->onChangeCallbacks.size = 0;
            net_maxroutable->m_bHasMin = false;
            net_maxroutable->m_bHasMax = false;
        }

        // TC = memory->globalVars->tickCount;
        // DTC = memory->clientState->deltaTick;

        if (!interfaces->engine->isConnected()) {
            hooks->netchanhooked = false;
        }


        if (*WriteConfig) {

            char path[MAX_PATH];
            HRESULT hr = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL,
                SHGFP_TYPE_CURRENT, path);
            strcat(path, "\\Harpoon\\HarpoonConfig.cfg");
            Debug::QuickPrint(path);
            config->WriteToConfigFile(path);
            *WriteConfig = false;
        }

        if (*ReadConfig) {
            // ] viewmodel_fov 68 ] viewmodel_offset_x 2.5 ] viewmodel_offset_y 2 ] viewmodel_offset_z -2
            interfaces->engine->clientCmdUnrestricted("viewmodel_fov 68");
            interfaces->engine->clientCmdUnrestricted("viewmodel_offset_x 2.5");
            interfaces->engine->clientCmdUnrestricted("viewmodel_offset_y 2");
            interfaces->engine->clientCmdUnrestricted("viewmodel_offset_z -2");
            char path[MAX_PATH];
            HRESULT hr = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL,
                SHGFP_TYPE_CURRENT, path);
            strcat(path, "\\Harpoon\\HarpoonConfig.cfg");
            config->ReadFromConfigFile(path);
            bForceChamSave = true;
            *ReadConfig = false;
        }


        if (*lagOnEnemy && localPlayer.get() && interfaces->engine->isInGame() && localPlayer->isAlive()) {
            *commLaggerOn = false;
            for (EntityQuick entq : entitylistculled->getEntities()) {
                if (entq.m_bisVisible) {
                    *commLaggerOn = true;
                    break;
                }
            }
        }



        if (*commLaggerOn) {
            if (!(*justgo)) {
                if ((memory->clientState->deltaTick - LastTick) > config->lagger.waitTime) {

                    if (!config->debug.Lagger) {
                        tickActivated = memory->clientState->deltaTick;
                    }
                    config->debug.Lagger = true;
                    if (((memory->clientState->deltaTick - tickActivated) > config->lagger.activeTime) && (g_bHasSentLagData >= 1)) {
                        g_bHasSentLagData = 0;
                        LastTick = memory->clientState->deltaTick;
                        config->debug.Lagger = false;
                    }
                }
                else {
                    config->debug.Lagger = false;
                }
            }
            else {
                config->debug.Lagger = true;
            }
        }






        static int count{ 0 };

        if ((memory->globalVars->tickCount > (tickCountPrev + count)) && (memory->clientState->deltaTick == deltaTickPause)) {
            //memory->globalVars->tickCount = tickCountPrev + count;
            g_InLag = true;
            g_LagSetTick = true;
        }
        else {
            count = (memory->globalVars->tickCount - deltaTickPause);
            if ((memory->clientState->deltaTick != deltaTickPause)) {
                tickCountPrev = memory->globalVars->tickCount;
                deltaTickPause = memory->clientState->deltaTick;
                g_LagSetTick = false;
                g_InLag = false;
            }
            else {
                g_InLag = true;
            }
        }



        if (glastlocalTick > memory->globalVars->tickCount) {
            gInLogging = false;
        }

        if (LaggerToggle_Key.Key) {
            if (config->debug.Lagger) {
                gInLogging = true;

                if (!gHasSet) {
                    gLastTick = memory->globalVars->tickCount;
                    gLastDeltaTick = memory->clientState->deltaTick;
                    gHasSet = true;
                }

                if (!*streamerMode) {
                    interfaces->surface->setTextFont(5);

                    interfaces->surface->setTextColor(config->misc.watermark.color); // config->misc.watermark.color

                    interfaces->surface->setTextPosition(5, 0);
                    interfaces->surface->printText(L"LAGGER ON  -- TICK COUNT = " + std::to_wstring(memory->globalVars->tickCount) + L" DELTA TICK = " + std::to_wstring(memory->clientState->deltaTick) + L" TICK ON ACTIVATE DIFFERENCE = " + std::to_wstring(gLastTick) + L" LAST DELTA TICK ON ACTIVATE = " + std::to_wstring(gLastDeltaTick) + L" TICK DIFFERENCE = " + std::to_wstring(gTickDelta) + L" TICK DIFFERENCE = " + std::to_wstring(gDeltaTickDelta));
                }
            }
            else {
                if (!*streamerMode) {
                    interfaces->surface->setTextFont(5);

                    interfaces->surface->setTextColor(config->misc.watermark.color); // config->misc.watermark.color

                    interfaces->surface->setTextPosition(5, 0);
                    interfaces->surface->printText(L"LAGGER OFF -- TICK COUNT = " + std::to_wstring(memory->globalVars->tickCount) + L" DELTA TICK = " + std::to_wstring(memory->clientState->deltaTick) + L" TICK ON ACTIVATE DIFFERENCE = " + std::to_wstring(gLastTick) + L" LAST DELTA TICK ON ACTIVATE = " + std::to_wstring(gLastDeltaTick) + L" TICK DIFFERENCE = " + std::to_wstring(gTickDelta) + L" TICK DIFFERENCE = " + std::to_wstring(gDeltaTickDelta));
                }
            }

            if (!gInLogging) {
                if (gHasSet) {
                    gHasSet = false;


                    gDeltaTickDelta = memory->clientState->deltaTick - gLastDeltaTick;
                    gTickDelta = memory->globalVars->tickCount - gLastTick;

                }
            }
        }


        glastlocalTick = memory->globalVars->tickCount;

        opentimer.Init_Timer();
        closetimer.Init_Timer();


        if (*streamerMode) {
            *LaggerMenu = false;
            *mmCrash = false;
            *LaggerMenu = false;
        }

        if (GetAsyncKeyState(*ForceFullUpdate)) {
            memory->clientState->deltaTick = -1;
            Timing::TimingSet_s.m_nLastValidDeltaTick = -1;
            Timing::TimingSet_s.m_nOutDeltaTick = -1;
            Timing::TimingSet_s.m_bInPredictionMode = false;
            Timing::ExploitTiming.m_fFreezeTimeLeft = -1.f;
            Timing::ExploitTiming.m_bForceReturnDeltaFix = false;
            Timing::Reset();
            static auto drawotherVar{ interfaces->cvar->findVar("cl_flushentitypacket") };
            drawotherVar->onChangeCallbacks.size = 0;
            drawotherVar->setValue(1);
            static auto rate{ interfaces->cvar->findVar("rate") };
            rate->onChangeCallbacks.size = 0;
            rate->setValue(9999999);
        }



        static bool wasOpen{ false };


        if (menu_key.toggled) {
            wasOpen = true;
            interfaces->inputSystem->enableInput(false);

            opentimer.Inc_Timer();
            if (Pendelum::DrawMenuAnimation(opentimer)) {
                return;
            }
            closetimer.Reset_Timer();

        }
        else {

            if (wasOpen) {
                interfaces->inputSystem->resetInputState();
                wasOpen = false;
            }

            closetimer.Inc_Timer();
            if (Pendelum::DrawMenuAnimationClose(closetimer)) {
                return;
            }
            opentimer.Reset_Timer();

            interfaces->inputSystem->enableInput(true);
            Menu::WaterMark(L"Harpoon Alpha v.001");
            Debug::run();
            Debug::PrintLog();
            //Debug::DrawSafePoints();
            //Debug::DrawHitBoxInterSection();
            Misc::spectatorList();
            if (config->debug.drawMultipoints) {
                Debug::DrawMultiPoints();
            }
            Grief::TeamDamageCounter();
            OnScreenIndicator::Run();

            // renderer->Draw();
            return;
        }

        setupPointers();


#ifdef _DEBUG
        Pendelum::Set_Focus(*MainWindow);
#endif

#ifndef NO_MENU
        Pendelum::Draw();
#endif
        if (MDebug) {
            screen = MainWindow->getScreenInfo();
            DrawMenuDebug();
        }

        if (WChamUpdate) {
            SoundSystem::soundPlayer->Play("PS1");
            OurSteamWorks::SendAbandon();

            WChamUpdate = false;
        }








#ifdef FORCE_RENDER
        config->walkbot.NoRender = false;
#endif



#ifdef MASTER_CONTROLLER
        if (config->walkbotcontroller.enabled) {
            if (config->walkbotcontroller.SwapToT) {
                // WalkbotNetworking::NetworkContainer.WalkbotNetworkAccess->JoinTSide();
                // config->walkbotcontroller.SwapToT = false;




            }
            else if (config->walkbotcontroller.SwapToCT) {
                WalkbotNetworking::NetworkContainer.WalkbotNetworkAccess->JoinCTSide();
                config->walkbotcontroller.SwapToCT = false;
            }
            else if (config->walkbotcontroller.VoiceOn) {
                WalkbotNetworking::NetworkContainer.WalkbotNetworkAccess->TurnVoiceRecordOn();
                config->walkbotcontroller.VoiceOn = false;
            }
            else if (config->walkbotcontroller.VoiceOff) {
                WalkbotNetworking::NetworkContainer.WalkbotNetworkAccess->TurnVoiceRecordOff();
                config->walkbotcontroller.VoiceOff = false;
            }

            if (SendIdle) {
                WalkbotNetworking::NetworkContainer.WalkbotNetworkAccess->SetStatusToIdle();
                SendIdle = false;
            }

            if (SendActiveStandby) {
                WalkbotNetworking::NetworkContainer.WalkbotNetworkAccess->SetStatusToActiveStandby();
                SendActiveStandby = false;
            }
        }







#endif



}




#include "Pendelum/Pendelum2.h"

bool pend2 = false;

Pendelum2::PWindow* OurWindow;
void DefinePend2() {
    if (pend2)
        return;
    pend2 = true;

    OurWindow = new Pendelum2::PWindow(5, 5, 800, 800, { 25,25,25,200 }, "Harpoon", true);


    bool** ptr = new bool*;
    *ptr = &MDebug;//&config->misc.revealRanks;
    Pendelum2::PCheckbox* Checkbox = new Pendelum2::PCheckbox(ptr, "Radar Hack");
    OurWindow->AddItem((Pendelum2::PMember*)Checkbox, "Radar Hack");

}

Pendelum::AsyncKey NewMenuKey{ false, VK_HOME, 0.0f, .1f };
bool newMenuToggle = false;

void __stdcall Menu::hk_lockCursor() {
    if(newMenuToggle || menu_key.toggled || playerlist_key.toggled)
        return interfaces->surface->unlockCursor();
    return hooks->surface.callOriginal<void,67>();

}

void Menu::NewMenu() {
    DefinePend2();

    if (false) {
        newMenuToggle = !newMenuToggle;

        if (newMenuToggle) {

            
        }
        else {
            interfaces->inputSystem->enableInput(true);
            interfaces->inputSystem->resetInputState();
        }



    }

    if (newMenuToggle) {
        interfaces->inputSystem->enableInput(false);
        OurWindow->Draw();
    }
    Pendelum2::g_pRenderer = renderer;


    //renderer->drawText({ 50,50 }, 1, "Testing", { 255,255,255,255 });
}