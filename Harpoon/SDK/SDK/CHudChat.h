#pragma once
#include <cstdarg>
#include <stdio.h>
#include <cstdint>
#include "../OsirisSDK/VirtualMethod.h"

#if 0
template<class T>
static T* FindHudElement(const char* name)
{
    static auto pThis = *reinterpret_cast<unsigned long**>(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "B9 ? ? ? ? E8 ? ? ? ? 83 7D F8 ?") + 1);

    static auto find_hud_element = reinterpret_cast<unsigned long(__thiscall*)(void*, const char*)>(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28"));
    return (T*)find_hud_element(pThis, name);
}
#endif

#define CHATLINE_NUM_FLASHES 8.0f
#define CHATLINE_FLASH_TIME 5.0f
#define CHATLINE_FADE_TIME 1.0f

#define CHAT_HISTORY_FADE_TIME 0.25f
#define CHAT_HISTORY_IDLE_TIME 15.0f
#define CHAT_HISTORY_IDLE_FADE_TIME 2.5f
#define CHAT_HISTORY_ALPHA 127

class Color {
public:
	Color()
	{
		*((int*)this) = 0;
	}
	Color(int _r, int _g, int _b)
	{
		SetColor(_r, _g, _b, 0);
	}
	Color(int _r, int _g, int _b, int _a)
	{
		SetColor(_r, _g, _b, _a);
	}

	// set the color
	// r - red component (0-255)
	// g - green component (0-255)
	// b - blue component (0-255)
	// a - alpha component, controls transparency (0 - transparent, 255 - opaque);
	void SetColor(int _r, int _g, int _b, int _a = 0)
	{
		_color[0] = (unsigned char)_r;
		_color[1] = (unsigned char)_g;
		_color[2] = (unsigned char)_b;
		_color[3] = (unsigned char)_a;
	}

	void GetColor(int& _r, int& _g, int& _b, int& _a) const
	{
		_r = _color[0];
		_g = _color[1];
		_b = _color[2];
		_a = _color[3];
	}

	void SetRawColor(int color32)
	{
		*((int*)this) = color32;
	}

	int GetRawColor() const
	{
		return *((int*)this);
	}

	inline int r() const { return _color[0]; }
	inline int g() const { return _color[1]; }
	inline int b() const { return _color[2]; }
	inline int a() const { return _color[3]; }

	unsigned char& operator[](int index)
	{
		return _color[index];
	}

	const unsigned char& operator[](int index) const
	{
		return _color[index];
	}

	bool operator == (const Color& rhs) const
	{
		return (*((int*)this) == *((int*)&rhs));
	}

	bool operator != (const Color& rhs) const
	{
		return !(operator==(rhs));
	}

	Color& operator=(const Color& rhs)
	{
		SetRawColor(rhs.GetRawColor());
		return *this;
	}



private:
    unsigned char _color[4];
};

#if 0
class CBaseHudChat : public CHudElement, public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE(CBaseHudChat, vgui::EditablePanel);
public:
	DECLARE_MULTIPLY_INHERITED();

	enum
	{
		CHAT_INTERFACE_LINES = 6,
		MAX_CHARS_PER_LINE = 128
	};

	CBaseHudChat(const char* pElementName);

	virtual void	CreateChatInputLine(void);
	virtual void	CreateChatLines(void);

	virtual void	Init(void);

	void			LevelInit(const char* newmap);
	void			LevelShutdown(void);

	void			MsgFunc_TextMsg(const char* pszName, int iSize, void* pbuf);

	virtual void	Printf(int iFilter, PRINTF_FORMAT_STRING const char* fmt, ...);
	virtual void	ChatPrintf(int iPlayerIndex, int iFilter, PRINTF_FORMAT_STRING const char* fmt, ...);

	virtual void	StartMessageMode(int iMessageModeType);
	virtual void	StopMessageMode(void);
	void			Send(void);

	MESSAGE_FUNC(OnChatEntrySend, "ChatEntrySend");
	MESSAGE_FUNC(OnChatEntryStopMessageMode, "ChatEntryStopMessageMode");

	virtual void	ApplySchemeSettings(vgui::IScheme* pScheme);
	virtual void	Paint(void);
	virtual void	OnTick(void);
	virtual void	Reset();
#ifdef _XBOX
	virtual bool	ShouldDraw();
#endif
	vgui::Panel* GetInputPanel(void);

	static int		m_nLineCounter;

	virtual int		GetChatInputOffset(void);

	// IGameEventListener interface:
	virtual void FireGameEvent(IGameEvent* event);

	CHudChatHistory* GetChatHistory();

	void					FadeChatHistory();
	float					m_flHistoryFadeTime;
	float					m_flHistoryIdleTime;

	virtual void			MsgFunc_SayText(bf_read& msg);
	virtual void			MsgFunc_SayText2(bf_read& msg);
	virtual void			MsgFunc_TextMsg(bf_read& msg);
	virtual void			MsgFunc_VoiceSubtitle(bf_read& msg);


	CBaseHudChatInputLine* GetChatInput(void) { return m_pChatInput; }
	CHudChatFilterPanel* GetChatFilterPanel(void);

	virtual int				GetFilterFlags(void) { return m_iFilterFlags; }
	void					SetFilterFlag(int iFilter);

	//-----------------------------------------------------------------------------
	virtual Color	GetDefaultTextColor(void);
	virtual Color	GetTextColorForClient(TextColor colorNum, int clientIndex);
	virtual Color	GetClientColor(int clientIndex);

	virtual int		GetFilterForString(const char* pString);

	virtual const char* GetDisplayedSubtitlePlayerName(int clientIndex);

	bool			IsVoiceSubtitle(void) { return m_bEnteringVoice; }
	void			SetVoiceSubtitleState(bool bState) { m_bEnteringVoice = bState; }
	int				GetMessageMode(void) { return m_nMessageMode; }

	void			SetCustomColor(Color colNew) { m_ColorCustom = colNew; }
	void			SetCustomColor(const char* pszColorName);

protected:
	CBaseHudChatLine* FindUnusedChatLine(void);

	CBaseHudChatInputLine* m_pChatInput;
	CBaseHudChatLine* m_ChatLine;
	int					m_iFontHeight;

	CHudChatHistory* m_pChatHistory;

	CHudChatFilterButton* m_pFiltersButton;
	CHudChatFilterPanel* m_pFilterPanel;

	Color			m_ColorCustom;

private:
	void			Clear(void);

	int				ComputeBreakChar(int width, const char* text, int textlen);

	int				m_nMessageMode;

	int				m_nVisibleHeight;

	vgui::HFont		m_hChatFont;

	int				m_iFilterFlags;
	bool			m_bEnteringVoice;

};
#endif


#define WhiteChat "\x01"
#define RedChat "\x02"
#define PurpleChat "\x03"
#define GreenChat "\x04"
#define RedChat "\x07"
#define GreyChat "\x08"
#define GoldChat "\x09"
#define LightGreyChat "\x0A"
#define BlueChat "\x0C"
#define GoldChat "\x10"


typedef enum
{
	VOTE_FAILED_GENERIC = 0,
	VOTE_FAILED_TRANSITIONING_PLAYERS,
	VOTE_FAILED_RATE_EXCEEDED,
	VOTE_FAILED_YES_MUST_EXCEED_NO,
	VOTE_FAILED_QUORUM_FAILURE,
	VOTE_FAILED_ISSUE_DISABLED,
	VOTE_FAILED_MAP_NOT_FOUND,
	VOTE_FAILED_MAP_NAME_REQUIRED,
	VOTE_FAILED_FAILED_RECENTLY,
	VOTE_FAILED_FAILED_RECENT_KICK,
	VOTE_FAILED_FAILED_RECENT_CHANGEMAP,
	VOTE_FAILED_FAILED_RECENT_SWAPTEAMS,
	VOTE_FAILED_FAILED_RECENT_SCRAMBLETEAMS,
	VOTE_FAILED_FAILED_RECENT_RESTART,
	VOTE_FAILED_TEAM_CANT_CALL,
	VOTE_FAILED_WAITINGFORPLAYERS,
	VOTE_FAILED_PLAYERNOTFOUND,
	VOTE_FAILED_CANNOT_KICK_ADMIN,
	VOTE_FAILED_SCRAMBLE_IN_PROGRESS,
	VOTE_FAILED_SWAP_IN_PROGRESS,
	VOTE_FAILED_SPECTATOR,
	VOTE_FAILED_DISABLED,
	VOTE_FAILED_NEXTLEVEL_SET,
	VOTE_FAILED_REMATCH,
	VOTE_FAILED_TOO_EARLY_SURRENDER,
	VOTE_FAILED_CONTINUE,
	VOTE_FAILED_MATCH_PAUSED,
	VOTE_FAILED_MATCH_NOT_PAUSED,
	VOTE_FAILED_NOT_IN_WARMUP,
	VOTE_FAILED_NOT_10_PLAYERS,
	VOTE_FAILED_TIMEOUT_ACTIVE,
	VOTE_FAILED_TIMEOUT_INACTIVE,
	VOTE_FAILED_TIMEOUT_EXHAUSTED,
	VOTE_FAILED_CANT_ROUND_END,
	VOTE_FAILED_MAX,
} vote_create_failed_t;

#define MAX_VOTE_DETAILS_LENGTH 64
#define INVALID_ISSUE -1
#define MAX_VOTE_OPTIONS 5

enum
{
	VOTEISSUE_UNDEFINED = -1,
	VOTEISSUE_KICK,
	VOTEISSUE_CHANGELEVEL,
	VOTEISSUE_NEXTLEVEL,
	VOTEISSUE_SWAPTEAMS,
	VOTEISSUE_SCRAMBLE,
	VOTEISSUE_RESTARTGAME,
	VOTEISSUE_SURRENDER,
	VOTEISSUE_REMATCH,
	VOTEISSUE_CONTINUE,
	VOTEISSUE_PAUSEMATCH,
	VOTEISSUE_UNPAUSEMATCH,
	VOTEISSUE_LOADBACKUP,
	VOTEISSUE_ENDWARMUP,
	VOTEISSUE_STARTTIMEOUT,
	VOTEISSUE_ENDTIMEOUT,
	VOTEISSUE_READYFORMATCH,
	VOTEISSUE_NOTREADYFORMATCH,
	VOTEISSUE_LAST
};

typedef void(__cdecl* ChatPrintFFn)(void*, int, int, const char*, ...);

class CHudChat
 {
    public:
    	enum ChatFilters
    	{
    		CHAT_FILTER_NONE = 0,
    		CHAT_FILTER_JOINLEAVE = 0x000001,
    		CHAT_FILTER_NAMECHANGE = 0x000002,
    		CHAT_FILTER_PUBLICCHAT = 0x000004,
    		CHAT_FILTER_SERVERMSG = 0x000008,
    		CHAT_FILTER_TEAMCHANGE = 0x000010,
    		//=============================================================================
    		// HPE_BEGIN:
    		// [tj]Added a filter for 
            
             // ment announce
    		//=============================================================================
     
    		CHAT_FILTER_ACHIEVEMENT = 0x000020,
     
    		//=============================================================================
    		// HPE_END
    		//=============================================================================
    	};

        enum TextColor
        {
            COLOR_NORMAL = 1,
            COLOR_USEOLDCOLORS = 2,
            COLOR_PLAYERNAME = 3,
            COLOR_LOCATION = 4,
            COLOR_ACHIEVEMENT = 5,
            COLOR_CUSTOM = 6,		// Will use the most recently SetCustomColor()
            COLOR_HEXCODE = 7,		// Reads the color from the next six characters
            COLOR_HEXCODE_ALPHA = 8,// Reads the color and alpha from the next eight characters
            COLOR_MAX
        };




		virtual void sub_0() = 0;
		virtual void sub_1() = 0;
		virtual void sub_2() = 0;
		virtual void sub_3() = 0;
		virtual void sub_4() = 0;
		virtual void sub_5() = 0;
		virtual void sub_6() = 0;
		virtual void sub_7() = 0;
		virtual void sub_8() = 0;
		virtual void sub_9() = 0;
		virtual void sub_10() = 0;
		virtual void sub_11() = 0;
		virtual void sub_12() = 0;
		virtual void sub_13() = 0;
		virtual void sub_14() = 0;
		virtual void sub_15() = 0;
		virtual void sub_16() = 0;
		virtual void sub_17() = 0;
		virtual void sub_18() = 0;
		virtual void sub_19() = 0;
		virtual void sub_20() = 0;
		virtual void sub_21() = 0;
		virtual void sub_22() = 0;
		virtual void sub_23() = 0;
		virtual void sub_24() = 0;
		virtual void sub_25() = 0;
		virtual void sub_26() = 0;
		virtual void sub_27() = 0;
		virtual void __cdecl ChatPrintfVar(void* edx, int iPlayerIndex, int iFilter, const char* fmt, ...) = 0;
		virtual void sub_28() = 0;
		virtual void sub_29() = 0;
		virtual void sub_30() = 0;
		virtual void sub_31() = 0;
		virtual void sub_32() = 0;
		virtual void sub_33() = 0;
		virtual void sub_34() = 0;
		virtual void sub_35() = 0;
		virtual void sub_36() = 0;
		virtual void sub_37() = 0;
		virtual void sub_38() = 0;
		virtual void sub_39() = 0;
		virtual void sub_40() = 0;
		virtual void sub_41() = 0;
		virtual void sub_42() = 0;
		virtual void sub_43() = 0;

		// dd offset ??_R4CHudChatHistory@@6B@ ; const CHudChatHistory::`RTTI Complete Object Locator'

        struct TextRange
        {
            TextRange() { preserveAlpha = false; }
            int start;
            int end;
            Color color;
            bool preserveAlpha;
        };


        __forceinline void ChatPrintf(int iPlayerIndex, int iFilter, const char* fmt, ...)
        {
            va_list args;
            va_start(args, fmt);
			char msg[1024];
            vsprintf(msg, fmt, args);
            typedef void(__cdecl* printffn)(void*, int, int, const char*, ...);
            ((printffn)((*(unsigned long**)this)[27]))(this, iPlayerIndex, iFilter, fmt);
            va_end(args);
        }
		__forceinline void ChatPrintfW(int iPlayerIndex, int iFilter, const wchar_t* fmt)
        {
            typedef void(__cdecl* wprintffn)(void*, int, int, const wchar_t*);
            ((wprintffn)((*(unsigned long**)this)[28]))(this, iPlayerIndex, iFilter, fmt);
        }
};