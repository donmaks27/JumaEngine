// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include <set>

namespace JumaEngine
{
	template<typename T>
	class jset : public std::set<T>
	{
	public:
		using base_class = std::set<T>;

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

		bool contains(const T& value) const { return this->find(value) != this->end(); }
	};
}
