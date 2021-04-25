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

        bool isValidIndex(const size_t index) const
        {
            return (index >= 0) && (index < this->size());
        }

        size_t indexOf(const T& value) const
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
        
		void add(const T& value) { this->push_back(value); }
    	void addUnique(const T& value)
        {
	        if (indexOf(value) != -1)
	        {
		        add(value);
	        }
        }
    	
        bool removeAt(const size_t index)
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
