// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "engine/EngineContextObject.h"

#include "jutils/juid.h"
#include "jutils/jarray.h"
#include "jutils/math/vector2.h"
#include "subsystems/render/RenderPresentMode.h"

namespace JumaEngine
{
    class Window;

    using window_id_type = uint32;
    constexpr window_id_type INVALID_WINDOW_ID = juid<window_id_type>::invalidUID;

    class Window : public EngineContextObject
    {
        JUMAENGINE_ABSTRACT_CLASS(Window, EngineContextObject)

    public:
        Window() = default;
        virtual ~Window() override = default;

        virtual bool isValid() const = 0;

        virtual bool shouldClose() const = 0;
        virtual void destroy() = 0;

        window_id_type getID() const { return m_WindowID; }

        const jstring& getTitle() const { return m_Title; }
        const math::uvector2& getSize() const { return m_Size; }

        bool isSupportPresentMode(const RenderPresentMode presentMode) const { return m_SupportedPresentModes.contains(presentMode); }

        virtual void startRender() {}
        virtual void finishRender() {}

    protected:

        window_id_type m_WindowID = INVALID_WINDOW_ID;

        jstring m_Title;
        math::uvector2 m_Size = { 0, 0 };

        jarray<RenderPresentMode> m_SupportedPresentModes;


        virtual void onWindowResized(const math::uvector2& newSize) { m_Size = newSize; }
    };
}
