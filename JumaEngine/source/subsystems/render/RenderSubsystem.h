// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "subsystems/SubsystemBase.h"

#include "RenderOptions.h"
#include "subsystems/render/WindowDescription.h"

namespace JumaEngine
{
    class RenderPrimitive;
    class Image;
    class VertexBuffer;
    class Material;
    class Shader;

    class RenderSubsystem : public SubsystemBase
    {
        JUMAENGINE_ABSTRACT_CLASS(RenderSubsystem, SubsystemBase)

    public:
        RenderSubsystem() = default;
        virtual ~RenderSubsystem() override = default;
        
        WindowDescription* getMainWindow() const { return m_MainWindow; }
        bool shouldCloseMainWindow() const { return (m_MainWindow != nullptr) && shouldCloseWindowInternal(m_MainWindow); }

        glm::uvec2 getMainWindowSize() const { return m_MainWindow != nullptr ? m_MainWindow->size : glm::uvec2(0); }
        void setMainWindowSize(const glm::uvec2& size) { setWindowSize(getMainWindow(), size); }

        jstring getMainWindowTitle() const { return m_MainWindow != nullptr ? m_MainWindow->title : JSTR(""); }
        void setMainWindowTitle(const jstring& title);

        virtual void render() = 0;

        virtual void onEnginePreTerminate() {}

        virtual jshared_ptr<VertexBuffer> createVertexBuffer() = 0;
        virtual jshared_ptr<Shader> createShader() = 0;
        virtual jshared_ptr<Material> createMaterial() = 0;
        virtual jshared_ptr<Image> createImage() = 0;
        virtual jshared_ptr<RenderPrimitive> createRenderPrimitive() = 0;

    protected:

        template<typename T>
        static T* castWindow(WindowDescription* window) { return dynamic_cast<T*>(window); }

        bool createMainWindow();
        void terminateMainWindow();

        virtual void setWindowSize(WindowDescription* window, const glm::uvec2& size);

        virtual WindowDescription* createWindowInternal(const glm::uvec2& size, const jstring& title) = 0;
        virtual void terminateWindowInternal(WindowDescription* window) = 0;

        virtual bool shouldCloseWindowInternal(WindowDescription* window) const = 0;
        virtual void setWindowSizeInternal(WindowDescription* window, const glm::uvec2& size) = 0;
        virtual void setWindowTitleInternal(WindowDescription* window, const jstring& title) = 0;

        void callEngineRender(const RenderOptions& options) const;

    private:

        WindowDescription* m_MainWindow = nullptr;
    };
}
