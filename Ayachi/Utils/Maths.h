#pragma once
#define PI (3.1415927f)
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

static constexpr float DEG_RAD2 = PI / 360.0f;
static constexpr float DEG_RAD = 180.0f / PI;
static constexpr float RAD_DEG = PI / 180.f;

template<typename T>
struct Vec2 {
	union {
		struct {
			T x, y;
		};
		T arr[2];
	};

	Vec2(T x = 0, T y = 0) {
		this->x = x;
		this->y = y;
	}

	ImVec2 toImVec2() {
		return ImVec2((float)x, (float)y);
	}

	Vec2<T> add(const Vec2<T>& o) const {
		return Vec2<T>(x + o.x, y + o.y);
	}

	Vec2<T> sub(const Vec2<T>& o) const {
		return Vec2<T>(x - o.x, y - o.y);
	}

	Vec2<T> mul(const Vec2<T>& o) const {
		return Vec2<T>(x * o.x, y * o.y);
	}

	Vec2<T> div(const Vec2<T>& o) const {
		return Vec2<T>(x / o.x, y / o.y);
	}

	template<typename Y>
	Vec2<Y> CastTo() const {
		return Vec2<Y>((Y)x, (Y)y);
	}
};

template<typename T>
struct Vec3 {

	union {
		struct {
			T x, y, z;
		};
		T arr[3];
	};

	Vec3(T x = 0, T y = 0, T z = 0) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	bool operator==(const Vec3<T>& other) const {
		return (x == other.x && y == other.y && z == other.z);
	}
	Vec3<T> operator-(const Vec3<T> other) const {
		return Vec3<T>(x - other.x, y - other.y, z - other.z);
	}
	Vec3<T> add(const Vec3<T>& o) const {
		return Vec3<T>(x + o.x, y + o.y, z + o.z);
	}

	Vec3<T> add(const T o) const {
		return Vec3<T>(x + o, y + o, z + o);
	}

	Vec3<T> add(const T _x, const T _y, const T _z) const {
		return Vec3<T>(x + _x, y + _y, z + _z);
	}

	Vec3<T> sub(const Vec3<T>& o) const {
		return Vec3<T>(x - o.x, y - o.y, z - o.z);
	}

	Vec3<T> sub(const T o) const {
		return Vec3<T>(x - o, y - o, z - o);
	}

	Vec3<T> sub(const T _x, const T _y, const T _z) const {
		return Vec3<T>(x - _x, y - _y, z - _z);
	}

	Vec3<T> mul(const Vec3<T>& o) const {
		return Vec3<T>(x * o.x, y * o.y, z * o.z);
	}

	Vec3<T> div(const Vec3<T>& o) const {
		return Vec3<T>(x / o.x, y / o.y, z / o.z);
	}

	Vec3<T> floor() const {
		return Vec3<T>(floorf(x), floorf(y), floorf(z));
	};

	float squaredlen() const {
		return (float)(x * x + y * y + z * z);
	}

	float squaredxzlen() const {
		return (float)(x * x + z * z);
	}

	float magnitude() const {
		return std::sqrtf(squaredlen());
	}

	float dist(const Vec3<T>& v3) const {
		return this->sub(v3).magnitude();
	}

	Vec3<T> normalize() const {
		const float& mag = this->magnitude();
		return Vec3<T>(this->x / mag, this->y / mag, this->z / mag);
	}
	Vec3<T> lerp(const Vec3<T>& other, float tx, float ty, float tz) const {
		Vec3<T> result;
		result.x = x + tx * (other.x - x);
		result.y = y + ty * (other.y - y);
		result.z = z + tz * (other.z - z);
		return result;
	}

	Vec2<float> CalcAngle(const Vec3<float>& dst) {
		Vec3<float> diff = dst.sub(*this);

		diff.y = diff.y / diff.magnitude();
		Vec2<float> angles;
		angles.x = asinf(diff.y) * -DEG_RAD;
		angles.y = (float)-atan2f(diff.x, diff.z) * DEG_RAD;

		return angles;
	}

	template<typename Y>
	Vec3<Y> CastTo() const {
		return Vec3<Y>((Y)x, (Y)y, (Y)z);
	}

	glm::vec3 toGlm() const {
		return glm::vec3(x, y, z);
	}
};

namespace std {
	template <>
	struct hash<Vec3<int>> {
		size_t operator()(const Vec3<int>& key) const {
			return std::hash<int>()(key.x) ^ std::hash<int>()(key.y) ^ std::hash<int>()(key.z);
		}
	};
}

template<typename T>
struct Vec4 {

	union {
		struct {
			T x, y, z, w;
		};
		T arr[4];
	};

	Vec4(T x = 0, T y = 0, T z = 0, T w = 0) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	inline bool contains(const Vec2<float>& point) {
		if (point.x < x || point.y < y)
			return false;

		if (point.x > z || point.y > w)
			return false;
		return true;
	}
};

struct AABB {
	Vec3<float> lower;
	Vec3<float> upper;

	bool operator==(const AABB& other) const {
		return (lower == other.lower && upper == other.upper);
	}

	Vec3<float> getCenter() const {
		return (lower.add(upper)).div(Vec3<float>(2.f, 2.f, 2.f));
	}

	bool intersects(const AABB& aabb) const {
		return aabb.upper.x > lower.x && upper.x > aabb.lower.x &&
			aabb.upper.y > lower.y && upper.y > aabb.lower.y &&
			aabb.upper.z > lower.z && upper.z > aabb.lower.z;
	}

	bool intersectsY(const AABB& aabb) const {
		return aabb.upper.y > lower.y && upper.y > aabb.lower.y;
	}

	bool intersectsXZ(const AABB& aabb) const {
		return aabb.upper.x > lower.x && upper.x > aabb.lower.x &&
			aabb.upper.z > lower.z && upper.z > aabb.lower.z;
	}
};

typedef Vec3<int> BlockPos;

class RayTrace {
public:
	enum class Result : uint8_t {
		Null = 0,
		Good = 1
	};
    Vec3<float> startPos;
    Vec3<float> endPos;
	Result mResult;
	int blockCount;
	BlockPos lastSolidBlock;
};

enum class Facing : uint8_t {
	North = 0,
	South = 1,
	East = 2,
	West = 3,
	Down = 4,
	Up
};

namespace Math {
	inline float lerp(float start, float end, float t) {
		return start + ((end - start) * t);
	}

	inline float wrap(float value, float min, float max) {
		return fmod(fmod(value - min, max - min) + (max - min), max - min) + min;
	}

	inline int randomInt(int start, int end) {
		return rand() % (end - start + 1) + start;
	}

	inline float randomFloat(float a, float b) {
		float random = ((float)rand()) / (float)RAND_MAX;
		float diff = b - a;
		float r = random * diff;
		return a + r;
	}

	inline static float animate(float endPoint, float current,
		float speed) {
		if (speed < 0.0)
			speed = 0.0;
		else if (speed > 1.0)
			speed = 1.0;

		float dif = std::fmax(endPoint, current) -
			std::fmin(endPoint, current);
		float factor = dif * speed;
		return current + (endPoint > current ? factor : -factor);
	}
}