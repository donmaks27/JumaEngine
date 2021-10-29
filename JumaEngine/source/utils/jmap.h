// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include <map>
#include "jarray.h"

namespace jutils
{
    template<typename K, typename V>
    class jmap : std::map<K, V>
    {
    public:
        using base_class = std::map<K, V>;
        using iterator = typename base_class::iterator;
        using const_iterator = typename base_class::const_iterator;

        jmap()
            : base_class()
        {}
        jmap(std::initializer_list<std::pair<const K, V>> initializer)
            : base_class(initializer)
        {}
        jmap(const base_class& m)
            : base_class(m)
        {}

        V& get(const K& key) { return this->base_class::find(key)->second; }
        const V& get(const K& key) const { return this->base_class::find(key)->second; }

        V* find(const K& key)
        {
            auto iter = this->base_class::find(key);
            return iter != end() ? &iter->second : nullptr;
        }
        const V* find(const K& key) const
        {
            auto iter = this->base_class::find(key);
            return iter != end() ? &iter->second : nullptr;
        }
        template<typename Pred>
        jarray<K> findByPredicate(Pred predicate) const
        {
            jarray<K> result;
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
        const K* findFirstByPredicate(Pred predicate) const
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

        bool contains(const K& key) const { return find(key) != nullptr; }
        template<typename Pred>
        bool containsByPredicate(Pred predicate) const { return findFirstByPredicate(predicate) != nullptr; }

        V& getOrAdd(const K& key) { return this->try_emplace(key).first->second; }
        const V& getOrAdd(const K& key) const { return this->try_emplace(key).first->second; }
        V& operator[](const K& key) { return getOrAdd(key); }
        const V& operator[](const K& key) const { return getOrAdd(key); }

        void set(const K& key, const V& value) { this->insert_or_assign(key, value); }

        void remove(const K& key) { this->erase(key); }
        template<typename Pred>
        void removeByPredicate(Pred predicate)
        {
            jarray<K> keys = findByPredicate(predicate);
            for (auto& key : keys)
            {
                remove(key);
            }
        }
        void clear() { this->base_class::clear(); }

        jarray<K> getKeys() const
        {
            jarray<K> result;
            for (const auto keyAndValue : *this)
            {
                result.add(keyAndValue.first);
            }
            return result;
        }

        iterator begin() { return this->base_class::begin(); }
        iterator end() { return this->base_class::end(); }

        const_iterator begin() const { return this->base_class::begin(); }
        const_iterator end() const { return this->base_class::end(); }

        uint64 getSize() const { return this->size(); }
    };
}
