#include "D3D9Render.h"







/*
http://www.directxtutorial.com/Lesson.aspx?lessonid=9-4-4
*/
/*

D3DFVF_XYZ	Indicates that the vertex format includes the X, Y and Z coordinates of an untransformed vertex. Untransformed means that the vertex has not yet been translated into screen coordinates. 	float, float, float
D3DFVF_XYZRHW	Indicates that the vertex format includes the X, Y and Z coordinates as well as an additional RHW value of a transformed vertex. This means that the vertex is already in screen coordinates. The Z and the RHW are used when building software engines, which we will not get into.	float, float, float, float
D3DFVF_DIFFUSE	Indicates that the vertex format contains a 32-bit color code for a vertex, used for the color of diffuse lighting.	DWORD
D3DFVF_SPECULAR	Indicates that the vertex format contains a 32-bit color code for a vertex, used for the color of specular highlighting.	DWORD

D3DFVF_TEX0 - through 3DFVF_TEX8	Indicates that the vertex format contains the coordinates for any textures that will be applied to a model.

*/
#define CUSTOMFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)








struct dTriangle : public dObject {
	dTriangle() { 
		V = (CUSTOMVERTEX*)calloc(3, sizeof(CUSTOMVERTEX)); 
		size = sizeof(CUSTOMVERTEX) * 3;
		PrimitiveCount = 1;
		type = D3DPT_TRIANGLELIST;
	}
	~dTriangle() { if (V) { free(V); } };
	UINT GetPrimitiveCount() { return 1; };
};

#if 1
struct dRectangleOld : public dObject {
	dRectangleOld() {
		V = (CUSTOMVERTEX*)malloc(sizeof(CUSTOMVERTEX) * 4);
		size = (sizeof(CUSTOMVERTEX) * 4);
		type = D3DPT_TRIANGLESTRIP;
		PrimitiveCount = 2;
	}
	~dRectangleOld() { if (V) { free(V); }; };
	UINT GetPrimitiveCount() { return 2; };
};
#endif

struct dRectangle : public dObject {
	dRectangle() {
		V = (CUSTOMVERTEX*)malloc(sizeof(CUSTOMVERTEX) * 6);
		size = (sizeof(CUSTOMVERTEX) * 6);
		type = D3DPT_TRIANGLELIST;
		PrimitiveCount = 2;
	}
	~dRectangle() { if (V) { free(V); }; };
	UINT GetPrimitiveCount() { return 2; };
};



struct dLineStrip : public dObject {
	dLineStrip(int in_LineCount) {
		V = (CUSTOMVERTEX*)malloc(sizeof(CUSTOMVERTEX) * in_LineCount);
		type = D3DPT_LINESTRIP;
		size = sizeof(CUSTOMVERTEX) * in_LineCount;
		PrimitiveCount = in_LineCount - 1;
	}
	~dLineStrip() { if (V) { free(V); } }
	UINT GetPrimitiveCount() { return LineCount; };
	int LineCount = 0;
};


struct dLineList : public dObject {
	UINT GetSize() { return ((LineCount + 1) * sizeof(CUSTOMVERTEX)); };
	CUSTOMVERTEX* GetVertex() { return V; }
	D3DPRIMITIVETYPE GetType() { return type; }
	UINT GetPrimitiveCount() { return LineCount; };
	int LineCount = 0;
	D3DPRIMITIVETYPE type = D3DPT_LINELIST;
};

struct dText : public dObject {
	dText(int ix, int iy, int ifont, RendererStructs::rCOLOR icolor, const char* pText) {
		Text = (char*)calloc(strlen(pText)+1,sizeof(char));
		strcpy(Text, pText);
		x = ix;
		y = iy;
		font = ifont;
		col = icolor;
		type = static_cast<D3DPRIMITIVETYPE>(-21);
	}
	char* Text;
	int x, y;
	int font;
	RendererStructs::rCOLOR col;

};




#include <cmath>

//float degreesToRadians (float degrees) noexcept { return degrees * static_cast<float>(M_PI) / 180.0f; };
//float radiansToDegrees (float radians) noexcept { return radians * 180.0f / static_cast<float>(M_PI); };



static dRectangle* CreateRectangle2D(RendererStructs::coordQuad coords, RendererStructs::rCOLOR color, bool Sort = true) {


#if 1
	if (Sort) {

		dRectangleOld* rect = new dRectangleOld;

		std::array< RendererStructs::coord, 4> Coords;
		Coords[0] = coords.lowerLeft;
		Coords[1] = coords.upperLeft;
		Coords[3] = coords.upperRight;
		Coords[2] = coords.lowerRight;
		// Ghetto and Slow

		RendererStructs::coord Center = { 0,0 };
		for (auto coord : Coords) {
			Center.x += coord.x;
			Center.y += coord.y;
		}
		Center.x /= 4.f;
		Center.y /= 4.f;
		auto sorter = [Center](RendererStructs::coord a, RendererStructs::coord b) {
			double a1 = (int)((int)radiansToDegrees(atan2(a.x - Center.x, a.y - Center.y)) + 360.f) % 360;
			double a2 = (int)((int)radiansToDegrees(atan2(b.x - Center.x, b.y - Center.y)) + 360.f) % 360;
			return (bool)(a1 > a2);
		};
		std::sort(Coords.begin(), Coords.end(), sorter);
		//std::swap(Coords[2], Coords[3]);
		coords.lowerLeft = Coords[0];
		coords.upperLeft = Coords[1];
		coords.upperRight = Coords[2];
		coords.lowerRight = Coords[3];


		rect->V[0] = { coords.lowerLeft.x, coords.lowerLeft.y, 0.f, 1.0f, D3DCOLOR_ARGB(color.a, color.r, color.g, color.b) };
		rect->V[1] = { coords.upperLeft.x, coords.upperLeft.y, 0.f, 1.0f, D3DCOLOR_ARGB(color.a, color.r, color.g, color.b) };
		rect->V[2] = { coords.lowerRight.x, coords.lowerRight.y, 0.f, 1.0f, D3DCOLOR_ARGB(color.a, color.r, color.g, color.b) };
		rect->V[3] = { coords.upperRight.x, coords.upperRight.y, 0.f, 1.0f, D3DCOLOR_ARGB(color.a, color.r, color.g , color.b) };

		return reinterpret_cast<dRectangle*>(rect);
	}
	else {
#else 
		{
#endif
		dRectangle* rect = new dRectangle;
		rect->V[1] = { coords.lowerLeft.x, coords.lowerLeft.y, 0.f, 1.0f, D3DCOLOR_ARGB(color.a, color.r, color.g, color.b) };
		rect->V[0] = { coords.upperLeft.x, coords.upperLeft.y, 0.f, 1.0f, D3DCOLOR_ARGB(color.a, color.r, color.g, color.b) };
		rect->V[2] = { coords.lowerRight.x, coords.lowerRight.y, 0.f, 1.0f, D3DCOLOR_ARGB(color.a, color.r, color.g, color.b) };

		rect->V[3] = { coords.lowerRight.x, coords.lowerRight.y, 0.f, 1.0f, D3DCOLOR_ARGB(color.a, color.r, color.g, color.b) };
		rect->V[4] = { coords.upperRight.x, coords.upperRight.y, 0.f, 1.0f, D3DCOLOR_ARGB(color.a, color.r, color.g , color.b) };
		rect->V[5] = { coords.upperLeft.x, coords.upperLeft.y, 0.f, 1.0f, D3DCOLOR_ARGB(color.a, color.r, color.g, color.b) };
		return rect;
	}
}

#include "../Hacks/OTHER/Debug.h"

void D3D9Renderer::renderText(dObject* obj) {
	dText* text = reinterpret_cast<dText*>(obj);
	auto [w, h] = getTextSize(text->font, text->Text);
	RECT rct;
	rct.left = text->x;
	rct.right = text->x + w;
	rct.top = text->y;
	rct.bottom = text->y + h;
	D3DCOLOR color;
	color = D3DCOLOR_RGBA(text->col.r, text->col.g, text->col.b, text->col.a);
	int font = std::clamp(text->font, 0, int(m_vpFonts.size() - 1));
	m_vpFonts.at(font)->DrawTextA(NULL, text->Text, -1, &rct, DT_NOCLIP, color);
}
void D3D9Renderer::drawText(RendererStructs::coord coords, int font, const char* text, RendererStructs::rCOLOR col) {
	dText* mtext = new dText(coords.x, coords.y, font, col, text);
	m_DrawList.AddItem(mtext);
}
void D3D9Renderer::drawShadowedText(RendererStructs::coord coords, int font, const char* text, RendererStructs::rCOLOR col) {


	dText* mtext = new dText(coords.x + 1, coords.y + 1, font, { 0,0,0,255 }, text);
	m_DrawList.AddItem(mtext);


	dText* mtext2 = new dText(coords.x, coords.y, font, col, text);
	m_DrawList.AddItem(mtext2);
}






void D3D9Renderer::Draw() {
	if (!m_DrawList.Size()) {
		m_pinStateBlock->Apply();
		return;
	}

	//m_pd3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1);


	m_pd3dDevice->SetTexture(0, 0);
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
	m_pd3dDevice->SetPixelShader(0);
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);


	dObject** List = m_DrawList.GetRaw();
	for (int i = 0; i < m_DrawList.Size(); i++) {
		dObject* Object = List[i];
		if (!v_buffer || !Object) {
			if (Object) {
				Debug::QuickPrint("No Object");
				delete Object;
			}
			else {
				Debug::QuickPrint("No v_buffer");
			}

			continue;
		}

		if (int(Object->type) == -21) {
			renderText(Object);
			delete Object;
			continue;
		}

		void* pVoid;
		v_buffer->Lock(0, 0, (void**)&pVoid, 0);
		if (!pVoid) {
			Debug::QuickPrint("No ppData");
			delete Object;
			continue;
		}
		memcpy(pVoid, Object->V, Object->size);
		v_buffer->Unlock();
		m_pd3dDevice->SetFVF(CUSTOMFVF);
		m_pd3dDevice->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
		m_pd3dDevice->DrawPrimitive(Object->type, 0, Object->PrimitiveCount);
		delete Object;
	}

	m_pinStateBlock->Apply();

	// REMOVE! 
	//m_DrawList.clear();


	/*
	for (dObject* Object : DrawList){

		if (!v_buffer || !Object) {
			delete Object;
			Debug::QuickPrint("No v_buffer");
			continue;
		}

		void* pVoid;
		v_buffer->Lock(0, 0, (void**)&pVoid, 0);

		if (!pVoid) {
			Debug::QuickPrint("No ppData");
			delete Object;
			continue;
		}

		memcpy(pVoid, Object->V, Object->size);
		v_buffer->Unlock();

		m_pd3dDevice->SetFVF(CUSTOMFVF);

		m_pd3dDevice->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));

		//if(Object->GetType() == D3DPT_LINESTRIP){
		//	Debug::QuickPrint("Line Strip Draw");
		//}


		m_pd3dDevice->DrawPrimitive(Object->type, 0, Object->GetPrimitiveCount());

		//delete Object; 
	}
	*/


	//m_pd3dDevice->EndScene();
	//m_pd3dDevice->EndStateBlock(&m_pinStateBlock);
	//DrawList.clear();
	//DrawList.shrink_to_fit();
}


void D3D9Renderer::drawMultiLine(std::vector<RendererStructs::coord> Coordinates, RendererStructs::rCOLOR color) {

	dLineStrip* Lines = new dLineStrip(Coordinates.size());
	for (int i = 0; i < Coordinates.size(); i++) {
		Lines->V[i] = { Coordinates.at(i).x, Coordinates.at(i).y ,0.f, 1.f, D3DCOLOR_ARGB(color.a, color.r, color.g , color.b) };
	}
	m_DrawList.AddItem(Lines);
}

void D3D9Renderer::drawMultiLine(RendererStructs::coordinate* Coordinates, int size, RendererStructs::rCOLOR color) {

	dLineStrip* Lines = new dLineStrip(size);
	for (int i = 0; i < size; i++) {
		Lines->V[i] = { Coordinates[i].x, Coordinates[i].y ,0.f, 1.f, D3DCOLOR_ARGB(color.a, color.r, color.g , color.b) };
	}
	m_DrawList.AddItem(Lines);
}

void D3D9Renderer::drawMultiLine(std::vector<RendererStructs::coord> Coordinates, RendererStructs::rCOLOR color, int Thickness) {
	for (int j = 0; j < Thickness; j++) {
		{
			dLineStrip* Lines = new dLineStrip(Coordinates.size());
			for (int i = 0; i < Coordinates.size(); i++) {
				Lines->V[i] = { Coordinates.at(i).x + j, Coordinates.at(i).y + j ,0.f, 1.f, D3DCOLOR_ARGB(color.a, color.r, color.g , color.b) };
			}
			//DrawList.push_back(Lines);
			m_DrawList.AddItem(Lines);
		}
		{
			if (j > 0) {
				dLineStrip* Lines = new dLineStrip(Coordinates.size());
				for (int i = 0; i < Coordinates.size(); i++) {
					Lines->V[i] = { Coordinates.at(i).x + -j, Coordinates.at(i).y + -j,0.f, 1.f, D3DCOLOR_ARGB(color.a, color.r, color.g , color.b) };
				}
				m_DrawList.AddItem(Lines);
			}
		}
	}
}








void D3D9Renderer::drawFilledRectangle2D(RendererStructs::coordQuad coords, RendererStructs::rCOLOR color, bool bSort) {
	m_DrawList.AddItem(CreateRectangle2D(coords, color, bSort));
}

//#define CLEAR_OP
void D3D9Renderer::drawFilledRectangle2D(RendererStructs::coordinate coord, RendererStructs::objectInfo size, RendererStructs::rCOLOR color) {
#ifdef CLEAR_OP
	if (color.a >= 255) {
		D3DRECT Rec;
		Rec.x1 = coord.x;
		Rec.x2 = coord.x + size.width;
		Rec.y1 = coord.y;
		Rec.y2 = coord.y + size.height;
		m_pd3dDevice->Clear(1, &Rec, D3DCLEAR_TARGET, D3DCOLOR_ARGB(color.a,color.r,color.g,color.b ), 0, 0);
		return;
	}
#endif
	RendererStructs::coordQuad coords;
	coords.upperLeft = coord;
	coords.upperRight = { coord.x + size.width, coord.y };
	coords.lowerRight = { coord.x + size.width, coord.y + size.width};
	coords.lowerLeft = { coord.x, coord.y + size.width};
	m_DrawList.AddItem(CreateRectangle2D(coords, color));
	//DrawList.push_back(rectPtr);;
}



void D3D9Renderer::drawFilledTriangle(RendererStructs::coordinate* coords, RendererStructs::rCOLOR color) {
	dTriangle* tri = new dTriangle;	
	for(int i = 0; i < 3; i++){
		tri->V[i] = { coords[i].x, coords[i].y,0.f,1.f,D3DCOLOR_ARGB(color.a, color.r, color.g , color.b) };
	}
	m_DrawList.AddItem(tri);
}

std::pair<int, int> D3D9Renderer::getTextSize(int font, const char* szText) {
		RECT rcRect = { 0,0,0,0 };
		font = std::clamp(font, 0, int(m_vpFonts.size() - 1));
		if (m_vpFonts.at(font))
		{
			m_vpFonts.at(font)->DrawText(NULL, szText, strlen(szText), &rcRect, DT_CALCRECT,
				D3DCOLOR_XRGB(0, 0, 0));
		}
		return std::pair<int,int>(rcRect.right - rcRect.left, rcRect.top - rcRect.bottom);
}