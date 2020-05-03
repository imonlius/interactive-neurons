// Copyright (c) 2020 Simon Liu. All rights reserved.
#ifndef FINALPROJECT_NEURONS_MODULE_NODE_H_
#define FINALPROJECT_NEURONS_MODULE_NODE_H_

#include <flashlight/flashlight.h>

#include "node.h"

namespace neurons {

 class ModuleNode : public Node, public fl::Module {

 public:

  // Node Constructor
  ModuleNode(size_t id, NodeType type, std::unique_ptr<fl::Module> module);

  // Wrapper methods that will be called on the wrapped fl::Module.
  void train() override;
  void eval() override;
  void setParams(const fl::Variable& var, int position) override;
  std::vector<fl::Variable> forward(
      const std::vector<fl::Variable>& inputs) override;
  [[nodiscard]] std::string prettyString() const override;

 private:
  // Unique pointer ensures that Node has sole ownership over module
  std::unique_ptr<fl::Module> module_;
};

}  // namespace neurons

#endif // FINALPROJECT_NEURONS_MODULE_NODE_H_
