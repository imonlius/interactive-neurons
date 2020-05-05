// Copyright (c) 2020 Simon Liu. All rights reserved.

#include "neurons/network.h"

namespace neurons {

// Helper function, returns whether the deque contains the Node
bool ContainsNode(const NodeDeque& nodes,
                  const std::shared_ptr<Node>& node) {
  for (const auto& it : nodes) {
    if(it == node) {
      return true;
    }
  }
  return false;
}

Link* Network::AddLink(const std::shared_ptr<Node>& input,
    const std::shared_ptr<Node>& output) {

  // if Network does not have the Nodes, it cannot link them.
  if (!ContainsNode(nodes_, input) || !ContainsNode(nodes_, output)) {
    return nullptr;
  }

  links_.emplace_back(unique_id_++, input, output);
  return &links_.back();
}

std::shared_ptr<Node> Network::AddNode(neurons::NodeType type,
    std::unique_ptr<fl::Module> module) {
  // new Node takes ownership of the module_ptr
  nodes_.push_back(std::make_unique<neurons::ModuleNode>(
      neurons::ModuleNode(unique_id_++, type, std::move(module))));
  return nodes_.back();
}

std::shared_ptr<Node> Network::AddNode(
    std::unique_ptr<fl::Dataset> train_set,
    std::unique_ptr<fl::Dataset> valid_set,
    std::unique_ptr<fl::Dataset> test_set) {
  // if Dataset node already exists, just return the pointer to that
  for (auto& node : nodes_) {
    if (node->GetNodeType() == Dataset) {
      return node;
    }
  }
  nodes_.push_back(std::make_unique<neurons::DataNode>(unique_id_++,
      std::move(train_set), std::move(valid_set), std::move(test_set)));
  return nodes_.back();
}

void Network::DeleteLink(const neurons::Link& link) {
  // assumes that only one copy of the link exists
  for (auto it = links_.begin(); it != links_.end(); ++it) {
    // check if address of Link value of iterator matches
    if (&(*it) == &link) {
      links_.erase(it);
      return;
    }
  }
}

void Network::DeleteNode(const neurons::Node& node) {

  // remove the node first
  for (auto it = nodes_.begin(); it != nodes_.end(); ++it) {
    // assumes that only one copy of the Node exists
    if ((*it).get() == &node) {
      nodes_.erase(it);
      break;
    }
  }

  // remove any links with the node
  for (auto it = links_.begin(); it != links_.end(); ++it) {
    // check if link has node as input or output based on addresses
    if (it->input_.get() == &node || it->output_.get() == &node) {
      // removes the element and adjusts the iterator at the same time
      it = links_.erase(it);
      --it;
    }
  }

}

std::deque<Link>& Network::GetLinks() {
  return links_;
}

NodeDeque& Network::GetNodes() {
  return nodes_;
}

std::shared_ptr<DataNode> Network::GetDataNode() const {
  for (const auto& node : nodes_) {
    NodeType type = node->GetNodeType();
    if (type == Dataset) {
      return std::dynamic_pointer_cast<DataNode>(node);
    }
  }
  return nullptr;
}

std::shared_ptr<Node> Network::GetLossNode() const {
  for (const auto& node : nodes_) {
    NodeType type = node->GetNodeType();
    if (type == CategoricalCrossEntropy || type == MeanAbsoluteError ||
        type == MeanSquaredError) {
      return node;
    }
  }
  return nullptr;
}

}  // namespace neurons