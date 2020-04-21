// Copyright (c) 2020 Simon Liu. All rights reserved.
#ifndef FINALPROJECT_NEURONS_NODE_H_
#define FINALPROJECT_NEURONS_NODE_H_

#include <flashlight/flashlight.h>

namespace neurons {

enum NodeType {
  Conv2D,
  Linear
};

class Node {

 public:

  // Get Node ID
  [[nodiscard]] size_t GetId() const;

  // Get Node type
  [[nodiscard]] NodeType GetNodeType() const;

  // Node Constructor
  Node(size_t id, NodeType type, fl::Module&& module) :
      id_(id), type_(type), module_(&module) {}

  // Wrapper methods that will be called on the wrapped fl::Module.
  [[nodiscard]] std::vector<fl::Variable> params() const;
  void train();
  void eval();
  [[nodiscard]] fl::Variable param(int position) const;
  void setParams(const fl::Variable& var, int position);
  void zeroGrad();
  std::vector<fl::Variable> forward(const std::vector<fl::Variable>& inputs);
  [[nodiscard]] std::string prettyString() const;

 private:

  size_t id_;
  NodeType type_;

  // Wrapped fl::Module
  fl::Module* module_;
};

}  // namespace neurons

#endif // FINALPROJECT_NEURONS_NODE_H_
