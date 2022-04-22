// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "subsystems/SubsystemBase.h"
#include "subsystems/render/RenderAPIObject.h"

#include "jutils/jdelegate_multicast.h"
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
        bool hidden = false;

        jstring title = JSTR("None");
        math::uvector2 size = { 0, 0 };

        jarray<RenderPresentMode> supportedPresentModes;
    };
    struct WindowDescription_RenderAPI
    {
        window_id_type windowID = INVALID_WINDOW_ID;
    };

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

        jmap<window_id_type, WindowDescription>& getParentWindows() const;
        bool createWindowsFromParent();

        WindowDescription* findParentWindow(window_id_type windowID);

        virtual WindowDescription_RenderAPI* findWindow(window_id_type windowID) = 0;
        template<typename Type, TEMPLATE_ENABLE(is_base<WindowDescription_RenderAPI, Type>)>
        Type* findWindow(const window_id_type windowID) { return static_cast<Type*>(findWindow(windowID)); }

        virtual bool createWindow(window_id_type windowID) = 0;
        virtual void destroyWindow(window_id_type windowID) = 0;

        virtual void onWindowResized(window_id_type windowID, const math::uvector2& newSize);

        virtual void onStartRender() {}
        virtual void onStartWindowRender(window_id_type windowID) {}
        virtual void onFinishWindowRender(window_id_type windowID) {}
        virtual void onFinishRender() {}
    };

    CREATE_JUTILS_MULTICAST_DELEGATE_OneParam(FOnWindowSubsystemWindowEvent, window_id_type, windowID);

    class WindowSubsystem final : public SubsystemBase, public RenderAPIWrapperBase<WindowSubsystem_RenderAPIObject>
    {
        JUMAENGINE_CLASS(WindowSubsystem, SubsystemBase)

        friend WindowSubsystem_RenderAPIObject;

    public:

        WindowSubsystem() = default;
        virtual ~WindowSubsystem() override = default;

        FOnWindowSubsystemWindowEvent onWindowCreated;
        FOnWindowSubsystemWindowEvent onWindowDestroying;

        const jmap<window_id_type, WindowDescription>& getWindows() const { return m_Windows; }
        const WindowDescription* findWindow(const window_id_type windowID) const { return m_Windows.find(windowID); }
        bool isWindowValid(const window_id_type windowID) const { return m_Windows.contains(windowID); }

        window_id_type createWindow(const jstring& title, const math::uvector2& size, bool hiddenWindow = false);
        void destroyWindow(window_id_type windowID);
        void destroyAllWindows();
        bool shouldCloseWindow(window_id_type windowID) const;

        window_id_type getMainWindowID() const { return m_MainWindowID; }
        window_id_type createMainWindow(const jstring& title, const math::uvector2& size);
        void destroyMainWindow();
        bool shouldCloseMainWindow() const { return shouldCloseWindow(getMainWindowID()); }

        void onStartRender();
        void onStartWindowRender(window_id_type windowID);
        void onFinishWindowRender(window_id_type windowID);
        void onFinishRender();

    protected:

        virtual WindowSubsystem_RenderAPIObject* createRenderAPIObjectInternal() override;

        virtual void clearSubsystemInternal() override;

    private:

        juid<window_id_type> m_WindowIDs;
        jmap<window_id_type, WindowDescription> m_Windows;

        window_id_type m_MainWindowID = INVALID_WINDOW_ID;
    };
}
