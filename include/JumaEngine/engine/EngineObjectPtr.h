// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"

#include <jutils/jdescriptor_table.h>

namespace JumaEngine
{
	class Engine;
	class EngineContextObject;
	template<typename T>
	class EngineObjectPtr;

	class EngineObjectPtrBase
	{
		friend Engine;
		friend EngineContextObject;

	protected:

		using TableType = jdescriptor_table<EngineContextObject>;
		using PointerType = jdescriptor_table_pointer<TableType::uid_type>;

		constexpr EngineObjectPtrBase() = default;
		constexpr EngineObjectPtrBase(const EngineObjectPtrBase&) = default;
	public:

		constexpr bool operator<(const EngineObjectPtrBase& ptr) const { return m_ObjectPointer < ptr.m_ObjectPointer; }

	protected:

		PointerType m_ObjectPointer;


		bool isDescriptorValid() const;
		EngineContextObject* getObject() const;
		template<typename T>
		T* getObject() const { return dynamic_cast<T*>(getObject()); }
		static PointerType GetPointerFromObject(EngineContextObject* object);

		bool addReference();
		bool removeReference();
		
	private:

		static Engine* s_Engine;
	};

	template<typename T>
	class EngineObjectWeakPtr : public EngineObjectPtrBase
	{
		template<typename Type>
		friend class EngineObjectWeakPtr;
		template<typename Type>
		friend class EngineObjectPtr;

	public:
		constexpr EngineObjectWeakPtr() = default;
		constexpr EngineObjectWeakPtr(nullptr_t) : EngineObjectWeakPtr() {}
		EngineObjectWeakPtr(const EngineObjectWeakPtr& ptr)
		{
			m_ObjectPointer = ptr.m_ObjectPointer;
			m_CachedObject = ptr.updatePtr();
		}
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		EngineObjectWeakPtr(const EngineObjectWeakPtr<T1>& ptr)
		{
			m_ObjectPointer = ptr.m_ObjectPointer;
			m_CachedObject = ptr.updatePtr();
		}
		explicit EngineObjectWeakPtr(T* object)
		{
			m_ObjectPointer = EngineObjectPtrBase::GetPointerFromObject(object);
			if (m_ObjectPointer != nullptr)
			{
				m_CachedObject = object;
			}
		}

		constexpr EngineObjectWeakPtr& operator=(nullptr_t)
		{
			m_ObjectPointer = nullptr;
			m_CachedObject = nullptr;
			return *this;
		}
		EngineObjectWeakPtr& operator=(const EngineObjectWeakPtr& ptr) { return this->_assignWeak(ptr); }
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		EngineObjectWeakPtr& operator=(const EngineObjectWeakPtr<T1>& ptr) { return this->_assignWeak(ptr); }

		T* updatePtr() const
		{
			if ((m_CachedObject != nullptr) && !isDescriptorValid())
			{
				m_CachedObject = nullptr;
			}
			return m_CachedObject;
		}
		T* get() const { return m_CachedObject; }
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		EngineObjectWeakPtr<T1> castWeak() const
		{
			T1* object = dynamic_cast<T1*>(updatePtr());
			if (object == nullptr)
			{
				return nullptr;
			}
			EngineObjectWeakPtr<T1> pointer;
			pointer.m_ObjectPointer = m_ObjectPointer;
			pointer.m_CachedObject = object;
			return pointer;
		}

		T* operator->() const { return get(); }
		T& operator*() const { return *get(); }

		bool operator==(nullptr_t) const { return updatePtr() == nullptr; }
		bool operator!=(nullptr_t) const { return !this->operator==(nullptr); }

		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1> || is_base<T1, T>)>
		bool operator==(const EngineObjectWeakPtr<T1>& ptr) const { return updatePtr() == ptr.updatePtr(); }
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1> || is_base<T1, T>)>
		bool operator!=(const EngineObjectWeakPtr<T1>& ptr) const { return !this->operator==(ptr); }
		
	protected:

		mutable T* m_CachedObject = nullptr;

	private:

		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		EngineObjectWeakPtr& _assignWeak(const EngineObjectWeakPtr<T1>& ptr)
		{
			if (this != &ptr)
			{
				if (m_ObjectPointer != ptr.m_ObjectPointer)
				{
					m_ObjectPointer = ptr.m_ObjectPointer;
					m_CachedObject = ptr.updatePtr();
				}
				else
				{
					m_CachedObject = ptr.updatePtr();
				}
			}
			return *this;
		}
	};

	template<typename T>
	class EngineObjectPtr : public EngineObjectWeakPtr<T>
	{
		friend Engine;
		template<typename Type>
		friend class EngineObjectPtr;

		using Super = EngineObjectWeakPtr<T>;

	public:
		constexpr EngineObjectPtr() = default;
		constexpr EngineObjectPtr(nullptr_t) : EngineObjectPtr() {}
		EngineObjectPtr(const EngineObjectPtr& ptr)
			: Super(ptr)
		{
			EngineObjectPtrBase::addReference();
		}
		EngineObjectPtr(EngineObjectPtr&& ptr) noexcept
			: Super(ptr)
		{
			EngineObjectPtrBase::m_ObjectPointer = ptr.m_ObjectPointer;
			Super::m_CachedObject = ptr.m_CachedObject;
			Super::updatePtr();

			ptr.m_ObjectPointer = nullptr;
			ptr.m_CachedObject = nullptr;
		}
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		EngineObjectPtr(const EngineObjectPtr<T1>& ptr)
			: Super(ptr)
		{
			EngineObjectPtrBase::addReference();
		}
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		EngineObjectPtr(EngineObjectPtr<T1>&& ptr) noexcept
		{
			EngineObjectPtrBase::m_ObjectPointer = ptr.m_ObjectPointer;
			Super::m_CachedObject = ptr.m_CachedObject;
			Super::updatePtr();

			ptr.m_ObjectPointer = nullptr;
			ptr.m_CachedObject = nullptr;
		}
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		EngineObjectPtr(const EngineObjectWeakPtr<T1>& ptr)
			: Super(ptr)
		{
			EngineObjectPtrBase::addReference();
		}
		explicit EngineObjectPtr(T* object)
			: Super(object)
		{
			EngineObjectPtrBase::addReference();
		}
		~EngineObjectPtr()
		{
			EngineObjectPtrBase::removeReference();
		}
	private:
		EngineObjectPtr(const EngineObjectPtrBase::PointerType& pointer)
		{
			EngineObjectPtrBase::m_ObjectPointer = pointer;
			Super::m_CachedObject = EngineObjectPtrBase::getObject<T>();
			EngineObjectPtrBase::addReference();
		}
	public:

		EngineObjectPtr& operator=(nullptr_t)
		{
			if (EngineObjectPtrBase::m_ObjectPointer != nullptr)
			{
				EngineObjectPtrBase::removeReference();
				EngineObjectPtrBase::m_ObjectPointer = nullptr;
				Super::m_CachedObject = nullptr;
			}
			return *this;
		}
		EngineObjectPtr& operator=(const EngineObjectPtr& ptr) { return this->_assign(ptr); }
		EngineObjectPtr& operator=(EngineObjectPtr&& ptr) noexcept { return this->_assign(std::move(ptr)); }
		template<typename T1, TEMPLATE_ENABLE(is_base_and_not_same<T, T1>)>
		EngineObjectPtr& operator=(const EngineObjectPtr<T1>& ptr) { return this->_assign(ptr); }
		template<typename T1, TEMPLATE_ENABLE(is_base_and_not_same<T, T1>)>
		EngineObjectPtr& operator=(EngineObjectPtr<T1>&& ptr) noexcept { return this->_assign(std::move(ptr)); }
		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		EngineObjectPtr& operator=(const EngineObjectWeakPtr<T1>& ptr) { return this->_assign(ptr); }

		template<typename T1, TEMPLATE_ENABLE(is_base<T, T1>)>
		EngineObjectPtr<T1> cast() const
		{
			T1* object = dynamic_cast<T1*>(Super::updatePtr());
			if (object == nullptr)
			{
				return nullptr;
			}
			EngineObjectPtr<T1> pointer;
			pointer.m_ObjectPointer = EngineObjectPtrBase::m_ObjectPointer;
			pointer.m_CachedObject = object;
			pointer.addReference();
			return pointer;
		}

	private:

		template<typename T1>
		EngineObjectPtr& _assign(const EngineObjectWeakPtr<T1>& ptr)
		{
			if (this != &ptr)
			{
				if (EngineObjectPtrBase::m_ObjectPointer != ptr.m_ObjectPointer)
				{
					EngineObjectPtrBase::removeReference();
					EngineObjectPtrBase::m_ObjectPointer = ptr.m_ObjectPointer;
					Super::m_CachedObject = ptr.updatePtr();
					EngineObjectPtrBase::addReference();
				}
				else
				{
					Super::m_CachedObject = ptr.updatePtr();
				}
			}
			return *this;
		}
		template<typename T1>
		EngineObjectPtr& _assign(EngineObjectPtr<T1>&& ptr) noexcept
		{
			if (EngineObjectPtrBase::m_ObjectPointer != ptr.m_ObjectPointer)
			{
				EngineObjectPtrBase::removeReference();

				EngineObjectPtrBase::m_ObjectPointer = ptr.m_ObjectPointer;
				Super::m_CachedObject = ptr.m_CachedObject;
				Super::updatePtr();

				ptr.m_ObjectPointer = nullptr;
				ptr.m_CachedObject = nullptr;
			}
			else
			{
				Super::updatePtr();
				ptr = nullptr;
			}
			return *this;
		}
	};
}
