#pragma once
#include "Renderer.h"
#include <Windows.h>
#include <d3d9.h>
#include <vector>
#include <array>
#include <string>
#include <d3d9helper.h>
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\d3dx9.h>
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\lib\\x86\\d3dx9.lib")
#include <cassert>
#include <memory>


#define DEFAULT_BUFFERSIZE 2048



/*

If you came across this code in something, or intend to use this code, don't. I don't exactly know what im doing, just going by example
and the microsoft docs.

This is just meant to be a quick implementation for me to learn and have something i'm familiar with.


*/



#include "../Hacks/OTHER/Debug.h"




class D3D9Initializer : public RendererUtils::RendererInitializer {
public:
	void SetDevice(IDirect3DDevice9* device) { Device = device; Device->AddRef(); }
	IDirect3DDevice9* GetDevice() { return Device; };

	void SetScreenFromDevice() {
		D3DDEVICE_CREATION_PARAMETERS cparams;
		RECT rect;

		Device->GetCreationParameters(&cparams);
		GetWindowRect(cparams.hFocusWindow, &rect);

		m_sScreen.coords.start.x = rect.left;
		m_sScreen.coords.start.y = rect.top;
		m_sScreen.coords.end.x = rect.right;
		m_sScreen.coords.end.y = rect.bottom;
		m_sScreen.size.width = m_sScreen.coords.end.x - m_sScreen.coords.start.x;
		m_sScreen.size.height = m_sScreen.coords.end.y - m_sScreen.coords.start.y;
	}


private:
	IDirect3DDevice9* Device;
};

struct CUSTOMVERTEX {
	FLOAT x, y, z, rhw;    // from the D3DFVF_XYZRHW flag
	D3DCOLOR color;    // from the D3DFVF_DIFFUSE flag
};


struct dObject {
	CUSTOMVERTEX* V;
	D3DPRIMITIVETYPE type;
	UINT size;
	UINT PrimitiveCount;


	//virtual UINT GetSize() = 0;
	//virtual CUSTOMVERTEX* GetVertex() = 0;
	//virtual D3DPRIMITIVETYPE GetType() = 0;
	//virtual UINT GetPrimitiveCount() = 0;
};


class D3D9DrawList {
public:
	D3D9DrawList(){ m_pList = (dObject**)calloc(DEFAULT_BUFFERSIZE, sizeof(dObject*)), m_pList[0] = 0; size = 0; }
	void init() { if (m_pList) { free(m_pList); }  m_pList = (dObject**)calloc(DEFAULT_BUFFERSIZE, sizeof(dObject*)), m_pList[0] = 0; size = 0; }
	void clear() { init();}
	int Size() { return size; }
	void AddItem(dObject* obj) { 
		size++;	
		if (size < DEFAULT_BUFFERSIZE) {
			m_pList[size - 1] = obj;
		}
		else {
			dObject** reallocd = (dObject**)realloc(m_pList, sizeof(dObject*) * size);
			if (reallocd) {
				m_pList = reallocd;
				m_pList[size - 1] = obj;
			}
		}
	}
	dObject* Get(int index) { if (index >= size) { return 0; } return m_pList[index]; }
	dObject** GetRaw() { return m_pList; }
	void Set(int index, dObject* in_obj) {if (index >= size) { return; } m_pList[index] = in_obj;}
	void Delete(int index) {if (index >= size) { return; } delete m_pList[index];}
private:
	dObject** m_pList;
	int size = 0;
};






class D3D9Renderer : public Renderer::Renderer {
public:

	bool Init(RendererUtils::RendererInitializer* RI) {
		D3D9Initializer* initObj = reinterpret_cast<D3D9Initializer*>(RI);
		m_pd3dDevice = initObj->GetDevice();
		if (!m_pd3dDevice) {
			return true;
		}
		m_pd3dDevice->AddRef();
		m_sScreen = initObj->getScreen();
		HRESULT FuckYou = m_pd3dDevice->CreateVertexBuffer(sizeof(CUSTOMVERTEX)*256, 0, (D3DFVF_XYZRHW | D3DFVF_DIFFUSE), D3DPOOL_DEFAULT, &v_buffer, nullptr);

		if (!v_buffer) {
			Debug::QuickPrint(std::to_string(FuckYou).c_str());

			switch (FuckYou) {
			case D3DERR_INVALIDCALL:
				Debug::QuickPrint("Invalid Call");
				break;
			case D3DERR_OUTOFVIDEOMEMORY:
				Debug::QuickPrint("Out Of Video Memory");
				break;
			case E_OUTOFMEMORY:
				Debug::QuickPrint("Out Of Memeory");
				break;
			}


			Debug::QuickPrint("D3D9Renderer::INIT Failed to create Vertex Buffer");
			throw std::exception("D3D9Renderer::INIT Failed to create Vertex Buffer");
			return true;
		}

		//const char* fontName = "zorque";
		//const char* fontName = "tahoma";
		const char* fontName = "constanb";
		ID3DXFont* pfont; 
		auto hr = D3DXCreateFont(m_pd3dDevice,
			20,  // font height
			8,                //Font width

			FW_NORMAL,        //Font Weight

			1,                //MipLevels

			false,            //Italic

			DEFAULT_CHARSET,  //CharSet

			OUT_DEFAULT_PRECIS, //OutputPrecision

			ANTIALIASED_QUALITY, //Quality

			DEFAULT_PITCH | FF_DONTCARE,//PitchAndFamily

			fontName,          //pFacename,

			&pfont);         //ppFont

		m_vpFonts.push_back(pfont);

		ID3DXFont* pfont2;
		auto nhr = D3DXCreateFont(m_pd3dDevice,
			14,  // font height
			5,                //Font width

			FW_NORMAL,        //Font Weight

			1,                //MipLevels

			false,            //Italic

			DEFAULT_CHARSET,  //CharSet

			OUT_DEFAULT_PRECIS, //OutputPrecision

			ANTIALIASED_QUALITY, //Quality

			DEFAULT_PITCH | FF_DONTCARE,//PitchAndFamily

			fontName,          //pFacename,

			&pfont2);         //ppFont
		m_vpFonts.push_back(pfont2);
		m_pd3dDevice->CreateStateBlock(D3DSBT_ALL, &m_pinStateBlock);


		ID3DXFont* pfont3;

		auto hr3 = D3DXCreateFont(m_pd3dDevice,
			8,  // font height
			3,                //Font width

			FW_NORMAL,        //Font Weight

			1,                //MipLevels

			false,            //Italic

			DEFAULT_CHARSET,  //CharSet

			OUT_DEFAULT_PRECIS, //OutputPrecision

			ANTIALIASED_QUALITY, //Quality

			DEFAULT_PITCH | FF_DONTCARE,//PitchAndFamily

			fontName,          //pFacename,

			&pfont3);         //ppFont

		m_vpFonts.push_back(pfont3);



		return false;
	}

	void Begin() {
		//m_pd3dDevice->Clear();
		//m_pd3dDevice->BeginScene();

		m_DrawList.init();
		m_pinStateBlock->Capture();
	}

	void Draw();

	void drawFilledRectangle2D(RendererStructs::coordQuad, RendererStructs::rCOLOR, bool bSort);
	void drawFilledRectangle2D(RendererStructs::coordinate, RendererStructs::objectInfo size, RendererStructs::rCOLOR);

	void drawFilledTriangle(RendererStructs::coordinate*, RendererStructs::rCOLOR);

	void drawMultiLine(std::vector<RendererStructs::coord> Cooordinate, RendererStructs::rCOLOR color);
	void drawMultiLine(std::vector<RendererStructs::coord> Cooordinate, RendererStructs::rCOLOR color, int Thickness);
	void drawMultiLine(RendererStructs::coord* Cooordinate, int size, RendererStructs::rCOLOR color);


	std::pair<int, int> getTextSize(int font, const char* szText);
	void renderText(RendererStructs::coord coords, int font, const char* text, RendererStructs::rCOLOR col) {
		LPD3DXFONT pFont = 0;
		RECT rct;
		rct.left = coords.x;
		rct.right = coords.x + 20;
		rct.top = coords.y;
		rct.bottom = coords.y + 20;
		D3DCOLOR color;	
		color = D3DCOLOR_RGBA(col.r, col.g, col.b, col.a);
		font = std::clamp(font, 0, int(m_vpFonts.size() - 1));
		m_vpFonts.at(font)->DrawTextA(NULL, text, -1, &rct, DT_NOCLIP, color);
	}

	void renderText(dObject*);

	void drawText(RendererStructs::coord coords, int font, const char* text, RendererStructs::rCOLOR col);
	void drawShadowedText(RendererStructs::coord coords, int font, const char* text, RendererStructs::rCOLOR col);

private:

	std::vector<ID3DXFont*> m_vpFonts;


	IDirect3DStateBlock9* m_pinStateBlock;




	LPDIRECT3DVERTEXBUFFER9 v_buffer;

	IDirect3DDevice9* m_pd3dDevice;
	IDirect3DSurface9* m_suSurface;
	//std::vector<dObject*> DrawList;
	D3D9DrawList m_DrawList;




};