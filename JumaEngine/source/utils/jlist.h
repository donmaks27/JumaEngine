// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include <list>

#include "type_defines.h"
#include "math/jmath.h"

namespace jutils
{
    template<typename T>
    class jlist : std::list<T>
    {
        using base_class = std::list<T>;

    public:

        using type = T;
        using iterator = typename base_class::iterator;
        using const_iterator = typename base_class::const_iterator;

        using index_type = int32;

        jlist()
            : base_class()
        {}
        explicit jlist(const index_type size)
            : base_class(jutils::math::max(0, size))
        {}
        jlist(const index_type size, const type& defaultValue)
            : base_class(size, defaultValue)
        {}
        jlist(std::initializer_list<type> list)
            : base_class(list)
        {}
        jlist(const jlist& value)
            : base_class(value)
        {}
        jlist(jlist&& value) noexcept
            : base_class(std::move(value))
        {}

        jlist& operator=(std::initializer_list<type> list)
        {
            this->base_class::operator=(list);
            return *this;
        }
        jlist& operator=(jlist&& value) noexcept
        {
            this->base_class::operator=(std::move(value));
            return *this;
        }
        jlist& operator=(const jlist& value)
        {
            this->base_class::operator=(value);
            return *this;
        }
        
        index_type getSize() const { return static_cast<index_type>(this->base_class::size()); }
        bool isEmpty() const { return this->base_class::empty(); }
        bool isValidIndex(const index_type index) const { return jutils::math::isWithin(index, 0, getSize() - 1); }
        
        iterator begin() noexcept { return this->base_class::begin(); }
        iterator end() noexcept { return this->base_class::end(); }

        const_iterator begin() const noexcept { return this->base_class::begin(); }
        const_iterator end() const noexcept { return this->base_class::end(); }
        
        iterator getIterByIndex(const index_type index) { return isValidIndex(index) ? getIterByIndexInternal(index) : end(); }
        const_iterator getIterByIndex(const index_type index) const { return isValidIndex(index) ? getIterByIndexInternal(index) : end(); }

        iterator getIterByValue(const type& value)
        {
            if (!isEmpty())
            {
                for (auto iter = begin(); iter != end(); ++iter)
                {
                    if (*iter == value)
                    {
                        return iter;
                    }
                }
            }
            return end();
        }
        const_iterator getIterByValue(const type& value) const
        {
            if (!isEmpty())
            {
                for (auto iter = begin(); iter != end(); ++iter)
                {
                    if (*iter == value)
                    {
                        return iter;
                    }
                }
            }
            return end();
        }

        type& get(const index_type index)
        {
            checkIsValidIndex(index);
            return *getIterByIndexInternal(index);
        }
        const type& get(const index_type index) const
        {
            checkIsValidIndex(index);
            return *getIterByIndexInternal(index);
        }
        type& operator[](const index_type index) { return get(index); }
        const type& operator[](const index_type index) const { return get(index); }
        
        type& getFirst()
        {
            checkIfEmpty();
            return this->base_class::front();
        }
        type& getLast()
        {
            checkIfEmpty();
            return this->base_class::back();
        }
        const type& getFirst() const
        {
            checkIfEmpty();
            return this->base_class::front();
        }
        const type& getLast() const
        {
            checkIfEmpty();
            return this->base_class::back();
        }

        type* findByIndex(const index_type index) { return isValidIndex(index) ? &*getIterByIndexInternal(index) : nullptr; }
        type* findByValue(const type& value)
        {
            auto iter = getIterByValue(value);
            return iter != end() ? &*iter : nullptr;
        }
        const type* findByIndex(const index_type index) const { return isValidIndex(index) ? &*getIterByIndexInternal(index) : nullptr; }
        const type* findByValue(const type& value) const
        {
            auto iter = getIterByValue(value);
            return iter != end() ? &*iter : nullptr;
        }

        index_type indexOf(const type& value) const
        {
            if (!isEmpty())
            {
                index_type index = 0;
                for (const auto& element : *this)
                {
                    if (element == value)
                    {
                        return index;
                    }
                    index++;
                }
            }
            return -1;
        }
        bool contains(const type& value) const { return getIterByValue(value) != end(); }

        template<typename... Args>
        type& put(Args&&... args)
        {
            this->base_class::emplace_back(std::forward<Args>(args)...);
            return this->base_class::back();
        }
        template<typename... Args>
        type& putAt(const_iterator iter, Args&&... args)
        {
            return *this->base_class::emplace(iter, std::forward<Args>(args)...);
        }
        template<typename... Args>
        type& putAt(const index_type index, Args&&... args)
        {
            checkIsValidIndex(index);
            return putAt(getIterByIndexInternal(index), std::forward<Args>(args)...);
        }

        type& add(const type& value) { return put(value); }
        type& add(type&& value) { return put(std::move(value)); }
        type& addDefault() { return put(); }
        type& addUnique(const type& value)
        {
            auto iter = getIterByValue(value);
            return iter != end() ? add(value) : *iter;
        }
        type& addUnique(type&& value)
        {
            auto iter = getIterByValue(value);
            return iter != end() ? add(std::move(value)) : *iter;
        }

        type& addAt(const_iterator iter, const type& value) { return putAt(iter, value); }
        type& addAt(const_iterator iter, type&& value) { return putAt(iter, std::move(value)); }
        type& addDefaultAt(const_iterator iter) { return putAt(iter); }

        type& addAt(const index_type index, const type& value) { return putAt(index, value); }
        type& addAt(const index_type index, type&& value) { return putAt(index, std::move(value)); }
        type& addDefaultAt(const index_type index) { return putAt(index); }

        void reserve(const index_type size) { this->base_class::reserve(jutils::math::max(0, size)); }
        void resize(const index_type size) { this->base_class::resize(jutils::math::max(0, size)); }
        void resize(const index_type size, const type& defaultValue) { this->base_class::resize(jutils::math::max(0, size), defaultValue); }

        void removeAt(const_iterator iter) { this->base_class::erase(iter); }
        void removeAt(const index_type index)
        {
            checkIsValidIndex(index);
            removeAt(getIterByIndexInternal(index));
        }

        index_type remove(const type& value)
        {
            index_type count = 0;
            if (!isEmpty())
            {
                auto iter = begin();
                while (iter != end())
                {
                    if (*iter == value)
                    {
                        iter = this->base_class::erase(iter);
                        count++;
                    }
                    else
                    {
                        ++iter;
                    }
                }
            }
            return count;
        }
        void clear() { return this->base_class::clear(); }

        jlist& operator+=(const type& value)
        {
            add(value);
            return *this;
        }
        jlist& operator+=(type&& value)
        {
            add(std::move(value));
            return *this;
        }
        jlist& operator+=(std::initializer_list<type> list)
        {
            for (auto& value : list)
            {
                add(value);
            }
            return *this;
        }
        jlist& operator+=(const jlist& value)
        {
            if ((this != &value) && !value.isEmpty())
            {
                for (const auto& element : value)
                {
                    add(element);
                }
            }
            return *this;
        }

    private:
        
        void checkIsValidIndex(const index_type index) const
        {
            if (!isValidIndex(index))
            {
                throw std::out_of_range("Invalid jlist<T> index!");
            }
        }
        void checkIfEmpty() const
        {
            if (isEmpty())
            {
                throw std::length_error("jlist<T> is empty!");
            }
        }

        iterator getIterByIndexInternal(const index_type index)
        {
            auto iter = begin();
            for (index_type i = 0; i < index; i++)
            {
                ++iter;
            }
            return iter;
        }
        const_iterator getIterByIndexInternal(const index_type index) const
        {
            auto iter = begin();
            for (index_type i = 0; i < index; i++)
            {
                ++iter;
            }
            return iter;
        }
    };
    
    template<typename T>
    jlist<T> operator+(const jlist<T>& value1, const T& value2) { return jlist<T>(value1) += value2; }
    template<typename T>
    jlist<T> operator+(const T& value1, const jlist<T>& value2) { return (jlist<T>() += value1) += value2; }
    template<typename T>
    jlist<T> operator+(const jlist<T>& value1, const jlist<T>& value2) { return jlist<T>(value1) += value2; }
}
