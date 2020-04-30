// Copyright (c) 2020 Simon Liu. All rights reserved.

#include "imgui_adapter/node-adapter.h"

namespace neurons::adapter {

NodeAdapter::NodeAdapter(const std::shared_ptr<Node>& node) {
  node_ = node;

  id_ = kIdMultiplier * node->GetId();
  input_id_ = kIdMultiplier * node->GetId() + 1;
  output_id_ = kIdMultiplier * node->GetId() + 2;
}

std::vector<NodeAdapter> BuildNodeAdapters(
    std::deque<std::shared_ptr<Node>>& nodes) {
  auto adapters = std::vector<NodeAdapter>();
  adapters.reserve(nodes.size());
  for (auto& node : nodes) {
    adapters.emplace_back(node);
  }
  return adapters;
}

NodeAdapter* FindOwnerNode(std::vector<NodeAdapter>& nodes, size_t id) {
  for (auto& node : nodes) {
    if (node.id_ == id) {
      return &node;
    }
  }
  return nullptr;
}

NodeAdapter* FindPinOwner(std::vector<NodeAdapter>& nodes, size_t id) {
  for (auto& node : nodes) {
    if (node.input_id_ == id || node.output_id_ == id) {
      return &node;
    }
  }
  return nullptr;
}

}  // namespace neurons::adapter