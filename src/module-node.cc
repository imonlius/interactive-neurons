// Copyright (c) 2020 Simon Liu. All rights reserved.

#include <flashlight/flashlight.h>
#include <neurons/module-node.h>

namespace neurons {

std::string NodeTypeToString(NodeType type) {
  switch (type) {
    case Dummy:
      return "Dummy";
    case Conv2D:
      return "Conv2D";
    case Linear:
      return "Linear";
    case Sigmoid:
      return "Sigmoid";
    case Tanh:
      return "Tanh";
    case HardTanh:
      return "HardTanh";
    case ReLU:
      return "ReLU";
    case LeakyReLU:
      return "LeakyReLU";
    case ELU:
      return "ELU";
    case ThresholdReLU:
      return "ThresholdReLU";
    case GatedLinearUnit:
      return "GatedLinearUnit";
    case LogSoftmax:
      return "LogSoftmax";
    case Log:
      return "Log";
    case Dropout:
      return "Dropout";
    case Pool2D:
      return "Pool2D";
    case View:
      return "View";
    case LayerNorm:
      return "LayerNorm";
    case BatchNorm:
      return "BatchNorm";
    case CategoricalCrossEntropy:
      return "CategoricalCrossEntropy";
    case MeanAbsoluteError:
      return "MeanAbsoluteError";
    case MeanSquaredError:
      return "MeanSquaredError";
    default:
      throw std::exception();
  }
}

ModuleNode::ModuleNode(size_t id, NodeType type,
    std::unique_ptr<fl::Module> module) : Node(id, type){
  module_ = std::move(module);
}

std::vector<fl::Variable> ModuleNode::params() const {
  return this->module_->params();
}

void ModuleNode::train() {
  this->module_->train();
}

void ModuleNode::eval() {
  this->module_->eval();
}

fl::Variable ModuleNode::param(int position) const {
  return this->module_->param(position);
}

void ModuleNode::setParams(const fl::Variable& var, int position) {
  this->module_->setParams(var, position);
}

void ModuleNode::zeroGrad() {
  this->module_->zeroGrad();
}

std::vector<fl::Variable> ModuleNode::forward(
    const std::vector<fl::Variable>& inputs) {
  return this->module_->forward(inputs);
}

std::string ModuleNode::prettyString() const {
  return this->module_->prettyString();
}

}  // namespace neurons
