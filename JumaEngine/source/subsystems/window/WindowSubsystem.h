// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "subsystems/SubsystemBase.h"
#include "subsystems/render/RenderAPIObject.h"

#include "jutils/jmap.h"
#include "jutils/jarray.h"
#include "jutils/math/vector2.h"
#include "subsystems/render/RenderPresentMode.h"
#include "WindowID.h"

namespace JumaEngine
{
    class WindowSubsystem;

    struct WindowDescription
    {
        jstring title = JSTR("None");
        math::uvector2 size = { 0, 0 };

        jarray<RenderPresentMode> supportedPresentModes;
    };
    struct WindowDescription_RenderAPI
    {};

    class WindowSubsystem_RenderAPIObject : public RenderAPIObject<WindowSubsystem>
    {
        friend WindowSubsystem;

    public:
        WindowSubsystem_RenderAPIObject() = default;
        virtual ~WindowSubsystem_RenderAPIObject() override = default;

        virtual const WindowDescription_RenderAPI* findWindow(window_id_type windowID) const = 0;
        template<typename Type, TEMPLATE_ENABLE(is_base<WindowDescription_RenderAPI, Type>)>
        const Type* findWindow(const window_id_type windowID) const { return static_cast<const Type*>(findWindow(windowID)); }

        virtual bool shouldCloseWindow(window_id_type windowID) const = 0;

    protected:

        virtual bool initInternal() override { return true; }

        bool createWindowsFromParent();

        WindowDescription* findParentWindow(window_id_type windowID);

        virtual WindowDescription_RenderAPI* findWindow(window_id_type windowID) = 0;
        template<typename Type, TEMPLATE_ENABLE(is_base<WindowDescription_RenderAPI, Type>)>
        Type* findWindow(const window_id_type windowID) { return static_cast<Type*>(findWindow(windowID)); }

        virtual bool createWindow(window_id_type windowID) = 0;
        virtual void destroyWindow(window_id_type windowID) = 0;

        virtual void onWindowResized(window_id_type windowID, const math::uvector2& newSize);

        virtual void startRender() {}
        virtual void finishRender() {}
    };

    class WindowSubsystem final : public SubsystemBase, public RenderAPIWrapperBase<WindowSubsystem_RenderAPIObject>
    {
        JUMAENGINE_CLASS(WindowSubsystem, SubsystemBase)

        friend WindowSubsystem_RenderAPIObject;

    public:

        WindowSubsystem() = default;
        virtual ~WindowSubsystem() override = default;

        const jmap<window_id_type, WindowDescription>& getAllWindows() const { return m_Windows; }
        const WindowDescription* findWindow(const window_id_type windowID) const { return m_Windows.find(windowID); }
        bool isWindowValid(const window_id_type windowID) const { return m_Windows.contains(windowID); }

        window_id_type createWindow(const jstring& title, const math::uvector2& size);
        void destroyWindow(window_id_type windowID);
        bool shouldCloseWindow(window_id_type windowID) const;

        window_id_type getMainWindowID() const { return m_MainWindowID; }
        window_id_type createMainWindow(const jstring& title, const math::uvector2& size);
        void destroyMainWindow();
        bool shouldCloseMainWindow() const { return shouldCloseWindow(getMainWindowID()); }

        void startRender();
        void finishRender();

    protected:

        virtual WindowSubsystem_RenderAPIObject* createRenderAPIObjectInternal() override;

        virtual void clearSubsystemInternal() override;

    private:

        juid<window_id_type> m_WindowIDs;
        jmap<window_id_type, WindowDescription> m_Windows;

        window_id_type m_MainWindowID = INVALID_WINDOW_ID;
    };
}
