// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
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
		bool contains(const K& key) const { return this->find(key) != this->end(); }

		void add(const K& key, const V& value)
		{
			this->insert_or_assign(key, value);
		}
	};
}
