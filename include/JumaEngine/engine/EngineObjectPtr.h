// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"

#include <jutils/jdescriptor_table.h>

namespace JumaEngine
{
	class Engine;
	class EngineContextObject;
	template<typename T>
	class EngineObjectWeakPtr;

    template<typename T>
	class EngineObjectPtr
	{
		friend Engine;
		template<typename T1>
		friend class EngineObjectPtr;
		template<typename T1>
		friend class EngineObjectWeakPtr;

		using PointerType = jdescriptor_table<EngineContextObject>::pointer;

	public:
		constexpr EngineObjectPtr() = default;
		constexpr EngineObjectPtr(nullptr_t) : EngineObjectPtr() {}
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		EngineObjectPtr(const EngineObjectPtr<T1>& ptr) : EngineObjectPtr(ptr.m_ObjectPointer) {}
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		EngineObjectPtr(EngineObjectPtr<T1>&& ptr) noexcept : EngineObjectPtr(std::move(ptr.m_ObjectPointer)) {}
		explicit EngineObjectPtr(const T* object)
            : m_ObjectPointer(object != nullptr ? object->weakPointerFromThis() : nullptr)
		{
		    forsceUpdatePtr();
		}
	private:
		EngineObjectPtr(const PointerType& pointer)
			: m_ObjectPointer(pointer)
		{
		    forsceUpdatePtr();
		}
		EngineObjectPtr(PointerType&& pointer)
			: m_ObjectPointer(std::move(pointer))
		{
		    forsceUpdatePtr();
		}
	public:

		constexpr EngineObjectPtr& operator=(nullptr_t)
		{
			m_ObjectPointer = nullptr;
			m_CachedObject = nullptr;
			return *this;
		}
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		EngineObjectPtr& operator=(const EngineObjectPtr<T1>& ptr)
		{
			if (this != &ptr)
			{
				this->operator=(ptr.m_ObjectPointer);
			}
			return *this;
		}
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		EngineObjectPtr& operator=(EngineObjectPtr<T1>&& ptr) noexcept
		{
			return this->operator=(std::move(ptr.m_ObjectPointer));
		}
	private:
		EngineObjectPtr& operator=(const PointerType& pointer)
		{
			m_ObjectPointer = pointer;
			forsceUpdatePtr();
			return *this;
		}
		EngineObjectPtr& operator=(PointerType&& pointer)
		{
			m_ObjectPointer = std::move(pointer);
			forsceUpdatePtr();
			return *this;
		}
	public:

		T* updatePtr() const { return m_CachedObject != nullptr ? forsceUpdatePtr() : nullptr; }
		constexpr bool isValid() const { return m_CachedObject != nullptr; }

		constexpr T* get() const { return m_CachedObject; }
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		EngineObjectPtr<T1> cast() const
		{
		    EngineContextObject* objectBase = m_ObjectPointer.get();

			T1* object = dynamic_cast<T1*>(objectBase);
			if (object == nullptr)
			{
			    m_CachedObject = dynamic_cast<T*>(objectBase);
				return nullptr;
			}
			m_CachedObject = object;

			EngineObjectPtr<T1> pointer;
			pointer.m_ObjectPointer = m_ObjectPointer;
			pointer.m_CachedObject = object;
			return pointer;
		}

		T* operator->() const { return get(); }
		T& operator*() const { return *get(); }

		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1> || is_base<T1, T>)>
		constexpr bool operator==(const EngineObjectPtr<T1>& ptr) const { return m_ObjectPointer == ptr.m_ObjectPointer; }
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1> || is_base<T1, T>)>
		constexpr bool operator!=(const EngineObjectPtr<T1>& ptr) const { return m_ObjectPointer != ptr.m_ObjectPointer; }
		
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1> || is_base<T1, T>)>
		constexpr bool operator==(const EngineObjectWeakPtr<T1>& ptr) const;
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1> || is_base<T1, T>)>
		constexpr bool operator!=(const EngineObjectWeakPtr<T1>& ptr) const { return !this->operator==(ptr); }
		
		constexpr bool operator==(nullptr_t) const { return !isValid(); }
		constexpr bool operator!=(nullptr_t) const { return isValid(); }

		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1> || is_base<T1, T>)>
		constexpr bool operator<(const EngineObjectPtr<T1>& ptr) const { return m_ObjectPointer < ptr.m_ObjectPointer; }
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1> || is_base<T1, T>)>
		constexpr bool operator<(const EngineObjectWeakPtr<T1>& ptr) const;

    private:

		mutable T* m_CachedObject = nullptr;
		PointerType m_ObjectPointer = nullptr;


		T* forsceUpdatePtr() const { return m_CachedObject = dynamic_cast<T*>(m_ObjectPointer.get()); }
	};

	template<typename T>
	class EngineObjectWeakPtr
	{
	    using PointerType = jdescriptor_table<EngineContextObject>::weak_pointer;

	public:
		constexpr EngineObjectWeakPtr() = default;
		constexpr EngineObjectWeakPtr(nullptr_t) : EngineObjectWeakPtr() {}
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		constexpr EngineObjectWeakPtr(const EngineObjectWeakPtr<T1>& ptr) : m_ObjectWeakPointer(ptr.m_ObjectWeakPointer) {}
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		constexpr EngineObjectWeakPtr(const EngineObjectPtr<T1>& ptr) : m_ObjectWeakPointer(ptr.m_ObjectPointer) {}
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		constexpr EngineObjectWeakPtr(EngineObjectPtr<T1>&& ptr) noexcept
	        : m_ObjectWeakPointer(ptr.m_ObjectPointer)
		{
		    ptr = nullptr;
		}
		explicit EngineObjectWeakPtr(const T* object)
	        : m_ObjectWeakPointer(object != nullptr ? static_cast<const EngineContextObject*>(object)->weakPointerFromThis() : nullptr)
	    {}

		constexpr EngineObjectWeakPtr& operator=(nullptr_t)
		{
		    m_ObjectWeakPointer = nullptr;
			return *this;
		}
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		constexpr EngineObjectWeakPtr& operator=(const EngineObjectWeakPtr<T1>& ptr)
		{
		    if (this != &ptr)
		    {
		        m_ObjectWeakPointer = ptr.m_ObjectWeakPointer;
		    }
			return *this;
		}
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		constexpr EngineObjectWeakPtr& operator=(const EngineObjectPtr<T1>& ptr)
		{
		    m_ObjectWeakPointer = ptr.m_ObjectPointer;
			return *this;
		}
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		constexpr EngineObjectWeakPtr& operator=(EngineObjectPtr<T1>&& ptr) noexcept
		{
		    m_ObjectWeakPointer = ptr.m_ObjectPointer;
			ptr = nullptr;
			return *this;
		}

		constexpr bool isValid() const { return m_ObjectWeakPointer != nullptr; }

		T* get() const { return dynamic_cast<T*>(m_ObjectWeakPointer.get()); }
		EngineObjectPtr<T> getObjectPtr() const { return jdescriptor_table<EngineContextObject>::pointer(m_ObjectWeakPointer); }

		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1> || is_base<T1, T>)>
		constexpr bool operator==(const EngineObjectWeakPtr<T1>& ptr) const { return m_ObjectWeakPointer == ptr.m_ObjectWeakPointer; }
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1> || is_base<T1, T>)>
		constexpr bool operator!=(const EngineObjectWeakPtr<T1>& ptr) const { return m_ObjectWeakPointer != ptr.m_ObjectWeakPointer; }
		
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1> || is_base<T1, T>)>
		constexpr bool operator==(const EngineObjectPtr<T1>& ptr) const { return m_ObjectWeakPointer == ptr.m_ObjectPointer; }
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1> || is_base<T1, T>)>
		constexpr bool operator!=(const EngineObjectPtr<T1>& ptr) const { return m_ObjectWeakPointer != ptr.m_ObjectPointer; }

		constexpr bool operator==(nullptr_t) const { return m_ObjectWeakPointer == nullptr; }
		constexpr bool operator!=(nullptr_t) const { return m_ObjectWeakPointer != nullptr; }

		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1> || is_base<T1, T>)>
		constexpr bool operator<(const EngineObjectWeakPtr<T1>& ptr) const { return m_ObjectWeakPointer < ptr.m_ObjectWeakPointer; }
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1> || is_base<T1, T>)>
		constexpr bool operator<(const EngineObjectPtr<T1>& ptr) const { return m_ObjectWeakPointer < ptr.m_ObjectPointer; }

	private:

		PointerType m_ObjectWeakPointer = nullptr;
	};

	template<typename T>
    template<typename T1, TEMPLATE_ENABLE_IMPL(is_base<T, T1> || is_base<T1, T>)>
    constexpr bool EngineObjectPtr<T>::operator==(const EngineObjectWeakPtr<T1>& ptr) const
    {
		return ptr == *this;
    }
    template<typename T>
    template<typename T1, std::enable_if_t<(is_base<T, T1> || is_base<T1, T>)>*>
    constexpr bool EngineObjectPtr<T>::operator<(const EngineObjectWeakPtr<T1>& ptr) const
	{
	    return m_ObjectPointer < ptr.m_ObjectWeakPointer;
	}
}
