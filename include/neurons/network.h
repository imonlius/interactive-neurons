// Copyright (c) 2020 Simon Liu. All rights reserved.
#ifndef FINALPROJECT_NEURONS_NETWORK_H
#define FINALPROJECT_NEURONS_NETWORK_H

#include "link.h"
#include "node.h"

namespace neurons {

class Network {

 public:

  // Retrieve read-only std::vector of Nodes.
  [[nodiscard]] std::vector<Node>& GetNodes();

  // Retrieve read-only std::vector of Links.
  [[nodiscard]] std::vector<Link>& GetLinks();

  // Add a Node to the network with a unique_ptr to an fl::Module.
  // Network will take ownership of the fl::Module pointer.
  // Returns a pointer to the Node if successful. Otherwise, returns nullptr.
  Node* AddNode(NodeType type, std::unique_ptr<fl::Module> module);

  // Add a Link to the network if the input and output form a valid Link.
  // Returns a pointer to the Link if successful. Otherwise, returns nullptr.
  Link* AddLink(Node& input, Node& output);

  // Delete a Link from the network.
  // Returns whether successful.
  void DeleteLink(const Link& link);

  // Delete a Node from the network.
  // Returns whether successful.
  void DeleteNode(const Node& node);

 private:

  // Keep track of next unique ID for Nodes/Links.
  size_t unique_id_;

  // Vector of all the Nodes in the network.
  std::vector<Node> nodes_;

  // Vector of all the Links in the network.
  std::vector<Link> links_;

};

}  // namespace neurons

#endif  // FINALPROJECT_NEURONS_NETWORK_H
