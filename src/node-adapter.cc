// Copyright (c) 2020 Simon Liu. All rights reserved.

#include "imgui_adapter/node-adapter.h"

namespace neurons {

namespace adapter {

NodeAdapter::NodeAdapter(Node& node) {
  node_ = &node;

  id_ = kIdMultiplier * node.GetId();
  input_id_ = kIdMultiplier * node.GetId() + 1;
  output_id_ = kIdMultiplier * node.GetId() + 2;
}

std::vector<NodeAdapter> BuildNodeAdapters(std::vector<Node>& nodes) {
  auto adapters = std::vector<NodeAdapter>();
  for (auto& node : nodes) {
    adapters.emplace_back(node);
  }
  return adapters;
}

NodeAdapter* FindOwnerNode(std::vector<NodeAdapter>& nodes,
                           const ax::NodeEditor::NodeId& id) {
  for (auto& node : nodes) {
    if (node.id_ == id) {
      return &node;
    }
  }
  return nullptr;
}

NodeAdapter* FindOwnerNode(std::vector<NodeAdapter>& nodes,
    const ax::NodeEditor::PinId& id) {
  for (auto& node : nodes) {
    if (node.input_id_ == id || node.output_id_ == id) {
      return &node;
    }
  }
  return nullptr;
}

}  // namespace adapter

}  // namespace neurons