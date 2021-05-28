#include "../../Interfaces.h"
#include "../../Memory.h"

#include "../../SDK/OsirisSDK/Entity.h"
#include "../../SDK/OsirisSDK/Vector.h"
#include "../../SDK/OsirisSDK/matrix3x4.h"
#include "../../SDK/OsirisSDK/LocalPlayer.h"
#include "../../SDK/OsirisSDK/GlobalVars.h"
#include "../../SDK/OsirisSDK/ModelInfo.h"

#include "../../MemAlloc.h"
#include "../../SDK/OsirisSDK/GlobalVars.h"
#include "Multipoints.h"



#include "../../Helpers.h"
#include "../../GameData.h"
//#include "../Hacks/StreamProofESP.h"
#include "../../SDK/OsirisSDK/Math.h"
#include "../OTHER/Debug.h"
#include "../../SDK/OsirisSDK/Surface.h"
#include "../../SDK/SDKAddition/Utils/CapsuleMath.h"


static bool worldToScreen(const Vector& in) noexcept
{
    const auto& matrix = GameData::toScreenMatrix();

    const auto w = matrix._41 * in.x + matrix._42 * in.y + matrix._43 * in.z + matrix._44;



    //auto out = interfaces->surface / 2.0f;
    //auto tester = StreamProofESP::ScreenSize / 2.0f;

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
        return false;
    }

    return true;
}


#include <winnt.h>
#include "../../SDK/SDKAddition/Utils/VectorMath.h"

namespace Multipoints
{
    bool retrieveAll(Entity* entity, float multiPointsExpansion, float secondExpan, Vector(&multiPoints)[Multipoints::HITBOX_MAX][Multipoints::MULTIPOINTS_MAX], matrix3x4* bonesptr, bool MatrixPassed)
    {
        if (!localPlayer)
            return false;

        if (!entity)
            return false;

        static matrix3x4 bones[256];


        if (!MatrixPassed || !bonesptr || config->debug.forcesetupBones) {
            if (!entity->setupBones(bones, _ARRAYSIZE(bones), 256, memory->globalVars->currenttime)) {
                return false;
            }
        }
        else {
            memcpy(bones, bonesptr, sizeof(matrix3x4) * 256);
        }

        const Model* model = entity->getModel();

        if (!model)
            return false;

        StudioHdr* hdr = interfaces->modelInfo->getStudioModel(model);

        if (!hdr)
            return false;

        int hitBoxSet = entity->hitboxSet();

        if (hitBoxSet < 0)
            return false;

        StudioHitboxSet* hitBoxSetPtr = hdr->getHitboxSet(hitBoxSet);

        if (!hitBoxSetPtr)
            return false;

        for (int hitBox = Multipoints::HITBOX_START; hitBox < (std::min) (hitBoxSetPtr->numHitboxes, (decltype(hitBoxSetPtr->numHitboxes))Multipoints::HITBOX_MAX); hitBox++)
        {
            StudioBbox* box = hitBoxSetPtr->getHitbox(hitBox);

            if (box)
            {
                Vector min = box->bbMin;
                Vector max = box->bbMax;
                Vector minexpan = box->bbMin;
                Vector maxexpan = box->bbMax;
                if (box->capsuleRadius > 0.0f)
                {
                    min -= Vector{ box->capsuleRadius * multiPointsExpansion, box->capsuleRadius * multiPointsExpansion, box->capsuleRadius * multiPointsExpansion, };
                    max += Vector{ box->capsuleRadius * multiPointsExpansion, box->capsuleRadius * multiPointsExpansion, box->capsuleRadius * multiPointsExpansion, };

                    minexpan -= Vector{ box->capsuleRadius * secondExpan, box->capsuleRadius * secondExpan, box->capsuleRadius * secondExpan, };
                    maxexpan += Vector{ box->capsuleRadius * secondExpan, box->capsuleRadius * secondExpan, box->capsuleRadius * secondExpan, };
                };

                static Vector locations[Multipoints::MULTIPOINTS_MAX];

                locations[Multipoints::MULTIPOINTS_CENTER] = (min + max) * 0.5f;

                locations[1] = Vector{min.x,                    ((min.y + max.y) * 0.5f),   ((min.z + max.z) * .5f) };
                locations[2] = Vector{((min.x + max.x) * 0.5f)  , max.y,                  ((min.z + max.z) * .5f) };
                locations[3] = Vector{max.x,                    ((min.y + max.y) * 0.5f),   ((min.z + max.z) * .5f) };
                locations[4] = Vector{((min.x + max.x) * 0.5f)  , min.y,                  ((min.z + max.z) * .5f) };

                locations[5] = Vector{ min.x, min.y, min.z, };
                locations[6] = Vector{ min.x, min.y, max.z, };

                locations[7] = Vector{ max.x, max.y, max.z, };
                locations[8] = Vector{ max.x, max.y, min.z, };


                locations[9] = Vector{ max.x, min.y, min.z, };
                locations[10] = Vector{ max.x, min.y, max.z, };


                locations[11] = Vector{ min.x, max.y, min.z, };
                locations[12] = Vector{ min.x, max.y, max.z, };


                locations[13] = Vector{ minexpan.x, minexpan.y,minexpan.z, };
                locations[14] = Vector{ minexpan.x, minexpan.y, maxexpan.z, };

                locations[15] = Vector{ maxexpan.x, maxexpan.y, maxexpan.z, };
                locations[16] = Vector{ maxexpan.x, maxexpan.y, minexpan.z, };

                locations[17] = Vector{ maxexpan.x, minexpan.y, minexpan.z, };
                locations[18] = Vector{ maxexpan.x, minexpan.y, maxexpan.z, };

                locations[19] = Vector{ minexpan.x, maxexpan.y, minexpan.z, };
                locations[20] = Vector{ minexpan.x, maxexpan.y, maxexpan.z, };

                for (int multiPoint = Multipoints::MULTIPOINTS_START; multiPoint < Multipoints::MULTIPOINTS_MAX; multiPoint++)
                {
                    if (multiPoint > Multipoints::MULTIPOINTS_START)
                    {
                        locations[multiPoint] = (((locations[multiPoint] + locations[Multipoints::MULTIPOINTS_CENTER]) * 0.5f) + locations[multiPoint]) * 0.5f;
                    };

                    multiPoints[hitBox][multiPoint] = locations[multiPoint].transform(bones[box->bone]);
                };
            };
        };

        return true;
    };





    bool MultipointSetup(Entity* entity, EntityStudioInfo& save, matrix3x4* bonesptr, bool MatrixPassed){

 \

        if (!MatrixPassed || !bonesptr || config->debug.forcesetupBones) {
            if (!entity->setupBones(save.bones, _ARRAYSIZE(save.bones), 256, memory->globalVars->currenttime)) {
                return false;
            }
        }
        else {
            memcpy(save.bones, bonesptr, sizeof(matrix3x4) * 256);
        }

        save.model = entity->getModel();

        if (!save.model)
            return false;

        save.hdr = interfaces->modelInfo->getStudioModel(save.model);

        if (!save.hdr)
            return false;

        save.hitBoxSet = entity->hitboxSet();

        if (save.hitBoxSet < 0)
            return false;

        save.hitBoxSetPtr = save.hdr->getHitboxSet(save.hitBoxSet);

        if (!save.hitBoxSetPtr)
            return false;


        return true;
    }



    bool retrieveHitbox(EntityStudioInfo& save, HitboxPoints& CurrHitbox, PointScales scales, Hitbox hitBox, float secondExpan, bool fetchCenterOnly) {
        StudioBbox* box = save.hitBoxSetPtr->getHitbox(hitBox);


        if (!box)
            return false;

        float scale = scales.body;
        if ((hitBox == Multipoints::HITBOX_HEAD) || (hitBox == Multipoints::HITBOX_NECK)) {
            scale = scales.head;
        }
        switch (hitBox) {
        case Multipoints::HITBOX_HEAD:
        case Multipoints::HITBOX_NECK:
            scale = scales.head;
            break;
        case Multipoints::HITBOX_RIGHT_THIGH:
        case Multipoints::HITBOX_LEFT_THIGH:
        case Multipoints::HITBOX_RIGHT_CALF:
        case Multipoints::HITBOX_LEFT_CALF:
        case Multipoints::HITBOX_RIGHT_FOOT:
        case Multipoints::HITBOX_LEFT_FOOT:
        case Multipoints::HITBOX_RIGHT_HAND:
        case Multipoints::HITBOX_LEFT_HAND:
        case Multipoints::HITBOX_RIGHT_UPPER_ARM:
        case Multipoints::HITBOX_RIGHT_FOREARM:
        case Multipoints::HITBOX_LEFT_UPPER_ARM:
        case Multipoints::HITBOX_LEFT_FOREARM:
            scale = scales.appendage;
            break;
        default:
            break;
        }

        scale = min(scale, box->capsuleRadius);

            Vector min = box->bbMin;
            Vector max = box->bbMax;
            Vector minexpan = box->bbMin;
            Vector maxexpan = box->bbMax;
            if (box->capsuleRadius > 0.0f)
            {
                min -= Vector{ box->capsuleRadius * scale, box->capsuleRadius * scale, box->capsuleRadius * scale, };
                max += Vector{ box->capsuleRadius * scale, box->capsuleRadius * scale, box->capsuleRadius * scale, };

                minexpan -= Vector{ box->capsuleRadius * secondExpan, box->capsuleRadius * secondExpan, box->capsuleRadius * secondExpan, };
                maxexpan += Vector{ box->capsuleRadius * secondExpan, box->capsuleRadius * secondExpan, box->capsuleRadius * secondExpan, };
            };

            CurrHitbox.HitBox = hitBox;




            Point Temp;

            CurrHitbox.Origin.Point = (min + max) * 0.5f;

            if (fetchCenterOnly) {
                CurrHitbox.Origin.Point = CurrHitbox.Origin.Point.transform(save.bones[box->bone]);
                return true;
            }

            Temp.Point = Vector{ min.x,                    ((min.y + max.y) * 0.5f),   ((min.z + max.z) * .5f) };
            CurrHitbox.Tester.push_back(Temp);



            Temp.Point = Vector{ ((min.x + max.x) * 0.5f)  , max.y,                  ((min.z + max.z) * .5f) };
            CurrHitbox.Tester.push_back(Temp);

            Temp.Point = Vector{ max.x,                    ((min.y + max.y) * 0.5f),   ((min.z + max.z) * .5f) };
            CurrHitbox.Tester.push_back(Temp);

            Temp.Point = Vector{ ((min.x + max.x) * 0.5f)  , min.y,                  ((min.z + max.z) * .5f) };
            CurrHitbox.Tester.push_back(Temp);

            Temp.Point = Vector{ min.x, min.y, min.z, };
            CurrHitbox.Points.push_back(Temp);
            Temp.Point = Vector{ min.x, min.y, max.z, };
            CurrHitbox.Points.push_back(Temp);

            Temp.Point = Vector{ max.x, max.y, max.z, };
            CurrHitbox.Points.push_back(Temp);
            Temp.Point = Vector{ max.x, max.y, min.z, };
            CurrHitbox.Points.push_back(Temp);

            Temp.Point = Vector{ max.x, min.y, min.z, };
            CurrHitbox.Points.push_back(Temp);
            Temp.Point = Vector{ max.x, min.y, max.z, };
            CurrHitbox.Points.push_back(Temp);


            Temp.Point = Vector{ min.x, max.y, min.z, };
            CurrHitbox.Points.push_back(Temp);
            Temp.Point = Vector{ min.x, max.y, max.z, };
            CurrHitbox.Points.push_back(Temp);

            if (secondExpan > 0.f) {
                Temp.Point = Vector{ minexpan.x, minexpan.y,minexpan.z, };
                CurrHitbox.Extended_Points.push_back(Temp);
                Temp.Point = Vector{ minexpan.x, minexpan.y, maxexpan.z, };
                CurrHitbox.Extended_Points.push_back(Temp);

                Temp.Point = Vector{ maxexpan.x, maxexpan.y, maxexpan.z, };
                CurrHitbox.Extended_Points.push_back(Temp);
                Temp.Point = Vector{ maxexpan.x, maxexpan.y, minexpan.z, };
                CurrHitbox.Extended_Points.push_back(Temp);

                Temp.Point = Vector{ maxexpan.x, minexpan.y, minexpan.z, };
                CurrHitbox.Extended_Points.push_back(Temp);
                Temp.Point = Vector{ maxexpan.x, minexpan.y, maxexpan.z, };
                CurrHitbox.Extended_Points.push_back(Temp);

                Temp.Point = Vector{ minexpan.x, maxexpan.y, minexpan.z, };
                CurrHitbox.Extended_Points.push_back(Temp);
                Temp.Point = Vector{ minexpan.x, maxexpan.y, maxexpan.z, };
                CurrHitbox.Extended_Points.push_back(Temp);
            }


            CurrHitbox.Origin.Point = CurrHitbox.Origin.Point.transform(save.bones[box->bone]);


            for (int i = 0; i < CurrHitbox.Points.size(); i++) {
                //CurrHitbox.Points.at(i).Point = (((CurrHitbox.Points.at(i).Point + CurrHitbox.Origin.Point) * 0.5f) + CurrHitbox.Points.at(i).Point) * 0.5f;
                CurrHitbox.Points.at(i).Point = CurrHitbox.Points.at(i).Point.transform(save.bones[box->bone]);
            }





            if (secondExpan > 0.0f) {
                for (int i = 0; i < CurrHitbox.Points.size(); i++) {
                    CurrHitbox.Extended_Points.at(i).Point = (((CurrHitbox.Extended_Points.at(i).Point + CurrHitbox.Origin.Point) * 0.5f) + CurrHitbox.Extended_Points.at(i).Point) * 0.5f;
                    CurrHitbox.Extended_Points.at(i).Point = CurrHitbox.Extended_Points.at(i).Point.transform(save.bones[box->bone]);
                }
            }





            /*
            for (int multiPoint = Multipoints::MULTIPOINTS_START; multiPoint < Multipoints::MULTIPOINTS_MAX; multiPoint++)
            {
                if (multiPoint > Multipoints::MULTIPOINTS_START)
                {
                    locations[multiPoint] = (((locations[multiPoint] + locations[Multipoints::MULTIPOINTS_CENTER]) * 0.5f) + locations[multiPoint]) * 0.5f;
                };

                multiPoints[multiPoint] = locations[multiPoint].transform(bones[box->bone]);
            };
            */










        return true;
    }


    bool retrieveSafePoint(EntityStudioInfo& save, HitboxPoints& CurrHitbox, PointScales scales, Hitbox hitBox, matrix3x4* matrix2) {
        StudioBbox* box = save.hitBoxSetPtr->getHitbox(hitBox);


        if (!box)
            return false;

        float scale = scales.body;
        if ((hitBox == Multipoints::HITBOX_HEAD) || (hitBox == Multipoints::HITBOX_NECK)) {
            scale = scales.head;
        }
        switch (hitBox) {
        case Multipoints::HITBOX_HEAD:
        case Multipoints::HITBOX_NECK:
            scale = scales.head;
            break;
        case Multipoints::HITBOX_RIGHT_THIGH:
        case Multipoints::HITBOX_LEFT_THIGH:
        case Multipoints::HITBOX_RIGHT_CALF:
        case Multipoints::HITBOX_LEFT_CALF:
        case Multipoints::HITBOX_RIGHT_FOOT:
        case Multipoints::HITBOX_LEFT_FOOT:
        case Multipoints::HITBOX_RIGHT_HAND:
        case Multipoints::HITBOX_LEFT_HAND:
        case Multipoints::HITBOX_RIGHT_UPPER_ARM:
        case Multipoints::HITBOX_RIGHT_FOREARM:
        case Multipoints::HITBOX_LEFT_UPPER_ARM:
        case Multipoints::HITBOX_LEFT_FOREARM:
            scale = scales.appendage;
            break;
        default:
            break;
        }

        scale = min(scale, box->capsuleRadius);

        Vector min = box->bbMin;
        Vector max = box->bbMax;
        Vector minexpan = box->bbMin;
        Vector maxexpan = box->bbMax;

        Vector SafePoint;

        HitboxPoints point;
        Point Temp;
        Temp.isSafePoint = true;
  
        if (!CapsuleMath::findCommonIntersection(box, save.bones[box->bone], matrix2[box->bone], Temp.Point))
            return false;

        CurrHitbox.Points.push_back(Temp);

        return true;

    }

    bool retrieveAll(Entity* entity, Multipoint& points, PointScales scales, std::vector<Hitbox> hitboxes, float secondExpan, matrix3x4* bonesptr, bool MatrixPassed)
    {
        if (!localPlayer)
            return false;

        if (!entity)
            return false;

        static matrix3x4 bones[256];


        if (!MatrixPassed || !bonesptr || config->debug.forcesetupBones) {
            if (!entity->setupBones(bones, _ARRAYSIZE(bones), 256, memory->globalVars->currenttime)) {
                return false;
            }
        }
        else {
            memcpy(bones, bonesptr, sizeof(matrix3x4) * 256);
        }

        const Model* model = entity->getModel();

        if (!model)
            return false;

        StudioHdr* hdr = interfaces->modelInfo->getStudioModel(model);

        if (!hdr)
            return false;

        int hitBoxSet = entity->hitboxSet();

        if (hitBoxSet < 0)
            return false;

        StudioHitboxSet* hitBoxSetPtr = hdr->getHitboxSet(hitBoxSet);

        if (!hitBoxSetPtr)
            return false;

        for (Hitbox hitBox : hitboxes)//int hitBox = Multipoints::HITBOX_START; hitBox < (std::min) (hitBoxSetPtr->numHitboxes, (decltype(hitBoxSetPtr->numHitboxes))Multipoints::HITBOX_MAX); hitBox++)
        {
            StudioBbox* box = hitBoxSetPtr->getHitbox(hitBox);

            float scale = scales.body;
            if ((hitBox == Multipoints::HITBOX_HEAD) || (hitBox == Multipoints::HITBOX_NECK)) {
                scale = scales.head;
            }
            switch (hitBox) {
                case Multipoints::HITBOX_HEAD:
                case Multipoints::HITBOX_NECK:
                    scale = scales.head;
                    break;
                case Multipoints::HITBOX_RIGHT_THIGH:
                case Multipoints::HITBOX_LEFT_THIGH:
                case Multipoints::HITBOX_RIGHT_CALF:
                case Multipoints::HITBOX_LEFT_CALF:
                case Multipoints::HITBOX_RIGHT_FOOT:
                case Multipoints::HITBOX_LEFT_FOOT:
                case Multipoints::HITBOX_RIGHT_HAND:
                case Multipoints::HITBOX_LEFT_HAND:
                case Multipoints::HITBOX_RIGHT_UPPER_ARM:
                case Multipoints::HITBOX_RIGHT_FOREARM:
                case Multipoints::HITBOX_LEFT_UPPER_ARM:
                case Multipoints::HITBOX_LEFT_FOREARM:
                    scale = scales.appendage;
                    break;
                default:
                    break;
            }

            if (box)
            {
                Vector min = box->bbMin;
                Vector max = box->bbMax;
                Vector minexpan = box->bbMin;
                Vector maxexpan = box->bbMax;
                if (box->capsuleRadius > 0.0f)
                {
                    min -= Vector{ box->capsuleRadius * scale, box->capsuleRadius * scale, box->capsuleRadius * scale, };
                    max += Vector{ box->capsuleRadius * scale, box->capsuleRadius * scale, box->capsuleRadius * scale, };

                    minexpan -= Vector{ box->capsuleRadius * secondExpan, box->capsuleRadius * secondExpan, box->capsuleRadius * secondExpan, };
                    maxexpan += Vector{ box->capsuleRadius * secondExpan, box->capsuleRadius * secondExpan, box->capsuleRadius * secondExpan, };
                };

                static Vector locations[Multipoints::MULTIPOINTS_MAX];
                

                HitboxPoints CurrHitbox;
                CurrHitbox.HitBox = hitBox;

                //CurrHitbox.Points.push_back()


                Point Temp;
                
                CurrHitbox.Origin.Point = (min + max) * 0.5f;



                Temp.Point = Vector{ min.x,                    ((min.y + max.y) * 0.5f),   ((min.z + max.z) * .5f) };
                CurrHitbox.Tester.push_back(Temp);



                Temp.Point = Vector{ ((min.x + max.x) * 0.5f)  , max.y,                  ((min.z + max.z) * .5f) };
                CurrHitbox.Tester.push_back(Temp);

                Temp.Point = Vector{ max.x,                    ((min.y + max.y) * 0.5f),   ((min.z + max.z) * .5f) };
                CurrHitbox.Tester.push_back(Temp);

                Temp.Point = Vector{ ((min.x + max.x) * 0.5f)  , min.y,                  ((min.z + max.z) * .5f) };
                CurrHitbox.Tester.push_back(Temp);
                
                Temp.Point = Vector{ min.x, min.y, min.z, };
                CurrHitbox.Points.push_back(Temp);
                Temp.Point = Vector{ min.x, min.y, max.z, };
                CurrHitbox.Points.push_back(Temp);

                Temp.Point = Vector{ max.x, max.y, max.z, };
                CurrHitbox.Points.push_back(Temp);
                Temp.Point = Vector{ max.x, max.y, min.z, };
                CurrHitbox.Points.push_back(Temp);

                Temp.Point = Vector{ max.x, min.y, min.z, };
                CurrHitbox.Points.push_back(Temp);
                Temp.Point = Vector{ max.x, min.y, max.z, };
                CurrHitbox.Points.push_back(Temp);


                Temp.Point = Vector{ min.x, max.y, min.z, };
                CurrHitbox.Points.push_back(Temp);
                Temp.Point = Vector{ min.x, max.y, max.z, };
                CurrHitbox.Points.push_back(Temp);
                
                if (secondExpan > 0.f) {
                    Temp.Point = Vector{ minexpan.x, minexpan.y,minexpan.z, };
                    CurrHitbox.Extended_Points.push_back(Temp);
                    Temp.Point = Vector{ minexpan.x, minexpan.y, maxexpan.z, };
                    CurrHitbox.Extended_Points.push_back(Temp);

                    Temp.Point = Vector{ maxexpan.x, maxexpan.y, maxexpan.z, };
                    CurrHitbox.Extended_Points.push_back(Temp);
                    Temp.Point = Vector{ maxexpan.x, maxexpan.y, minexpan.z, };
                    CurrHitbox.Extended_Points.push_back(Temp);

                    Temp.Point = Vector{ maxexpan.x, minexpan.y, minexpan.z, };
                    CurrHitbox.Extended_Points.push_back(Temp);
                    Temp.Point = Vector{ maxexpan.x, minexpan.y, maxexpan.z, };
                    CurrHitbox.Extended_Points.push_back(Temp);

                    Temp.Point = Vector{ minexpan.x, maxexpan.y, minexpan.z, };
                    CurrHitbox.Extended_Points.push_back(Temp);
                    Temp.Point = Vector{ minexpan.x, maxexpan.y, maxexpan.z, };
                    CurrHitbox.Extended_Points.push_back(Temp);
                }


                CurrHitbox.Origin.Point = CurrHitbox.Origin.Point.transform(bones[box->bone]);


                for (int i = 0; i < CurrHitbox.Points.size(); i++) {
                    CurrHitbox.Points.at(i).Point = (((CurrHitbox.Points.at(i).Point + CurrHitbox.Origin.Point) * 0.5f) + CurrHitbox.Points.at(i).Point) * 0.5f;
                    CurrHitbox.Points.at(i).Point = CurrHitbox.Points.at(i).Point.transform(bones[box->bone]);
                }


                if (secondExpan > 0.0f) {
                    for (int i = 0; i < CurrHitbox.Points.size(); i++) {
                        CurrHitbox.Extended_Points.at(i).Point = (((CurrHitbox.Extended_Points.at(i).Point + CurrHitbox.Origin.Point) * 0.5f) + CurrHitbox.Extended_Points.at(i).Point) * 0.5f;
                        CurrHitbox.Extended_Points.at(i).Point = CurrHitbox.Extended_Points.at(i).Point.transform(bones[box->bone]);
                    }
                }


                points.push_back(CurrHitbox);
                /*

                for (int multiPoint = Multipoints::MULTIPOINTS_START; multiPoint < Multipoints::MULTIPOINTS_MAX; multiPoint++)
                {
                    if (multiPoint > Multipoints::MULTIPOINTS_START)
                    {
                        locations[multiPoint] = (((locations[multiPoint] + CurrHitbox.Origin.Point) * 0.5f) + locations[multiPoint]) * 0.5f;
                    };

                    multiPoints[hitBox][multiPoint] = locations[multiPoint].transform(bones[box->bone]);
                };
                */
            };
        };

        return true;
    };

    /*
    
        Basically we calculate the multipoints & w2s them. Then we figure out the > X, > Y, and > Z points of the hitbox. If that returns less than zero (out of screen) we default to the old way of 
        finding multipoints (return the points we had). Otherwise, we return many less points.

        This can certainly be done by using crazy matrix math. 
    
    
    
    
    */


    // Retrieves MultiPoints, that are NOT Guarenteed to be safepoints
    bool retrieveAll(Entity* entity, float expansion, PointScales scales, Multipoint& points, matrix3x4* bonesptr, bool MatrixPassed) {
        if (!localPlayer)
            return false;

        if (!entity)
            return false;

        static matrix3x4 bones[256];


        if (!MatrixPassed || !bonesptr || config->debug.forcesetupBones) {
            if (!entity->setupBones(bones, _ARRAYSIZE(bones), 256, memory->globalVars->currenttime)) {
                return false;
            }
        }
        else {
            memcpy(bones, bonesptr, sizeof(matrix3x4) * 256);
        }

        const Model* model = entity->getModel();

        if (!model)
            return false;

        StudioHdr* hdr = interfaces->modelInfo->getStudioModel(model);

        if (!hdr)
            return false;

        int hitBoxSet = entity->hitboxSet();

        if (hitBoxSet < 0)
            return false;

        StudioHitboxSet* hitBoxSetPtr = hdr->getHitboxSet(hitBoxSet);

        if (!hitBoxSetPtr)
            return false;
        

        for (int hitBox = 0; hitBox < Multipoints::HITBOX_MAX; hitBox++)
        {
            HitboxPoints point;
            point.HitBox = hitBox;
            StudioBbox* box = hitBoxSetPtr->getHitbox(hitBox);
            float pscale = 1.0f;
            Point temp;
            if (box)
            {
                    Vector bmin = box->bbMin;
                    Vector bmax = box->bbMax;
                    Vector CenterPoint = (bmin * bmax) * .5f; /* Calculate Center Point of Capsule*/
                    Vector RelCenter = CenterPoint;
                    CenterPoint = CenterPoint.transform(bones[box->bone]); /* Transform To World / Body Matrix */

                    if (box->capsuleRadius > 0.0f)
                    {
                        bmin -= Vector{ box->capsuleRadius * pscale, box->capsuleRadius * pscale, box->capsuleRadius * pscale, };
                        bmax += Vector{ box->capsuleRadius * pscale, box->capsuleRadius * pscale, box->capsuleRadius * pscale, };
                    }


               
                    temp.Point = CenterPoint;
                    point.Points.push_back(temp);

                    
                    Vector Angle = VectorMath::calculateAngle(CenterPoint, localPlayer->getEyePosition());
                    
                    /*
                    double x1 = point.x - center.x;
                    double y1 = point.y - center.y;

                    double x2 = x1 * cos(angle) - y1 * sin(angle));
                    double y2 = x1 * sin(angle) + y1 * cos(angle));

                    point.x = x2 + center.x;
                    point.y = y2 + center.y;
                    */

                    Vector Perp = { 1,0,0 };
                    Angle.normalize();
                    Perp.CrossProduct(Angle); // Get the cross product, vector perpendicular to main 
                    Perp.x = Perp.x * bmin.x;
                    Perp.y = Perp.y * bmin.y;
                    Perp.z = bmin.z;
                    temp.Point = Perp.transform(bones[box->bone]);
                    point.Points.push_back(temp);

                    Perp = (Perp + bmax);
                    temp.Point = Perp.transform(bones[box->bone]);
                    temp.Point = Perp;
                   // point.Points.push_back(temp);
                    
                
                          
            }
            points.push_back(point);
            
        }
        return true;




        /*
        
                *----------------------*
                |                      |
                |                      |
                |          GG          |
                |          GG          |
                |                      |
                |                      |
                *----------------------*
                
                
             
        //-----------------------------------------------------------------------------
        // Purpose: Draws a box around an entity
        //-----------------------------------------------------------------------------

        void NDebugOverlay::EntityBounds( const CBaseEntity *pEntity, int r, int g, int b, int a, float flDuration )
        {
	        const CCollisionProperty *pCollide = pEntity->CollisionProp();
	        // Draw the base OBB for the object (default color is orange)
	        BoxAngles( pCollide->GetCollisionOrigin(), pCollide->OBBMins(), pCollide->OBBMaxs(), pCollide->GetCollisionAngles(), r, g, b, a, flDuration );

	        // This is the axis of rotation in world space
	        Vector rotationAxisWs(0,0,1);
	        const float rotationAngle = gpGlobals->curtime*10; // 10 degrees per second animated rotation
	        //const float rotationAngle = 45; // degrees, Source's convention is that positive rotation is counter-clockwise
	

	        // Example 1: Applying the rotation in the local space of the entity

	        // Compute rotation axis in entity local space
	        // Compute the transform as a matrix so we can concatenate it with the entity's current transform
	        Vector rotationAxisLs;

	        // The matrix maps vectors from entity space to world space, since we have a world space 
	        //vector that we want in entity space we use the inverse operator VectorIRotate instead of VectorRotate
            //    Note, you could also invert the matrix and use VectorRotate instead 
                VectorIRotate(rotationAxisWs, pEntity->EntityToWorldTransform(), rotationAxisLs);

                // Build a transform that rotates around that axis in local space by the angle
                // If there were an AxisAngleMatrix() routine we could use that directly, but there isn't
                // So convert to a quaternion first, then a matrix 
                Quaternion q;

                // NOTE: Assumes axis is a unit vector, non-unit vectors will bias the resulting rotation angle (but not the axis)
                AxisAngleQuaternion(rotationAxisLs, rotationAngle, q);

                // Convert to a matrix
                matrix3x4_t xform;
                QuaternionMatrix(q, vec3_origin, xform);

                // Apply the rotation to the entity input space (local)
                matrix3x4_t localToWorldMatrix;
                ConcatTransforms(pEntity->EntityToWorldTransform(), xform, localToWorldMatrix);

                // Extract the compound rotation as a QAngle
                QAngle localAngles;
                MatrixAngles(localToWorldMatrix, localAngles);

                // Draw the rotated box in blue
                BoxAngles(pCollide->GetCollisionOrigin(), pCollide->OBBMins(), pCollide->OBBMaxs(), localAngles, 0, 0, 255, a, flDuration);


                {

                    // Example 2: Applying the rotation in world space directly

                    // Build a transform that rotates around that axis in world space by the angle
                    //NOTE: Add ten degrees so the boxes are separately visible
                   // Then compute the transform as a matrix so we can concatenate it with the entity's current transform 
                    Quaternion q;
                    AxisAngleQuaternion(rotationAxisWs, rotationAngle + 10, q);

                    // Convert to a matrix
                    matrix3x4_t xform;
                    QuaternionMatrix(q, vec3_origin, xform);

                    // Apply the rotation to the entity output space (world)
                    matrix3x4_t localToWorldMatrix;
                    ConcatTransforms(xform, pEntity->EntityToWorldTransform(), localToWorldMatrix);

                    // Extract the compound rotation as a QAngle
                    QAngle localAngles;
                    MatrixAngles(localToWorldMatrix, localAngles);

                    // Draw the rotated + 10 box in yellow
                    BoxAngles(pCollide->GetCollisionOrigin(), pCollide->OBBMins(), pCollide->OBBMaxs(), localAngles, 255, 255, 0, a, flDuration);
                }
            }

               
    */













    }




    




    bool retrieveOne(Entity* entity, float multiPointsExpansion, Vector(&multiPoints)[Multipoints::MULTIPOINTS_MAX], int desiredHitBox)
    {
        if (desiredHitBox < Multipoints::HITBOX_START || desiredHitBox > Multipoints::HITBOX_LAST_ENTRY)
            return false;

        if (!localPlayer)
            return false;

        if (!entity)
            return false;

        static matrix3x4 bones[256];

        if (!entity->setupBones(bones, _ARRAYSIZE(bones), 256, memory->globalVars->currenttime))
            return false;

        const Model* model = entity->getModel();

        if (!model)
            return false;

        StudioHdr* hdr = interfaces->modelInfo->getStudioModel(model);

        if (!hdr)
            return false;

        int hitBoxSet = entity->hitboxSet();

        if (hitBoxSet < 0)
            return false;

        StudioHitboxSet* hitBoxSetPtr = hdr->getHitboxSet(hitBoxSet);

        if (!hitBoxSetPtr)
            return false;

        StudioBbox* box = hitBoxSetPtr->getHitbox(desiredHitBox);

        if (!box)
            return false;

        Vector min = box->bbMin;
        Vector max = box->bbMax;

        if (box->capsuleRadius > 0.0f)
        {
            min -= Vector{ box->capsuleRadius * multiPointsExpansion, box->capsuleRadius * multiPointsExpansion, box->capsuleRadius * multiPointsExpansion, };
            max += Vector{ box->capsuleRadius * multiPointsExpansion, box->capsuleRadius * multiPointsExpansion, box->capsuleRadius * multiPointsExpansion, };
        };

        static Vector locations[Multipoints::MULTIPOINTS_MAX];

        locations[Multipoints::MULTIPOINTS_CENTER] = (min + max) * 0.5f;

        locations[1] = Vector{ min.x, min.y, min.z, };
        locations[2] = Vector{ min.x, max.y, min.z, };
        locations[3] = Vector{ max.x, max.y, min.z, };
        locations[4] = Vector{ max.x, min.y, min.z, };
        locations[5] = Vector{ max.x, max.y, max.z, };
        locations[6] = Vector{ min.x, max.y, max.z, };
        locations[7] = Vector{ min.x, min.y, max.z, };
        locations[8] = Vector{ max.x, min.y, max.z, };

        for (int multiPoint = Multipoints::MULTIPOINTS_START; multiPoint < Multipoints::MULTIPOINTS_MAX; multiPoint++)
        {
            if (multiPoint > Multipoints::MULTIPOINTS_START)
            {
                locations[multiPoint] = (((locations[multiPoint] + locations[Multipoints::MULTIPOINTS_CENTER]) * 0.5f) + locations[multiPoint]) * 0.5f;
            };

            multiPoints[multiPoint] = locations[multiPoint].transform(bones[box->bone]);
        };

        return true;
    };
};

