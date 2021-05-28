#pragma once

#include <vector>
#include "../../SDK/OsirisSDK/matrix3x4.h"
class Entity;

struct Vector;

namespace Multipoints
{
    /*
    enum HitBoxes
    {
        HITBOX_HEAD,
        HITBOX_NECK,
        HITBOX_PELVIS,
        HITBOX_ABDOMEN,
        HITBOX_KIDNEYS,
        HITBOX_STERNUM,
        HITBOX_CLAVICLES,
        HITBOX_LEFT_THIGH,
        HITBOX_RIGHT_THIGH,
        HITBOX_LEFT_SHIN,
        HITBOX_RIGHT_SHIN,
        HITBOX_LEFT_ANKLE,
        HITBOX_RIGHT_ANKLE,
        HITBOX_LEFT_HAND,
        HITBOX_RIGHT_HAND,
        HITBOX_LEFT_ARM,
        HITBOX_LEFT_FOREARM,
        HITBOX_RIGHT_ARM,
        HITBOX_RIGHT_FOREARM,
        HITBOX_LAST_ENTRY = HITBOX_RIGHT_FOREARM,
        HITBOX_ALL,
        HITBOX_START = HITBOX_HEAD,
        HITBOX_MAX = HITBOX_ALL,
    };
    */
    enum Hitbox : int
    {
        HITBOX_HEAD,
        HITBOX_NECK,
        HITBOX_LOWER_NECK,
        HITBOX_PELVIS,
        HITBOX_BODY,
        HITBOX_THORAX,
        HITBOX_CHEST,
        HITBOX_UPPER_CHEST,
        HITBOX_RIGHT_THIGH,
        HITBOX_LEFT_THIGH,
        HITBOX_RIGHT_CALF,
        HITBOX_LEFT_CALF,
        HITBOX_RIGHT_FOOT,
        HITBOX_LEFT_FOOT,
        HITBOX_RIGHT_HAND,
        HITBOX_LEFT_HAND,
        HITBOX_RIGHT_UPPER_ARM,
        HITBOX_RIGHT_FOREARM,
        HITBOX_LEFT_UPPER_ARM,
        HITBOX_LEFT_FOREARM,
        HITBOX_MAX,
        HITBOX_START = HITBOX_HEAD,
        HITBOX_LAST_ENTRY = HITBOX_LEFT_FOREARM,
    };





    enum
    {
        MULTIPOINTS_CENTER,
        MULTIPOINTS_START = MULTIPOINTS_CENTER,
        MULTIPOINTS_ALL = 21,
        MULTIPOINTS_MAX = MULTIPOINTS_ALL,
    };

    static const char* hitBoxes[Multipoints::HITBOX_MAX] =
    {
        "Head",
        "Neck",
        "Pelvis",
        "Abdomen",
        "Kidneys",
        "Sternum",
        "Clavicles",
        "Left Thigh",
        "Right Thigh",
        "Left Shin",
        "Right Shin",
        "Left Ankle",
        "Right Ankle",
        "Left Hand",
        "Right Hand",
        "Left Arm",
        "Left Forearm",
        "Right Arm",
        "Right Forearm",
    };






    struct PointScales {
        float head = 0.0f;
        float body = 0.0f;
        float appendage = 0.0f;
    };

    struct Point {
        Vector Point;
        bool isSafePoint = false;
    };

    struct HitboxPoints {
        int HitBox;
        Point Origin;
        std::vector<Point> Tester;
        std::vector<Point> Points;
        std::vector<Point> Extended_Points;
    };


    struct EntityStudioInfo {
        matrix3x4 bones[256];
        const Model* model = nullptr;
        StudioHdr* hdr = nullptr;
        int hitBoxSet = -1;
        StudioHitboxSet* hitBoxSetPtr = nullptr;
    };

    typedef std::vector<HitboxPoints> Multipoint;






    bool MultipointSetup(Entity* entity, EntityStudioInfo& save, matrix3x4* bonesptr = nullptr, bool MatrixPassed = false);
    bool retrieveHitbox(EntityStudioInfo& save, HitboxPoints& CurrHitbox, PointScales scales, Hitbox hitBox, float secondExpan = -1.f, bool fetchCenterOnly = false);

    bool retrieveSafePoint(EntityStudioInfo& save, HitboxPoints& CurrHitbox, PointScales scales, Hitbox hitBox, matrix3x4* matrix2);




    bool retrieveAll(Entity* entity, float multiPointsExpansion, float secondExpan, Vector(&multiPoints)[Multipoints::HITBOX_MAX][Multipoints::MULTIPOINTS_MAX], matrix3x4* bonesptr = nullptr, bool MatrixPassed = false);
    bool retrieveAll(Entity* entity, float expansion, PointScales scales, Multipoint& points, matrix3x4* bonesptr = nullptr, bool MatrixPassed = false);
    bool retrieveAll(Entity* entity, Multipoint& points, PointScales scales, std::vector<Hitbox> hitboxes, float secondExpan = 0.0f, matrix3x4* bonesptr = nullptr, bool MatrixPassed = false);
    bool retrieveOne(Entity* entity, float multiPointsExpansion, Vector(&multiPoints)[Multipoints::MULTIPOINTS_MAX], int desiredHitBox = Multipoints::HITBOX_HEAD);






};