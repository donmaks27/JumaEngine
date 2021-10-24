// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderOptions.h"
#include "subsystems/SubsystemBase.h"
#include "subsystems/render/window/WindowDescription.h"

namespace JumaEngine
{
    class RenderSubsystem : public SubsystemBase
    {
        JUMAENGINE_CLASS(RenderSubsystem, SubsystemBase)

    public:
        RenderSubsystem() = default;
        virtual ~RenderSubsystem() override = default;

        bool shouldCloseMainWindow() const { return (m_MainWindow != nullptr) && shouldCloseWindowInternal(m_MainWindow); }

        glm::uvec2 getMainWindowSize() const { return m_MainWindow != nullptr ? m_MainWindow->size : glm::uvec2(0); }
        void setMainWindowSize(const glm::uvec2& size);

        jstring getMainWindowTitle() const { return m_MainWindow != nullptr ? m_MainWindow->title : JSTR(""); }
        void setMainWindowTitle(const jstring& title);

        virtual void render(const RenderQuery& query) = 0;

    protected:
        
        virtual void terminateSubsystem() override;

        const WindowDescription* getMainWindow() const { return m_MainWindow; }

        void createMainWindow();
        void terminateMainWindow();

        virtual WindowDescription* createWindowInternal(const glm::uvec2& size, const jstring& title) = 0;
        virtual void terminateWindowInternal(WindowDescription* window) = 0;

        virtual bool shouldCloseWindowInternal(const WindowDescription* window) const = 0;
        virtual void setWindowSizeInternal(const WindowDescription* window, const glm::uvec2& size) = 0;
        virtual void setWindowTitleInternal(const WindowDescription* window, const jstring& title) = 0;

        void callEngineRender(const RenderOptions& options) const;

    private:

        WindowDescription* m_MainWindow = nullptr;
    };
}
