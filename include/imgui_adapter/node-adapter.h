// Copyright (c) 2020 Simon Liu. All rights reserved.

#ifndef FINALPROJECT_IMGUI_ADAPTER_ADAPTER_H
#define FINALPROJECT_IMGUI_ADAPTER_ADAPTER_H

#include "neurons/node.h"

namespace neurons {

namespace adapter {

// Each NodeAdapter requires three unique IDs: NodeId, input PinId, output PinId
// To create a bijection from Link IDs and Node IDs to Adapter IDs:
// Link IDs and Node IDs must be multiplied by 3 to get their Adapter IDs
static const size_t kIdMultiplier = 3;

// Adapter between the imgui-node-editor graph node and a neuron Node
struct NodeAdapter {
  size_t id_;
  size_t input_id_;
  size_t output_id_;

  Node* node_;

  // Constructor from Node
  explicit NodeAdapter(Node& node);
};


// Return a vector of NodeAdapters wrapped around the passed nodes
std::vector<NodeAdapter> BuildNodeAdapters(std::vector<Node>& nodes);

// Returns a pointer to a Node in the passed vector that owns the node ID.
// If multiple nodes have the node ID, will return the first one.
// Returns nullptr if none of the Nodes match the criteria.
NodeAdapter* FindOwnerNode(std::vector<NodeAdapter>& nodes, size_t id);

// Returns a pointer to a Node in the passed vector that owns the pin ID.
// If multiple nodes have the pin ID, will return the first one.
// Returns nullptr if none of the Nodes match the criteria.
NodeAdapter* FindPinOwner(std::vector<NodeAdapter>& nodes, size_t id);

}  // namespace adapter

}  // namespace neurons

#endif  // FINALPROJECT_IMGUI_ADAPTER_ADAPTER_H
