#include "PlayerList.h"
#include "Memory.h"
#include "SDK/OsirisSDK/GlobalVars.h"

#include "SDK/OsirisSDK/Entity.h"
#include "SDK/SDKAddition/EntityListCull.hpp"

#include <wchar.h>


#include <iostream>
#include <fstream>

#include <ShlObj.h>
#include "Helpers.h"
#include <iomanip>


#include "nlohmann/json.hpp"

using json = nlohmann::json;


#define STEAM_API_KEY ""
/* Make a class based menu system, then need a function base one. Fuck my life!*/


extern bool IsMouseClicked(bool SetFalse = false);
extern bool IsMouseWithinBounds(int x, int y, int x2, int y2);



using namespace Pendelum;


std::pair <int, int> g_CursorPosition;


void PlayerList::PlayerList::Draw(bool Focus, Pendelum::color* col /*= nullptr*/, bool colorOveride /*= 0*/) {
    Pendelum::ScreenVec Vec = Master_Window->getScreenInfo();
    int x, y;
    x = Vec.x + Pos.x;
    y = Vec.y + Pos.y;

    auto [width, height] = interfaces->surface->getTextSize(5, L"H");

    interfaces->surface->setDrawColor(120, 120, 120, 150);
    interfaces->surface->drawFilledRect(x + 5, y, width + x + height, y + height);
    x = width + x + height + 5;

    interfaces->surface->setTextPosition(x, y);
    //interfaces->surface->setTextColor((int)(value->color[0] * 255.f), (int)(value->color[1] * 255.f), (int)(value->color[2] * 255.f), 255);
    if (Focus) {
        interfaces->surface->setTextColor(200, 200, 255, 255);

        auto [memberWidth, memberheight] = CalcSize();
        bool InBounds = IsMouseWithinBounds((x - 20), y, x + (memberWidth * 2), y + memberheight + 2);
        bool Clicked = IsMouseClicked();

        if (Pendelum::GetKey(Pendelum::Enter, memory->globalVars->currenttime) || (InBounds && Clicked)) {
            open = !open;
        }
    }
    std::wstring stext = { text };
    if (open) {
#ifndef _DEBUG
        Pendelum::Set_Focus(*m_pPlayerListContents);
#endif
    }

    interfaces->surface->printText(stext);
}


__forceinline void RenderBoxOutline(ScreenVec vBox) {
    interfaces->surface->drawLine(vBox.x, vBox.y, vBox.x, vBox.y + vBox.height);
    interfaces->surface->drawLine(vBox.x, vBox.y + vBox.height, vBox.x + vBox.width, vBox.y + vBox.height);
    interfaces->surface->drawLine(vBox.x, vBox.y, vBox.x + vBox.width, vBox.y);
    interfaces->surface->drawLine(vBox.x + vBox.width, vBox.y, vBox.x + vBox.width, vBox.y + vBox.height);
}


std::pair<int, int> ClickableText(std::string strText, bool* pWasClicked, bool bToggleType = false, bool bDrawAsTrue = false) {
    
    std::wstring text(strText.begin(), strText.end());

    interfaces->surface->setTextFont(5);
    std::pair <int, int> ipTextSize = interfaces->surface->getTextSize(5, text.c_str());

    if (IsMouseWithinBounds(g_CursorPosition.first, g_CursorPosition.second, g_CursorPosition.first + ipTextSize.first, g_CursorPosition.second + ipTextSize.second)) 
    {
        if (IsMouseClicked())
        {
            if (bToggleType)
                *pWasClicked = !(*pWasClicked);
            else
                *pWasClicked = true;
        }
        else 
        {
            if(!bToggleType)
                *pWasClicked = false;
        }
        interfaces->surface->setTextColor(200, 200, 255, 255);
    }
    else {
        interfaces->surface->setTextColor(0, 250, 0, 255);

        if((bToggleType && *pWasClicked) || bDrawAsTrue)
            interfaces->surface->setTextColor(200, 200, 255, 255);

    }

    interfaces->surface->printText(text);
    return ipTextSize;
}



void DrawPlayerInfoItem(std::string strString, int nHeaderH) {
    std::wstring wstrIDStr(strString.begin(), strString.end());
    interfaces->surface->setTextPosition(g_CursorPosition.first, g_CursorPosition.second);
    interfaces->surface->setTextColor(0, 250, 0, 255);
    interfaces->surface->printText(wstrIDStr);
    g_CursorPosition.second += nHeaderH + 1;
}

static const char* Ranks[] =
{
        "Unranked",
        "Silver I",
        "Silver II",
        "Silver III",
        "Silver IV",
        "Silver Elite",
        "Silver Elite Master",

        "Gold Nova I",
        "Gold Nova II",
        "Gold Nova III",
        "Gold Nova Master",
        "Master Guardian I",
        "Master Guardian II",

        "Master Guardian Elite",
        "Distinguished Master Guardian",
        "Legendary Eagle",
        "Legendary Eagle Master",
        "Supreme Master First Class",
        "The Global Elite"
};

void PlayerList::PlayerListContents::DrawPlayerList(ScreenVec vPosition) {

    // Draw SideBar
    ScreenVec Sidebar;
    Sidebar.x = vPosition.x;
    Sidebar.y = vPosition.y;
    Sidebar.height = vPosition.height;
    Sidebar.width = vPosition.width * (1.f / 5.f);



    RenderBoxOutline(Sidebar);


    g_CursorPosition.first = Sidebar.x + 5;
    g_CursorPosition.second = Sidebar.y + 5;

    PlayerItem* pSelectedItem{ nullptr };
    static int nLastFocused{ 0 };

    std::vector<PlayerItem*> vTeam;
    std::vector<PlayerItem*> vEnemies;




    for (PlayerItem& Item : g_arrPlayerData)
    {
        if (!localPlayer || !localPlayer.get())
            continue;

        if (!Item.m_bHasItem)
            continue;

        Entity* pEnt = interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(Item.m_nUserID));

        if (!pEnt)
            continue;

        if (pEnt->isOtherEnemy(localPlayer.get()))
        {
            Item.m_bIsTeammate = false;
            vEnemies.push_back(&Item);
        }
        else {
            Item.m_bIsTeammate = true;
            vTeam.push_back(&Item);
        }


    }

    if (vEnemies.size() > 0)
    {
        interfaces->surface->setTextColor(0, 250, 0, 255);
        auto [CatW, CatH] = interfaces->surface->getTextSize(5, L"Enemies");
        interfaces->surface->setTextPosition(g_CursorPosition.first + ((Sidebar.width / 2) - (CatW / 2)), g_CursorPosition.second);
        interfaces->surface->printText(L"Enemies");
        interfaces->surface->drawLine(g_CursorPosition.first + ((Sidebar.width / 2) - (CatW / 2)), g_CursorPosition.second + 1 + CatH, g_CursorPosition.first + ((Sidebar.width / 2) + (CatW / 2)), g_CursorPosition.second + 1 + CatH);
        g_CursorPosition.second += CatH + 2;
    }

    std::vector<PlayerItem*>* pData = &vEnemies;
    for (int i = 0; i < 2; i++) {
        for (PlayerItem* Item : *pData)
        {
            if (!Item->m_bHasItem)
                continue;

            bool bWasClicked = false;

            std::string strName = Item->m_strName;
            if (strName.size() > 20) {
                strName.resize(20);
                strName.append("...");
            }
            interfaces->surface->setTextPosition(g_CursorPosition.first, g_CursorPosition.second);
            std::pair<int, int> ipTextSize = ClickableText(strName, &bWasClicked, false, Item->m_bOpen);
            g_CursorPosition.second += ipTextSize.second + 2;

            if (bWasClicked)
            {
                Item->m_bOpen = true;
                nLastFocused = Item->m_nItemArrayIndex;
            }
            else if (Item->m_bOpen && (Item->m_nItemArrayIndex != nLastFocused))
            {
                Item->m_bOpen = false;
            }

            if (Item->m_bOpen) {
                pSelectedItem = Item;
            }
        }

        pData = &vTeam;

        if ((i == 0) && (vTeam.size() > 0))
        {
            interfaces->surface->setTextColor(0, 250, 0, 255);
            auto [CatW, CatH] = interfaces->surface->getTextSize(5, L"Teammates");
            interfaces->surface->setTextPosition(g_CursorPosition.first + ((Sidebar.width / 2) - (CatW / 2)), g_CursorPosition.second);
            interfaces->surface->printText(L"Teammates");
            interfaces->surface->drawLine(g_CursorPosition.first + ((Sidebar.width / 2) - (CatW / 2)), g_CursorPosition.second + 1 + CatH, g_CursorPosition.first + ((Sidebar.width / 2) + (CatW / 2)), g_CursorPosition.second + 1 + CatH);
            g_CursorPosition.second += CatH + 2;
        }
    }

    ScreenVec vMainWindow;
    vMainWindow.x = vPosition.x + Sidebar.width + 3;
    vMainWindow.y = vPosition.y;
    vMainWindow.height = vPosition.height;
    vMainWindow.width = (vPosition.width * (4.f / 5.f)) - 3;
    RenderBoxOutline(vMainWindow);


    if (pSelectedItem) {

        PlayerItem RecordItem;
        if (!pSelectedItem->m_bIsBot && !pSelectedItem->m_bBeenRead && CheckForRecord(RecordItem, pSelectedItem->m_SteamID3))
        {
            pSelectedItem->m_bBeenRead = true;
            pSelectedItem->m_bCheater = RecordItem.m_bCheater;
            pSelectedItem->m_bHarpoonUser = RecordItem.m_bHarpoonUser;
            pSelectedItem->m_bFriend = RecordItem.m_bFriend;
            pSelectedItem->m_strLastSeeDate = RecordItem.m_strLastSeeDate;
            pSelectedItem->m_strPreviousName = RecordItem.m_strName;
        }



        g_CursorPosition.first = vMainWindow.x + 3;
        g_CursorPosition.second = vMainWindow.y;


        int nMiddlePoint = (vMainWindow.x + (vMainWindow.width / 2));


        std::string strHeader = { "Player : " + pSelectedItem->m_strName };
        std::wstring wstrHeader(strHeader.begin(), strHeader.end());
        auto [nHeaderW, nHeaderH] = interfaces->surface->getTextSize(5, wstrHeader.c_str());

        g_CursorPosition.second += nHeaderH + 1;
        interfaces->surface->setTextPosition(nMiddlePoint - (nHeaderW/2), g_CursorPosition.second);
        interfaces->surface->setTextColor(0, 250, 0, 255);
        interfaces->surface->printText(wstrHeader);
        g_CursorPosition.second += nHeaderH + 1;
        interfaces->surface->drawLine(g_CursorPosition.first - 3, g_CursorPosition.second + 3, g_CursorPosition.first + vMainWindow.width, g_CursorPosition.second + 3);
        g_CursorPosition.second += nHeaderH + 1;
        g_CursorPosition.second += nHeaderH + 1;


        static int nEndOfAvatar{ -1 };
        if (!pSelectedItem->m_bIsBot) {
            if (pSelectedItem->m_Avatar.IsValid() && ((pSelectedItem->m_Avatar.GetFetchedTime() + 6000.f) > memory->globalVars->realtime)) {
                int nAvatarPosX = (((vMainWindow.width / 2)/2) + nMiddlePoint) - ((pSelectedItem->m_Avatar.GetWidth()) / 2);
                pSelectedItem->m_Avatar.SetPos(nAvatarPosX, g_CursorPosition.second);
                pSelectedItem->m_Avatar.Paint();
                nEndOfAvatar = g_CursorPosition.second + pSelectedItem->m_Avatar.GetHeight();

                ScreenVec vBorder;
                vBorder.x = nAvatarPosX;
                vBorder.y = g_CursorPosition.second;
                vBorder.height = pSelectedItem->m_Avatar.GetHeight();
                vBorder.width = pSelectedItem->m_Avatar.GetWidth();
                RenderBoxOutline(vBorder);
            }
            else {
                VCON("Updating Avatar For Player : %s (SteamID3: %s)\n", pSelectedItem->m_strName.c_str(), pSelectedItem->m_strIDString.c_str());
                pSelectedItem->m_Avatar.SetAvatarSteamID(pSelectedItem->m_SteamID);
            }
        }

        Entity* pEnt = interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(pSelectedItem->m_nUserID));

        if (pSelectedItem->m_bIsTeammate && !pSelectedItem->m_bIsBot && pEnt && (nEndOfAvatar > -1))
        {
            std::pair<int, int> nCursorSave = g_CursorPosition;
            auto [nTextW, nTextH] = interfaces->surface->getTextSize(5, L"Kick Player");
            int nKickPosX = (((vMainWindow.width / 2) / 2) + nMiddlePoint) - (nTextW / 2);
            g_CursorPosition.first = nKickPosX;
            g_CursorPosition.second = nEndOfAvatar + 2;
            interfaces->surface->setTextPosition(nKickPosX, nEndOfAvatar + 2);

            bool bKickPlayer{ false };          
            ClickableText("Kick Player", &bKickPlayer);

            if (bKickPlayer)
            {
                char buffer[4096];
                snprintf(buffer, 4096, "callvote kick %d %d", pSelectedItem->m_nUserID, pEnt->index());
                interfaces->engine->clientCmdUnrestricted(buffer);
            }
            g_CursorPosition = nCursorSave;
        }

        char buffer[4096];
        // FUCK I HATE WIDE CHARACTERS


        DrawPlayerInfoItem(std::string("Profile Name : " + pSelectedItem->m_strSteamProfileName), nHeaderH);
        DrawPlayerInfoItem(std::string("Previous Name : " + pSelectedItem->m_strPreviousName), nHeaderH);
        DrawPlayerInfoItem(std::string("SteamID64 : " + std::to_string(pSelectedItem->m_SteamID.ConvertToUint64())), nHeaderH);
        DrawPlayerInfoItem(std::string("SteamID3 : " + pSelectedItem->m_strIDString), nHeaderH);
        DrawPlayerInfoItem(std::string("UserID : " + std::to_string(pSelectedItem->m_nUserID)), nHeaderH);
        if (pEnt) {
            DrawPlayerInfoItem(std::string("Index : " + std::to_string(pEnt->index())), nHeaderH);
        }

        const char* YoN = pSelectedItem->m_bIsCheater ? "Yes" : "No";
        DrawPlayerInfoItem(std::string("Cheater : " + std::string(YoN)), nHeaderH);

        if (pEnt && config->debug.bPrintRank) {
            int nWins = *((int*)&(((Entity*)(*memory->m_pPlayerResource))->CompetitiveWins()) + (pEnt->index() * 0x4));
            int nRank = *((int*)&(((Entity*)(*memory->m_pPlayerResource))->CompetitiveRanking()) + (pEnt->index() * 0x4));
            DrawPlayerInfoItem(std::string("Competetive Wins : " + std::to_string(nWins)), nHeaderH);
            DrawPlayerInfoItem(std::string("Competetive Rank : " + std::string(Ranks[nRank])), nHeaderH);
        }



        if (pEnt) {
            Vector vOrigin = pEnt->getAbsOrigin();
            snprintf(buffer, 4096, "Pos : {%.*f,%.*f,%.*f} ", 2, vOrigin.x, 2, vOrigin.y, 2, vOrigin.z);
            DrawPlayerInfoItem(std::string(buffer), nHeaderH);
        }

        DrawPlayerInfoItem("Date Seen : " + pSelectedItem->m_strLastSeeDate, nHeaderH);









        if (nEndOfAvatar < 0) {
            g_CursorPosition.second += nHeaderH + 1;
            g_CursorPosition.second += nHeaderH + 1;
        }
        else {
            g_CursorPosition.second = nEndOfAvatar + 5;
        }

        {
            bool bFriendsAdd = false;
            interfaces->surface->setTextPosition(g_CursorPosition.first, g_CursorPosition.second);
            ClickableText("Add Player To Friends List", &bFriendsAdd);

            if (bFriendsAdd && !pSelectedItem->m_bIsBot)
            {
                VCON("Adding Player : %s (SteamID3: %s) To FriendsList\n", pSelectedItem->m_strName.c_str(), pSelectedItem->m_strIDString.c_str());

                char path[MAX_PATH];
                HRESULT hr = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL,
                    SHGFP_TYPE_CURRENT, path);
                strcat(path, "\\Harpoon\\Friends.txt");

                std::fstream FriendsFile(path, std::fstream::in | std::fstream::out | std::fstream::app);
                std::string ID = std::to_string(pSelectedItem->m_SteamID3) + "\n";
                FriendsFile.write(ID.c_str(), ID.size());
                FriendsFile.close();
            }

        }
        g_CursorPosition.second += nHeaderH + 1;


        {
            bool bFriendsRemove = false;
            interfaces->surface->setTextPosition(g_CursorPosition.first, g_CursorPosition.second);
            ClickableText("Remove Player From Friends List", &bFriendsRemove);

            if (bFriendsRemove && !pSelectedItem->m_bIsBot)
            {
                std::vector<CSteamID> friends;
                VCON("Removing Player : %s (SteamID3: %s) From FriendsList\n", pSelectedItem->m_strName.c_str(), pSelectedItem->m_strIDString.c_str());
                std::fstream playerFile;
                char path[MAX_PATH];
                HRESULT hr = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL,
                    SHGFP_TYPE_CURRENT, path);
                strcat(path, "\\Harpoon\\Friends.txt");
		        playerFile.open(path, std::fstream::in | std::fstream::out);
		        int32_t id = 0;
		        std::string friendsstr;
		        while (getline(playerFile, friendsstr)) {
			        try {
				        id = std::stoi(friendsstr);
				        friendsstr.erase(std::remove(friendsstr.begin(), friendsstr.end(), '\n'),
					        friendsstr.end());
				        //Debug::QuickPrint(("Friend ID Is Parsed = " + std::to_string(id)).c_str());
				        CSteamID steamid(id, k_EUniversePublic, k_EAccountTypeIndividual);

                        if (steamid.GetAccountID() == pSelectedItem->m_SteamID.GetAccountID())
                            continue;

				        //steamid.SetFromString(friendsstr.c_str(), k_EUniversePublic);
				        friends.push_back(steamid);
			        }
			        catch (std::exception& e) {
				        Debug::QuickPrint(friendsstr.c_str());
			        }
		        }
                playerFile.clear();
                playerFile.close();
                playerFile.open(path, std::fstream::out | std::fstream::trunc);


                for (CSteamID Account : friends) {
                    std::string id = std::to_string(Account.GetAccountID()) + "\n";
                    playerFile.write(id.c_str(), id.size());
                }
		        playerFile.close();
                friends.clear();
            }

        }
        g_CursorPosition.second += nHeaderH + 1;

        {
            bool bRecordAdd = false;
            interfaces->surface->setTextPosition(g_CursorPosition.first, g_CursorPosition.second);
            ClickableText("Add Player To Records", &bRecordAdd);

            if (bRecordAdd && !pSelectedItem->m_bIsBot)
            {
                VCON("Adding Player : %s (SteamID3: %s) To Records\n", pSelectedItem->m_strName.c_str(), pSelectedItem->m_strIDString.c_str());
                WriteItemToRecords(pSelectedItem);
            }
        }

        g_CursorPosition.second += nHeaderH + 1;

        {
            bool bRecordDelete = false;
            interfaces->surface->setTextPosition(g_CursorPosition.first, g_CursorPosition.second);
            ClickableText("Remove Player From Records", &bRecordDelete);

            if (bRecordDelete && !pSelectedItem->m_bIsBot)
            {
                VCON("Removing Player : %s (SteamID3: %s) From Records\n", pSelectedItem->m_strName.c_str(), pSelectedItem->m_strIDString.c_str());
                DeleteRecord(pSelectedItem->m_SteamID3);
            }
        }
        g_CursorPosition.second += nHeaderH + 1;

        {
            bool bAddCheater = false;
            interfaces->surface->setTextPosition(g_CursorPosition.first, g_CursorPosition.second);
            ClickableText("Mark Player As Cheater", &bAddCheater);

            if (bAddCheater && !pSelectedItem->m_bIsBot)
            {
                VCON("Marking Player : %s (SteamID3: %s) As Cheater\n", pSelectedItem->m_strName.c_str(), pSelectedItem->m_strIDString.c_str());
                pSelectedItem->m_bIsCheater = !pSelectedItem->m_bIsCheater;
            }
        }
        g_CursorPosition.second += nHeaderH + 1;

        {
            bool bInitiateSpam = false;
            interfaces->surface->setTextPosition(g_CursorPosition.first, g_CursorPosition.second);
            ClickableText("Invite Spam", &bInitiateSpam);

            if (bInitiateSpam && !pSelectedItem->m_bIsBot)
            {
                if (pSelectedItem->m_bIsSpamming)
                {
                    VCON("No Longer Invite Spamming Player : %s (SteamID3: %s)\n", pSelectedItem->m_strName.c_str(), pSelectedItem->m_strIDString.c_str());

                    int i = 0;
                    for (uint32_t& ID : OurSteamWorks::m_vIDs)
                    {

                        if (ID == pSelectedItem->m_SteamID3)
                        {
                            
                            OurSteamWorks::m_vIDs.erase(OurSteamWorks::m_vIDs.begin()+i);//(OurSteamWorks::m_vIDs.at(i))
                            break;
                        }
                        i++;
                    }

                }
                else {
                    VCON("Invite Spamming Player : %s (SteamID3: %s)\n", pSelectedItem->m_strName.c_str(), pSelectedItem->m_strIDString.c_str());
                    OurSteamWorks::m_vIDs.push_back(pSelectedItem->m_SteamID3);
                }

            }

        }



    }
}


#include "Other/CrossCheatTalk.h"
void PlayerList::PlayerListContents::DrawPlayerRecords(ScreenVec vPosition) {

    // Draw SideBar
    ScreenVec Sidebar;
    Sidebar.x = vPosition.x;
    Sidebar.y = vPosition.y;
    Sidebar.height = vPosition.height;
    Sidebar.width = vPosition.width * (1.f / 5.f);



    RenderBoxOutline(Sidebar);


    g_CursorPosition.first = Sidebar.x + 5;
    g_CursorPosition.second = Sidebar.y + 5;

    PlayerItem* pSelectedItem{ nullptr };
    static int nLastFocused{ 0 };


    interfaces->surface->setTextColor(0, 250, 0, 255);
    auto [CatW, CatH] = interfaces->surface->getTextSize(5, L"Reload Records");
    interfaces->surface->setTextPosition(g_CursorPosition.first + ((Sidebar.width / 2) - (CatW / 2)), g_CursorPosition.second);
    bool bReload = false;
    interfaces->surface->drawLine(g_CursorPosition.first + ((Sidebar.width / 2) - (CatW / 2)), g_CursorPosition.second + 1 + CatH, g_CursorPosition.first + ((Sidebar.width / 2) + (CatW / 2)), g_CursorPosition.second + 1 + CatH);
    ClickableText("Reload Records", &bReload);
    if (bReload)
    {
        ReloadRecords();
    }
    g_CursorPosition.second += CatH + 2;
    g_CursorPosition.second += CatH + 2;

    for (PlayerItem& Item : m_vRecords)
    {
        if (!Item.m_bHasItem)
            continue;


            bool bWasClicked = false;

            std::string strName = Item.m_strName;
            if (strName.size() > 20) {
                strName.resize(20);
                strName.append("...");
            }
            interfaces->surface->setTextPosition(g_CursorPosition.first, g_CursorPosition.second);
            std::pair<int, int> ipTextSize = ClickableText(strName, &bWasClicked, false, Item.m_bOpen);
            g_CursorPosition.second += ipTextSize.second + 2;

            if (bWasClicked)
            {
                Item.m_bOpen = true;
                nLastFocused = Item.m_nItemArrayIndex;
            }
            else if (Item.m_bOpen && (Item.m_nItemArrayIndex != nLastFocused))
            {
                Item.m_bOpen = false;
            }

            if (Item.m_bOpen) {
                pSelectedItem = &Item;
            }
    }



    ScreenVec vMainWindow;
    vMainWindow.x = vPosition.x + Sidebar.width + 3;
    vMainWindow.y = vPosition.y;
    vMainWindow.height = vPosition.height;
    vMainWindow.width = (vPosition.width * (4.f / 5.f)) - 3;
    RenderBoxOutline(vMainWindow);


    if (pSelectedItem) {

        g_CursorPosition.first = vMainWindow.x + 3;
        g_CursorPosition.second = vMainWindow.y;


        int nMiddlePoint = (vMainWindow.x + (vMainWindow.width / 2));


        std::string strHeader = { "Player : " + pSelectedItem->m_strName };
        std::wstring wstrHeader(strHeader.begin(), strHeader.end());
        auto [nHeaderW, nHeaderH] = interfaces->surface->getTextSize(5, wstrHeader.c_str());

        g_CursorPosition.second += nHeaderH + 1;
        interfaces->surface->setTextPosition(nMiddlePoint - (nHeaderW / 2), g_CursorPosition.second);
        interfaces->surface->setTextColor(0, 250, 0, 255);
        interfaces->surface->printText(wstrHeader);
        g_CursorPosition.second += nHeaderH + 1;
        interfaces->surface->drawLine(g_CursorPosition.first - 3, g_CursorPosition.second + 3, g_CursorPosition.first + vMainWindow.width, g_CursorPosition.second + 3);
        g_CursorPosition.second += nHeaderH + 1;
        g_CursorPosition.second += nHeaderH + 1;


        static int nEndOfAvatar{ -1 };
        if (!pSelectedItem->m_bIsBot) {
            if (pSelectedItem->m_Avatar.IsValid() && ((pSelectedItem->m_Avatar.GetFetchedTime() + 6000.f) > memory->globalVars->realtime)) {
                int nAvatarPosX = (((vMainWindow.width / 2) / 2) + nMiddlePoint) - ((pSelectedItem->m_Avatar.GetWidth()) / 2);
                pSelectedItem->m_Avatar.SetPos(nAvatarPosX, g_CursorPosition.second);
                pSelectedItem->m_Avatar.Paint();
                nEndOfAvatar = g_CursorPosition.second + pSelectedItem->m_Avatar.GetHeight();

                ScreenVec vBorder;
                vBorder.x = nAvatarPosX;
                vBorder.y = g_CursorPosition.second;
                vBorder.height = pSelectedItem->m_Avatar.GetHeight();
                vBorder.width = pSelectedItem->m_Avatar.GetWidth();
                RenderBoxOutline(vBorder);
            }
            else {
                VCON("Updating Avatar For Player : %s (SteamID3: %s)\n", pSelectedItem->m_strName.c_str(), pSelectedItem->m_strIDString.c_str());
                pSelectedItem->m_Avatar.SetAvatarSteamID(pSelectedItem->m_SteamID);
                steamFriends->RequestUserInformation(pSelectedItem->m_SteamID, false);
            }
        }


        char buffer[4096];
        // FUCK I HATE WIDE CHARACTERS
        Entity* pEnt = interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(pSelectedItem->m_nUserID));

        DrawPlayerInfoItem(std::string("Profile Name : " + pSelectedItem->m_strSteamProfileName), nHeaderH);
        DrawPlayerInfoItem(std::string("SteamID64 : " + std::to_string(pSelectedItem->m_SteamID.ConvertToUint64())), nHeaderH);
        DrawPlayerInfoItem(std::string("SteamID3 : " + pSelectedItem->m_strIDString), nHeaderH);
        const char* YoN = pSelectedItem->m_bIsCheater ? "Yes" : "No";
        DrawPlayerInfoItem(std::string("Cheater : " + std::string(YoN)), nHeaderH);
        DrawPlayerInfoItem("Date Seen : " + pSelectedItem->m_strLastSeeDate, nHeaderH);

        if (nEndOfAvatar < 0) {
            g_CursorPosition.second += nHeaderH + 1;
            g_CursorPosition.second += nHeaderH + 1;
        }
        else {
            g_CursorPosition.second = nEndOfAvatar + 5;
        }


        {
            bool bStartConn = false;
            interfaces->surface->setTextPosition(g_CursorPosition.first, g_CursorPosition.second);
            ClickableText("Start Harpoon Net Connection", &bStartConn);

            if (bStartConn && !pSelectedItem->m_bIsBot)
            {
                CrossCheatTalk::ClientConnection NewConn;
                NewConn.nSteamID = pSelectedItem->m_SteamID3;
                NewConn.nVirtualDataPort = 58;
                CrossCheatInitMsg NewConnectionInit;
                NewConnectionInit.set_steamid(g_pSteamUser->GetSteamID().GetAccountID());
                CrossCheatTalk::g_pSteamNetSocketsNetChannel->SendMessageToUser(CrossCheatMsgType::k_CrossCheatInitMsg, &NewConnectionInit, NewConn.nSteamID);
                VCON("Starting HarpoonNet Connection With Player : %s (SteamID3: %s)\n", pSelectedItem->m_strName.c_str(), pSelectedItem->m_strIDString.c_str());
            }

        }
        g_CursorPosition.second += nHeaderH + 1;


        {
            bool bFriendsAdd = false;
            interfaces->surface->setTextPosition(g_CursorPosition.first, g_CursorPosition.second);
            ClickableText("Add Player To Friends List", &bFriendsAdd);

            if (bFriendsAdd && !pSelectedItem->m_bIsBot)
            {
                char path[MAX_PATH];
                HRESULT hr = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL,
                    SHGFP_TYPE_CURRENT, path);
                strcat(path, "\\Harpoon\\Friends.txt");
                VCON("Adding Player : %s (SteamID3: %s) To FriendsList\n", pSelectedItem->m_strName.c_str(), pSelectedItem->m_strIDString.c_str());
                std::fstream FriendsFile(path, std::fstream::in | std::fstream::out | std::fstream::app);
                std::string ID = std::to_string(pSelectedItem->m_SteamID3) + "\n";
                FriendsFile.write(ID.c_str(), ID.size());
                FriendsFile.close();
            }

        }
        g_CursorPosition.second += nHeaderH + 1;


        {
            bool bFriendsRemove = false;
            interfaces->surface->setTextPosition(g_CursorPosition.first, g_CursorPosition.second);
            ClickableText("Remove Player From Friends List", &bFriendsRemove);

            if (bFriendsRemove && !pSelectedItem->m_bIsBot)
            {
                std::vector<CSteamID> friends;
                VCON("Removing Player : %s (SteamID3: %s) From FriendsList\n", pSelectedItem->m_strName.c_str(), pSelectedItem->m_strIDString.c_str());
                std::fstream playerFile;
                char path[MAX_PATH];
                HRESULT hr = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL,
                    SHGFP_TYPE_CURRENT, path);
                strcat(path, "\\Harpoon\\Friends.txt");
                playerFile.open(path, std::fstream::in | std::fstream::out);
                int32_t id = 0;
                std::string friendsstr;
                while (getline(playerFile, friendsstr)) {
                    try {
                        id = std::stoi(friendsstr);
                        friendsstr.erase(std::remove(friendsstr.begin(), friendsstr.end(), '\n'),
                            friendsstr.end());
                        //Debug::QuickPrint(("Friend ID Is Parsed = " + std::to_string(id)).c_str());
                        CSteamID steamid(id, k_EUniversePublic, k_EAccountTypeIndividual);

                        if (steamid.GetAccountID() == pSelectedItem->m_SteamID.GetAccountID())
                            continue;

                        //steamid.SetFromString(friendsstr.c_str(), k_EUniversePublic);
                        friends.push_back(steamid);
                    }
                    catch (std::exception& e) {
                        Debug::QuickPrint(friendsstr.c_str());
                    }
                }
                playerFile.clear();
                playerFile.close();
                playerFile.open(path, std::fstream::out | std::fstream::trunc);


                for (CSteamID Account : friends) {
                    std::string id = std::to_string(Account.GetAccountID()) + "\n";
                    playerFile.write(id.c_str(), id.size());
                }
                playerFile.close();
                friends.clear();
            }

        }
        g_CursorPosition.second += nHeaderH + 1;

        {
            bool bRecordAdd = false;
            interfaces->surface->setTextPosition(g_CursorPosition.first, g_CursorPosition.second);
            ClickableText("Add Player To Records", &bRecordAdd);

            if (bRecordAdd && !pSelectedItem->m_bIsBot)
            {
                VCON("Adding Player : %s (SteamID3: %s) To Records\n", pSelectedItem->m_strName.c_str(), pSelectedItem->m_strIDString.c_str());
                WriteItemToRecords(pSelectedItem);
            }
        }
        g_CursorPosition.second += nHeaderH + 1;

        {
            bool bRecordDelete = false;
            interfaces->surface->setTextPosition(g_CursorPosition.first, g_CursorPosition.second);
            ClickableText("Remove Player From Records", &bRecordDelete);

            if (bRecordDelete && !pSelectedItem->m_bIsBot)
            {
                VCON("Removing Player : %s (SteamID3: %s) From Records\n", pSelectedItem->m_strName.c_str(), pSelectedItem->m_strIDString.c_str());
                DeleteRecord(pSelectedItem->m_SteamID3);
            }
        }
        g_CursorPosition.second += nHeaderH + 1;

        {
            bool bAddCheater = false;
            interfaces->surface->setTextPosition(g_CursorPosition.first, g_CursorPosition.second);
            ClickableText("Mark Player As Cheater", &bAddCheater);

            if (bAddCheater && !pSelectedItem->m_bIsBot)
            {
                VCON("Marking Player : %s (SteamID3: %s) As Cheater\n", pSelectedItem->m_strName.c_str(), pSelectedItem->m_strIDString.c_str());
                pSelectedItem->m_bIsCheater = !pSelectedItem->m_bIsCheater;
            }
        }
        g_CursorPosition.second += nHeaderH + 1;

        {
            bool bInitiateSpam = false;
            interfaces->surface->setTextPosition(g_CursorPosition.first, g_CursorPosition.second);
            ClickableText("Invite Spam", &bInitiateSpam);

            if (bInitiateSpam && !pSelectedItem->m_bIsBot)
            {
                if (pSelectedItem->m_bIsSpamming)
                {
                    VCON("No Longer Invite Spamming Player : %s (SteamID3: %s)\n", pSelectedItem->m_strName.c_str(), pSelectedItem->m_strIDString.c_str());

                    int i = 0;
                    for (uint32_t& ID : OurSteamWorks::m_vIDs)
                    {

                        if (ID == pSelectedItem->m_SteamID3)
                        {

                            OurSteamWorks::m_vIDs.erase(OurSteamWorks::m_vIDs.begin() + i);//(OurSteamWorks::m_vIDs.at(i))
                            break;
                        }
                        i++;
                    }

                }
                else {
                    VCON("Invite Spamming Player : %s (SteamID3: %s)\n", pSelectedItem->m_strName.c_str(), pSelectedItem->m_strIDString.c_str());
                    OurSteamWorks::m_vIDs.push_back(pSelectedItem->m_SteamID3);
                }

            }
        }
    }
}




void PlayerList::PlayerListContents::Draw(bool Focus, Pendelum::color* col /*= nullptr*/, bool colorOveride /* = 0*/) {

    static bool bRenderList{ false };
    static bool bRenderRecords{ false };

    // Do Base Calculations
    ScreenVec vPosition = Master_Window->getScreenInfo();
    int nWindowWidth = vPosition.width;
    int nWindowHeight = vPosition.height;
    if (vPosition.isPad) {
        int padw = (int)((float)ScreenSizes.first * (float)((float)vPosition.padx / 100.f));
        int padh = (int)((float)ScreenSizes.second * (float)((float)vPosition.pady / 100.f));
        nWindowWidth = (ScreenSizes.first - padw) - padw;
        nWindowHeight = (ScreenSizes.second - padh) - padh;

        vPosition.width = nWindowWidth;
        vPosition.height = nWindowHeight;

    }

    // Draw Header
    auto [nPlayerListTextW, nPlayerListTextWh] = interfaces->surface->getTextSize(5, L"PlayerList");

    int nHeaderHeight = nPlayerListTextWh;
    nHeaderHeight += 4;

    int nHeaderWidth = vPosition.width - 1;
    ScreenVec vHeader;
    vHeader.x = vPosition.x;
    vHeader.y = vPosition.y;
    vHeader.height = nHeaderHeight;
    vHeader.width = nHeaderWidth;
    interfaces->surface->setDrawColor(60, 60, 60, 255);
    interfaces->surface->drawFilledRect(vHeader.x, vHeader.y, vHeader.x + vHeader.width, vHeader.y + nHeaderHeight);
    interfaces->surface->setDrawColor(190, 190, 190, 255);


    int nMiddlePoint = nHeaderWidth / 2;
    {
        g_CursorPosition.first = vHeader.x + ((nMiddlePoint / 2) - (nPlayerListTextW / 2));
        g_CursorPosition.second = vHeader.y + 2;
        interfaces->surface->setTextPosition(g_CursorPosition.first, g_CursorPosition.second);
        bool bList = bRenderList;
        ClickableText("PlayerList", &bList, true);
        if (bList) {
            bRenderList = true;
            bRenderRecords = false;
        }
    }
    {
        auto [nPlayerRecordsW, nPlayerRecordsH] = interfaces->surface->getTextSize(5, L"PlayerRecords");
        g_CursorPosition.first = vHeader.x + (((nMiddlePoint / 2) + nMiddlePoint) - (nPlayerRecordsW / 2));
        g_CursorPosition.second = vHeader.y + 2;
        interfaces->surface->setTextPosition(g_CursorPosition.first, g_CursorPosition.second);
        bool bRecords = bRenderRecords;
        ClickableText("PlayerRecords", &bRecords, true);
        if (bRecords) {
            bRenderList = false;
            bRenderRecords = true;
        }

        g_CursorPosition.second = vHeader.y + 2;
       


    }


    RenderBoxOutline(vHeader);
    vPosition.y += nHeaderHeight + 3;
    vPosition.height -= nHeaderHeight + 3;

   

    if (bRenderList)
        DrawPlayerList(vPosition);
    else if (bRenderRecords)
        DrawPlayerRecords(vPosition);

}



bool PlayerList::PlayerListContents::QuerySteamAPI(PlayerItem* pItem, std::string Request) {
    // GET https://api.steampowered.com/ISteamUser/GetFriendList/v1/
    HTTPRequestHandle pHandle = g_pSteamHTTP->CreateHTTPRequest(EHTTPMethod::k_EHTTPMethodGET, "https://api.steampowered.com/ISteamUser/GetFriendList/v1/");
    
    
}


bool PlayerList::PlayerListContents::RequestFriendsList(PlayerItem* pItem)
{
    char pRequest[4096];
    uint64_t uidraw = pItem->m_SteamID.ConvertToUint64();
    snprintf(pRequest, 4096, "https://api.steampowered.com/ISteamUser/GetFriendList/v1/?key=%s&steamid=%lld&relationship=friend", STEAM_API_KEY, uidraw);

    
}



void PlayerList::UpdatePlayerList() {
    for (int i = 0; i < interfaces->engine->getMaxClients(); i++) {
        Entity* pPlayer = interfaces->entityList->getEntity(i);
        if (i > 64 || i < 0)
            continue;

        PlayerItem* pItem = &(g_arrPlayerData[i]);

        if (!pItem)
            std::exception("No pItem");

        pItem->m_nItemArrayIndex = i;

        if (!pPlayer || !pPlayer->isPlayer()) {
            if (pItem->m_bHasItem) {
                pItem->m_bHasItem = false;
                PlayerItem NewItem;
                *pItem = NewItem;
            }
            continue;
        }

        if (pPlayer->isBot())
            pItem->m_bIsBot = true;


        PlayerInfo PlayerInfo;

        if (!interfaces->engine->getPlayerInfo(i, PlayerInfo))
            continue;


        if (PlayerInfo.friendsId != pItem->m_SteamID3) {
            PlayerItem NewItem;
            NewItem.m_bHasItem = true;
            NewItem.m_strName = pPlayer->getPlayerName(false);
            NewItem.m_SteamID3 = PlayerInfo.friendsId;
 
            NewItem.m_nUserID = pPlayer->getUserId();
            if (!pItem->m_bIsBot) {
                NewItem.m_SteamID.Set(PlayerInfo.friendsId, k_EUniversePublic, k_EAccountTypeIndividual);
                steamFriends->SetPlayedWith(NewItem.m_SteamID);
                steamFriends->RequestUserInformation(NewItem.m_SteamID, false);
                NewItem.m_Avatar.SetAvatarSteamID(NewItem.m_SteamID);

                *pItem = NewItem;
                VCON("Added %s [U:1:%d] to the Player List\n", NewItem.m_strName.c_str(), NewItem.m_SteamID3);

                static char SteamIDBuffer[4096];
                snprintf(SteamIDBuffer, 4096, "[U:1:%d]", pItem->m_SteamID3);
                pItem->m_strIDString = SteamIDBuffer;
            }
            else {
                *pItem = NewItem;
                pItem->m_strIDString = "N/A";
                pItem->m_strSteamProfileName = "N/A";
            }
        }

        if (!pItem->m_bIsBot) {
            pItem->m_strSteamProfileName = steamFriends->GetFriendPersonaName(pItem->m_SteamID);
        }
        else {
            pItem->m_strSteamProfileName = "N/A";
        }
    }

}

#include <chrono>
#include <ctime>    


#define ReadValue(VAL, TYPE) try { VAL = recordFile[StrID][#VAL].get<TYPE>(); } catch(std::exception &e){Debug::QuickWarning(#VAL); Debug::QuickWarning(e.what());}
#define WriteValue(VAL, TYPE) try { recordFile[StrID][#VAL] = VAL; } catch(std::exception &e){Debug::QuickWarning(#VAL); Debug::QuickWarning(e.what());}

#define VALUE WriteValue

bool PlayerList::PlayerListContents::WriteItemToRecords(PlayerItem* pItem)
{
    try {
        char path[MAX_PATH];
        HRESULT hr = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL,
            SHGFP_TYPE_CURRENT, path);
        strcat(path, "\\Harpoon\\Records.dat");

        std::fstream RecordsFile(path, std::fstream::in | std::fstream::out | std::fstream::app);

        std::string StrID = std::to_string(pItem->m_SteamID3);

        json recordFile;
        std::string str((std::istreambuf_iterator<char>(RecordsFile)),
            std::istreambuf_iterator<char>());
        try {
            recordFile = json::parse(str);
        }
        catch (std::exception& e) {};

        int nRecordCount = 0;
        std::vector<uint32_t> vAccounts;
        try {
            nRecordCount = recordFile["RecordCount"].get<int>();
            vAccounts = recordFile["AccountList"].get<std::vector<uint32_t>>();
        }
        catch (std::exception& e) {}

        bool bFound = false;
        for (uint32_t ID : vAccounts)
        {
            if (ID == pItem->m_SteamID3)
            {
                bFound = true;
                break;
            }

        }

        if (!bFound)
        {
            nRecordCount++;
            recordFile["RecordCount"] = nRecordCount;
            vAccounts.push_back(pItem->m_SteamID3);
            recordFile["AccountList"] = vAccounts;
        }

        auto time = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t(time);
        pItem->m_strLastSeeDate = std::ctime(&end_time);


        VALUE(pItem->m_strLastSeeDate);
        VALUE(pItem->m_strName);
        VALUE(pItem->m_strSteamProfileName);
        VALUE(pItem->m_SteamID3);
        VALUE(pItem->m_SteamID.ConvertToUint64());
        VALUE(pItem->m_bCheater);
        VALUE(pItem->m_bHarpoonUser);


        RecordsFile.close();
        RecordsFile.open(path, std::fstream::out | std::fstream::trunc);
        std::string seralizedStr = recordFile.dump(4);
        RecordsFile.write(seralizedStr.c_str(), seralizedStr.size());
        RecordsFile.close();
    }
    catch (std::exception& e)
    {
        Debug::QuickWarning(e.what());
    }
}

#undef VALUE
#define VALUE ReadValue

bool PlayerList::PlayerListContents::CheckForRecord(PlayerItem& Item, uint32_t ID)
{

    try {
        char path[MAX_PATH];
        HRESULT hr = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL,
            SHGFP_TYPE_CURRENT, path);
        strcat(path, "\\Harpoon\\Records.dat");

        std::fstream RecordsFile(path, std::fstream::in | std::fstream::out | std::fstream::app);

        std::string StrID = std::to_string(ID);

        json recordFile;
        std::string str((std::istreambuf_iterator<char>(RecordsFile)),
            std::istreambuf_iterator<char>());
        try {
            recordFile = json::parse(str);
        }
        catch (std::exception& e) {};

        int nRecordCount = 0;
        std::vector<uint32_t> vAccounts;
        try {
            nRecordCount = recordFile["RecordCount"].get<int>();
            vAccounts = recordFile["AccountList"].get<std::vector<uint32_t>>();
        }
        catch (std::exception& e) {}

        bool bFound = false;
        for (uint32_t ID : vAccounts)
        {
            if (ID == ID)
            {
                bFound = true;
                break;
            }
        }

        if (!bFound)
            return false;

        auto time = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t(time);
        PlayerItem* pItem = &Item;

        VALUE(pItem->m_strLastSeeDate, std::string);
        VALUE(pItem->m_strName, std::string);
        VALUE(pItem->m_strSteamProfileName, std::string);
        VALUE(pItem->m_SteamID3, uint32_t);
        pItem->m_SteamID.Set(pItem->m_SteamID3, k_EUniversePublic, k_EAccountTypeIndividual);
        VALUE(pItem->m_bCheater, bool);
        VALUE(pItem->m_bHarpoonUser, bool);


        recordFile[StrID]["pItem->m_strLastSeeDate"] = std::ctime(&end_time);


        RecordsFile.close();
        RecordsFile.open(path, std::fstream::out | std::fstream::trunc);
        std::string seralizedStr = recordFile.dump(4);
        RecordsFile.write(seralizedStr.c_str(), seralizedStr.size());
        RecordsFile.close();
        return true;
    }
    catch (std::exception& e)
    {
            Debug::QuickWarning(e.what());
    }
}

void PlayerList::PlayerListContents::DeleteRecord(uint32_t ID)
{

    try {
        char path[MAX_PATH];
        HRESULT hr = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL,
            SHGFP_TYPE_CURRENT, path);
        strcat(path, "\\Harpoon\\Records.dat");

        std::fstream RecordsFile(path, std::fstream::in | std::fstream::out | std::fstream::app);

        std::string StrID = std::to_string(ID);

        json recordFile;
        std::string str((std::istreambuf_iterator<char>(RecordsFile)),
            std::istreambuf_iterator<char>());
        try {
            recordFile = json::parse(str);
        }
        catch (std::exception& e) {};

        int nRecordCount = 0;
        std::vector<uint32_t> vAccounts;
        try {
            nRecordCount = recordFile["RecordCount"].get<int>();
            vAccounts = recordFile["AccountList"].get<std::vector<uint32_t>>();
        }
        catch (std::exception& e) {}

        bool bFound = false;
        int nIndex = 0;
        for (uint32_t ID : vAccounts)
        {
            if (ID == ID)
            {
                bFound = true;
                break;
            }
            nIndex++;
        }

        if (!bFound)
            return;


        vAccounts.erase(vAccounts.begin() + nIndex);
        nRecordCount--;

        recordFile["AccountList"] = vAccounts;
        recordFile["RecordCount"] = nRecordCount;
        recordFile.erase(StrID);
        RecordsFile.close();
        RecordsFile.open(path, std::fstream::out | std::fstream::trunc);
        std::string seralizedStr = recordFile.dump(4);
        RecordsFile.write(seralizedStr.c_str(), seralizedStr.size());
        RecordsFile.close();
    }
    catch (std::exception& e)
    {
        Debug::QuickWarning(e.what());
    }
}

void PlayerList::PlayerListContents::ReloadRecords()
{

    try {
        char path[MAX_PATH];
        HRESULT hr = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL,
            SHGFP_TYPE_CURRENT, path);
        strcat(path, "\\Harpoon\\Records.dat");

        std::fstream RecordsFile(path, std::fstream::in | std::fstream::out | std::fstream::app);

        std::string StrID = std::to_string(ID);

        json recordFile;
        std::string str((std::istreambuf_iterator<char>(RecordsFile)),
            std::istreambuf_iterator<char>());
        try {
            recordFile = json::parse(str);
        }
        catch (std::exception& e) {};

        int nRecordCount = 0;
        std::vector<uint32_t> vAccounts;
        try {
            nRecordCount = recordFile["RecordCount"].get<int>();
            vAccounts = recordFile["AccountList"].get<std::vector<uint32_t>>();
        }
        catch (std::exception& e) {}
      
        m_vRecords.clear();
        int i = 0;
        for (uint32_t ID : vAccounts)
        {
            std::string StrID = std::to_string(ID);
            PlayerItem Item;
            PlayerItem* pItem = &Item;
            VALUE(pItem->m_strLastSeeDate, std::string);
            VALUE(pItem->m_strName, std::string);
            VALUE(pItem->m_strSteamProfileName, std::string);
            VALUE(pItem->m_SteamID3, uint32_t);
            pItem->m_strIDString = StrID;
            pItem->m_SteamID.Set(pItem->m_SteamID3, k_EUniversePublic, k_EAccountTypeIndividual);
            VALUE(pItem->m_bCheater, bool);
            VALUE(pItem->m_bHarpoonUser, bool);
            pItem->m_nItemArrayIndex = i;
            pItem->m_bHasItem = true;
            i++;
            m_vRecords.push_back(Item);
        }
        RecordsFile.close();

    }
    catch (std::exception& e)
    {
        Debug::QuickWarning(e.what());
    }
}




