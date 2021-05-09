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
        jdelegate_multicast(jdelegate_multicast&& delegate) = default;
        jdelegate_multicast(const jdelegate_multicast& delegate) = default;
        ~jdelegate_multicast() = default;

        jdelegate_multicast& operator=(jdelegate_multicast&& delegate) = delete;
        jdelegate_multicast& operator=(const jdelegate_multicast& delegate) = delete;

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

        void _call(ArgTypes... args)
        {
            for (auto& delegate : m_Delegates)
            {
                delegate._call(args...);
            }
        }

    private:

        jarray<jdelegate<ArgTypes...>> m_Delegates;
    };
}

#define DECLARE_JUMAENGINE_DELEGATE_MULTICAST_INTERNAL(DelegateName, ParamsTypes, ParamsNames, Params) \
class DelegateName : public jdelegate_multicast<ParamsTypes> \
{ \
    using base_class = jdelegate_multicast<ParamsTypes>; \
public: \
    DelegateName() : base_class() {} \
    DelegateName(base_class&& delegate) noexcept : base_class(std::move(delegate)) {} \
    DelegateName(const base_class& delegate) : base_class(delegate) {} \
    void call(Params) { _call(ParamsNames); } \
};

#define DECLARE_JUMAENGINE_DELEGATE_MULTICAST(DelegateName) DECLARE_JUMAENGINE_DELEGATE_MULTICAST_INTERNAL(DelegateName, , , )
#define DECLARE_JUMAENGINE_DELEGATE_MULTICAST_OneParam(DelegateName, ParamType1, ParamName1) DECLARE_JUMAENGINE_DELEGATE_MULTICAST_INTERNAL(DelegateName, \
    JUMAENGINE_CONCAT_HELPER(ParamType1), JUMAENGINE_CONCAT_HELPER(ParamName1), \
    JUMAENGINE_CONCAT_HELPER(ParamType1 ParamName1))
#define DECLARE_JUMAENGINE_DELEGATE_MULTICAST_TwoParams(DelegateName, ParamType1, ParamName1, ParamType2, ParamName2) DECLARE_JUMAENGINE_DELEGATE_MULTICAST_INTERNAL(DelegateName, \
    JUMAENGINE_CONCAT_HELPER(ParamType1, ParamType2), JUMAENGINE_CONCAT_HELPER(ParamName1, ParamName2), \
    JUMAENGINE_CONCAT_HELPER(ParamType1 ParamName1, ParamType2 ParamName2))
#define DECLARE_JUMAENGINE_DELEGATE_MULTICAST_ThreeParams(DelegateName, ParamType1, ParamName1, ParamType2, ParamName2, ParamType3, ParamName3) DECLARE_JUMAENGINE_DELEGATE_MULTICAST_INTERNAL(DelegateName, \
    JUMAENGINE_CONCAT_HELPER(ParamType1, ParamType2, ParamType3), JUMAENGINE_CONCAT_HELPER(ParamName1, ParamName2, ParamName3), \
    JUMAENGINE_CONCAT_HELPER(ParamType1 ParamName1, ParamType2 ParamName2, ParamType3 ParamName3))
#define DECLARE_JUMAENGINE_DELEGATE_MULTICAST_FourParams(DelegateName, ParamType1, ParamName1, ParamType2, ParamName2, ParamType3, ParamName3, ParamType4, ParamName4) DECLARE_JUMAENGINE_DELEGATE_MULTICAST_INTERNAL(DelegateName, \
    JUMAENGINE_CONCAT_HELPER(ParamType1, ParamType2, ParamType3, ParamType4), JUMAENGINE_CONCAT_HELPER(ParamName1, ParamName2, ParamName3, ParamName4), \
    JUMAENGINE_CONCAT_HELPER(ParamType1 ParamName1, ParamType2 ParamName2, ParamType3 ParamName3, ParamType4 ParamName4))
