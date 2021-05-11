// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include <vector>

namespace JumaEngine
{
    template<typename T>
    class jarray : public std::vector<T>
    {
    public:
		using base_class = std::vector<T>;
    	
        jarray()
		    : base_class()
		{}
        jarray(std::initializer_list<T> list)
		    : base_class(list)
		{}
        jarray(const base_class& vector)
            : base_class(vector)
        {}

        bool isValidIndex(const int64 index) const { return (index >= 0) && (index < this->size()); }

        int64 indexOf(const T& value) const
        {
            if (!this->empty())
            {
                for (uint32 index = 0; index < this->size(); index++)
                {
                    if (this->at(index) == value)
                    {
                        return index;
                    }
                }
            }
            return -1;
        }
        bool contains(const T& value) const { return indexOf(value) != -1; }
        
		T& add(const T& value) { return this->emplace_back(value); }
		T& addDefault() { return this->emplace_back(T()); }
    	T& addUnique(const T& value)
        {
            const int64 index = indexOf(value);
	        if (index == -1)
	        {
		        return add(value);
	        }
            return this->at(index);
        }
    	
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
                if (this->at(index) == value)
                {
                    count++;
                    this->erase(this->begin() + index);
                }
                else
                {
                    index++;
                }
            }
            return count;
        }
    };
}
