#include "CSphere.h"
namespace SphereMath {
    bool intersectsRay(CSphere Sp, const CustomRay r) {
        float t;
        float t0, t1; // solutions for t if the ray intersects 

        // geometric solution
        Vector L = Sp.getCenter() - r.Start;
        float tca = L.dotProduct(VectorMath::normalize(r.Direction));
        if (tca < 0) return false;

        float d2 = L.dotProduct(L) - tca * tca;

        if (d2 > Sp.getRadius2()) return false;
        float thc = sqrt(Sp.getRadius2() - d2);
        //float thc = sqrt(d2 - m_flRadius2);
        t0 = tca - thc;
        t1 = tca + thc;

        if (t0 > t1) std::swap(t0, t1);

        if (t0 < 0) {
            t0 = t1; // if t0 is negative, let's use t1 instead 
            if (t0 < 0) return false; // both t0 and t1 are negative 
        }

        t = t0;
        //vecIntersection = r.Direction + r.Start * t;
        return true;
    }
}