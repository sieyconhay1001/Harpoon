#include "AvatarImage.h"

#include "../../Interfaces.h"
#include "../../Hacks/SteamWorks/SteamWorks.h"
#include "../OsirisSDK/GlobalVars.h"
#include "../OsirisSDK/Surface.h"
CGameUiAvatarImage::CGameUiAvatarImage(void)
{
	m_bValid = false;
	m_flFetchedTime = 0.0f;
	m_iTextureID = (-1);
}

void CGameUiAvatarImage::ClearAvatarSteamID(void)
{
	m_bValid = false;
	m_flFetchedTime = 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CGameUiAvatarImage::SetAvatarSteamID(CSteamID steamIDUser)
{
	ClearAvatarSteamID();

	if (steamFriends && g_pSteamUtils)
	{
		int iAvatar = steamFriends->GetLargeFriendAvatar(steamIDUser);

		/*
		// See if it's in our list already
		*/

		uint32 wide, tall;
		if (g_pSteamUtils->GetImageSize(iAvatar, &wide, &tall))
		{
			bool bUseSteamImage = true;
			if (wide == 0 || tall == 0)
			{
				// attempt to handle rare data integrity issue, avatar got lost
				bUseSteamImage = false;
				// mock up solid white as 64x64
				wide = tall = 64;
			}

			int cubImage = wide * tall * 4;
			byte* rgubDest = (byte*)_alloca(cubImage);
			if (bUseSteamImage)
			{
				g_pSteamUtils->GetImageRGBA(iAvatar, rgubDest, cubImage);
			}
			else
			{
				// solid white, avoids any issue with where the alpha channel is
				memset(rgubDest, 0xFF, cubImage);
			}
			InitFromRGBA((const char*)rgubDest, wide, tall);

			m_flFetchedTime = memory->globalVars->realtime;
		}
	}

	return m_bValid;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CGameUiAvatarImage::InitFromRGBA(const char* rgba, int width, int height)
{
	if (m_iTextureID == -1)
	{
		m_iTextureID = interfaces->surface->CreateNewTextureID(true);
	}

	interfaces->surface->DrawSetTextureRGBA(m_iTextureID, (const unsigned char*)rgba, width, height);

	int screenWide, screenTall;
	interfaces->surface->getScreenSize(screenWide, screenTall);
	m_nWide = width/*width * (((float)screenWide) / 640.0f)*/;
	m_nTall = height;/*height * (((float)screenTall) / 480.0f)*/
	

	m_bValid = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CGameUiAvatarImage::Paint(void)
{
	if (m_bValid)
	{
		interfaces->surface->setDrawColor(255, 255, 255,255);
		interfaces->surface->DrawSetTexture(m_iTextureID);
		interfaces->surface->DrawTexturedRect(m_nX, m_nY, m_nX + m_nWide, m_nY + m_nTall);
	}
}
