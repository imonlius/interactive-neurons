// Copyright (c) 2020 Simon Liu. All rights reserved.

#include "neurons/network.h"

namespace neurons {

// Helper function, returns whether the vector contains the Node
bool ContainsNode(const std::vector<neurons::Node>& nodes,
    const neurons::Node& node) {
  for (auto& it : nodes) {
    if(&it == &node) {
      return true;
    }
  }
  return false;
}

Link* Network::AddLink(neurons::Node& input,
    neurons::Node& output) {

  links_.reserve(links_.size() + 1);

  // if Network does not have the Nodes, it cannot link them.
  if (!ContainsNode(nodes_, input) || !ContainsNode(nodes_, output)) {
    return nullptr;
  }

  if (Link::CanLink(input, output)) {
    links_.push_back(Link::BuildLink(unique_id_++, input, output));
    return &links_.back();
  }
  return nullptr;
}

Node* Network::AddNode(neurons::NodeType type,
    std::unique_ptr<fl::Module> module) {
  // new Node takes ownership of the module_ptr
  nodes_.reserve(nodes_.size() + 1);
  nodes_.emplace_back(unique_id_++, type, std::move(module));
  return &nodes_.back();
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
    if (&(*it) == &node) {
      nodes_.erase(it);
      break;
    }
  }

  // remove any links with the node
  for (auto it = links_.begin(); it != links_.end(); ++it) {
    // check if link has node as input or output based on addresses
    if (it->input_ == &node || it->output_ == &node) {
      // removes the element and adjusts the iterator at the same time
      it = links_.erase(it);
      --it;
    }
  }

}

std::vector<Link>& Network::GetLinks() {
  return links_;
}

std::vector<Node>& Network::GetNodes() {
  return nodes_;
}

}  // namespace neurons