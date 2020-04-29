// Copyright (c) 2020 Simon Liu. All rights reserved.
#ifndef FINALPROJECT_NEURONS_NETWORK_H
#define FINALPROJECT_NEURONS_NETWORK_H

#include "link.h"
#include "node.h"

namespace neurons {

class Network {

 public:

  // Retrieve std::deque of Nodes.
  [[nodiscard]] std::deque<Node>& GetNodes();

  // Retrieve std::deque of Links.
  [[nodiscard]] std::deque<Link>& GetLinks();

  // Add a Node to the network with a unique_ptr to an fl::Module.
  // Network will take ownership of the fl::Module pointer.
  // Returns a pointer to the Node if successful. Otherwise, returns nullptr.
  Node* AddNode(NodeType type, std::unique_ptr<fl::Module> module);

  // Add a Link to the network if the input and output form a valid Link.
  // Returns a pointer to the Link if successful. Otherwise, returns nullptr.
  Link* AddLink(Node& input, Node& output);

  // Delete a Link from the network.
  void DeleteLink(const Link& link);

  // Delete a Node from the network.
  void DeleteNode(const Node& node);

  // Returns pointer to the loss node of the network. If network does not have
  // a loss node set, returns nullptr.
  [[nodiscard]] const Node* GetLossNode() const;

 private:

  // Keep track of next unique ID for Nodes/Links.
  size_t unique_id_;

  // Deque of all the Nodes in the network.
  std::deque<Node> nodes_;

  // Deque of all the Links in the network.
  std::deque<Link> links_;

};

}  // namespace neurons

#endif  // FINALPROJECT_NEURONS_NETWORK_H
