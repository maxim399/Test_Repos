#ifndef IIPCT_POINT_H
#define IIPCT_POINT_H

#include "Utility/Headers/serialization.hpp"
#include <vector>
#include <cmath>

template <typename Type = float>
struct Point
{
    Point() : x_(0), y_(0) {}

    Point(Type x, Type y) : x_(x), y_(y) {}

    template<typename N>
    Point(const Point<N> &in) :  x_(in.x_), y_(in.y_) {}

    Point& operator=(const Point &other)
    {
        Point temp(other);
        swap(temp);
        return *this;
    }

    Point& operator+=(const Point &other)
    {
        x_ += other.x_;
        y_ += other.y_;
        return *this;
    }

    template<typename T, typename = typename  std::enable_if_t<std::is_arithmetic<T>::value, T>>
    Point& operator+=(T other)
    {
        x_ += other;
        y_ += other;
        return *this;
    }

    Point& operator-=(const Point &other)
    {
        x_ -= other.x_;
        y_ -= other.y_;
        return *this;
    }

    template<typename T, typename = typename  std::enable_if_t<std::is_arithmetic<T>::value, T>>
    Point& operator-=(T other)
    {
        x_ -= other;
        y_ -= other;
        return *this;
    }

    Point& operator*=(const Point &other)
    {
        x_ *= other.x_;
        y_ *= other.y_;
        return *this;
    }

    template<typename T, typename = typename  std::enable_if_t<std::is_arithmetic<T>::value, T>>
    Point& operator*=(T other)
    {
        x_ *= other;
        y_ *= other;
        return *this;
    }

    Point& operator/=(const Point &other)
    {
        x_ /= other.x_;
        y_ /= other.y_;
        return *this;
    }

    template<typename T, typename = typename  std::enable_if_t<std::is_arithmetic<T>::value, T>>
    Point& operator/=(T other)
    {
        x_ /= other;
        y_ /= other;
        return *this;
    }

    void reset()
    {
        x_ = 0;
        y_ = 0;
    }

    Type x() const
    {
        return x_;
    }

    Type y() const
    {
        return y_;
    }

    Type distance(const Point &target) const
    {
        return sqrtf(powf(x_ - target.x_, 2.0f) + powf(y_ - target.y_, 2.0f));
    }

    Type length() const
    {
        return sqrtf(lengthSquare());
    }

    Type lengthSquare() const
    {
        return (x_)*(x_)+(y_)*(y_);
    }

    Type distanceSquare(const Point &target) const
    {
        return powf(x_ - target.x_, 2.0f) + powf(y_ - target.y_, 2.0f);
    }

    Type distanceSquare(const Type target_x, const Type target_y)
    {
        return powf(x_ - target_x, 2.0f) + powf(y_ - target_y, 2.0f);
    }

    Type angle(const Point &target) const
    {
        return static_cast<Type>(std::atan2(static_cast<float>(target.y_ - y_), static_cast<float>(target.x_ - x_)) * 180.0f / M_PI);
    }

    void swap(Point& other) noexcept {
        using std::swap;
        swap(x_, other.x_);
        swap(y_, other.y_);
    }

    void serialize(std::ostream& out) const
    {
        using ::serialize;
        serialize(out, x_);
        serialize(out, y_);
    }

    void deserialize(std::istream& in)
    {
        using ::deserialize;
        Point tmp;
        deserialize(in, tmp.x_);
        deserialize(in, tmp.y_);
        swap(tmp);
    }

    void transform(std::vector<double> & transformationMatrix)
    {
        if (transformationMatrix.size() != 9)
        {
            return;
        }

        double projectionPointZ = transformationMatrix[6] * x_ + transformationMatrix[7] * y_ + transformationMatrix[8];
        double projectionPointX = (transformationMatrix[0] * x_ + transformationMatrix[1] * y_ + transformationMatrix[2]) / projectionPointZ;
        double projectionPointY = (transformationMatrix[3] * x_ + transformationMatrix[4] * y_ + transformationMatrix[5]) / projectionPointZ;

        x_ = static_cast<Type>(projectionPointX);
        y_ = static_cast<Type>(projectionPointY);
    }

    Type x_;
    Type y_;
};


template<typename T, typename N>
inline auto operator+(const Point<T> &a, const Point<N> &b)
{
    return Point(a.x_ + b.x_, a.y_ + b.y_);
}

template<typename T, typename N, typename = typename std::enable_if<std::is_arithmetic<N>::value, N>::type>
inline auto operator+(const Point<T> &a, const N b)
{
    return Point(a.x_ + b, a.y_ + b);
}

template<typename T, typename N>
inline auto operator-(const Point<T> &a, const Point<N> &b)
{
    return Point(a.x_ - b.x_, a.y_ - b.y_);
}

template<typename T, typename N, typename = typename std::enable_if<std::is_arithmetic<N>::value, N>::type>
inline auto operator-(const Point<T> &a, const N b)
{
    return Point(a.x_ - b, a.y_ - b);
}

template<typename T, typename N>
inline auto operator*(const Point<T> &a, const Point<N> &b)
{
    return Point(a.x_ * b.x_, a.y_ * b.y_);
}

template<typename T, typename N, typename = typename std::enable_if<std::is_arithmetic<N>::value, N>::type>
inline auto operator*(const Point<T> &a, const N b)
{
    return Point(a.x_ * b, a.y_ * b);
}

template<typename T, typename N>
inline auto operator/(const Point<T> &a, const Point<N> &b)
{
    return Point(a.x_ / b.x_, a.y_ / b.y_);
}

template<typename T, typename N, typename = typename std::enable_if<std::is_arithmetic<N>::value, N>::type>
inline auto operator/(const Point<T> &a, const N b)
{
    return Point(a.x_ / b, a.y_ / b);
}

template<typename T, typename N>
inline bool operator==(const Point<T> &a, const Point<N> &b)
{
    return a.x_ == b.x_ && a.y_ == b.y_;
}

template<typename T, typename N>
inline bool operator!=(const Point<T> &a, const Point<N> &b)
{
    return a.x_ != b.x_ || a.y_ != b.y_;
}

template<typename T>
inline void swap(Point<T>& obj1, Point<T>& obj2) {
    obj1.swap(obj2);
}

template<typename T>
void transform(Point<T> &point, std::vector<double> & matrix)
{
    if (matrix.size() != 9)
        return;

    double projection_z = matrix[6] * point.x_ + matrix[7] * point.y_ + matrix[8];
    double projection_x = (matrix[0] * point.x_ + matrix[1] * point.y_ + matrix[2]) / projection_z;
    double projection_y = (matrix[3] * point.x_ + matrix[4] * point.y_ + matrix[5]) / projection_z;

    point.set(projection_x,  projection_y);
}

#endif //IIPCT_POINT_H
