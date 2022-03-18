// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "engine/EngineContextObject.h"

#include "jutils/jarray.h"

namespace JumaEngine
{
    class RenderTarget;

    class RenderGraph : public EngineContextObject
    {
        JUMAENGINE_ABSTRACT_CLASS(RenderGraph, EngineContextObject)

    public:
        RenderGraph() = default;
        virtual ~RenderGraph() override = default;

        int32 addNode(RenderTarget* renderTarget = nullptr);
        int32 addNodeOutput(int32 nodeIndex);
        bool addNodeInput(int32 nodeIndex, int32 edgeIndex);
        bool completeGraphBuild();

    protected:

        struct GraphEdge
        {
            int32 inputNode = -1;
            int32 outputNode = -1;
        };
        struct GraphNode
        {
            jarray<int32> inputEdges;
            jarray<int32> outputEdges;

            RenderTarget* renderTarget = nullptr;
        };

        jarray<int32> m_NodeIndices;

        bool m_GraphCompleted = false;


        virtual int32 createGraphEdge() = 0;
        virtual int32 createGraphNode() = 0;

        virtual GraphEdge* getGraphEdge(int32 edgeIndex) = 0;
        virtual GraphNode* getGraphNode(int32 nodeIndex) = 0;

        virtual bool completeGraphBuildInternal() = 0;

    private:

        bool sortGraphNodes();
    };
}
