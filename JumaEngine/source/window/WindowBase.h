// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineContextObject.h"
#include "render/RenderAPI.h"
#include "glm/vec2.hpp"

namespace JumaEngine
{
    class WindowBase : public EngineContextObject
    {
    public:
        WindowBase() = default;
        virtual ~WindowBase() override = default;

        void setWindowTitle(const jstring& title);
    	jstring getWindowTite() const { return m_WindowTitle; }

        void setWindowSize(const glm::uvec2& windowSize);
        glm::uvec2 getWindowSize() const { return m_WindowSize; }
        float getWindowAspectRatio() const;

        bool init();
        virtual bool isInit() const = 0;
        virtual void termiante() = 0;

        virtual void onEngineLoopStart() = 0;
        virtual void onRenderFinish() = 0;

        virtual double getDeltaTime() const = 0;
        virtual bool shouldCloseWindow() const = 0;

    protected:

    	jstring m_WindowTitle = JTEXT("JUMAEngine");
        glm::uvec2 m_WindowSize = glm::uvec2(800, 600);


        virtual bool isSupportedRenderAPI(RenderAPI api) const = 0;
		virtual void initInternal() = 0;

    	virtual void updateWindowTitle() = 0;
        virtual void updateWindowSize() = 0;
    };
}
