// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include <vector>

#include "type_defines.h"
#include "math/jmath.h"

namespace jutils
{
    template<typename T, typename Allocator = std::allocator<T>>
    class jarray : std::vector<T, Allocator>
    {
        using base_class = std::vector<T, Allocator>;

    public:

        using type = T;
        using allocator = Allocator;
        using iterator = typename base_class::iterator;
        using const_iterator = typename base_class::const_iterator;

        jarray()
            : base_class()
        {}
        explicit jarray(const allocator& alloc)
            : base_class(alloc)
        {}
        explicit jarray(const int32 size, const allocator& alloc = allocator())
            : base_class(jutils::math::max(0, size), alloc)
        {}
        explicit jarray(const int32 size, const T& defaultValue, const allocator& alloc = allocator())
            : base_class(size, defaultValue, alloc)
        {}
        jarray(std::initializer_list<T> list, const allocator& alloc = allocator())
            : base_class(list, alloc)
        {}
        jarray(const jarray& value)
            : base_class(value)
        {}
        jarray(const jarray& value, const allocator& alloc)
            : base_class(value, alloc)
        {}
        jarray(jarray&& value) noexcept
            : base_class(std::move(value))
        {}
        jarray(jarray&& value, const allocator& alloc)
            : base_class(std::move(value), alloc)
        {}

        jarray& operator=(std::initializer_list<T> list)
        {
            this->base_class::operator=(list);
            return *this;
        }
        jarray& operator=(jarray&& value) noexcept
        {
            this->base_class::operator=(std::move(value));
            return *this;
        }
        jarray& operator=(const jarray& value)
        {
            this->base_class::operator=(value);
            return *this;
        }

        int32 getSize() const { return static_cast<int32>(this->base_class::size()); }
        bool isValidIndex(const int32 index) const { return jutils::math::isWithin(index, 0, getSize() - 1); }
        bool isEmpty() const { return this->base_class::empty(); }
        
        T* getData() noexcept { return this->base_class::data(); }
        const T* getData() const noexcept { return this->base_class::data(); }

        T& get(const int32 index)
        {
            if (index < 0)
            {
                throw std::out_of_range("Invalid jarray<T> index!");
            }
            return this->base_class::operator[](index);
        }
        const T& get(const int32 index) const
        {
            if (index < 0)
            {
                throw std::out_of_range("Invalid jarray<T> index!");
            }
            return this->base_class::operator[](index);
        }
        T& operator[](const int32 index) { return get(index); }
        const T& operator[](const int32 index) const { return get(index); }

        int32 indexOf(const T& value) const
        {
            if (!isEmpty())
            {
                for (int32 index = 0; index < getSize(); index++)
                {
                    if (getInternal(index) == value)
                    {
                        return index;
                    }
                }
            }
            return -1;
        }
        bool contains(const T& value) const { return indexOf(value) != -1; }

        template<typename... Args>
        T& put(Args&&... args)
        {
            this->base_class::emplace_back(std::forward<Args>(args)...);
            return getInternal(getSize() - 1);
        }

        T& add(const T& value) { return put(value); }
        T& add(T&& value) { return put(std::move(value)); }
        T& addDefault() { return put(); }
        T& addUnique(const T& value)
        {
            const int32 index = indexOf(value);
            return index == -1 ? add(value) : getInternal(index);
        }
        T& addUnique(T&& value)
        {
            const int32 index = indexOf(value);
            return index == -1 ? add(std::move(value)) : getInternal(index);
        }
        
        void reserve(const int32 size) { this->base_class::reserve(jutils::math::max(0, size)); }
        void resize(const int32 size) { this->base_class::resize(jutils::math::max(0, size)); }
        void resize(const int32 size, const type& defaultValue) { this->base_class::resize(jutils::math::max(0, size), defaultValue); }

        void removeAt(const int32 index)
        {
            if (isValidIndex(index))
            {
                removeAtInternal(index);
            }
        }
        void remove(const T& value)
        {
            int32 index = 0;
            while (isValidIndex(index))
            {
                if (get(index) == value)
                {
                    removeAtInternal(index);
                }
                else
                {
                    index++;
                }
            }
        }
        void clear() { return this->base_class::clear(); }

        iterator begin() noexcept { return this->base_class::begin(); }
        iterator end() noexcept { return this->base_class::end(); }

        const_iterator begin() const noexcept { return this->base_class::begin(); }
        const_iterator end() const noexcept { return this->base_class::end(); }

        template<typename OtherAllocator>
        bool operator==(const jarray<type, OtherAllocator>& value) const
        {
            if (getSize() != value.getSize())
            {
                return false;
            }
            if (isEmpty())
            {
                return true;
            }
            for (int32 index = 0; index < getSize(); index++)
            {
                if (getInternal(index) != value[index])
                {
                    return false;
                }
            }
            return true;
        }
        template<typename OtherAllocator>
        bool operator!=(const jarray<type, OtherAllocator>& value) const { return !this->operator==(value); }

    private:

        T& getInternal(const int32 index) { return this->base_class::operator[](index); }
        const T& getInternal(const int32 index) const { return this->base_class::operator[](index); }

        void removeAtInternal(const int32 index) { this->base_class::erase(begin() + index); }
    };
}
