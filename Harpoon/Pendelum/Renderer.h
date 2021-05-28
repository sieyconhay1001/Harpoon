#pragma once





#include <vector>

namespace RendererStructs {
	struct coordinate {
		float x, y;
	};

	typedef coordinate coord;

	struct coordPair {
		coordinate start;
		coordinate end;
	};

	struct coordinate3D {
		float x, y;
	};

	typedef coordinate3D coord3D;

	struct coordQuad {
		coordinate upperLeft;
		coordinate upperRight;
		coordinate lowerLeft;
		coordinate lowerRight;
	};


	struct coordQuad3D {
		coordinate3D upperLeft;
		coordinate3D upperRight;
		coordinate3D lowerLeft;
		coordinate3D lowerRight;
	};




	struct coordPair3D {
		coordinate3D start;
		coordinate3D end;
	};

	struct objectInfo {
		float width, height;
	};

	struct R_iCOLOR {
		int r, g, b, a;
	};

	struct R_fCOLOR {
		float r, g, b, a;
	};

	struct rCOLOR {
		int r, g, b, a;
	};

	struct Screen {
		coordPair coords;
		objectInfo size;
	};
}

namespace RendererUtils {
	class RendererInitializer {
	public:
		RendererStructs::Screen getScreen() { return m_sScreen; }
		virtual void SetScreenFromDevice() = 0;
	protected:
		RendererStructs::Screen m_sScreen;
	};

	


}

namespace Renderer {




	class Renderer {
	public:
		virtual bool Init(RendererUtils::RendererInitializer*) = 0;
		virtual void Draw() = 0;
		virtual void Begin() = 0;


		virtual void drawFilledRectangle2D(RendererStructs::coordQuad, RendererStructs::rCOLOR, bool bSort = true) = 0;
		virtual void drawFilledRectangle2D(RendererStructs::coordinate, RendererStructs::objectInfo size, RendererStructs::rCOLOR) = 0;
		void drawFilledRectangle2D(float x, float y, float width, float height, RendererStructs::rCOLOR color) {
			drawFilledRectangle2D({ x,y }, { width,height }, color);
		}
		void drawFilledRectangle2D(RendererStructs::coordinate* coords, RendererStructs::rCOLOR color) {
			drawFilledRectangle2D({ {coords[0].x, coords[0].y}, {coords[1].x, coords[1].y },{coords[2].x, coords[2].y},	{coords[3].x, coords[3].y}}, color);
		}
		virtual void drawFilledTriangle(RendererStructs::coordinate*, RendererStructs::rCOLOR) = 0;

		virtual void drawMultiLine(std::vector<RendererStructs::coord> Cooordinate, RendererStructs::rCOLOR color) = 0;
		virtual void drawMultiLine(RendererStructs::coord* Cooordinate, int size, RendererStructs::rCOLOR color) = 0;
		virtual void drawMultiLine(std::vector<RendererStructs::coord> Cooordinate, RendererStructs::rCOLOR color, int Thickness) = 0;

		virtual void drawText(RendererStructs::coord Coords, int font, const char* text, RendererStructs::rCOLOR color) = 0;
		virtual void drawShadowedText(RendererStructs::coord Coords, int font, const char* text, RendererStructs::rCOLOR color) = 0;

		virtual std::pair<int, int> getTextSize(int font, const char* szText) = 0;
		//virtual void drawFilledRectangle3D(RendererStructs::coordPair3D, RendererStructs::rCOLOR) = 0;


		//virtual void drawLine(RendererStructs::coordPair, RendererStructs::rCOLOR) = 0;
	protected:
		RendererStructs::Screen m_sScreen;
	};
}