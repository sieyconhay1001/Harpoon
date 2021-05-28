#include "CSphere.h"


#pragma once

class CCapsule
{
public:

    CCapsule() {};
    CCapsule(StudioBbox* box, float scale = 1.f) {
        setup(box->bbMin, box->bbMax, box->capsuleRadius * scale);
    }


    CCapsule(StudioBbox* box, matrix3x4 bone1, matrix3x4 bone2, float scale = 1.f) {
        if (box->capsuleRadius == -1)
            return;

        Vector Center = ((box->bbMax + box->bbMin) * .5f);
        Vector Center2 = Center.transform(bone1);
        Center = Center.transform(bone2);

        Vector Delta = Center - Center2;

        setup(box->bbMin + Delta, box->bbMax + Delta, box->capsuleRadius * scale);
    }

    void setup(StudioBbox* box, matrix3x4 bone, float scale = 1) {
        if (box->capsuleRadius == -1)
            return;
        setup(box->bbMin, box->bbMax, box->capsuleRadius * scale);
    }

    void setup(const Vector vecMin, const Vector vecMax, float flRadius) {
        auto vecDelta = VectorMath::normalize(vecMax - vecMin);
        auto vecCenter = vecMin;

        m_cSpheres.push_back(CSphere{ vecMin, flRadius });

        for (size_t i = 1; i < std::floor(vecMin.distTo(vecMax)); ++i) {
            m_cSpheres.push_back(CSphere{ vecMin + vecDelta * static_cast<float>(i), flRadius });
        }

        m_cSpheres.push_back(CSphere{ vecMax, flRadius });
    }

    bool InterestionOther(CCapsule cap2, Vector& vec) {
        for (CSphere mySphere : m_cSpheres) {
            for (CSphere theirSphere : cap2.GetSpheres()) {
                if(mySphere.intersectsSphere(theirSphere, vec))
                    return true;
            }
        }
    }


    bool IsInteresection(CustomRay ray){
        for (CSphere sphere : m_cSpheres) {
            if (sphere.intersectsRay(ray))
                return true;
        }
    }

    bool IsInteresection(CustomRay ray, Vector& vec) {
        for (CSphere sphere : m_cSpheres) {
            if (sphere.intersectsRay(ray, vec))
                return true;
        }
    }

    std::vector<CSphere> GetSpheres() {
        return m_cSpheres;
    }

private:
    std::vector<CSphere> m_cSpheres;
};