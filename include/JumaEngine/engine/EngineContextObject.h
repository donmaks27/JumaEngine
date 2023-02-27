// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "EngineClass.h"

#include <jutils/jdescriptor_table.h>

namespace JumaEngine
{
    class Engine;
	template<typename T>
	class EngineObjectPtr;

    class EngineContextObject
    {
        friend Engine;
		friend EngineObjectPtr<EngineContextObject>;

		using WeakPointerType = jdescriptor_table<EngineContextObject>::weak_pointer;

    public:

        class EngineContextObject_Class : public EngineClass
        {
            friend Engine;

        public:
            EngineContextObject_Class() = default;
            virtual ~EngineContextObject_Class() override = default;

            virtual jstring getClassName() const override { return JSTR("EngineContextObject"); }

        protected:

            virtual bool isDerrivedOrEqual(const EngineClass* engineClass) const override
            {
                return (engineClass != nullptr) && (engineClass == EngineContextObject::GetClassStatic());
            }
            virtual EngineContextObject* createBaseObject() const override { return createObject<EngineContextObject>(); }
        };
        using ClassType = EngineContextObject_Class;

        static ClassType* GetClassStatic()
        {
            static ClassType engineClass;
            return &engineClass;
        }
        virtual EngineClass* getClass() const = 0;

        EngineContextObject() = default;
        virtual ~EngineContextObject() = default;

        Engine* getEngine() const { return m_Engine; }
        template<typename T, TEMPLATE_ENABLE(is_base<Engine, T>)>
        T* getEngine() const { return dynamic_cast<T*>(getEngine()); }

    private:

        Engine* m_Engine = nullptr;
		WeakPointerType m_ObjectWeakPointer = nullptr;
    };
	
	template<typename T>
	class EngineObjectPtr
	{
		static_assert(is_base<EngineContextObject, T>, "Object class should be derrived from EngineContextObject");

		friend Engine;

		using PointerType = jdescriptor_table<EngineContextObject>::pointer;

	public:
		constexpr EngineObjectPtr() = default;
		constexpr EngineObjectPtr(nullptr_t) : EngineObjectPtr() {}
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		EngineObjectPtr(const EngineObjectPtr<T1>& ptr)
			: m_ObjectPointer(ptr.m_ObjectPointer)
		{}
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		constexpr EngineObjectPtr(EngineObjectPtr<T1>&& ptr) noexcept
			: m_ObjectPointer(std::move(ptr.m_ObjectPointer))
		{}
		explicit EngineObjectPtr(const T* object)
			: m_ObjectPointer(object != nullptr ? object->m_ObjectWeakPointer : nullptr)
		{}
	private:
		constexpr EngineObjectPtr(const PointerType& pointer)
			: m_ObjectPointer(pointer)
		{}
		constexpr EngineObjectPtr(PointerType&& pointer)
			: m_ObjectPointer(std::move(pointer))
		{}
	public:

		EngineObjectPtr& operator=(nullptr_t)
		{
			m_ObjectPointer = nullptr;
			return *this;
		}
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		EngineObjectPtr& operator=(const EngineObjectPtr<T1>& ptr)
		{
			if (this != &ptr)
			{
				m_ObjectPointer = ptr.m_ObjectPointer;
			}
			return *this;
		}
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		EngineObjectPtr& operator=(EngineObjectPtr<T1>&& ptr) noexcept
		{
			m_ObjectPointer = std::move(ptr.m_ObjectPointer);
			return *this;
		}
	private:
		EngineObjectPtr& operator=(const PointerType& pointer)
		{
			m_ObjectPointer = pointer;
			return *this;
		}
		EngineObjectPtr& operator=(PointerType&& pointer)
		{
			m_ObjectPointer = std::move(pointer);
			return *this;
		}
	public:

		bool isValid() const { return m_ObjectPointer.isValid(); }

		T* get() const { return dynamic_cast<T*>(m_ObjectPointer.get()); }
		T* operator->() const { return dynamic_cast<T*>(m_ObjectPointer.get()); }
		T& operator*() const { return *get(); }

		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		constexpr bool operator==(const EngineObjectPtr<T1>& ptr) const { return m_ObjectPointer == ptr.m_ObjectPointer; }
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		constexpr bool operator!=(const EngineObjectPtr<T1>& ptr) const { return m_ObjectPointer != ptr.m_ObjectPointer; }
		
		constexpr bool operator==(nullptr_t) const { return m_ObjectPointer == nullptr; }
		constexpr bool operator!=(nullptr_t) const { return m_ObjectPointer != nullptr; }

	private:

		PointerType m_ObjectPointer = nullptr;
	};
}

#define DECLARE_JUMAENGINE_CLASS(ClassName, ParentClassName)                                                    \
public:                                                                                                         \
    class ClassName##_Class : public ParentClassName::ClassType                                                 \
    {                                                                                                           \
		friend JumaEngine::Engine;                                                                              \
    public:                                                                                                     \
        ClassName##_Class() = default;                                                                          \
        virtual ~ClassName##_Class() override = default;                                                        \
        virtual jutils::jstring getClassName() const override { return JSTR(#ClassName); }                      \
    protected:                                                                                                  \
        virtual bool isDerrivedOrEqual(const JumaEngine::EngineClass* engineClass) const override               \
        {                                                                                                       \
            if (engineClass == nullptr) { return false; }                                                       \
            if (engineClass == ClassName::GetClassStatic()) { return true; }                                    \
            return ParentClassName::ClassType::isDerrivedOrEqual(engineClass);                                  \
        }                                                                                                       \
        virtual EngineContextObject* createBaseObject() const override { return createObject<ClassName>(); }    \
    };                                                                                                          \
    using ClassType = ClassName##_Class;                                                                        \
    static ClassType* GetClassStatic()                                                                          \
    {                                                                                                           \
        static ClassType engineClass;                                                                           \
        return &engineClass;                                                                                    \
    }                                                                                                           \
    using Super = ParentClassName;

#define JUMAENGINE_CLASS(ClassName, ParentClassName) DECLARE_JUMAENGINE_CLASS(ClassName, ParentClassName)   \
    virtual JumaEngine::EngineClass* getClass() const override { return ClassName::GetClassStatic(); }      \
private:

#define JUMAENGINE_CLASS_ABSTRACT(ClassName, ParentClassName) DECLARE_JUMAENGINE_CLASS(ClassName, ParentClassName)  \
    virtual JumaEngine::EngineClass* getClass() const override = 0;                                                 \
private:
