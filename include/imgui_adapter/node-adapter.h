// Copyright (c) 2020 Simon Liu. All rights reserved.

#ifndef FINALPROJECT_IMGUI_ADAPTER_ADAPTER_H
#define FINALPROJECT_IMGUI_ADAPTER_ADAPTER_H

#include "neurons/node.h"
#include "imgui_node_editor.h"

namespace neurons {

namespace adapter {

// Each NodeAdapter requires three unique IDs: NodeId, input PinId, output PinId
// To create a bijection from Link IDs and Node IDs to Adapter IDs:
// Link IDs and Node IDs must be multiplied by 3 to get their Adapter IDs
static const size_t kIdMultiplier = 3;

// Adapter between the imgui-node-editor graph node and a neuron Node
struct NodeAdapter {
  ax::NodeEditor::NodeId id_;
  ax::NodeEditor::PinId input_id_;
  ax::NodeEditor::PinId output_id_;

  Node* node_;

  // Constructor from Node
  explicit NodeAdapter(Node& node);
};


// Return a vector of NodeAdapters wrapped around the passed nodes
std::vector<NodeAdapter> BuildNodeAdapters(std::vector<Node>& nodes);

// Returns a pointer to the Node in the passed vector that owns the PinId.
// Returns nullptr if none of the Nodes match the criteria.
NodeAdapter* FindOwnerNode(std::vector<NodeAdapter>& nodes,
    const ax::NodeEditor::PinId& id);

}  // namespace adapter

}  // namespace neurons

#endif  // FINALPROJECT_IMGUI_ADAPTER_ADAPTER_H
