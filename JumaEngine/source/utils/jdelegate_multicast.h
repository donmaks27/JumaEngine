// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "jdelegate.h"
#include "jarray.h"

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

#define CREATE_JDELEGATE_MULTICAST_TYPE_INTERNAL(DelegateName, ParamsTypes, ParamsNames, Params) \
class DelegateName : public jdelegate_multicast<ParamsTypes> \
{ \
    using base_class = jdelegate_multicast<ParamsTypes>; \
public: \
    DelegateName() : base_class() {} \
    DelegateName(base_class&& delegate) noexcept : base_class(std::move(delegate)) {} \
    DelegateName(const base_class& delegate) : base_class(delegate) {} \
    void call(Params) { _call(ParamsNames); } \
};

#define CREATE_JDELEGATE_MULTICAST_TYPE(DelegateName) CREATE_JDELEGATE_MULTICAST_TYPE_INTERNAL(DelegateName, , , )
#define CREATE_JDELEGATE_MULTICAST_TYPE_OneParam(DelegateName, ParamType1, ParamName1) CREATE_JDELEGATE_MULTICAST_TYPE_INTERNAL(DelegateName, \
    JDELEGATE_CONCAT_HELPER(ParamType1), JDELEGATE_CONCAT_HELPER(ParamName1), \
    JDELEGATE_CONCAT_HELPER(ParamType1 ParamName1))
#define CREATE_JDELEGATE_MULTICAST_TYPE_TwoParams(DelegateName, ParamType1, ParamName1, ParamType2, ParamName2) CREATE_JDELEGATE_MULTICAST_TYPE_INTERNAL(DelegateName, \
    JDELEGATE_CONCAT_HELPER(ParamType1, ParamType2), JDELEGATE_CONCAT_HELPER(ParamName1, ParamName2), \
    JDELEGATE_CONCAT_HELPER(ParamType1 ParamName1, ParamType2 ParamName2))
#define CREATE_JDELEGATE_MULTICAST_TYPE_ThreeParams(DelegateName, ParamType1, ParamName1, ParamType2, ParamName2, ParamType3, ParamName3) CREATE_JDELEGATE_MULTICAST_TYPE_INTERNAL(DelegateName, \
    JDELEGATE_CONCAT_HELPER(ParamType1, ParamType2, ParamType3), JDELEGATE_CONCAT_HELPER(ParamName1, ParamName2, ParamName3), \
    JDELEGATE_CONCAT_HELPER(ParamType1 ParamName1, ParamType2 ParamName2, ParamType3 ParamName3))
#define CREATE_JDELEGATE_MULTICAST_TYPE_FourParams(DelegateName, ParamType1, ParamName1, ParamType2, ParamName2, ParamType3, ParamName3, ParamType4, ParamName4) CREATE_JDELEGATE_MULTICAST_TYPE_INTERNAL(DelegateName, \
    JDELEGATE_CONCAT_HELPER(ParamType1, ParamType2, ParamType3, ParamType4), JDELEGATE_CONCAT_HELPER(ParamName1, ParamName2, ParamName3, ParamName4), \
    JDELEGATE_CONCAT_HELPER(ParamType1 ParamName1, ParamType2 ParamName2, ParamType3 ParamName3, ParamType4 ParamName4))
