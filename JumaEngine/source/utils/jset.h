// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include <set>

#include "type_defines.h"

namespace jutils
{
    template<typename T, typename Allocator = std::allocator<T>>
    class jset : std::set<T, std::less<>, Allocator>
    {
        using base_class = std::set<T, std::less<>, Allocator>;

    public:

        using type = T;
        using allocator_type = Allocator;
        using iterator = typename base_class::iterator;
        using const_iterator = typename base_class::const_iterator;

        jset()
            : base_class()
        {}
        explicit jset(const allocator_type& alloc)
            : base_class(alloc)
        {}
        jset(std::initializer_list<type> list, const allocator_type& alloc = allocator_type())
            : base_class(list, alloc)
        {}
        jset(const jset& value)
            : base_class(value)
        {}
        jset(const jset& value, const allocator_type& alloc)
            : base_class(value, alloc)
        {}
        jset(jset&& value) noexcept
            : base_class(std::move(value))
        {}
        jset(jset&& value, const allocator_type& alloc)
            : base_class(std::move(value), alloc)
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

        bool contains(const type& value) const { return this->base_class::find(value) != end(); }

        template<typename... Args>
        const type& put(Args&&... args) { return *this->base_class::emplace(std::forward<Args>(args)...).first; }

        const type& add(const type& value) { return put(value); }
        const type& add(type&& value) { return put(std::move(value)); }

        void remove(const type& value) { this->base_class::erase(value); }
        void clear() { return this->base_class::clear(); }

        iterator begin() { return this->base_class::begin(); }
        iterator end() { return this->base_class::end(); }

        const_iterator begin() const { return this->base_class::begin(); }
        const_iterator end() const { return this->base_class::end(); }
    };
}
