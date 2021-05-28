#include "ScreenMath.h"
#include "../../../Interfaces.h"
#include "../../OsirisSDK/Surface.h"
#include "../../../GameData.h"
#include "../../OsirisSDK/matrix3x4.h"
#include "../../OsirisSDK/Engine.h"

namespace ScreenMath {

    float worldToScreenDistanceFromCenter(const Vector& in) noexcept
    {
        
        if (!interfaces->surface) {
            //Debug::QuickPrint("No Surface");
            return 99999;
        }
        
        const auto& matrix = GameData::toScreenMatrix();

        const auto w = matrix._41 * in.x + matrix._42 * in.y + matrix._43 * in.z + matrix._44;



        auto [ww, h] = interfaces->surface->getScreenSize();

        struct coord {
            float x;
            float y;
        } out, tester;

        out.x = ww / 2;
        tester.x = ww / 2;
        out.y = h / 2;
        tester.y = h / 2;


        out.x *= 1.0f + (matrix._11 * in.x + matrix._12 * in.y + matrix._13 * in.z + matrix._14) / w;
        out.y *= 1.0f - (matrix._21 * in.x + matrix._22 * in.y + matrix._23 * in.z + matrix._24) / w;

        if (w < 0.001f) {
            return 99999999.0f;
        }

        return (std::abs(out.x - tester.x) + std::abs(out.y - tester.y)) / 2;
    }

    bool worldToScreen(const Vector& in,float& x,float& y) 
    {
        const auto& matrix = GameData::toScreenMatrix();

        const auto w = matrix._41 * in.x + matrix._42 * in.y + matrix._43 * in.z + matrix._44;



        //auto out = interfaces->surface / 2.0f;
        //auto tester = StreamProofESP::ScreenSize / 2.0f;

        auto [ww, h] = interfaces->surface->getScreenSize();

        struct coord {
            float x;
            float y;
        } tester;

        x = ww / 2;
        tester.x = ww / 2;
        y = h / 2;
        tester.y = h / 2;


        x *= 1.0f + (matrix._11 * in.x + matrix._12 * in.y + matrix._13 * in.z + matrix._14) / w;
        y *= 1.0f - (matrix._21 * in.x + matrix._22 * in.y + matrix._23 * in.z + matrix._24) / w;

        if (w < 0.001f) {
            return false;
        }

        return true;
    }


}