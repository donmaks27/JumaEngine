// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/RenderGraph.h"

namespace JumaEngine
{
    class RenderGraph_Vulkan : public RenderGraph
    {
        JUMAENGINE_CLASS(RenderGraph_Vulkan, RenderGraph)

    public:
        RenderGraph_Vulkan() = default;
        virtual ~RenderGraph_Vulkan() override = default;

    protected:

        struct GraphEdge_Vulkan : public GraphEdge
        {
            
        };
        struct GraphNode_Vulkan : public GraphNode
        {
            
        };

        jarray<GraphEdge_Vulkan> m_GraphEdges;
        jarray<GraphNode_Vulkan> m_GraphNodes;


        virtual int32 createGraphEdge() override
        {
            m_GraphEdges.addDefault();
            return m_GraphEdges.getSize() - 1;
        }
        virtual int32 createGraphNode() override
        {
            m_GraphNodes.addDefault();
            return m_GraphNodes.getSize() - 1;
        }

        virtual GraphEdge* getGraphEdge(const int32 edgeIndex) override { return m_GraphEdges.isValidIndex(edgeIndex) ? &m_GraphEdges[edgeIndex] : nullptr; }
        virtual GraphNode* getGraphNode(const int32 nodeIndex) override { return m_GraphNodes.isValidIndex(nodeIndex) ? &m_GraphNodes[nodeIndex] : nullptr; }
    };
}

#endif
