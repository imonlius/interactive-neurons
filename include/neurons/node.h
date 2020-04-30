// Copyright (c) 2020 Simon Liu. All rights reserved.
#ifndef FINALPROJECT_NEURONS_NODE_H_
#define FINALPROJECT_NEURONS_NODE_H_

#include <string>

namespace neurons {

enum NodeType {
  Dummy, Conv2D, Linear, Sigmoid, Tanh, HardTanh, ReLU, LeakyReLU, ELU,
  ThresholdReLU, GatedLinearUnit, LogSoftmax, Log, Dropout, Pool2D,
  View, LayerNorm, BatchNorm, CategoricalCrossEntropy, MeanAbsoluteError,
  MeanSquaredError, Dataset
};

// Get the NodeType as an std::string
std::string NodeTypeToString(NodeType type);

// Abstract class to represent a Node in the Network.
class Node {

 public:

  // Get Node ID
  [[nodiscard]] size_t GetId() const;

  // Get Node type
  [[nodiscard]] NodeType GetNodeType() const;

  // Get String representation of Node. Must be overriden in derived classes.
  [[nodiscard]] virtual std::string prettyString() const = 0;

  // Public constructor
  Node(size_t id, NodeType type) : id_(id), type_(type) {};

  // Virtual destructor
  virtual ~Node() = default;

 private:

  size_t id_;
  NodeType type_;

};

}  // namespace neurons

#endif //FINALPROJECT_NEURONS_NODE_H_