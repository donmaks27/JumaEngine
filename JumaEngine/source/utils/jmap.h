// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "jarray.h"
#include <map>

namespace JumaEngine
{
	template<typename K, typename V>
	class jmap : public std::map<K, V>
	{
	public:
		using base_class = std::map<K, V>;
		
		jmap()
			: base_class()
		{}
		jmap(const base_class& m)
			: base_class(m)
		{}

        jarray<K> getKeys() const
		{
		    jarray<K> result;
            for (const auto keyAndValue : *this)
            {
                result.add(keyAndValue.first);
            }
            return result;
		}

		V* findByKey(const K& key)
		{
			typename base_class::iterator iter = this->find(key);
			if (iter != this->end())
			{
				return &iter->second;
			}
			return nullptr;
		}
		const V* findByKey(const K& key) const
		{
			typename base_class::const_iterator iter = this->find(key);
			if (iter != this->end())
			{
				return &iter->second;
			}
			return nullptr;
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
		
		bool contains(const K& key) const { return this->find(key) != this->end(); }
        template<typename Pred>
		bool containsByPredicate(Pred predicate) const { return findFirstByPredicate(predicate) != nullptr; }

		void add(const K& key, const V& value) { this->insert_or_assign(key, value); }

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
	};
}
