#include "../../OsirisSDK/Vector.h"

namespace VectorMath{
	Vector calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept;
	Vector calculateAngle(const Vector& source, const Vector& destination) noexcept;
	Vector normalize(Vector v);
	float dotProduct(Vector a, Vector b);


}