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
            if (otherDelegate.delegate_container != nullptr)
            {
                delegate_container = otherDelegate.delegate_container->copy();
            }
        }
        jdelegate(jdelegate&& otherDelegate) noexcept
        {
            delegate_container = otherDelegate.delegate_container;
            otherDelegate.delegate_container = nullptr;
        }
        ~jdelegate() { clear(); }

        jdelegate& operator=(const jdelegate& otherDelegate)
        {
            if (this != &otherDelegate)
            {
                clear();
                if (otherDelegate.delegate_container != nullptr)
                {
                    delegate_container = otherDelegate.delegate_container->copy();
                }
            }
            return *this;
        }
        jdelegate& operator=(jdelegate&& otherDelegate) noexcept
        {
            clear();
            delegate_container = otherDelegate.delegate_container;
            otherDelegate.delegate_container = nullptr;
            return *this;
        }

        template<typename T>
        void bind(T* object, void (T::*function)(ArgTypes...))
        {
            clear();
            if (object != nullptr)
            {
                delegate_container = new jdelegate_container_implementation<T>(object, function);
            }
        }

        template<typename T>
        bool isBinded(T* object, void (T::*function)(ArgTypes...)) const
        {
            if ((object != nullptr) && (delegate_container != nullptr))
            {
                const jdelegate_container_implementation<T>* container = dynamic_cast<const jdelegate_container_implementation<T>*>(delegate_container);
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
            if ((object != nullptr) && (delegate_container != nullptr))
            {
                const jdelegate_container_implementation<T>* container = dynamic_cast<const jdelegate_container_implementation<T>*>(delegate_container);
                if (container != nullptr)
                {
                    return container->isBinded(object);
                }
            }
            return false;
        }

        void clear()
        {
            if (delegate_container != nullptr)
            {
                delete delegate_container;
                delegate_container = nullptr;
            }
        }

        void _call_internal(ArgTypes... args)
        {
            if (delegate_container != nullptr)
            {
                delegate_container->call(args...);
            }
        }

    private:

        class jdelegate_container_interface
        {
        public:

            virtual ~jdelegate_container_interface() = default;

            virtual jdelegate_container_interface* copy() = 0;

            virtual void call(ArgTypes...) = 0;
        };
        template<typename T>
        class jdelegate_container_implementation : public jdelegate_container_interface
        {
        public:

            using class_type = T;
            using function_type = void (class_type::*)(ArgTypes...);

            jdelegate_container_implementation(class_type* object, function_type function)
                : object(object)
                , function(function)
            {}
            virtual ~jdelegate_container_implementation() override = default;

            virtual jdelegate_container_interface* copy() override { return new jdelegate_container_implementation(object, function); }

            bool isBinded(class_type* checking_object, function_type checking_function) const { return isBinded(checking_object) && (function == checking_function); }
            bool isBinded(class_type* checking_object) const { return object == checking_object; }

            virtual void call(ArgTypes... args) override
            {
                if (object != nullptr)
                {
                    (object->*function)(args...);
                }
            }

        private:

            T* object;
            function_type function;
        };

        jdelegate_container_interface* delegate_container = nullptr;
    };
}

#define JUTILS_DELEGATE_CONCAT_HELPER(...) __VA_ARGS__

#define CREATE_JUTILS_DELEGATE_INTERNAL(DelegateName, ParamsTypes, ParamsNames, Params) \
class DelegateName : public jutils::jdelegate<ParamsTypes>                              \
{                                                                                       \
    using base_class = jutils::jdelegate<ParamsTypes>;                                  \
public:                                                                                 \
    DelegateName() : base_class() {}                                                    \
    DelegateName(const base_class& value) : base_class(value) {}                        \
    DelegateName(base_class&& value) noexcept : base_class(std::move(value)) {}         \
    void call(Params) { _call_internal(ParamsNames); }                                           \
}

#define CREATE_JUTILS_DELEGATE(DelegateName) CREATE_JUTILS_DELEGATE_INTERNAL(DelegateName, , , )
#define CREATE_JUTILS_DELEGATE_OneParam(DelegateName, ParamType1, ParamName1) CREATE_JUTILS_DELEGATE_INTERNAL(DelegateName, \
    JUTILS_DELEGATE_CONCAT_HELPER(ParamType1), JUTILS_DELEGATE_CONCAT_HELPER(ParamName1), \
    JUTILS_DELEGATE_CONCAT_HELPER(ParamType1 ParamName1))
#define CREATE_JUTILS_DELEGATE_TwoParams(DelegateName, ParamType1, ParamName1, ParamType2, ParamName2) CREATE_JUTILS_DELEGATE_INTERNAL(DelegateName, \
    JUTILS_DELEGATE_CONCAT_HELPER(ParamType1, ParamType2), JUTILS_DELEGATE_CONCAT_HELPER(ParamName1, ParamName2), \
    JUTILS_DELEGATE_CONCAT_HELPER(ParamType1 ParamName1, ParamType2 ParamName2))
#define CREATE_JUTILS_DELEGATE_ThreeParams(DelegateName, ParamType1, ParamName1, ParamType2, ParamName2, ParamType3, ParamName3) CREATE_JUTILS_DELEGATE_INTERNAL(DelegateName, \
    JUTILS_DELEGATE_CONCAT_HELPER(ParamType1, ParamType2, ParamType3), JUTILS_DELEGATE_CONCAT_HELPER(ParamName1, ParamName2, ParamName3), \
    JUTILS_DELEGATE_CONCAT_HELPER(ParamType1 ParamName1, ParamType2 ParamName2, ParamType3 ParamName3))
#define CREATE_JUTILS_DELEGATE_FourParams(DelegateName, ParamType1, ParamName1, ParamType2, ParamName2, ParamType3, ParamName3, ParamType4, ParamName4) CREATE_JUTILS_DELEGATE_INTERNAL(DelegateName, \
    JUTILS_DELEGATE_CONCAT_HELPER(ParamType1, ParamType2, ParamType3, ParamType4), JUTILS_DELEGATE_CONCAT_HELPER(ParamName1, ParamName2, ParamName3, ParamName4), \
    JUTILS_DELEGATE_CONCAT_HELPER(ParamType1 ParamName1, ParamType2 ParamName2, ParamType3 ParamName3, ParamType4 ParamName4))
