#ifndef PRIMITIVES_ComplexImage_H_
#define PRIMITIVES_ComplexImage_H_
#include <vector>
#include <algorithm>
#include <functional>
#include <complex>
#include "assert.h"
#include <cstring>

#include "Utility/Headers/serialization.hpp"

class ComplexImage
{
public:

    ComplexImage() : width_(0), height_(0) {}
    ComplexImage(size_t width, size_t height) : width_(width), height_(height)
    {
        data_.resize(width_ * height_);
    }

    ComplexImage(ComplexImage &input) : width_(input.width_), height_(input.height_), data_(input.data_)
    {
    }

    ComplexImage(const ComplexImage &input) : width_(input.width_), height_(input.height_), data_(input.data_)
    {
    }


    ComplexImage(std::vector<std::complex<float>> & input, size_t width, size_t height) : width_(width), height_(height)
    {
        data_ = input;
    }

    ComplexImage(float * input, size_t width, size_t height) : width_(width), height_(height)
    {
        data_.resize(width_ * height_);
        for (auto &it : data_)
           it = std::complex<float>(*input++, 0.0f);
    }

    void swap(ComplexImage& other)
    {
        using std::swap;
        swap(width_,   other.width_);
        swap(height_, other.height_);
        swap(data_,   other.data_);
    }

    void serialize(std::ostream& out) const
    {
        using ::serialize;
        serialize(out, width_);
        serialize(out, height_);
        serialize(out, data_);
    }

    void deserialize(std::istream& in)
    {
        using ::deserialize;
        ComplexImage tmp;
        deserialize(in, tmp.width_);
        deserialize(in, tmp.height_);
        deserialize(in, tmp.data_);
        swap(tmp);
    }

    void clear()
    {
        data_.clear();
        width_ = 0;
        height_ = 0;
    }


    float squareNorm() const
    {
        float sumSquareNorm = 0.0f;

        for (auto &lhs :data_)
            sumSquareNorm += lhs.real() * lhs.real() + lhs.imag() * lhs.imag();

        return sumSquareNorm /(width_ * height_);
    }

    ComplexImage squareMagnitude() const
    {
        return ComplexOperatorConst([](std::complex<float> & c) { c = c.real() * c.real() + c.imag() * c.imag(); });
    }

    ComplexImage conjugate() const
    {
        return ComplexOperatorConst([](std::complex<float> & c) { c = std::complex<float>(c.real(), -c.imag()); });
    }

    ComplexImage sumOverChannels() const
    {
        ComplexImage result(width_, height_);
        result.data_ = data_;
        transform(result.data_.begin(), result.data_.end(), data_.begin(), result.data_.begin(), std::plus<std::complex<float>>());
        return result;
    }

    size_t width() const
    {
        return width_;
    }

    size_t height() const
    {
        return height_;
    }

    size_t size() const
    {
        return data_.size();
    }

    std::complex<float>& operator[](const size_t index)
    {
        return data_[index];
    }

    auto begin()
    {
        return data_.begin();
    }

    auto end()
    {
        return data_.end();
    }

	// @brief Element-wise per channel multiplication, division and addition.
    ComplexImage operator*(const ComplexImage & rhs) const
	{
        return ComplexOperator([](std::complex<float> &clhs, const std::complex<float> &crhs) { clhs *= crhs; }, rhs);
	}

    ComplexImage operator/(const ComplexImage & rhs) const
	{
        return ComplexOperator([](std::complex<float> &clhs, const std::complex<float> &crhs) { clhs /= crhs; }, rhs);
	}

    ComplexImage operator+(const ComplexImage & rhs) const
	{
        return ComplexOperator([](std::complex<float> &clhs, const std::complex<float> &crhs) { clhs += crhs; }, rhs);
	}

	// @ Multiplying or adding constant.
    ComplexImage operator*(const float & rhs) const
	{
        return ComplexOperatorConst([&rhs](std::complex<float> & c) { c *= rhs; });
	}

    ComplexImage operator+(const float & rhs) const
	{
        return ComplexOperatorConst([&rhs](std::complex<float> & c) { c += rhs; });
	}

    // @brief Multiplying element-wise multichannel by one channel mats (rhs mat is with one channel).
    ComplexImage multiplying(const ComplexImage & rhs) const
    {
        return ComplexOperatorZero([](std::complex<float> & clhs, const std::complex<float> & crhs) { clhs *= crhs; }, rhs);
    }

private:

    size_t width_;
    size_t height_;
    std::vector<std::complex<float>> data_;

    ComplexImage ComplexOperator(void(*op)(std::complex<float> & clhs, const std::complex<float> & crhs), const ComplexImage & matrhs) const
	{
        assert(matrhs.width_ == width_ && matrhs.height_ == height_);
        ComplexImage result = *this;

        auto lhs = result.data_.begin();
        auto rhs = matrhs.data_.begin();
        for (; lhs != result.data_.end(); ++lhs, ++rhs)
            op(*lhs, *rhs);

		return result;
	}

    ComplexImage ComplexOperatorZero(void(*op)(std::complex<float> & clhs, const std::complex<float> & crhs), const ComplexImage & matrhs) const
	{
        assert(matrhs.width_ == width_ && matrhs.height_ == height_);
        ComplexImage result = *this;

        auto lhs = result.data_.begin();
        auto rhs = matrhs.data_.begin();

        for (; lhs != result.data_.end(); ++lhs, ++rhs)
            op(*lhs, *rhs);


		return result;
	}

    ComplexImage ComplexOperatorConst(const std::function<void(std::complex<float> & crhs)> & op) const
	{
        ComplexImage result = *this;

        for (auto lhs = result.data_.begin(); lhs != result.data_.end(); ++lhs)
            op(*lhs);

		return result;
	}

};

inline void swap(ComplexImage& obj1, ComplexImage& obj2) {
    obj1.swap(obj2);
}



inline ComplexImage sumOverChannels(std::vector<ComplexImage> &input)
{
    ComplexImage result(input[0]);
    for (size_t i = 1; i < input.size(); ++i)
        std::transform(result.begin(), result.end(), input[i].begin(), result.begin(), std::plus<std::complex<float>>());

    return result;
}

#endif // PRIMITIVES_ComplexImage_H_
