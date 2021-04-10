// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "utils/type_traits_macros.h"

namespace JumaEngine
{
	class AssetsManager;
	
	class AssetObject
	{
		friend AssetsManager;
		
	protected:
		AssetObject() = default;
		virtual ~AssetObject() = default;

		virtual void terminate() {}

	private:

		AssetsManager* m_AssetsManager = nullptr;
	};

    template<typename T>
    class asset_ptr
    {
        friend AssetsManager;

    public:
        asset_ptr() = default;

        template<typename U, TEMPLATE_ENABLE(std::is_base_of_v<T, U>)>
        asset_ptr(const asset_ptr<U>& otherRef)
            : m_AssetWeakPtr(otherRef.getWeakPtr())
            , m_AssetPtr(otherRef.get())
        {}

    private:

        template<typename U, TEMPLATE_ENABLE(std::is_base_of_v<T, U>)>
        asset_ptr(const std::shared_ptr<U>& asset)
            : m_AssetWeakPtr(asset)
            , m_AssetPtr(asset.get())
        {}

    public:

        T* get() const
        {
            updatePtr();
            return m_AssetPtr;
        }
        const std::weak_ptr<AssetObject>& getWeakPtr() const { return m_AssetWeakPtr; }

        T* operator->() const { return get(); }
        
        bool operator==(nullptr_t) const { return get() == nullptr; }
        bool operator!=(nullptr_t) const { return get() != nullptr; }

        template<typename U, TEMPLATE_ENABLE(std::is_base_of_v<T, U>)>
        bool operator==(const U* otherPtr) const { return get() == otherPtr; }
        template<typename U, TEMPLATE_ENABLE(std::is_base_of_v<T, U>)>
        bool operator==(const asset_ptr<U>& otherRef) const { return operator==(otherRef.get()); }
        template<typename U, TEMPLATE_ENABLE(std::is_base_of_v<T, U>)>
        bool operator==(const std::shared_ptr<U>& otherRef) const { return get() == otherRef.get(); }

        template<typename U>
        bool operator!=(U otherPtr) const { return !operator==(otherPtr); }

    private:

        std::weak_ptr<AssetObject> m_AssetWeakPtr;
        mutable T* m_AssetPtr = nullptr;


        void updatePtr() const
        {
            if (m_AssetPtr != nullptr)
            {
                if (m_AssetWeakPtr.expired())
                {
                    m_AssetPtr = nullptr;
                }
            }
        }
    };
}
