// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include <map>

#include "jarray.h"

namespace jutils
{
    template<typename K, typename V, typename ComparePred = std::less<>>
    class jmap : std::map<K, V, ComparePred>
    {
        using base_class = std::map<K, V, ComparePred>;

    public:

        using key_type = K;
        using value_type = V;
        using compare_predicate_type = ComparePred;
        using iterator = typename base_class::iterator;
        using const_iterator = typename base_class::const_iterator;

        jmap()
            : base_class()
        {}
        jmap(std::initializer_list<std::pair<const key_type, value_type>> initializer)
            : base_class(initializer)
        {}
        jmap(const jmap& value)
            : base_class(value)
        {}
        jmap(jmap&& value) noexcept
            : base_class(std::move(value))
        {}

        jmap& operator=(std::initializer_list<std::pair<const key_type, value_type>> initializer)
        {
            this->base_class::operator=(initializer);
            return *this;
        }
        jmap& operator=(const jmap& value)
        {
            this->base_class::operator=(value);
            return *this;
        }
        jmap& operator=(jmap&& value) noexcept
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
        
        value_type& get(const key_type& key) { return this->base_class::find(key)->second; }
        const value_type& get(const key_type& key) const { return this->base_class::find(key)->second; }
        value_type& getOrAdd(const key_type& key) { return this->base_class::try_emplace(key).first->second; }
        value_type& operator[](const key_type& key) { return getOrAdd(key); }
        const value_type& operator[](const key_type& key) const { return get(key); }

        value_type* find(const key_type& key)
        {
            auto iter = this->base_class::find(key);
            return iter != end() ? &iter->second : nullptr;
        }
        const value_type* find(const key_type& key) const
        {
            auto iter = this->base_class::find(key);
            return iter != end() ? &iter->second : nullptr;
        }
        template<typename Pred>
        jarray<key_type> findByPredicate(Pred predicate) const
        {
            jarray<key_type> result;
            for (const auto keyAndValue : *this)
            {
                if (predicate(keyAndValue.first, keyAndValue.second))
                {
                    result.add(keyAndValue.first);
                }
            }
            return result;
        }
        template<typename Pred>
        const key_type* findFirstByPredicate(Pred predicate) const
        {
            for (const auto keyAndValue : *this)
            {
                if (predicate(keyAndValue.first, keyAndValue.second))
                {
                    return &keyAndValue.first;
                }
            }
            return nullptr;
        }

        bool contains(const key_type& key) const { return find(key) != nullptr; }
        template<typename Pred>
        bool containsByPredicate(Pred predicate) const { return findFirstByPredicate(predicate) != nullptr; }

        template<typename... Args>
        value_type& assign(const key_type& key, Args&&... args) { return this->base_class::insert_or_assign(key, value_type(std::forward<Args>(args)...)).first->second; }
        template<typename... Args>
        value_type& assign(key_type&& key, Args&&... args) { return this->base_class::insert_or_assign(std::move(key), value_type(std::forward<Args>(args)...)).first->second; }

        value_type& set(const key_type& key, const value_type& value) { return assign(key, value); }
        value_type& set(const key_type& key, value_type&& value) { return this->base_class::insert_or_assign(key, std::move(value)).first->second; }
        value_type& set(key_type&& key, const value_type& value) { return assign(std::move(key), value); }
        value_type& set(key_type&& key, value_type&& value) { return this->base_class::insert_or_assign(std::move(key), std::move(value)).first->second; }

        void reserve(const int32 size) { this->base_class::reserve(jutils::math::max(0, size)); }

        void remove(const key_type& key) { this->erase(this->base_class::find(key)); }
        template<typename Pred>
        void removeByPredicate(Pred predicate)
        {
            for (const auto& key : findByPredicate(predicate))
            {
                remove(key);
            }
        }
        void clear() { this->base_class::clear(); }

        jmap& operator+=(std::initializer_list<std::pair<const key_type, value_type>> list)
        {
            for (auto& keyAndValue : list)
            {
                set(keyAndValue.first, keyAndValue.second);
            }
            return *this;
        }
        template<typename OtherComparePred>
        jmap& operator+=(const jmap<key_type, value_type, OtherComparePred>& value)
        {
            for (auto& keyAndValue : value)
            {
                set(keyAndValue.first, keyAndValue.second);
            }
            return *this;
        }
    };

    template<typename KeyType, typename ValueType, typename ComparePred, typename OtherComparePred>
    jmap<KeyType, ValueType, ComparePred> operator+(const jmap<KeyType, ValueType, ComparePred>& value1, const jmap<KeyType, ValueType, OtherComparePred>& value2)
    {
        return jmap<KeyType, ValueType, ComparePred>(value1) += value2;
    }
}
