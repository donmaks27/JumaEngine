// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include <set>

#include "type_defines.h"

namespace jutils
{
    template<typename T, typename ComparePred = std::less<>>
    class jset : std::set<T, ComparePred>
    {
        using base_class = std::set<T, ComparePred>;

    public:

        using type = T;
        using compare_predicate_type = ComparePred;
        using iterator = typename base_class::iterator;
        using const_iterator = typename base_class::const_iterator;

        jset()
            : base_class()
        {}
        jset(std::initializer_list<type> list)
            : base_class(list)
        {}
        jset(const jset& value)
            : base_class(value)
        {}
        jset(jset&& value) noexcept
            : base_class(std::move(value))
        {}

        jset& operator=(std::initializer_list<type> list)
        {
            this->base_class::operator=(list);
            return *this;
        }
        jset& operator=(const jset& value)
        {
            this->base_class::operator=(value);
            return *this;
        }
        jset& operator=(jset&& value) noexcept
        {
            this->base_class::operator=(std::move(value));
            return *this;
        }

        int32 getSize() const { return static_cast<int32>(this->base_class::size()); }
        bool isEmpty() const { return this->base_class::empty(); }

        iterator begin() { return this->base_class::begin(); }
        iterator end() { return this->base_class::end(); }

        const_iterator begin() const { return this->base_class::begin(); }
        const_iterator end() const { return this->base_class::end(); }

        bool contains(const type& value) const { return this->base_class::find(value) != end(); }

        template<typename... Args>
        const type& put(Args&&... args) { return *this->base_class::emplace(std::forward<Args>(args)...).first; }

        const type& add(const type& value) { return put(value); }
        const type& add(type&& value) { return put(std::move(value)); }

        void remove(const type& value) { this->base_class::erase(value); }
        void clear() { return this->base_class::clear(); }

        jset& operator+=(const type& value)
        {
            add(value);
            return *this;
        }
        jset& operator+=(std::initializer_list<type> list)
        {
            for (const auto& value : list)
            {
                add(value);
            }
            return *this;
        }
        template<typename OtherComparePred>
        jset& operator+=(const jset<type, OtherComparePred>& value)
        {
            for (const auto& key : value)
            {
                add(key);
            }
            return *this;
        }
    };
    
    template<typename Type, typename ComparePred>
    jset<Type, ComparePred> operator+(const jset<Type, ComparePred>& value1, const Type& value2) { return jset<Type, ComparePred>(value1) += value2; }
    template<typename Type, typename ComparePred>
    jset<Type, ComparePred> operator+(const Type& value1, const jset<Type, ComparePred>& value2) { return (jset<Type, ComparePred>() += value1) += value2; }
    template<typename Type, typename ComparePred, typename OtherComparePred>
    jset<Type, ComparePred> operator+(const jset<Type, ComparePred>& value1, const jset<Type, OtherComparePred>& value2) { return jset<Type, ComparePred>(value1) += value2; }
}
