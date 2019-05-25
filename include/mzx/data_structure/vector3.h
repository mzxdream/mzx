#ifndef __MZX_VECTOR3_H__
#define __MZX_VECTOR3_H__

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
        return x_ == v.x_ && y_ == v.y_ && z_ == v.z_;
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

private:
    T x_;
    T y_;
    T z_;
};

} // namespace mzx

#endif