#pragma once
#include "../../OsirisSDK/Vector.h"
#include "../../OsirisSDK/StudioRender.h"
#include "../../OsirisSDK/ModelInfo.h"
namespace MatrixMath {
	bool DoesRayIntersectMatrix(Vector startPos, Vector endPos, StudioHdr* hdr, int hitboxSet,  matrix3x4*, int matSize = 256);
	bool DoesRayIntersectMatrixAsync(Vector startPos, Vector endPos, StudioHdr* hdr, int hitBoxSet, matrix3x4* matrix, int matSize = 256);
}