#pragma once
#include <vector>
#include <float.h>

#include "../OsirisSDK/ModelInfo.h"
#include "CustomRay.h"
#include "Utils/VectorMath.h"

// https://sdm.scad.edu/faculty/mkesson/vsfx419/wip/best/winter12/jonathan_mann/raytracer.html
// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection\
// http://viclw17.github.io/2018/07/16/raytracing-ray-sphere-intersection/
// http://kylehalladay.com/blog/tutorial/math/2013/12/24/Ray-Sphere-Intersection.html





// https://www.unknowncheats.me/forum/1346434-post5.html  

/*
      This is mainly this post, with a few moditifcations by me. All credit to superdoc1234 on the unknowncheat forums
*/


class CSphere
{
public:
    CSphere(void) {};
    CSphere(const Vector& vecCenter, float flRadius) {
        setup(vecCenter, flRadius);
    }

    void setup(const Vector& vecCenter, float flRadius) {
        m_vecCenter = vecCenter;
        m_flRadius = flRadius;
        m_flRadius2 = flRadius;
    }


    void transform(matrix3x4 mat){
        m_vecCenter = m_vecCenter.transform(mat);
    }

    bool intersectsRay(const CustomRay r, Vector& vecIntersection) {
        float t;
        float t0, t1; // solutions for t if the ray intersects 

        // geometric solution
        Vector L = m_vecCenter - r.Start ;
        float tca = L.dotProduct(VectorMath::normalize(r.Direction));
        if (tca < 0) return false;

        float d2 = L.dotProduct(L) - tca * tca;

        if (d2 > m_flRadius2) return false;
        float thc = sqrt(m_flRadius2 - d2);
        //float thc = sqrt(d2 - m_flRadius2);
        t0 = tca - thc;
        t1 = tca + thc;

        if (t0 > t1) std::swap(t0, t1);

        if (t0 < 0) {
            t0 = t1; // if t0 is negative, let's use t1 instead 
            if (t0 < 0) return false; // both t0 and t1 are negative 
        }

        t = t0;
        vecIntersection = r.Direction + r.Start * t;
        return true;
    }



    bool intersectsRay(const CustomRay r) {
        float t;
        float t0, t1; // solutions for t if the ray intersects 

        // geometric solution
        Vector L = m_vecCenter - r.Start;
        float tca = L.dotProduct(VectorMath::normalize(r.Direction));
        if (tca < 0) return false;

        float d2 = L.dotProduct(L) - tca * tca;

        if (d2 > m_flRadius2) return false;
        float thc = sqrt(m_flRadius2 - d2);
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

    bool intersectsSphere(CSphere otherSphere, Vector& vecIntersection) {
        CustomRay Ray;
        Ray.Destination = otherSphere.getCenter();
        Ray.Start = m_vecCenter;
        Ray.Direction = VectorMath::calculateAngle(Ray.Start, Ray.Destination);
        Ray.Delta = Ray.Destination - Ray.Start;

        if (std::abs(Ray.Destination.distTo(Ray.Start)) > (m_flRadius + otherSphere.getRadius()))
            return false;

        return otherSphere.intersectsRay(Ray, vecIntersection);
    }


    Vector getCenter() {
        return m_vecCenter;
    }
    float getRadius() {
        return m_flRadius;
    }
    float getRadius2() {
        return m_flRadius2;
    }

private:
    Vector m_vecCenter = {};
    float   m_flRadius = 0.f;
    float   m_flRadius2 = 0.f; // r^2

};

namespace SphereMath {
    bool intersectsRay(CSphere Sp, const CustomRay r);
}