// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "Node.hpp"

namespace ouzel
{
    namespace audio
    {
        Node::~Node()
        {
            for (Node* destination : destinations)
                destination->removeSource(this);
        }

        void Node::process(std::vector<float>& samples, uint16_t& channels,
                           uint32_t& sampleRate, Vector3& position)
        {
            buffer.resize(samples.size());
            std::fill(buffer.begin(), buffer.end(), 0.0F);

            for (Node* source : sources)
            {
                source->process(buffer, channels, sampleRate, position);

                for (uint32_t i = 0; i < samples.size(); ++i)
                    samples[i] += buffer[i];
            }
        }

        void Node::addSource(Node* node)
        {
            auto i = std::find(sources.begin(), sources.end(), node);
            if (i == sources.end())
            {
                node->addDestination(this);
                sources.push_back(node);
            }
        }

        void Node::removeSource(Node* node)
        {
            auto i = std::find(sources.begin(), sources.end(), node);
            if (i != sources.end())
            {
                node->removeDestination(this);
                sources.erase(i);
            }
        }

        void Node::addDestination(Node* node)
        {
            auto i = std::find(destinations.begin(), destinations.end(), node);
            if (i == destinations.end())
                destinations.push_back(node);
        }

        void Node::removeDestination(Node* node)
        {
            auto i = std::find(destinations.begin(), destinations.end(), node);
            if (i != destinations.end())
                destinations.erase(i);
        }
    } // namespace audio
} // namespace ouzel