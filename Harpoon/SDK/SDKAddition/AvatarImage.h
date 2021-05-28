#pragma once
#include "../../Resource/SteamWorks/steam/steam_api.h"
#include "../SDK/IPanoramaUIEngine.h"

typedef unsigned long HTexture;
class CGameUiAvatarImage {
public:
	CGameUiAvatarImage(void);

	// Call this to set the steam ID associated with the avatar
	bool SetAvatarSteamID(CSteamID steamIDUser);
	void ClearAvatarSteamID(void);

	// Call to Paint the image
	// Image will draw within the current panel context at the specified position
	virtual void Paint(void);

	// Set the position of the image
	virtual void SetPos(int x, int y)
	{
		m_nX = x;
		m_nY = y;
	}

	// Gets the size of the content
	virtual void GetContentSize(int& wide, int& tall)
	{
		wide = m_nWide;
		tall = m_nTall;
	}

	// Get the size the image will actually draw in (usually defaults to the content size)
	virtual void GetSize(int& wide, int& tall)
	{
		GetContentSize(wide, tall);
	}

	int GetWidth() {
		return m_nWide;
	}

	int GetHeight() {
		return m_nTall;
	}



	// Sets the size of the image
	virtual void SetSize(int wide, int tall)
	{
		m_nWide = wide;
		m_nTall = tall;
	}

	// Set the draw color 
#if 0
	virtual void SetColor(Color col)
	{
		m_Color = col;
	}
#endif

	virtual void SetRotation(int)
	{
		(void)0;		// Not implemented
	}

	virtual bool Evict() { return false; }

	virtual int GetNumFrames() { return 0; }
	virtual void SetFrame(int nFrame) {}

	virtual HTexture GetID() { return m_iTextureID; }

	bool	IsValid(void) { return m_bValid; }

	float	GetFetchedTime() const { return m_flFetchedTime; }

protected:
	void InitFromRGBA(const char* rgba, int width, int height);

private:
	int m_iTextureID;
	int m_nX, m_nY, m_nWide, m_nTall;
	bool m_bValid;
	float m_flFetchedTime;
};