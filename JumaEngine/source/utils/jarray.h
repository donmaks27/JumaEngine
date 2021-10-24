// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "utils/int_defines.h"
#include <vector>

namespace jutils
{
    template<typename T>
    class jarray : std::vector<T>
    {
    public:
        using base_class = std::vector<T>;
        using iterator = typename base_class::iterator;
        using const_iterator = typename base_class::const_iterator;

        jarray()
            : base_class()
        {}
        explicit jarray(const size_t size)
            : base_class(size)
        {}
        explicit jarray(const size_t size, const T& defaultValue)
            : base_class(size, defaultValue)
        {}
        jarray(std::initializer_list<T> list)
            : base_class(list)
        {}
        jarray(const base_class& vector)
            : base_class(vector)
        {}

        int64 getSize() const { return this->size(); }
        bool isValidIndex(const int64 index) const { return (index >= 0) && (index < getSize()); }
        bool isEmpty() const { return this->empty(); }

        T& get(const int64 index) { return this->at(index); }
        const T& get(const int64 index) const { return this->at(index); }
        T& operator[](const int64 index) { return get(index); }
        const T& operator[](const int64 index) const { return get(index); }

        int64 indexOf(const T& value) const
        {
            if (!isEmpty())
            {
                for (uint32 index = 0; index < getSize(); ++index)
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

        T& add(const T& value)
        {
            this->emplace_back(value);
            return get(getSize() - 1);
        }
        T& addDefault() { return this->emplace_back(); }
        T& addUnique(const T& value)
        {
            const int64 index = indexOf(value);
            if (index == -1)
            {
                return add(value);
            }
            return get(index);
        }

        void resize(const int64 size) { this->base_class::resize(size > 0 ? size : 0); }

        bool removeAt(const int64 index)
        {
            if (isValidIndex(index))
            {
                this->erase(this->begin() + index);
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
                    ++count;
                    removeAt(index);
                }
                else
                {
                    ++index;
                }
            }
            return count;
        }
        void clear() { return this->base_class::clear(); }

        T* getData() noexcept { return this->data(); }
        const T* getData() const noexcept { return this->data(); }

        iterator begin() noexcept { return this->base_class::begin(); }
        iterator end() noexcept { return this->base_class::end(); }

        const_iterator begin() const noexcept { return this->base_class::begin(); }
        const_iterator end() const noexcept { return this->base_class::end(); }
    };
}
