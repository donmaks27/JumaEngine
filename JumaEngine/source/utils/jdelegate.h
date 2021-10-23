// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

namespace jutils
{
    template<typename... ArgTypes>
    class jdelegate
    {
    public:
        jdelegate() = default;
        jdelegate(const jdelegate& otherDelegate)
        {
            if (otherDelegate.m_Container != nullptr)
            {
                m_Container = otherDelegate.m_Container->copy();
            }
        }
        jdelegate(jdelegate&& otherDelegate) noexcept
        {
            m_Container = otherDelegate.m_Container;
            otherDelegate.m_Container = nullptr;
        }
        ~jdelegate() { clear(); }

        jdelegate& operator=(const jdelegate& otherDelegate) = delete;
        jdelegate& operator=(jdelegate&& otherDelegate) = delete;

        template<typename T>
        void bind(T* object, void (T::*function)(ArgTypes...))
        {
            clear();
            if (object != nullptr)
            {
                m_Container = new delegate_container_impl<T>(object, function);
            }
        }

        template<typename T>
        bool isBinded(T* object, void (T::*function)(ArgTypes...)) const
        {
            if ((object != nullptr) && (m_Container != nullptr))
            {
                const delegate_container_impl<T>* container = dynamic_cast<const delegate_container_impl<T>*>(m_Container);
                if (container != nullptr)
                {
                    return container->isBinded(object, function);
                }
            }
            return false;
        }
        template<typename T>
        bool isBinded(T* object) const
        {
            if ((object != nullptr) && (m_Container != nullptr))
            {
                const delegate_container_impl<T>* container = dynamic_cast<const delegate_container_impl<T>*>(m_Container);
                if (container != nullptr)
                {
                    return container->isBinded(object);
                }
            }
            return false;
        }

        void clear()
        {
            if (m_Container != nullptr)
            {
                delete m_Container;
                m_Container = nullptr;
            }
        }

        void _call(ArgTypes... args)
        {
            if (m_Container != nullptr)
            {
                m_Container->_call(args...);
            }
        }

    private:

        class delegate_container
        {
        public:
            virtual ~delegate_container() = default;

            virtual delegate_container* copy() = 0;

            virtual void _call(ArgTypes...) = 0;
        };
        template<typename T>
        class delegate_container_impl : public delegate_container
        {
            typedef void (T::*function_type)(ArgTypes...);

        public:
            delegate_container_impl(T* object, function_type function)
                : m_Object(object)
                , m_Function(function)
            {}

            virtual delegate_container* copy() override { return new delegate_container_impl(m_Object, m_Function); }

            bool isBinded(T* object, function_type function) const { return isBinded(object) && (m_Function == function); }
            bool isBinded(T* object) const { return m_Object == object; }

            virtual void _call(ArgTypes... args) override
            {
                if (m_Object != nullptr)
                {
                    (m_Object->*m_Function)(args...);
                }
            }

        private:

            T* m_Object;
            function_type m_Function;
        };

        delegate_container* m_Container = nullptr;
    };
}

#define JDELEGATE_CONCAT_HELPER(...) __VA_ARGS__

#define CREATE_JDELEGATE_TYPE_INTERNAL(DelegateName, ParamsTypes, ParamsNames, Params) \
class DelegateName : public jdelegate<ParamsTypes> \
{ \
    using base_class = jdelegate<ParamsTypes>; \
public: \
    DelegateName() : base_class() {} \
    DelegateName(base_class&& delegate) noexcept : base_class(std::move(delegate)) {} \
    DelegateName(const base_class& delegate) : base_class(delegate) {} \
    void call(Params) { _call(ParamsNames); } \
};

#define CREATE_JDELEGATE_TYPE(DelegateName) CREATE_JDELEGATE_TYPE_INTERNAL(DelegateName, , , )
#define CREATE_JDELEGATE_TYPE_OneParam(DelegateName, ParamType1, ParamName1) CREATE_JDELEGATE_TYPE_INTERNAL(DelegateName, \
    JDELEGATE_CONCAT_HELPER(ParamType1), JDELEGATE_CONCAT_HELPER(ParamName1), \
    JDELEGATE_CONCAT_HELPER(ParamType1 ParamName1))
#define CREATE_JDELEGATE_TYPE_TwoParams(DelegateName, ParamType1, ParamName1, ParamType2, ParamName2) CREATE_JDELEGATE_TYPE_INTERNAL(DelegateName, \
    JDELEGATE_CONCAT_HELPER(ParamType1, ParamType2), JDELEGATE_CONCAT_HELPER(ParamName1, ParamName2), \
    JDELEGATE_CONCAT_HELPER(ParamType1 ParamName1, ParamType2 ParamName2))
#define CREATE_JDELEGATE_TYPE_ThreeParams(DelegateName, ParamType1, ParamName1, ParamType2, ParamName2, ParamType3, ParamName3) CREATE_JDELEGATE_TYPE_INTERNAL(DelegateName, \
    JDELEGATE_CONCAT_HELPER(ParamType1, ParamType2, ParamType3), JDELEGATE_CONCAT_HELPER(ParamName1, ParamName2, ParamName3), \
    JDELEGATE_CONCAT_HELPER(ParamType1 ParamName1, ParamType2 ParamName2, ParamType3 ParamName3))
#define CREATE_JDELEGATE_TYPE_FourParams(DelegateName, ParamType1, ParamName1, ParamType2, ParamName2, ParamType3, ParamName3, ParamType4, ParamName4) CREATE_JDELEGATE_TYPE_INTERNAL(DelegateName, \
    JDELEGATE_CONCAT_HELPER(ParamType1, ParamType2, ParamType3, ParamType4), JDELEGATE_CONCAT_HELPER(ParamName1, ParamName2, ParamName3, ParamName4), \
    JDELEGATE_CONCAT_HELPER(ParamType1 ParamName1, ParamType2 ParamName2, ParamType3 ParamName3, ParamType4 ParamName4))
