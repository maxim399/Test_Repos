#ifndef IIPCT_IMAGEVIEW_HPP
#define IIPCT_IMAGEVIEW_HPP

#include <mutex>
#include <memory>
#include <new>
#include "Primitives/Headers/strobe_rectangle.h"

class ImageView {
private:
    size_t width_ = 0;
    size_t height_ = 0;
    size_t channels_ = 0;
    size_t element_size_ = sizeof(uint8_t);//type elements in data_

    std::vector<uint8_t> data_; // uint8_t does not mean that the image will only be uint8_t (see "element_size_")
    mutable std::mutex dataIO_mutex_;

public:
    ImageView() = default;

    ImageView(const ImageView& other):
    width_(other.width()), height_(other.height()), channels_(other.channels()),
    element_size_(other.element_size()), data_(other.data_)
    {
    }

    ImageView(ImageView&& other) noexcept:
        width_(other.width()), height_(other.height()), channels_(other.channels()),
        element_size_(other.element_size()), data_(std::move(other.data_))
    {
        other.width_ = 0;
        other.height_ = 0;
        other.channels_ = 0;
        other.element_size_ = 0;
    }

// for "legacy" code
    ImageView* operator->()
    {
        return this;
    }

    const ImageView* operator->() const
    {
        return this;
    }


    /**
    * @brief Returns true if the array has no elements.
    **/
    bool empty() const { return data_.empty(); }

    /**
    * @brief Getting an width image.
    **/
    size_t width()         const {return width_;}

    /**
    * @brief Getting an height image.
    **/
    size_t height()        const {return height_;}

    /**
    * @brief Getting an channels image.
    **/
    size_t channels()      const {return channels_;}

    /**
    * @brief Getting an size image.
    **/
    size_t size()          const {return data_.size();}

    /**
    * @brief Getting an sizeof(Type) image.
    **/
    size_t element_size()  const {return element_size_;}


    /**
    * @brief Reinitializing the entire image.
    *
    * @param width new width image.
    * @param height new height image.
    **/
    void reinit(size_t width, size_t height)
    {
        width_ = width;
        height_ = height;
        data_.resize(width_ * height_ * channels_ * element_size_);
    }

    void swap(ImageView& other) noexcept
    {
        using std::swap;
        swap(width_, other.width_);
        swap(height_, other.height_);
        swap(channels_, other.channels_);
        swap(element_size_, other.element_size_);
        swap(data_, other.data_);
    }

    /**
    * @brief Assignment operators. These are available assignment operators.
    *
    * @param rhs assigned, right-hand-side matrix. Matrix assignment is an O(1) operation.
    **/
    ImageView&operator=( const ImageView& rhs)
    {
        if (&rhs == this)
            return *this;

        ImageView tmp = rhs;
        swap(tmp);
        return *this;
    }

    /**
    * @brief Assignment operators. These are available assignment operators.
    *
    * @param rhs assigned, right-hand-side matrix. Matrix assignment is an O(1) operation.
    **/
    ImageView&operator=(ImageView&& rhs) noexcept
    {
        if (&rhs == this)
            return *this;

        ImageView tmp = std::move(rhs);
        swap(tmp);
        return *this;
    }

    /**
    * @brief Create object image.
    *
    * @param width width image.
    * @param height height image.
    * @param channels channels image.
    * @param element_size element_size image.
    **/
    ImageView(size_t width, size_t height,
              size_t channels, size_t element_size=sizeof(uint8_t)):
        width_(width), height_(height), channels_(channels),  element_size_(element_size),
        data_(width * height * channels * element_size)
    {}

    /**
    * @brief Create object image.
    *
    * @param source input source image.
    * @param ROIrectthe rectangle from which the new image appears.
    **/
    template <class T>
    ImageView(const ImageView& source, const strobeRectangle<T>& ROIrect):
            width_(ROIrect.width()), height_(ROIrect.height()), channels_(source.channels()),
            element_size_(source.element_size()),
            data_(width_ * height_ * channels_ * element_size_)
    {

        size_t bigStep = source.width() * channels_ * element_size_;
        size_t smallStep = width_ * channels_ * element_size_;
        const uint8_t* starting_point = source.ptr<uint8_t>() +
                static_cast<size_t>(ROIrect.left) * channels_ * element_size_;
        size_t bigStride = bigStep * ROIrect.bottom;
        size_t smallStride = 0;

        std::lock_guard<std::mutex> lock(source.dataIO_mutex_);

        for (auto y = ROIrect.bottom; y < ROIrect.top; y++)
        {
            memcpy(data_.data() + smallStride, starting_point + bigStride, smallStep);
            bigStride += bigStep;
            smallStride += smallStep;
        }
    }

    ImageView copy() const;

    /**
    * @brief Create object image.
    *
    * @param ROIrect rectangle from which the new image appears.
    *
    * @return new crop image
    **/
    template<class T>
    ImageView crop(const strobeRectangle<T> &ROIrect) const
    {
        return ImageView(*this, ROIrect);
    }

    /**
    * @brief getting a pointer to data()
    *
    * @param x coordinate by x
    * @param y coordinate by y
    * @param c selected channel
    *
    * @return item to data()
    **/
    template <class T>
    T* ptr(size_t x, size_t y, size_t c)
    {
        return reinterpret_cast<T*>(&data_[((y * width_ + x) * channels_ + c) * element_size_]);
    }

    /**
    * @brief getting a pointer to an item from data()
    *
    * @param x coordinate by x
    * @param y coordinate by y
    * @param c selected channel
    *
    * @return item to data()
    **/
    template <class T>
    const T* ptr(size_t x, size_t y, size_t c) const
    {
        return reinterpret_cast<const T*>(&data_[((y * width_ + x) * channels_ + c) * element_size_]);
    }

    /**
    * @brief getting a pointer to an item from data()
    *
    * @param index index in data()
    *
    * @return item to data()
    **/
    template <class T>
    T* ptr(size_t index=0)
    {
        return reinterpret_cast<T*>(&data_[index * element_size_]);
    }

    /**
    * @brief getting a pointer to an item from data()
    *
    * @param index index in data()
    *
    * @return item to data()
    **/
    template <class T>
    const T* ptr(size_t index=0) const
    {
        return reinterpret_cast<const T*>(&data_[index * element_size_]);
    }

    /**
    * @brief getting an item from data
    *
    * @param x coordinate by x
    * @param y coordinate by y
    * @param c selected channel
    *
    * @return item to data()
    **/
    template <class T>
    T& get(size_t x, size_t y, size_t c)
    {
        return *ptr<T>(x, y, c);
    }

    /**
    * @brief getting an item from data
    *
    * @param x coordinate by x
    * @param y coordinate by y
    * @param c selected channel
    *
    * @return item to data()
    **/
    template <class T>
    const T& get(size_t x, size_t y, size_t c) const
    {
        return *ptr<T>(x, y, c);
    }

    /**
    * @brief getting an item from data
    *
    * @param index index in data()
    *
    * @return item to data()
    **/
    template <class T>
    T& get(size_t index=0)
    {
        return *ptr<T>(index);
    }

    /**
    * @brief getting an item from data
    *
    * @param index index in data()
    *
    * @return item to data()
    **/
    template <class T>
    const T& get(size_t index=0) const
    {
        return *ptr<T>(index);
    }

};

#endif //IIPCT_IMAGEVIEW_HPP
