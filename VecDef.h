#pragma once
#include <cmath>
#include <cassert>

// Vector2 class template
template <typename T>
class Vector2
{
public:
    // Member data
    T x{};
    T y{};

    // Constructors
    constexpr Vector2() = default;
    constexpr Vector2(T x, T y) : x(x), y(y) {}

    // Type conversion
    template <typename U>
    constexpr explicit operator Vector2<U>() const
    {
        return Vector2<U>(static_cast<U>(x), static_cast<U>(y));
    }

    // Vector operations
    constexpr T lengthSquared() const
    {
        return x * x + y * y;
    }

    T length() const
    {
        return static_cast<T>(std::sqrt(lengthSquared()));
    }

    Vector2<T> normalized() const
    {
        T len = length();
        assert(len != 0 && "Cannot normalize zero-length vector");
        return Vector2<T>(x / len, y / len);
    }

    constexpr Vector2<T> perpendicular() const
    {
        return Vector2<T>(-y, x);
    }

    constexpr T dot(Vector2<T> rhs) const
    {
        return x * rhs.x + y * rhs.y;
    }

    constexpr T cross(Vector2<T> rhs) const
    {
        return x * rhs.y - y * rhs.x;
    }

    constexpr Vector2<T> componentWiseMul(Vector2<T> rhs) const
    {
        return Vector2<T>(x * rhs.x, y * rhs.y);
    }

    constexpr Vector2<T> componentWiseDiv(Vector2<T> rhs) const
    {
        assert(rhs.x != 0 && "Cannot divide by zero");
        assert(rhs.y != 0 && "Cannot divide by zero");
        return Vector2<T>(x / rhs.x, y / rhs.y);
    }

    constexpr T distanceSquaredTo(const Vector2<T>& rhs) const
    {
        const T dx = x - rhs.x;
        const T dy = y - rhs.y;
        return dx * dx + dy * dy;
    }

    template <typename T>
    Vector2<T>& Normalize()
    {
        T len = length();
        assert(len != 0 && "Cannot normalize zero-length vector");
        x /= len;
        y /= len;
        return *this;
    }

    template <typename T>
    constexpr T DotProduct(const Vector2<T>& rhs) const
    {
        return dot(rhs);
    }

    T distanceTo(const Vector2<T>& rhs) const
    {
        return std::sqrt(distanceSquaredTo(rhs));
    }
};

// Vector3 class template
template <typename T>
class Vector3
{
public:
    // Member data
    T x{};
    T y{};
    T z{};

    // Constructors
    constexpr Vector3() = default;
    constexpr Vector3(T x, T y, T z) : x(x), y(y), z(z) {}

    // Type conversion
    template <typename U>
    constexpr explicit operator Vector3<U>() const
    {
        return Vector3<U>(static_cast<U>(x), static_cast<U>(y), static_cast<U>(z));
    }

    // Vector operations
    constexpr T lengthSquared() const
    {
        return x * x + y * y + z * z;
    }

    constexpr bool empty() const
    {
        return x == 0 && y == 0 && z == 0;
    }

    T length() const
    {
        return static_cast<T>(std::sqrt(lengthSquared()));
    }

    Vector3<T> normalized() const
    {
        T len = length();
        assert(len != 0 && "Cannot normalize zero-length vector");
        return Vector3<T>(x / len, y / len, z / len);
    }

    constexpr T dot(const Vector3<T>& rhs) const
    {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }

    constexpr Vector3<T> cross(const Vector3<T>& rhs) const
    {
        return Vector3<T>(
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x
        );
    }

    constexpr Vector3<T> componentWiseMul(const Vector3<T>& rhs) const
    {
        return Vector3<T>(x * rhs.x, y * rhs.y, z * rhs.z);
    }

    constexpr Vector3<T> componentWiseDiv(const Vector3<T>& rhs) const
    {
        assert(rhs.x != 0 && "Cannot divide by zero");
        assert(rhs.y != 0 && "Cannot divide by zero");
        assert(rhs.z != 0 && "Cannot divide by zero");
        return Vector3<T>(x / rhs.x, y / rhs.y, z / rhs.z);
    }

    constexpr T distanceSquared2dTo(const Vector3<T>& rhs) const
    {
        const T dx = x - rhs.x;
        const T dy = y - rhs.y;
        return dx * dx + dy * dy;
    }

    constexpr T distanceSquaredTo(const Vector3<T>& rhs) const
    {
        const T dx = x - rhs.x;
        const T dy = y - rhs.y;
        const T dz = z - rhs.z;
        return dx * dx + dy * dy + dz * dz;
    }

    T distance2dTo(const Vector3<T>& rhs) const
    {
        return std::sqrt(distanceSquared2dTo(rhs));
    }

    T distanceTo(const Vector3<T>& rhs) const
    {
        return std::sqrt(distanceSquaredTo(rhs));
    }

    Vector3<T>& Normalize()
    {
        T len = length();
        assert(len != 0 && "Cannot normalize zero-length vector");
        x /= len;
        y /= len;
        z /= len;
        return *this;
    }

    constexpr T DotProduct(const Vector3<T>& rhs) const
    {
        return dot(rhs);
    }
};

// Vector4 class
class Vector4
{
public:
    // Member data
    float x{};
    float y{};
    float z{};
    float w{};

    // Constructors
    constexpr Vector4() = default;
    constexpr Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    // Construct from pointers
    Vector4(float* px, float* py, float* pz, float* pw)
        : x(px ? *px : 0), y(py ? *py : 0), z(pz ? *pz : 0), w(pw ? *pw : 0) {
    }

    // Vector operations
    constexpr float lengthSquared() const
    {
        return x * x + y * y + z * z + w * w;
    }

    float length() const
    {
        return std::sqrt(lengthSquared());
    }

    Vector4 normalized() const
    {
        float len = length();
        assert(len != 0 && "Cannot normalize zero-length vector");
        return Vector4(x / len, y / len, z / len, w / len);
    }

    constexpr float dot(const Vector4& rhs) const
    {
        return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
    }
};

// Define common vector types
using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;
using Vector2u = Vector2<unsigned int>;

using Vector3f = Vector3<float>;
using Vector3i = Vector3<int>;
using Vector3u = Vector3<unsigned int>;

using Vector4f = Vector4;

// Define vector aliases to match your previous code
using Vector2D = Vector2f;
using Vector3D = Vector3f;
using Vec2D = Vector2f;
using Vec3D = Vector3f;
using Vec4D = Vector4f;

//------------------------------------------------------------------------
// Operator overloads for Vector2
//------------------------------------------------------------------------

// Unary negation
template <typename T>
constexpr Vector2<T> operator-(Vector2<T> right)
{
    return Vector2<T>(-right.x, -right.y);
}

// Addition
template <typename T>
constexpr Vector2<T>& operator+=(Vector2<T>& left, Vector2<T> right)
{
    left.x += right.x;
    left.y += right.y;
    return left;
}

template <typename T>
constexpr Vector2<T> operator+(Vector2<T> left, Vector2<T> right)
{
    return Vector2<T>(left.x + right.x, left.y + right.y);
}

// Subtraction
template <typename T>
constexpr Vector2<T>& operator-=(Vector2<T>& left, Vector2<T> right)
{
    left.x -= right.x;
    left.y -= right.y;
    return left;
}

template <typename T>
constexpr Vector2<T> operator-(Vector2<T> left, Vector2<T> right)
{
    return Vector2<T>(left.x - right.x, left.y - right.y);
}

// Scalar multiplication
template <typename T>
constexpr Vector2<T> operator*(Vector2<T> left, T right)
{
    return Vector2<T>(left.x * right, left.y * right);
}

template <typename T>
constexpr Vector2<T> operator*(T left, Vector2<T> right)
{
    return Vector2<T>(right.x * left, right.y * left);
}

template <typename T>
constexpr Vector2<T>& operator*=(Vector2<T>& left, T right)
{
    left.x *= right;
    left.y *= right;
    return left;
}

// Scalar division
template <typename T>
constexpr Vector2<T> operator/(Vector2<T> left, T right)
{
    assert(right != 0 && "Cannot divide by zero");
    return Vector2<T>(left.x / right, left.y / right);
}

template <typename T>
constexpr Vector2<T>& operator/=(Vector2<T>& left, T right)
{
    assert(right != 0 && "Cannot divide by zero");
    left.x /= right;
    left.y /= right;
    return left;
}

// Comparison
template <typename T>
constexpr bool operator==(Vector2<T> left, Vector2<T> right)
{
    return (left.x == right.x) && (left.y == right.y);
}

template <typename T>
constexpr bool operator!=(Vector2<T> left, Vector2<T> right)
{
    return !(left == right);
}

//------------------------------------------------------------------------
// Operator overloads for Vector3
//------------------------------------------------------------------------

// Unary negation
template <typename T>
constexpr Vector3<T> operator-(const Vector3<T>& right)
{
    return Vector3<T>(-right.x, -right.y, -right.z);
}

// Addition
template <typename T>
constexpr Vector3<T>& operator+=(Vector3<T>& left, const Vector3<T>& right)
{
    left.x += right.x;
    left.y += right.y;
    left.z += right.z;
    return left;
}

template <typename T>
constexpr Vector3<T> operator+(const Vector3<T>& left, const Vector3<T>& right)
{
    return Vector3<T>(left.x + right.x, left.y + right.y, left.z + right.z);
}

// Subtraction
template <typename T>
constexpr Vector3<T>& operator-=(Vector3<T>& left, const Vector3<T>& right)
{
    left.x -= right.x;
    left.y -= right.y;
    left.z -= right.z;
    return left;
}

template <typename T>
constexpr Vector3<T> operator-(const Vector3<T>& left, const Vector3<T>& right)
{
    return Vector3<T>(left.x - right.x, left.y - right.y, left.z - right.z);
}

// Scalar multiplication
template <typename T>
constexpr Vector3<T> operator*(const Vector3<T>& left, T right)
{
    return Vector3<T>(left.x * right, left.y * right, left.z * right);
}

template <typename T>
constexpr Vector3<T> operator*(T left, const Vector3<T>& right)
{
    return Vector3<T>(right.x * left, right.y * left, right.z * left);
}

template <typename T>
constexpr Vector3<T>& operator*=(Vector3<T>& left, T right)
{
    left.x *= right;
    left.y *= right;
    left.z *= right;
    return left;
}

// Scalar division
template <typename T>
constexpr Vector3<T> operator/(const Vector3<T>& left, T right)
{
    assert(right != 0 && "Cannot divide by zero");
    return Vector3<T>(left.x / right, left.y / right, left.z / right);
}

template <typename T>
constexpr Vector3<T>& operator/=(Vector3<T>& left, T right)
{
    assert(right != 0 && "Cannot divide by zero");
    left.x /= right;
    left.y /= right;
    left.z /= right;
    return left;
}

// Comparison
template <typename T>
constexpr bool operator==(const Vector3<T>& left, const Vector3<T>& right)
{
    return (left.x == right.x) && (left.y == right.y) && (left.z == right.z);
}

template <typename T>
constexpr bool operator!=(const Vector3<T>& left, const Vector3<T>& right)
{
    return !(left == right);
}

//------------------------------------------------------------------------
// Operator overloads for Vector4
//------------------------------------------------------------------------

// Unary negation
constexpr Vector4 operator-(const Vector4& right)
{
    return Vector4(-right.x, -right.y, -right.z, -right.w);
}

// Addition
constexpr Vector4& operator+=(Vector4& left, const Vector4& right)
{
    left.x += right.x;
    left.y += right.y;
    left.z += right.z;
    left.w += right.w;
    return left;
}

constexpr Vector4 operator+(const Vector4& left, const Vector4& right)
{
    return Vector4(left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w);
}

// Subtraction
constexpr Vector4& operator-=(Vector4& left, const Vector4& right)
{
    left.x -= right.x;
    left.y -= right.y;
    left.z -= right.z;
    left.w -= right.w;
    return left;
}

constexpr Vector4 operator-(const Vector4& left, const Vector4& right)
{
    return Vector4(left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w);
}

// Scalar multiplication
constexpr Vector4 operator*(const Vector4& left, float right)
{
    return Vector4(left.x * right, left.y * right, left.z * right, left.w * right);
}

constexpr Vector4 operator*(float left, const Vector4& right)
{
    return Vector4(right.x * left, right.y * left, right.z * left, right.w * left);
}

constexpr Vector4& operator*=(Vector4& left, float right)
{
    left.x *= right;
    left.y *= right;
    left.z *= right;
    left.w *= right;
    return left;
}

// Scalar division
constexpr Vector4 operator/(const Vector4& left, float right)
{
    assert(right != 0 && "Cannot divide by zero");
    return Vector4(left.x / right, left.y / right, left.z / right, left.w / right);
}

constexpr Vector4& operator/=(Vector4& left, float right)
{
    assert(right != 0 && "Cannot divide by zero");
    left.x /= right;
    left.y /= right;
    left.z /= right;
    left.w /= right;
    return left;
}

// Comparison
constexpr bool operator==(const Vector4& left, const Vector4& right)
{
    return (left.x == right.x) && (left.y == right.y) && (left.z == right.z) && (left.w == right.w);
}

constexpr bool operator!=(const Vector4& left, const Vector4& right)
{
    return !(left == right);
}

#define DEG2RAD(x) ((x) * 0.01745329251f)
#define RAD2DEG(x) ((x) * 57.2957795131f)
#define M_PI 3.14159265358979323846f
#define DEG2RAD_CONST 0.01745329251f;
inline constexpr float RAD2DEG_CONST = 57.2957795131f;