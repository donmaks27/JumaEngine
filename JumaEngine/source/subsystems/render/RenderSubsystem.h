// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "subsystems/SubsystemBase.h"
#include "RenderAPIObject.h"

#include "RenderAPI.h"
#include "RenderPresentMode.h"
#include "jutils/jmap.h"
#include "vertex/VertexBufferData.h"

namespace JumaEngine
{
    class Texture;
    class RenderPipeline;
    class RenderPipeline_RenderAPIObject;
    class RenderTarget_RenderAPIObject;
    class Shader_RenderAPIObject;
    class Material_RenderAPIObject;
    class VertexBuffer_RenderAPIObject;
    class Texture_RenderAPIObject;
    class RenderSubsystem;

    class RenderSubsystem_RenderAPIObject : public RenderAPIObject<RenderSubsystem>
    {
        friend RenderSubsystem;

    public:
        RenderSubsystem_RenderAPIObject() = default;
        virtual ~RenderSubsystem_RenderAPIObject() override = default;

    protected:

        virtual bool initInternal() override { return true; }

        virtual void onVertexTypeRegistered(const jstringID& vertexName, const VertexDescription& description) {}

        virtual Shader_RenderAPIObject* createShaderObject() = 0;
        virtual Material_RenderAPIObject* createMaterialObject() = 0;
        virtual VertexBuffer_RenderAPIObject* createVertexBufferObject() = 0;
        virtual Texture_RenderAPIObject* createTextureObject() = 0;
        virtual RenderTarget_RenderAPIObject* createRenderTargetObject() = 0;
        virtual RenderPipeline_RenderAPIObject* createRenderPipelineObject() = 0;

        virtual void onStartRender() {}
        virtual void onFinishRender() {}

        virtual void waitForRenderFinish() {}

        void clearData();
    };

    class RenderSubsystem final : public SubsystemBase, public RenderAPIWrapperBase<RenderSubsystem_RenderAPIObject>
    {
        JUMAENGINE_CLASS(RenderSubsystem, SubsystemBase)

        friend RenderSubsystem_RenderAPIObject;

    public:
        RenderSubsystem() = default;
        virtual ~RenderSubsystem() override = default;

        RenderAPI getRenderAPI() const { return m_CurrentRenderAPI; }
        RenderPresentMode getPresentMode() const { return m_CurrentPresentMode; }

        template<typename T, TEMPLATE_ENABLE(is_vertex_type<T>)>
        void registerVertexType()
        {
            const jstringID& vertexName = VertexInfo<T>::getVertexName();
            if (!m_RegisteredVertexTypes.contains(vertexName))
            {
                const VertexDescription& description = m_RegisteredVertexTypes.add(
                    vertexName, { VertexInfo<T>::getVertexSize(), VertexInfo<T>::getVertexComponents() }
                );
                this->onVertexTypeRegistered(vertexName, description);
            }
        }
        const VertexDescription* findVertexDescription(const jstringID& vertexName) const { return m_RegisteredVertexTypes.find(vertexName); }

        template<typename T, TEMPLATE_ENABLE(is_vertex_type<T>)>
        VertexBufferDataBase* createVertexBufferData(jarray<T> vertices)
        {
            registerVertexType<T>();
            VertexBufferData<T>* data = new VertexBufferData<T>;
            data->setVertices(std::move(vertices));
            return data;
        }
        template<typename T, TEMPLATE_ENABLE(is_vertex_type<T>)>
        VertexBufferDataBase* createVertexBufferData(jarray<T> vertices, jarray<uint32> indices)
        {
            VertexBufferDataBase* vertexBufferData = createVertexBufferData(std::move(vertices));
            vertexBufferData->setVertexIndices(std::move(indices));
            return vertexBufferData;
        }

        Texture* getDefaultTexture() const { return m_DefaultTexture; }
        RenderPipeline* getRenderPipeline() const { return m_RenderPipeline; }

        Shader_RenderAPIObject* createShaderObject();
        Material_RenderAPIObject* createMaterialObject();
        VertexBuffer_RenderAPIObject* createVertexBufferObject();
        Texture_RenderAPIObject* createTextureObject();
        RenderTarget_RenderAPIObject* createRenderTargetObject();
        RenderPipeline_RenderAPIObject* createRenderPipelineObject();

        void render();
        void waitForRenderFinish();

    protected:

        virtual bool initSubsystemInternal() override;
        virtual void clearSubsystemInternal() override;

        virtual RenderSubsystem_RenderAPIObject* createRenderAPIObjectInternal() override;
        virtual void onRenderAPIObjectInitialized() override;
        
    private:

        RenderAPI m_CurrentRenderAPI = RenderAPI::OpenGL;
        RenderPresentMode m_CurrentPresentMode = RenderPresentMode::VSync;

        jmap<jstringID, VertexDescription> m_RegisteredVertexTypes;

        Texture* m_DefaultTexture = nullptr;
        RenderPipeline* m_RenderPipeline = nullptr;


        void onVertexTypeRegistered(const jstringID& vertexName, const VertexDescription& description);
    };
}
