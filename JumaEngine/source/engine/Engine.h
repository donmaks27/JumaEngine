// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineContextObject.h"
#include "subsystems/render/RenderInterface.h"
#include "utils/jshared_ptr.h"
#include <thread>

namespace JumaEngine
{
    class Image;
    class VertexBuffer;
    class Material;
    class Shader;
    class RenderSubsystem;

    class Engine final : public IRenderInterface
    {
    public:
        Engine() = default;
        virtual ~Engine() override = default;

        EngineContextObject* createObject(const EngineContextObject::ClassType* objectClass);
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<EngineContextObject, T>)>
        T* createObject() { return EngineContextObject::cast<T>(createObject(T::getClass())); }

        bool startEngine();
        
        virtual void render(const RenderOptions& options) override;

        RenderSubsystem* getRenderSubsystem() const { return m_RenderSubsystem; }

    private:

        bool m_Started = false;

        RenderSubsystem* m_RenderSubsystem = nullptr;

        jshared_ptr<Image> m_Image = nullptr;
        jshared_ptr<Shader> m_Shader = nullptr;
        jshared_ptr<Material> m_Material = nullptr;
        jshared_ptr<VertexBuffer> m_VertexBuffer = nullptr;


        void registerEngineObject(EngineContextObject* object);

        bool startEngineInternal();
        bool initEngine();

        void startEngineLoop();

        void terminate();
    };


    
    class RenderManagerBase;
    class AssetsManager;
    class MeshFileImporterBase;
    class TextureFileImporterBase;

    class EngineWorld;
    class RenderTargetDirectBase;

    class EngineOld
    {
    public:
        EngineOld();
        virtual ~EngineOld() = default;

        bool isMainThread() const { return std::this_thread::get_id() == m_MainThreadID; }

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<MeshFileImporterBase, T>)>
        void setMeshFileImporter()
        {
            if (m_MeshFileImporter == nullptr)
            {
                m_MeshFileImporter = createObject<T>();
            }
        }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<TextureFileImporterBase, T>)>
        void setTextureFileImporter()
        {
            if (m_TextureFileImporter == nullptr)
            {
                m_TextureFileImporter = createObject<T>();
            }
        }

        RenderManagerBase* getRenderManager() const { return m_RenderManager; }
        MeshFileImporterBase* getVertexBufferImporter() const { return m_MeshFileImporter; }

        EngineContextObjectOld* createObject(const EngineContextObjectOld::ClassType* objectClass);
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<EngineContextObjectOld, T>)>
        T* createObject() { return dynamic_cast<T*>(createObject(T::getClass())); }

        bool startEngine(int argc, char** argv);
        bool startEngine() { return startEngine(0, nullptr); }
    	
        bool shouldStopEngine() const;

        EngineWorld* getActiveWorld() const { return m_World; }

    private:

        std::thread::id m_MainThreadID;

		bool m_EngineStarted = false;
    	
        RenderManagerBase* m_RenderManager = nullptr;
        MeshFileImporterBase* m_MeshFileImporter = nullptr;
        TextureFileImporterBase* m_TextureFileImporter = nullptr;
    	AssetsManager* m_AssetsManager = nullptr;

        EngineWorld* m_World = nullptr;

        RenderTargetDirectBase* m_RenderTarget = nullptr;


        void registerEngineObject(EngineContextObjectOld* object);

        bool startEngineInternal(int argc, char** argv);

        bool initEngine();
        void onEngineInit();

        void startEngineLoop();
    	void onEngineLoopStart();

        void tick(double deltaTime);
    	void render();

        void stopEngine();
        void terminateEngine();
    };
}
