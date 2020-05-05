// Copyright (c) 2020 Simon Liu. All rights reserved.
#ifndef FINALPROJECT_NEURONS_NETWORK_H
#define FINALPROJECT_NEURONS_NETWORK_H

#include "data-node.h"
#include "link.h"
#include "module-node.h"
#include "node.h"

namespace neurons {

class Network {

 public:

  // Retrieve std::deque of Nodes.
  [[nodiscard]] NodeDeque& GetNodes();

  // Retrieve std::deque of Links.
  [[nodiscard]] std::deque<Link>& GetLinks();

  // Add a ModuleNode to the network with a unique_ptr to an fl::Module.
  // Network will take ownership of the fl::Module pointer.
  // Returns a pointer to the Node if successful. Otherwise, returns nullptr.
  std::shared_ptr<Node> AddNode(NodeType type,
      std::unique_ptr<fl::Module> module);

  // Add a DataNode to the network with a unique_ptr to an fl::Dataset.
  // Only adds a new node if no DataNode exists in the network.
  // If a DataNode already exists, returns a pointer to that node.
  std::shared_ptr<Node> AddNode(std::unique_ptr<fl::Dataset> train_set,
                                std::unique_ptr<fl::Dataset> valid_set,
                                std::unique_ptr<fl::Dataset> test_set);

  // Add a Link to the network if the input and output form a valid Link.
  // Returns a pointer to the Link if successful. Otherwise, returns nullptr.
  Link* AddLink(const std::shared_ptr<Node>& input, const std::shared_ptr<Node>& output);

  // Delete a Link from the network.
  void DeleteLink(const Link& link);

  // Delete a Node from the network.
  void DeleteNode(const Node& node);

  // Returns pointer to the data node of the network. If network does not have
  // a data node set, returns nullptr.
  [[nodiscard]] std::shared_ptr<DataNode> GetDataNode() const;

  // Returns pointer to the loss node of the network. If network does not have
  // a loss node set, returns nullptr.
  [[nodiscard]] std::shared_ptr<Node> GetLossNode() const;

 private:

  // Keep track of next unique ID for Nodes/Links.
  size_t unique_id_;

  // Deque of all the Node pointers in the network.
  NodeDeque nodes_;

  // Deque of all the Links in the network.
  std::deque<Link> links_;

};

}  // namespace neurons

#endif  // FINALPROJECT_NEURONS_NETWORK_H
