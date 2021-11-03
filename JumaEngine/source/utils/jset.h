// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include <set>

namespace jutils
{
    template<typename T>
    class jset : std::set<T>
    {
    public:
        using base_class = std::set<T>;
        using iterator = typename base_class::iterator;
        using const_iterator = typename base_class::const_iterator;

        jset()
            : base_class()
        {}
        jset(std::initializer_list<T> list)
            : base_class(list)
        {}
        jset(const base_class& s)
            : base_class(s)
        {}

        void add(const T& value) { this->emplace(value); }

        bool contains(const T& value) const { return this->find(value) != end(); }

        void remove(const T& value) { this->erase(value); }
        void clear() { return this->base_class::clear(); }

        iterator begin() { return this->base_class::begin(); }
        iterator end() { return this->base_class::end(); }

        const_iterator begin() const { return this->base_class::begin(); }
        const_iterator end() const { return this->base_class::end(); }

        uint64 getSize() const { return this->size(); }
    };
}
