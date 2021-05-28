#pragma once

#include <array>
#include <filesystem>
#include <memory>
#include <string>
#include <map>
#include <unordered_map>

//#include "imgui/imgui.h"
//#include "nSkinz/config_.hpp"
#include "ConfigStructs.h"
#include "COMPILERDEFINITIONS.h"
//#include "Multipoints.h"

class Config {
public:
    /*
    explicit Config(const char*) noexcept;
    void load(size_t) noexcept;
    void save(size_t) const noexcept;
    void add(const char*) noexcept;
    void remove(size_t) noexcept;
    void rename(size_t, const char*) noexcept;
    void reset() noexcept;
    void listConfigs() noexcept;
    */
    constexpr auto& getConfigs() noexcept
    {
        return configs;
    }

    void ReadFromConfigFile(const char* szFile);
    void WriteToConfigFile(const char* szFile);
    void RunConfig(bool Mode, const char* szFile);

    struct Color {
        std::array<float, 3> color{ 1.0f, 1.0f, 1.0f};
        float a_for_overload_dont_set = 0.0f;
        char buff[5];
        std::array<float, 3> colorset2{ 1.0f, 1.0f, 1.0f };

        bool rainbow{ false };
        float rainbowSpeed{ 0.6f };
    };

    struct ColorToggle : public Color {
        bool enabled{ false };
    };


    struct Exploits {
        bool bAllowUntrusted{ false };
        struct TickBase {
            struct DoubleTap {
                bool bEnabled{ false };
                bool bHideShots{ false };
                bool bClMoveDT{ true };
                bool bHostRunFrameInputDT{ false };
                bool bWriteUserCmdtoDeltaDT{ false };
                int nCustomMaxUserProcess{ 0 };
                int nTicksToShift{ 8 };
                bool bInstaRecharge{ false };
            }m_DoubleTap;
        } m_TickBase;
    } m_Exploits;



    struct Cham_s {
        struct Material : ColorA {
            bool bEnabled{ false };
            bool bRenderInSceneEnd{ false };
            bool bCallOriginalInSceneEnd{ false };
            std::string MaterialName = "Normal";
            struct Options {
                bool bEnabled = false;
                bool bHealthBased = false;
                bool bBlinking = false;
                bool bWireframe = false;
                bool bCover = false;
                bool bIgnorez = false;
                int nMaterial = 0;
                struct {
                    bool enabled = false;
                    float rate = .1f;
                    float a = 1.0f;
                } pulse;
            } Opts;
        };
        bool bEnabled{ false };
        bool bCallOriginal{ false };
        Material Materials[10];
    };
    std::map <std::string, Cham_s> m_mapChams;


    struct NEWESP {
        struct PlayerESP {
            bool bEnabled{ false };
            uint32_t m_nSteamID{ 0 }; // For Checking to see if theres a new player in that slot
            ColorToggle ctWeapon;
            Cham_s Cham; // Unused For EnemyESP and TeammateESP
        } m_arrPlayerESP[64]; /* For Custom Player ESP Shit */
    } m_ESP;


    struct grief_ {
        bool teamDamageOverlay{ false };

        struct ChatSpam {
            bool enabled{ false };
            bool position{ false };
            bool damage{ false };
            bool buys{ false };
        } spam;

        bool triggername{ false };
        int grenadeAIM{ 0 };
        int BlockBot{ 0 };

    } grief;

    struct walkbots {
        bool enabled{ false };
        bool DrawPath{ false };
        int lookSmoothing{ 1 };
        float AimFov{ 80.f };
        float AimSmoothing{ 10.f };
        float hitchance{ 40.f };
        bool variableSmooth{ false };
        bool ignoreHead{ false };
        bool boundingbox{ false };
        bool smoothPath{ false };
        bool pathonly{ false };
        bool bunnyhop{ false };
#ifndef WALKBOT_BUILD
        bool NoRender{ false };
#endif
#ifdef WALKBOT_BUILD
        bool NoRender{ false };
#endif

        float walkSmoothing{ 1.f };
        bool talk{ false };
    } walkbot;

#ifdef MASTER_CONTROLLER
    struct wcontrol {
        bool enabled{ true };
        bool SwapToT{ false };
        bool SwapToCT{ false };
        bool VoiceOn{ false };
        bool VoiceOff{ false };
        int SetPosMode{ 0 };
        int SetPos{ 0 };
    } walkbotcontroller;
#endif

    struct debug {
        ColorToggle desync_info;
        ColorToggle animStateMon;
        ColorToggle box;
        ColorToggle CustomHUD;
        ColorToggle DamageLog;
        ColorToggle networkchannel;
        ColorToggle clientstate;
        ColorToggle globalvars;
        ColorToggle ResolverOut;
        
        //out

#ifndef _DEBUG
        bool LocalTiming{ false };
#else
        bool LocalTiming{ true };
#endif

        bool dumpVoiceDataFormat{ false };

        bool dumpNetInAndOut{ false };

        bool AnimExtras{ false };
        bool backtrackCount{ false };
        bool resolverDebug{ true };
        bool bNoRender{ false };
        int overlay{ 13 };
        int entityid{ 0 };
        bool overlayall{ false };
        bool weight{ false };
        bool showall{ false };
        bool fourfifty{ false };
        bool forcesetupBones{ false };
        bool showshots{ false };
        bool aimbotcoutdebug{ false };
        bool tickbasedebug{ false };
        bool instareconnect{ false };
        bool airstucktoggle{ false };
        bool airstuckprovmove{ false };
        bool procmovereturn{ false };
        bool testLag{ false };
        bool spectateForce{ false };
        bool voiceBufferFuck{ false };
        bool InfinitePred{ false };
        int weird{ 150 };
        bool AAALWAYS{ false };
        bool parseALL{ false };
        int netSub{ 10 };
        bool in{ false };
        bool out{ false };
        bool outack{ false };
        bool Count{ true };
        bool number{ true };
        bool writedeltastuck{ false };
        bool HardMode{ false };
        bool Vest{ false };
        bool printNetMSG{ false };
        bool Lagger{ false };
        bool returnonCLCMOVE{ false };
        bool TimingStats{ false };
        bool DontRecieveVoice{ false };
        bool dumpVoiceData{ false };

        bool DeathMatchInvincibility{ false };
        //in
        struct {
            bool enabled{ false };
            ColorToggle color;
            int entityid{ 0 };
            bool newesttick{ false };
            bool findactive{ false };
        } backtrack;




        struct {

            bool enabled{ true };
            struct {
#ifndef WALKBOT_BUILD
                bool enabled{ true };
#endif
#ifdef WALKBOT_BUILD
                bool enabled{ false };
#endif

                bool basicResolver{ false };
                bool overRide{ false };
                int missed_shots{ 0 };
                int missedoffset{ 0 };
                bool goforkill{ false };
                bool missedshotsreset{ false };
            }resolver;

            bool manual{ false };
        } animstatedebug;

        struct {
            bool enabled;
            bool baim;
            bool resolver;
            bool choke;
        } indicators;


        bool ResolverRecords{ false };
        bool TargetOnly{ false };
        bool TraceLimit{ false };
        bool AnimModifier{ false };
        float GoalFeetYaw{ 0.0f };
        float Pitch{ 0.0f };
        float Yaw{ 0.0f };

        struct Graphs { /* Make Custom Color Class */
            bool enabled{ false };
            bool FPSBar{ true };
            bool Ping{ true };
            bool FakeLag{ false };
        } graph;
        bool spoofconvar{ false };
        bool showlagcomp{ false };
        bool showimpacts{ false };
        bool drawother{ false };
        bool grenadeTraject{ false };
        bool tracer{ false };
        bool bullethits{ false };
        bool engineprediction{ true };
        bool movefix{ false };
        bool dontsenddisconnect{ false };
        bool EnemySend{ false };
        bool HSpam{ false };
        bool bRenderChamsOverViewModel{ true };
        bool bPrintRank{ true };
        bool allowcalloriginal{ false };
        bool bSendMatrix{ false };
#ifndef WALKBOT_BUILD
        bool Animfix{ true };
#endif
#ifdef WALKBOT_BUILD
        bool Animfix{ false };
#endif
        bool AutoQueue{ false };
        bool veloFix{ false };
        bool forceSendOnShot{ false };
        float ABS{ 0.0f };
        bool drawMultipoints{ false };
        bool drawOrgMultipoints{ false };
        bool drawDynamicPoints{ false };
        bool drawHitPoints{ false };

        bool alwaystrueforhltv{ false };
        bool newRage{ true };

        bool LPAnimFix{ true };
        bool SteamNetworkingDebug{ false };
#ifdef DEV_SECRET_BUILD
        bool PrintConnectionless{ false };
#endif
        bool TransmitMatrix{ false };
    } debug;

    struct SharksCrasher {
        int key{ 0 };
    } sharkcrasher;

    struct GameWinner {
        bool enabled{ false };
    } autowin;

    struct regions {
#ifdef _ALLOW_ALL_REGIONS
        regions() {
            enabled = true;
            for (int i = 0; i < 55; i++) {
                if (i > 34 && i < 43) {
                    Regions[i] = true;
                }
                else {
                    Regions[i] = false; 
                }
            }
        }
#endif
        bool enabled{ false };
        bool Regions[55] = { 0 };
    } RegionChanger;

    struct Aimbot {
        bool enabled{ false };
        bool onKey{ false };
        int key{ 0 };
        int keyMode{ 0 };
        bool aimlock{ false };
        bool silent{ false };
        bool friendlyFire{ false };
        bool visibleOnly{ true };
        bool scopedOnly{ true };
        bool ignoreFlash{ false };
        bool ignoreSmoke{ false };
        bool autoShot{ false };
        bool m_bForcePeak{ false };
        bool autoScope{ false };
        float fov{ 0.0f };
        float smooth{ 1.0f };
        int bone{ 0 };
        float maxAimInaccuracy{ 1.0f };
        float maxShotInaccuracy{ 1.0f };
        int minDamage{ 1 };
        float minDamageVis{ 1.0f };
        bool killshot{ false };
        bool betweenShots{ true };
        bool multipointenabled{ false };
        float multidistance{ 0.1f };
        float extrapointdist{ 1.0f };
        float hitChance{ 0.0f };
        float maxTriggerInaccuracy{ 0.0f };
        bool autoStop{ 0 };
        bool oldstyle{ true };
        bool optimize{ true };
        bool baim{ false };
        int baimkey{ 0 };
        int pointstoScan{ 200 };
        bool ensureHC{ false };
        bool considerRecoilInHC{ false };
        bool multiincrease{ false };
        bool dynamicpoints{ false };
        bool veloPointSort{ false };
        bool safepoint{ false };
        bool ParseHitboxOnly{ false };
        bool Async{ false };
        bool shootonhconly{ false };
        bool ClipRay{ false };
        bool autowall{ false };
        bool hitboxes[18] = { // Multipoints::HITBOX_LAST_ENTRY
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
        };
        bool OnshotOrDesyncless{ false };
        int baimshots{ 8 };
        bool onshot{ false };
        bool pelvisAimOnLBYUpdate{ false };
        bool ShootForLBY{ false };
        bool prioritizeEventBT{ false };
        bool PrioritizeHeadOnNoDesync{false};
        bool respectHitbox{ false };
        
    };
    std::array<Aimbot, 40> aimbot;
    std::array<Aimbot, 40> RageBot;
    bool rageEnabled{ false };


    struct Triggerbot {
        bool enabled = false;
        bool friendlyFire = false;
        bool scopedOnly = true;
        bool ignoreFlash = false;
        bool ignoreSmoke = false;
        bool killshot = false;
        bool onKey = false;
        int key = 0;
        int hitgroup = 0;
        int shotDelay = 0;
        int minDamage = 1;
        float burstTime = 0.0f;
    };
    std::array<Triggerbot, 40> triggerbot;


    struct AimAssist {
        bool enabled = false;
        bool onlyAfterHit{ false };
        float Smooth = 1.f;
        float FOV = 1.f;
        float lockFOV = 5.f;
        float Penalty = 1.f;
        float AwayPenalty = 1.f;
        float maxSpeed = 1.f;
    } aimassist_single;
    std::array<AimAssist, 40> AimAssist;

    struct AntiAim {
        bool ignoreSmoke{ false };
        bool enabled{ false };
        bool pitch{ false };
        bool yaw{ false };
        float DeSyncManualSet{ 0.0f };
        float DeSyncMultiplier{ 0.0f };
        int DeSyncMultiplierVariance{ 0 };
        bool bJitter{ false };
        int JitterRange{ 1 };
        int JitterChance{ 100 };
        bool currAng{ false };
        float pitchAngle{ 0.0f };
        bool Spin{ false };
        int state{ 1 };
        bool KeyYaw{ false };
        int BackwardYKey{ false };
        int RightYKey{ false };
        int LeftYKey{ false };
        int ForwardYKey{ false };
        int currYaw{ 0 };
        float manYaw{ 0.0f };
        float clamped{ 0.0f };
        bool currViewBypass{ false };
        bool swapPacket{ false };
        bool swapsides{ false };
        int swapsidekey{ 0 };
        bool forceMovefix{ false };
        bool forceMovefixPost{ false };
        bool forceMovefixSend{ false };
        bool forceMovefixNoSend{ false };
        bool secretdebug{ false };
        bool toWall{ false };
        float LBYValue{ 0 };
        bool forcesendafterLBY{ false };
        bool useAnimState{ false };
        bool airstuckonLBY{ false };
        bool disableLBYbreaking{ false };

        struct test {
            bool forceHide = false;
            bool preserveCountOnLBY = false;
            struct cout {
                int tickCount = 0;
                int commandNumber = 0;
            } cmd;
        } test;

        bool blah{ true };
        bool suppress979{ false };
        bool preBreak{ false };
        int v1{ 120 };
        int v2{ 0 };
        int v3{ -1 };
        int v4{ 120 };
        int v5{ 1 };
        int v6{ 0 };

        struct {
            bool enabled{ false };

            struct {
                bool enabled{ false };
                int key{ 0 };
                int keyMode{ 0 };
                bool keyToggled{ false };
                int maxChoke{ 3 };
            } fakeWalk;
        } general;

        //float --- config->AntiAim.lastlby;
        //float --- config->AntiAim.lbyNextUpdate;
        float lbyNextUpdate{ 0.0f };
        float lastlby{ 0.0f };
        float lastlbyval{ 0.0f };
        float subtractor{ 0.0f };
        bool legitaa{ false };
        bool legitaatest{ false };
        bool micromove{ false };
        bool swapsidesspam;

    } antiAim;


    struct StutterStep {
        bool enabled{ false };
        int stepSize{ 0 };
    } stutterstep;

    struct Backtrack {
        bool enabled{ false };
        bool ignoreSmoke{ false };
        bool recoilBasedFov{ false };
        int timeLimit{ 0 };
        bool fakeLatency{ false };
        bool onKey{ false };
        int fakeLatencyKey{ 0 };
        bool backtrackAll{ false };
        bool backtrackx88{ false };
        ColorToggle x88;
        int step{ 1 };
        bool extendedrecords{ false };
        float breadcrumbtime{ 800.0f };
        float breadexisttime{ 50.0f };
        bool tickShift{ false };
    } backtrack;

    struct Glow : ColorA {
        bool enabled{ false };
        bool healthBased{ false };
        int style{ 0 };
    };
    std::array<Glow, 21> glow;

    struct Material : ColorA {
        bool enabled = false;
        bool healthBased = false;
        bool blinking = false;
        bool wireframe = false;
        bool cover = false;
        bool ignorez = false;
        int material = 0;
        struct {
            bool enabled = false;
            float rate = .1f;
            float a = 1.0f;
        } pulse;
    };

    //struct Chams {
    //    std::array<Material, 10> materials;
    //} backupCham;

    struct SingleCham {
        bool enabled = false;
        bool healthBased = false;
        bool blinking = false;
        bool wireframe = false;
        bool cover = false;
        bool ignorez = false;
        int material = 0;
        struct {
            bool enabled = false;
            float rate = .1f;
            float a = 1.0f;
        } pulse;
        std::array<float, 4> color;
    } StaticProps, World;


    Config::Cham_s::Material backupChamMaterial;
    Config::Cham_s backupChamCatagorey;
    //std::unordered_map<std::wstring, Chams> chams;




    //struct Font {
    //    ImFont* tiny;
    //    ImFont* medium;
    //    ImFont* big;
    //};

    std::vector<std::string> systemFonts{ "Default" };
    std::unordered_map<std::string, Font> fonts;

    struct Visuals {
        bool disablePostProcessing{ false };
        bool inverseRagdollGravity{ false };
        bool noFog{ false };
        bool no3dSky{ false };
        bool noAimPunch{ false };
        bool noViewPunch{ false };
        bool noHands{ false };
        bool noSleeves{ false };
        bool noWeapons{ false };
        bool noSmoke{ false };
        bool noBlur{ false };
        bool noScopeOverlay{ false };
        bool noGrass{ false };
        bool noShadows{ false };
        bool wireframeSmoke{ false };
        bool zoom{ false };
        bool fullHLTV{ false };
        int zoomKey{ 0 };
#ifdef WALKBOT_BUILD
        bool thirdperson{ true };
        int thirdpersonKey{ 0 };
        int thirdpersonDistance{ 80 };
#endif
#ifndef WALKBOT_BUILD
        bool thirdperson{ false };
        int thirdpersonKey{ 0 };
        int thirdpersonDistance{ 0 };
#endif
        int viewmodelFov{ 0 };
        int fov{ 0 };
        int farZ{ 0 };
        int flashReduction{ 0 };
        float brightness{ 0.0f };
        int skybox{ 0 };


        ColorToggle DrawNavs;
        ColorToggle AllDrawNavs;
        struct DrawNavSetting {
            bool DrawNavsRect{ false };
            bool DrawCenterLines{ false };
            bool CalcAllConnections{ false };
            bool CalculateAllNavVectors{ false };
            int DrawNavDepth{ 1 };
            
        }DrawNavSettings;



        ColorToggle all;
        


        /*Renderables Window*/


        ColorToggle stolenfromrifk;
        ColorToggle bulletTracers;
        ColorToggle bulletTracersEnemy;
        ColorToggle HitSkeleton;
        ColorToggle grenadeBeams;
        ColorToggle espbulletTracers;
        ColorToggle grenadeBounce;
        ColorToggle world;
        ColorToggle PrecacheWorld;
        ColorToggle NonCachedWorld;
        ColorToggle sky;
        ColorToggle props;
        ColorToggle PrecacheProps;
        ColorToggle NonCached;
        ColorToggle smokecolor;
        ColorToggle NightMode; //ambient light
        ColorToggle dlight;
        ColorToggle lpdlight;
        ColorToggle moltovcolor;
        ColorToggle DrawLights;
        float dlightRadius{ 300.f };
        int dlightExponent{ 5 };
        float lpdlightRadius{ 300.f };
        int lpdlightExponent{ 5 };
        /*End Of Renderables Window*/

        bool matgrey{ false };

        bool deagleSpinner{ false };
        int screenEffect{ 0 };
        int hitEffect{ 0 };
        float hitEffectTime{ 0.6f };
        int hitMarker{ 0 };
        float hitMarkerTime{ 0.6f };
        int playerModelT{ 0 };
        int playerModelCT{ 0 };



        struct ColorCorrection {
            bool enabled = false;
            float blue = 0.0f;
            float red = 0.0f;
            float mono = 0.0f;
            float saturation = 0.0f;
            float ghost = 0.0f;
            float green = 0.0f;
            float yellow = 0.0f;
        } colorCorrection;

        bool Ignore_precache{ true };
        bool Precache_Only{ false };
        bool smoketrans{ false };
        float smokealpha{ 1000.0f };


        bool worldtrans{ false };
        float alphaval{ 100.0f };
        bool proptrans{ false };
        float propalphaval{ 100.0f };

        bool preworldtrans{ false };
        float prealphaval{ 100.0f };
        bool preproptrans{ false };
        float prepropalphaval{ 100.0f };

        bool smokeflagmanual{ false };
        int customflag{ 0 };
        int beamseg{ 2 };

        struct FogController {
            struct FogSettings {
                Config::ColorToggle Color;
                int iStartDistance{ -1 };
                int iEndDistance{ -1 };
                float flHdrColorScale{ 1.f };
            } Fog, Sky;
        } FogControl;


    } visuals;

    //std::array<item_setting, 36> skinChanger;

    struct Sound {
        int chickenVolume{ 100 };

        struct Player {
            int masterVolume{ 100 };
            int headshotVolume{ 100 };
            int weaponVolume{ 100 };
            int footstepVolume{ 100 };
        };

        std::array<Player, 3> players;
    } sound;


    struct Lagger {
        int PacketSize{ 10 };
        int SubChannelPacketSize{ 22 };
        bool skipOnCount{ true };
        bool reliableOnly{ false };
        bool OnVoice{ false };
        int countToSkip{ 1 };
        bool ResetOnTransmit{ false };
        int waitTime{ 0 };
        int activeTime{ 0 };
        bool FULLONBOGANFUCKERY{ false };
    } lagger;


    struct Stutterer2 {
        bool enabled{ false };
        int key{ false };
        bool ToggleKeyMode{ false };
        float SleepTime{ 166.f };
        int CountToSend{ 1833 };
    } stutterer;

#ifdef DEV_SECRET_BUILD
#ifdef DUMBEST_LAGGER
    struct MMLagger {
        bool enabled{ false };
        int key{ 0 };
        bool toggleMode{ false };
        int packets{ 3 };
        int blippackets{ 0 };
        int skip{ 120 };
        float desiredPing{ 1850 };
        int constant{ 0 };
        int KeepAlive{ 0 };
        int blip{ 0 };
        int ticksToSimulate{ 3 };
        bool shouldLag{ false };
        bool stutter{ false };
        bool stutter2{ false };
        bool crimWalk{ false };
        bool KeepAliveSleep{ false };
        bool turnoffDesync{ false };
        int desyncTimer{ 8 };
        int tickOut{ 8 };
        bool teleport{ false };
        bool bNewPrediction{ true };
        bool cleanup{ true };
        //bool asTcp{ false };
        //int chan{ 0 };
        //bool isReplayDemo{ false };
        //bool reliableOnly{ false };
    } mmlagger;
#endif
#endif

    struct MMCrasher {
        bool enabled = false;
        bool toggleKey = false;
        int key{ false };
        int timesToPostpone = 0;
        int timesToSend = 500;
        int bufferSize = INT_MAX;
        int nagleTime = 15;
        int MTU = INT_MAX;
        int DupPercent = 100;
        bool cooldownMaker = false;
        bool annoyance = false;
        int annoyanceKey = 0;
        float annoyanceOn = .5f;
        float annoyanceOff = 8.f;
        bool antiVoteKick{ false };
        bool antiSurrender{ true };
        bool breturn{ false };
        float cooldowncookertime{ 15.f };
    } mmcrasher;


    struct TestingShit {
        int fuckKey{ 0 };
        int fuckKey2{ 0 };
        int fuckKey3{ 0 };
        int fuckKey4{ 0 };
        int fuckKey5{ 0 };
        int fuckKey6{ 0 };
        int fuckKey7{ 0 };
        int fuckKey8{ 0 };
        int fuckKey9{ 0 };
        int fuckKey10{ 0 };
        int fuckKey11{ 0 };
        int fuckKey12{ 0 };
        int fuckKey13{ 0 };
    } TestShit;




    struct Misc {


        

        bool ragestrafe{ false };

        int menuKey{ 0x2D }; // VK_INSERT
        bool antiAfkKick{ false };
        bool autoStrafe{ false };
        bool bunnyHop{ false };
        bool customClanTag{ false };
        bool clocktag{ false };
        char clanTag[16];
        bool animatedClanTag{ false };
        bool fastDuck{ false };
        bool moonwalk{ false };
        bool edgejump{ false };
        int edgejumpkey{ 0 };
        bool slowwalk{ false };
        int slowwalkKey{ 0 };
        bool sniperCrosshair{ false };
        bool recoilCrosshair{ false };
        bool autoPistol{ false };
        bool autoReload{ false };
#ifdef SLAVE
        bool autoAccept{ true };
#else
        bool autoAccept{ true };
#endif
        bool radarHack{ false };
        bool revealRanks{ false };
        bool revealMoney{ false };
        bool revealSuspect{ false };
        ColorToggle spectatorList;
        ColorToggle watermark;
        bool fixAnimationLOD{ false };
        bool fixBoneMatrix{ false };
        bool fixMovement{ false };
        bool disableModelOcclusion{ false };
        float aspectratio{ 0 };
        bool killMessage{ false };
        std::string killMessageString{ "Fuck" };
        bool nameStealer{ false };
        bool disablePanoramablur{ false };
        int banColor{ 6 };
        std::string banText{ "Cheater has been permanently banned from official CS:GO servers." };
        bool fastPlant{ false };
        ColorToggle bombTimer{ 1.0f, 0.55f, 0.0f };
        bool quickReload{ false };
        bool prepareRevolver{ false };
        int prepareRevolverKey{ 0 };
        int hitSound{ 0 };
        int chokedPackets{ 0 };
        int chokedPacketsKey{ 0 };
        float fakelagspeed{ 70.0f };
        bool peeklag{ false };
        int quickHealthshotKey{ 0 };
        bool nadePredict{ false };
        bool fixTabletSignal{ false };
        float maxAngleDelta{ 255.0f };
        bool fakePrime{ false };
        int killSound{0};
        bool showall{false};
        int maxSpeed{0};
        bool svpurebypass{ true };
        bool boganCunt{ false };
        struct {
            bool enabled { false };
            bool toggled { false };
            int key = { 0 };

        } testshit;

        struct {
            bool enabled{ false };
            int Amount{ 1 };
        } tickLag;

        //bool testshit{ false };



        bool showDamagedone{ true };

            bool perfectShot{ false };
            bool Counter{ false };
            int CounterKey{ 0 };
            //bool Choke{ false };
            int psState{ 0 };
            int nextState{ 0 };

        std::string customKillSound;
        std::string customHitSound;
        PurchaseList purchaseList;

        bool animStateMon{ false };
        int overlay{ 13 };
        int entityid{ 0 };
        bool overlayall{ false };
        bool weight{ false };

        int airstuckkey{ 0 };
        bool airstuck{ false };


        bool walkbot{ false };
        int walkbotspeed{ 255 };

        bool doorspam{ false };

        int bunnyHopChance{ 100 };
        int bunnymax{ 0 };


    } misc;


    struct ConfigFile {
        bool WriteToConfigFile{ false };
        std::string File{ "C:\\HarpoonConfig.cfg" };
    } configfile;


    struct ESP {
        ColorToggle Text;
        ColorToggle Box;
        struct esp_entity {
            ColorToggle Text;
            ColorToggle Box;
        };

        struct player : public esp_entity {
            bool PlayerName;
            bool Health;
        };

        bool HealthBar{ false };
      
        struct weapon : public esp_entity {

        };

        struct SharedESP {
            bool bEnabled{ false };
        } shared_esp;



    } esp;
    void scheduleFontLoad(const std::string& name) noexcept;
    bool loadScheduledFonts() noexcept;
private:
    std::vector<std::string> scheduledFonts{ "Default" };
    std::filesystem::path path;
    std::vector<std::string> configs;
};






inline std::unique_ptr<Config> config;