// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "utils/type_traits_macros.h"
#include "engine/EngineContextObject.h"

namespace JumaEngine
{
	class AssetsManager;

	class AssetObject : public EngineContextObject
	{
        JUMAENGINE_CLASS(AssetObject, EngineContextObject)

		friend AssetsManager;
		
	public:
		AssetObject() = default;
		virtual ~AssetObject() override = default;

	private:

		AssetsManager* m_AssetsManager = nullptr;
	};

    template<typename T>
    class asset_ptr : public std::shared_ptr<T>
    {
        friend AssetsManager;

    public:
		using base_class = std::shared_ptr<T>;
    	
        asset_ptr() = default;
		asset_ptr(nullptr_t)
			: base_class(nullptr)
    	{}
        template<typename U, TEMPLATE_ENABLE(std::is_base_of_v<T, U>)>
        asset_ptr(const asset_ptr<U>& otherRef)
            : base_class(otherRef)
        {}

    private:

        asset_ptr(const std::shared_ptr<T>& asset)
            : base_class(asset)
        {}
        asset_ptr(std::shared_ptr<T>&& asset)
            : base_class(asset)
        {}

    public:

    	template<typename U, TEMPLATE_ENABLE(std::is_base_of_v<T, U>)>
    	bool isA() const { return dynamic_cast<U>(this->get()) != nullptr; }
    };
}
