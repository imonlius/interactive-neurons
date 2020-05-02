// Copyright (c) 2020 Simon Liu. All rights reserved.
#ifndef FINALPROJECT_NEURONS_MODULE_NODE_H_
#define FINALPROJECT_NEURONS_MODULE_NODE_H_

#include <flashlight/flashlight.h>

#include "node.h"

namespace neurons {

class ModuleNode : public Node {

 public:

  // Node Constructor
  ModuleNode(size_t id, NodeType type, std::unique_ptr<fl::Module> module);

  // Wrapper methods that will be called on the wrapped fl::Module.
  [[nodiscard]] std::vector<fl::Variable> params() const;
  void train();
  void eval();
  [[nodiscard]] fl::Variable param(int position) const;
  void setParams(const fl::Variable& var, int position);
  void zeroGrad();
  std::vector<fl::Variable> forward(const std::vector<fl::Variable>& inputs);
  [[nodiscard]] std::string prettyString() const override;

 private:
  // Unique pointer ensures that Node has sole ownership over module
  std::unique_ptr<fl::Module> module_;
};

}  // namespace neurons

#endif // FINALPROJECT_NEURONS_MODULE_NODE_H_
