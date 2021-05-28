#pragma once
#include "Pendelum/Pendelum.h"
#include "Resource/SteamWorks/steam/isteamfriends.h"
#include "Resource/SteamWorks/steam/steamclientpublic.h"
#include "Resource/SteamWorks/steam/isteamfriends.h"
#include "Resource/SteamWorks/steam/isteamutils.h"
#include "Resource/SteamWorks/steam/isteamuser.h"
#include "Resource/SteamWorks/steam/isteamuserstats.h"
#include "Resource/SteamWorks/steam/isteamhttp.h"
#include "Hacks/SteamWorks/SteamWorks.h"
#include "COMPILERDEFINITIONS.h"
#include "Hacks/OTHER/Debug.h"
#include "SDK/OsirisSDK/EntityList.h"
#include "SDK/OsirisSDK/Entity.h"
#include "SDK/SDKAddition/AvatarImage.h"
namespace PlayerList {


    struct AvatarImage {
        PDIRECT3DTEXTURE9* pTexture{ nullptr }; // Not to be written to file
        int nTextureWidth;
        int nTextureHeight;
        bool m_bHasImage{ false };
        int m_nSize{ false };
        uint32_t nWidth;
        uint32_t nHeight;
        std::string m_strData;
    };
    struct PlayerAvater {
        AvatarImage m_Large;
        AvatarImage m_Medium;
        AvatarImage m_Small;
    };
    struct PlayerItem {

        //PlayerAvater m_Avatar;
        CGameUiAvatarImage m_Avatar;
        std::string m_strName{ "N/A" };
        std::string m_strIDString{ "N/A" };
        std::string m_strLastSeeDate{"No Record"};
        std::string m_strSteamProfileName{ "No Record" };
        std::string m_strPreviousName{ "N/A" };
        std::uint32_t m_SteamID3{ 0 };
        CSteamID m_SteamID;
        bool m_bHasItem{ false };
        bool m_bCheater{ false };
        bool m_bFriend{ false };
        bool m_bFlagged{ false };
        bool m_bIsBot{ false };
        bool m_bIsCheater{ false };
        bool m_bIsTeammate{ false };
        bool m_bIsSpamming{ false };
        bool m_bHarpoonUser{ false };
        bool m_bBeenRead{ false };
        int m_nUserID{ 0 };

        bool m_bFriendsListSet{ 0 };
        std::vector<uint32_t> m_arrFriends;
        // Window Shit
        bool m_bFocused{ false };
        bool m_bOpen{ false };
        int m_nItemArrayIndex{ 0 };
    };
    struct PlayerListRecord {
        PlayerItem m_piPreviousRecord;
        int m_nNumberOfGames{ false };
        bool m_bHasStoredESP{ false };
    };

    inline PlayerItem g_arrPlayerData[64];




    //void ReadPlayerRecords();
    //void UpdatePlayerRecords();
    void UpdatePlayerList();
    //void DrawPlayerRecords();
    //void DrawCurrentPlayerList();
    //void DrawPlayerRecord();
    //void DrawPlayerList();



	class PlayerListContents : Pendelum::MenuItem
	{

    public:
        PlayerListContents(wchar_t*, Pendelum::Window* Master) {

        }
        PlayerListContents(Pendelum::Window* Master) {
            Master_Window = Master;
        }

        void Draw(bool Focus, Pendelum::color* col = nullptr, bool colorOveride = 0);
        void SetText(wchar_t* inText) {
            text = inText;
        }
        void SetOwnerWindow(Pendelum::Window* window) {
            Master_Window = window;
        }

        void DrawPlayerList(Pendelum::ScreenVec vPosition);

        void SetPosition(int x, int y) {
            Pos.x = x;
            Pos.y = y;
        }

        std::pair<int, int> CalcSize() {
            return interfaces->surface->getTextSize(5, text.c_str());
        }
        bool* ExposeOpen() {
            return &open;
        }

        Pendelum::Window* ExposeWindow() {
            return Master_Window;
        }


        bool QuerySteamAPI(PlayerItem* pItem, std::string Request);
        bool RequestFriendsList(PlayerItem* pItem);

        bool WriteItemToRecords(PlayerItem* pItem);
        void DeleteRecord(uint32_t);
        bool CheckForRecord(PlayerItem&, uint32_t ID);
        void DrawPlayerRecords(Pendelum::ScreenVec vPosition);
        void ReloadRecords();

    private:
        bool open = false;
        std::wstring text;
        Pendelum::ScreenVec Pos;
        Pendelum::Window* Master_Window;
        std::vector<PlayerItem> m_vRecords;
	};





    inline Pendelum::Window* g_pPlayerListMenu;

	class PlayerList : Pendelum::MenuItem
	{

    public:
        PlayerList(wchar_t* inText, Pendelum::Window* Master) {
            text = inText;
            Master_Window = Master;

            PlayerListContents* pList = new PlayerListContents(Master);


            auto vInfo = Master->getScreenInfo();
            float height = vInfo.height / 2;
            float width = vInfo.width / 2;


            m_pPlayerListContents = Pendelum::CreateNewWindow();
            m_pPlayerListContents->setAsSlave(Master, &open);
            m_pPlayerListContents->SetColor(80, 80, 80, 150);
            m_pPlayerListContents->SetWindowInfo(NULL, NULL, width, height, 30, 30, true);
            m_pPlayerListContents->Add_Item((MenuItem*)pList);
            m_pPlayerListContents->SetMuteBackground(true);
            g_pPlayerListMenu = m_pPlayerListContents;
        }

        void Draw(bool Focus, Pendelum::color* col = nullptr, bool colorOveride = 0);
        void SetText(wchar_t* inText) {
            text = inText;
        }
        void SetOwnerWindow(Pendelum::Window* window) {
            Master_Window = window;
        }
        void SetPosition(int x, int y) {
            Pos.x = x;
            Pos.y = y;
        }

        std::pair<int, int> CalcSize() {
            return interfaces->surface->getTextSize(5, text.c_str());
        }

        Pendelum::Window* ExposeWindow() {
            return m_pPlayerListContents;
        }

        bool* ExposeOpen() {
            return &open;
        }
    private:
        Pendelum::Window* m_pPlayerListContents;
        std::wstring text;
        Pendelum::ScreenVec Pos;
        Pendelum::Window* Master_Window;
        bool open = false;
        bool floatOver = false;
	};


};





