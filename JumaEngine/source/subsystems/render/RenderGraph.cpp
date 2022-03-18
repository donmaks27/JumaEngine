// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "RenderGraph.h"

#include "jutils/jmap.h"

namespace JumaEngine
{
    int32 RenderGraph::addNode(RenderTarget* renderTarget)
    {
        const int32 index = !m_GraphCompleted ? createGraphNode() : -1;
        if (index != -1)
        {
            m_NodeIndices.add(index);
            if (renderTarget != nullptr)
            {
                getGraphNode(index)->renderTarget = renderTarget;
            }
        }
        return index;
    }
    int32 RenderGraph::addNodeOutput(const int32 nodeIndex)
    {
        GraphNode* node = !m_GraphCompleted ? getGraphNode(nodeIndex) : nullptr;
        if (node == nullptr)
        {
            return -1;
        }
        const int32 index = createGraphEdge();
        if (index != -1)
        {
            node->outputEdges.add(index);
        }
        return index;
    }
    bool RenderGraph::addNodeInput(const int32 nodeIndex, const int32 edgeIndex)
    {
        GraphNode* node = !m_GraphCompleted ? getGraphNode(nodeIndex) : nullptr;
        GraphEdge* edge = node != nullptr ? getGraphEdge(edgeIndex) : nullptr;
        if (edge == nullptr)
        {
            return false;
        }
        node->inputEdges.add(edgeIndex);
        return true;
    }
    bool RenderGraph::completeGraphBuild()
    {
        if (m_GraphCompleted || !sortGraphNodes() || !completeGraphBuildInternal())
        {
            return false;
        }
        m_GraphCompleted = true;
        return true;
    }

    bool RenderGraph::sortGraphNodes()
    {
        if (m_NodeIndices.isEmpty())
        {
            return true;
        }

        jmap<int32, GraphNode*> nodes;
        nodes.reserve(m_NodeIndices.getSize());
        for (const auto& nodeIndex : m_NodeIndices)
        {
            GraphNode* node = getGraphNode(nodeIndex);
            if (node == nullptr)
            {
                return false;
            }
            nodes.add(nodeIndex, node);
        }

        jarray<int32> sortedNodes;
        sortedNodes.reserve(m_NodeIndices.getSize());
        while (!nodes.isEmpty())
        {
            int32 nodeIndexToInsert = -1;
            for (const auto& indexAndNode : nodes)
            {
                bool readyToInsert = true;
                for (const auto& inputIndex : indexAndNode.value->inputEdges)
                {
                    if (!sortedNodes.contains(inputIndex))
                    {
                        readyToInsert = false;
                        break;
                    }
                }
                if (!readyToInsert)
                {
                    continue;
                }

                nodeIndexToInsert = indexAndNode.key;
                break;
            }
            if (nodeIndexToInsert == -1)
            {
                return false;
            }

            sortedNodes.add(nodeIndexToInsert);
            nodes.remove(nodeIndexToInsert);
        }

        m_NodeIndices = std::move(sortedNodes);
        return true;
    }
}
