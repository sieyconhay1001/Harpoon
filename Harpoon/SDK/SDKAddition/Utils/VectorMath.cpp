#include "VectorMath.h"
#include "../../OsirisSDK/Angle.h"
#include "../../OsirisSDK/Utils.h"
#include "../../OsirisSDK/EngineTrace.h"
#include <math.h>

float DEG2RAD(float degree) {
    return (float)(degree * 22.0 / (180.0 * 7.0));
}

Vector VectorMath::calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept
{
    Vector delta = destination - source;
    Vector angles{ radiansToDegrees(atan2f(-delta.z, std::hypotf(delta.x, delta.y))) - viewAngles.x,
                   radiansToDegrees(atan2f(delta.y, delta.x)) - viewAngles.y };
    angles.normalize();
    return angles;
}
#include <float.h>

Vector VectorMath::calculateAngle(const Vector& source, const Vector& destination) noexcept
{
    Vector delta = destination - source;
    Vector angles{ radiansToDegrees(atan2f(-delta.z, std::hypotf(delta.x, delta.y))),
                   radiansToDegrees(atan2f(delta.y, delta.x))};
    angles.normalize();
    return angles;
}







Vector VectorMath::normalize(Vector v)
{
    float len2 = v.x * v.x + v.y * v.y + v.z * v.z;
    // avoid division by 0
    if (len2 > 0) {
        float invLen = 1 / sqrt(len2);
        v.x *= invLen, v.y *= invLen, v.z *= invLen;
    }
    return v;
}

float VectorMath::dotProduct(Vector a, Vector b) {
    return (a.x * b.x + a.y * b.y + a.z * b.z);

} 


