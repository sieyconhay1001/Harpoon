#include "MatrixMath.h"
#include "CapsuleMath.h"

#include <future>
#include <vector>
namespace MatrixMath {

	bool DoesRayIntersectMatrix(Vector startPos, Vector endPos, StudioHdr* hdr, int hitBoxSet, matrix3x4* matrix, int matSize ) {
        if (hitBoxSet < 0)
            return false;

        StudioHitboxSet* hitBoxSetPtr = hdr->getHitboxSet(hitBoxSet);

        if (!hitBoxSetPtr)
            return false;

        for (int hitBox = 0; hitBox < (std::min) (hitBoxSetPtr->numHitboxes, (decltype(hitBoxSetPtr->numHitboxes))20); hitBox++)
        {
            StudioBbox* box = hitBoxSetPtr->getHitbox(hitBox);

            if (box)
            {
                //if(box->capsuleRadius < 0.f){ /*DO Box Intersection*/ }
                if (CapsuleMath::HitBoxRayIntersection(startPos, endPos, box, matrix[box->bone]))
                    return true;
            }
        }
        return false;
	}

    bool DoesRayIntersectMatrixAsync(Vector startPos, Vector endPos, StudioHdr* hdr, int hitBoxSet, matrix3x4* matrix, int matSize) {
        if (hitBoxSet < 0)
            return false;

        StudioHitboxSet* hitBoxSetPtr = hdr->getHitboxSet(hitBoxSet);

        if (!hitBoxSetPtr)
            return false;

        std::vector<std::shared_future<bool>> Calcs;

        for (int hitBox = 0; hitBox < (std::min) (hitBoxSetPtr->numHitboxes, (decltype(hitBoxSetPtr->numHitboxes))20); hitBox++)
        {
            StudioBbox* box = hitBoxSetPtr->getHitbox(hitBox);

            if (box)
            {
                Calcs.push_back(std::async(std::launch::async, CapsuleMath::HitBoxRayIntersectionA, startPos, endPos, *box, matrix[box->bone]));
                //if(box->capsuleRadius < 0.f){ /*DO Box Intersection*/ }
                //if (CapsuleMath::HitBoxRayIntersection(startPos, endPos, box, matrix[box->bone]))
                //    return true;
            }
            if ((hitBox % 3) ? false : true) { /* Every Third Instruction we check them*/
                for (std::shared_future<bool> Calc : Calcs) {
                    Calc.wait();
                    if (Calc.get()) {
                        return true;
                    }
                }
                Calcs.clear();
            }
        }




        return false;
    }
}