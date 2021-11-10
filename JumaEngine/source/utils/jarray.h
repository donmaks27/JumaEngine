// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include <vector>
#include "utils/int_defines.h"
#include "utils/math.h"

namespace jutils
{
    template<typename T, typename Allocator = std::allocator<T>>
    class jarray : std::vector<T, Allocator>
    {
    public:
        using base_class = std::vector<T>;
        using iterator = typename base_class::iterator;
        using const_iterator = typename base_class::const_iterator;

        jarray()
            : base_class()
        {}
        explicit jarray(const int32 size)
            : base_class(math::max(size, 0))
        {}
        explicit jarray(const int32 size, const T& defaultValue)
            : base_class(size, defaultValue)
        {}
        jarray(std::initializer_list<T> list)
            : base_class(list)
        {}
        jarray(const jarray& value)
            : base_class(value)
        {}
        jarray(jarray&& value) noexcept
            : base_class(std::move(value))
        {}

        jarray& operator=(jarray&& value) noexcept
        {
            this->base_class::operator=(value);
            return *this;
        }
        jarray& operator=(const jarray& value)
        {
            this->base_class::operator=(value);
            return *this;
        }
        jarray& operator=(std::initializer_list<T> list)
        {
            this->base_class::operator=(list);
            return *this;
        }

        int32 getSize() const { return static_cast<int32>(this->size()); }
        bool isValidIndex(const int32 index) const { return (index >= 0) && (index < getSize()); }
        bool isEmpty() const { return this->empty(); }

        T& get(const int32 index)
        {
            if (!isValidIndex(index))
            {
                throw std::out_of_range("invalid jarray<T> index");
            }
            return this->getData()[index];
        }
        const T& get(const int32 index) const
        {
            if (!isValidIndex(index))
            {
                throw std::out_of_range("invalid jarray<T> index");
            }
            return this->getData()[index];
        }
        T& operator[](const int32 index) { return get(index); }
        const T& operator[](const int32 index) const { return get(index); }

        int32 indexOf(const T& value) const
        {
            if (!isEmpty())
            {
                for (int32 index = 0; index < getSize(); index++)
                {
                    if (get(index) == value)
                    {
                        return index;
                    }
                }
            }
            return -1;
        }
        bool contains(const T& value) const { return indexOf(value) != -1; }

        T& add(const T& value) { return this->emplace_back(value); }
        T& add(T&& value) { return this->emplace_back(std::move(value)); }
        T& addDefault() { return add(T()); }
        T& addUnique(const T& value)
        {
            const int32 index = indexOf(value);
            return index == -1 ? add(value) : get(index);
        }
        T& addUnique(T&& value)
        {
            const int32 index = indexOf(value);
            return index == -1 ? add(value) : get(index);
        }
        
        void reserve(const int32 size) { this->base_class::reserve(size); }
        void resize(const int32 size) { this->base_class::resize(math::max(0, size)); }

        bool removeAt(const int32 index)
        {
            if (isValidIndex(index))
            {
                this->erase(begin() + index);
                return true;
            }
            return false;
        }
        uint32 remove(const T& value)
        {
            uint32 count = 0;
            uint32 index = 0;
            while (isValidIndex(index))
            {
                if (get(index) == value)
                {
                    count++;
                    removeAt(index);
                }
                else
                {
                    index++;
                }
            }
            return count;
        }
        void clear()
        {
            return this->base_class::clear();
        }

        T* getData() noexcept { return this->data(); }
        const T* getData() const noexcept { return this->data(); }

        iterator begin() noexcept { return this->base_class::begin(); }
        iterator end() noexcept { return this->base_class::end(); }

        const_iterator begin() const noexcept { return this->base_class::begin(); }
        const_iterator end() const noexcept { return this->base_class::end(); }
    };
}
