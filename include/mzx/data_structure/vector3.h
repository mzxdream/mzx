#ifndef __MZX_VECTOR3_H__
#define __MZX_VECTOR3_H__

#include <mzx/algorithm/math.h>

namespace mzx
{

template <typename T>
class Vector3
{
public:
    explicit Vector3(T x, T y, T z)
        : x_(x)
        , y_(y)
        , z_(z)
    {
    }
    Vector3()
        : x_()
        , y_()
        , z_()
    {
    }

public:
    T X() const
    {
        return x_;
    }
    T Y() const
    {
        return y_;
    }
    T Z() const
    {
        return z_;
    }
    void X(T x)
    {
        x_ = x;
    }
    void Y(T y)
    {
        y_ = y;
    }
    void Z(T z)
    {
        z_ = z;
    }

public:
    bool operator==(const Vector3<T> &v) const
    {
        return MZX_ISEQUAL(x_, v.x_) && MZX_ISEQUAL(y_, v.y_) &&
               MZX_ISEQUAL(z_, v.z_);
    }
    bool operator!=(const Vector3<T> &v) const
    {
        return !(*this == v);
    }
    Vector3<T> operator-() const
    {
        return Vector3(-x_, -y_, -z_);
    }
    Vector3<T> &operator=(const Vector3<T> &v)
    {
        x_ = v.x_;
        y_ = v.y_;
        z_ = v.z_;
        return *this;
    }
    Vector3<T> &operator+=(const Vector3<T> &v)
    {
        x_ += v.x_;
        y_ += v.y_;
        z_ += v.z_;
        return *this;
    }
    Vector3<T> &operator-=(const Vector3<T> &v)
    {
        x_ -= v.x_;
        y_ -= v.y_;
        z_ -= v.z_;
        return *this;
    }
    Vector3<T> &operator*=(const Vector3<T> &v)
    {
        x_ *= v.x_;
        y_ *= v.y_;
        z_ *= v.z_;
        return *this;
    }
    Vector3<T> operator+(const Vector3<T> &v) const
    {
        return Vector3<T>(x_ + v.x_, y_ + v.y_, z_ + v.z_);
    }
    Vector3<T> operator-(const Vector3<T> &v) const
    {
        return Vector3<T>(x_ - v.x_, y_ - v.y_, z_ - v.z_);
    }
    Vector3<T> operator*(const Vector3<T> &v) const
    {
        return Vector3<T>(x_ * v.x_, y_ * v.y_, z_ * v.z_);
    }
    Vector3<T> &operator*=(T d)
    {
        x_ *= d;
        y_ *= d;
        z_ *= d;
        return *this;
    }
    Vector3<T> &operator/=(T d)
    {
        x_ /= d;
        y_ /= d;
        z_ /= d;
    }
    Vector3<T> operator*(T d) const
    {
        return Vector3<T>(x_ * d, y_ * d, z_ * d);
    }
    Vector3<T> operator/(T d) const
    {
        return Vector3<T>(x_ / d, y_ / d, z_ / d);
    }
    T SqrtMagnitude() const
    {
        return x_ * x_ + y_ * y_ + z_ * z_;
    }
    T Magnitude() const
    {
        auto sqrt_magnitude = SqrtMagnitude();
        return MZX_ISZERO(sqrt_magnitude)
                   ? 0
                   : static_cast<T>(sqrt(sqrt_magnitude));
    }
    bool Normalize()
    {
        auto len = Magnitude();
        if (MZX_ISZERO(len))
        {
            return false;
        }
        x_ /= len;
        y_ /= len;
        z_ /= len;
        return true;
    }
    static Vector3<T> Normalize(const Vector3<T> &v)
    {
        auto len = v.Magnitude();
        if (MZX_ISZERO(len))
        {
            return Vector3<T>::ZERO;
        }
        return Vector3<T>(v.x_ / len, v.y_ / len, v.z_ / len);
    }
    static T Distance(const Vector3<T> &v, const Vector3<T> &u)
    {
        return (u - v).Magnitude();
    }

private:
    T x_;
    T y_;
    T z_;

public:
    static const Vector3<T> ZERO;
};

template <typename T>
const Vector3<T> Vector3<T>::ZERO = Vector3<T>(0, 0, 0);

} // namespace mzx

#endif
