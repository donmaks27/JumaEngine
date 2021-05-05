// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "jdelegate.h"
#include "jarray.h"

namespace JumaEngine
{
    template<typename... ArgTypes>
    class jdelegate_multicast
    {
    public:
        jdelegate_multicast() = default;

        template<typename T>
        void bind(T* object, void (T::*function)(ArgTypes...))
        {
            if ((object != nullptr) && !isBinded(object, function))
            {
                m_Delegates.add(jdelegate<ArgTypes...>());
                m_Delegates[m_Delegates.size() - 1].bind(object, function);
            }
        }

        template<typename T>
        bool isBinded(T* object, void (T::*function)(ArgTypes...)) const
        {
            if (object != nullptr)
            {
                for (const auto& delegate : m_Delegates)
                {
                    if (delegate.isBinded(object, function))
                    {
                        return true;
                    }
                }
            }
            return false;
        }
        template<typename T>
        bool isBinded(T* object) const
        {
            if (object != nullptr)
            {
                for (const auto& delegate : m_Delegates)
                {
                    if (delegate.isBinded(object))
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        template<typename T>
        void unbind(T* object)
        {
            if (object != nullptr)
            {
                for (int i = 0; i < m_Delegates.size(); i++)
                {
                    if (m_Delegates[i].isBinded(object))
                    {
                        m_Delegates.removeAt(i);
                    }
                }
            }
        }
        template<typename T>
        void unbind(T* object, void (T::*callback)(ArgTypes...))
        {
            if (object != nullptr)
            {
                for (int i = 0; i < m_Delegates.size(); i++)
                {
                    if (m_Delegates[i].isBinded(object, callback))
                    {
                        m_Delegates.removeAt(i);
                        return;
                    }
                }
            }
        }

        void clear() { m_Delegates.clear(); }

        void call(ArgTypes... args)
        {
            for (auto& delegate : m_Delegates)
            {
                delegate.call(args...);
            }
        }

    private:

        jarray<jdelegate<ArgTypes...>> m_Delegates;
    };
}
