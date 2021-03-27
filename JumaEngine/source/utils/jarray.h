﻿// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include <vector>

namespace JumaEngine
{
    template<typename T>
    class jarray : public std::vector<T>
    {
    public:
        jarray()
		    : std::vector<T>()
		{}
        jarray(std::initializer_list<T> list)
		    : std::vector<T>(list)
		{}
        jarray(const std::vector<T>& vector)
            : std::vector<T>(vector)
        {}

        bool isValidIndex(const int32 index) const
        {
            return (index >= 0) && (index < this->size());
        }

        int32 indexOf(const T& value) const
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

        bool removeAt(const int32 index)
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
                if (this->at[index] == value)
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
