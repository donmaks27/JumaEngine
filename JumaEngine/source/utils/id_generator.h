﻿// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "jarray.h"
#include "type_checks.h"

namespace jutils
{
    template<typename T = uint32, TEMPLATE_ENABLE(std::is_integral_v<T>)>
    class id_generator
    {
    public:
        id_generator() = default;
        id_generator(const id_generator& generator) = default;

        T generateID()
        {
            if (!m_UnusedIDs.isEmpty())
            {
                const int64 index = m_UnusedIDs.getSize() - 1;
                T id = m_UnusedIDs[index];
                m_UnusedIDs.removeAt(index);
                return id;
            }
            return generateUniqueID();
        }
        T generateUniqueID() { return m_NextID++; }

        void makeIDUnused(const T id)
        {
            if ((id > 0) && (id < m_NextID))
            {
                m_UnusedIDs.addUnique(id);
            }
        }

        void clear()
        {
            m_UnusedIDs.clear();
            m_NextID = 1;
        }

    private:

        T m_NextID = 1;
        jarray<T> m_UnusedIDs;
    };
}
