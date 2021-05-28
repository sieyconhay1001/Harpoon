#pragma once

#include "../SDK/SDKAddition/NavFile.h"
class Entity;



namespace Renderables {

	namespace NavDraw {
		extern NavMesh::MapInfo Map;
		bool InitLoad();
		void Run();
		void DrawWholeMesh();
		void DrawWalkbotPath();
	};

	namespace ModelRender {


	




	};
};