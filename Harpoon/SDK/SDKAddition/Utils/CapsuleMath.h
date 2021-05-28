#pragma once
#include "../CCapsule.h"
#include "../../OsirisSDK/matrix3x4.h"
namespace CapsuleMath {
	bool findCommonIntersection(CCapsule c, CCapsule c2, Vector& vec);
	bool findCommonIntersection(StudioBbox* box, matrix3x4 bone1, matrix3x4 bone2, Vector& vec);
	bool findCommonIntersection(StudioBbox* box, matrix3x4 bone1, matrix3x4 bone2, matrix3x4 bone3, Vector& vec);

	bool HitBoxRayIntersection(Vector StartPoint, Vector EndPoint, StudioBbox* box, matrix3x4 bone);
	bool HitBoxRayIntersection(Vector StartPoint, Vector EndPoint, StudioBbox box, matrix3x4 bone);
	bool HitBoxRayIntersectionA(Vector StartPoint, Vector EndPoint, StudioBbox box, matrix3x4 bone);
	bool HitBoxRayIntersectionAsync(Vector StartPoint, Vector EndPoint, StudioBbox box, matrix3x4 bone);

	void TestRayIntesection();
}




