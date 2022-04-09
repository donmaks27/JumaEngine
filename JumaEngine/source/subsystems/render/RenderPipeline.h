// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "engine/EngineContextObject.h"
#include "RenderAPIObject.h"

#include "jutils/jmap.h"
#include "jutils/jset.h"
#include "jutils/jstringID.h"
#include "subsystems/window/WindowID.h"

namespace JumaEngine
{
    struct RenderOptions;
    class RenderTarget;
    class RenderPipeline;
    
    enum class RenderPipelineStageType : uint8
    {
        RenderTarget, Window
    };
    struct RenderPipelineStage
    {
        RenderPipelineStageType type = RenderPipelineStageType::RenderTarget;
        jset<jstringID> dependencies;
    };

    class RenderPipeline_RenderAPIObject : public RenderAPIObject<RenderPipeline>
    {
        friend RenderPipeline;

    public:
        RenderPipeline_RenderAPIObject() = default;
        virtual ~RenderPipeline_RenderAPIObject() override = default;

    protected:

        virtual bool onRenderPipelineUpdated() { return true; }

        virtual bool renderPipeline() = 0;
        void renderPipelineStage(RenderOptions* options);
    };

    class RenderPipeline final : public EngineContextObject, public RenderAPIWrapper<RenderPipeline_RenderAPIObject>
    {
        JUMAENGINE_CLASS(RenderPipeline, EngineContextObject)

    public:
        RenderPipeline() = default;
        virtual ~RenderPipeline() override;

        bool init();

        bool isPipelineQueueValid() const { return isValid() && m_PipelineQueueValid; }
        const jarray<jstringID>& getPipelineQueue() const { return m_PipelineStagesQueue; }

        const jmap<jstringID, RenderPipelineStage>& getPipelineStages() const { return m_PipelineStages; }
        const RenderPipelineStage* getPipelineStage(const jstringID& name) const { return m_PipelineStages.find(name); }
        window_id_type getPipelineStageWindow(const jstringID& name) const;
        RenderTarget* getPipelineStageRenderTarget(const jstringID& name) const;

        bool addWindowPipelineStage(const jstringID& name, window_id_type windowID);
        bool addRenderTargetPipelineStage(const jstringID& name, RenderTarget* renderTarget);
        bool removePipelineStage(const jstringID& name);

        bool addPipelineStageDependency(const jstringID& name, const jstringID& dependencyName);
        bool removePipelineStageDependency(const jstringID& name, const jstringID& dependencyName);
        
        bool validatePipelineQueue();

        bool render();
        
    protected:

        virtual RenderPipeline_RenderAPIObject* createRenderAPIObjectInternal() override;

        virtual void clearInternal() override { clearData(); }

    private:

        jmap<jstringID, RenderPipelineStage> m_PipelineStages;
        jmap<jstringID, window_id_type> m_PipelineStages_Window;
        jmap<jstringID, RenderTarget*> m_PipelineStages_RenderTarget;

        jarray<jstringID> m_PipelineStagesQueue;
        bool m_PipelineQueueValid = false;


        void clearData();
    };
}
