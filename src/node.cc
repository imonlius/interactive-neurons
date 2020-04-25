// Copyright (c) 2020 Simon Liu. All rights reserved.

#include <flashlight/flashlight.h>
#include <neurons/node.h>

namespace neurons {

std::string NodeTypeToString(NodeType type) {
  switch (type) {
    case Dummy:
      return "Dummy";
    case Conv2D:
      return "Conv2D";
    case Linear:
      return "Linear";
    case Activation:
      return "Activation";
    default:
      throw std::exception();
  }
}

std::string NodeTypeToString(ActivationNodeType type) {
  switch (type) {
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
    default:
      throw std::exception();
  }
}

size_t Node::GetId() const {
  return id_;
}

NodeType Node::GetNodeType() const {
  return type_;
}

Node::Node(size_t id, NodeType type, std::unique_ptr<fl::Module> module) {
  id_ = id;
  type_ = type;
  module_ = std::move(module);
}

std::vector<fl::Variable> Node::params() const {
  return this->module_->params();
}

void Node::train() {
  this->module_->train();
}

void Node::eval() {
  this->module_->eval();
}

fl::Variable Node::param(int position) const {
  return this->module_->param(position);
}

void Node::setParams(const fl::Variable& var, int position) {
  this->module_->setParams(var, position);
}

void Node::zeroGrad() {
  this->module_->zeroGrad();
}

std::vector<fl::Variable> Node::forward(
    const std::vector<fl::Variable>& inputs) {
  return this->module_->forward(inputs);
}

std::string Node::prettyString() const {
  return this->module_->prettyString();
}

std::unique_ptr<fl::Module> SpawnActivation(ActivationNodeType type) {
  switch (type) {
    case Sigmoid:
      return std::make_unique<fl::Sigmoid>(fl::Sigmoid());
    case Tanh:
      return std::make_unique<fl::Tanh>(fl::Tanh());
    case HardTanh:
      return std::make_unique<fl::HardTanh>(fl::HardTanh());
    case ReLU:
      return std::make_unique<fl::ReLU>(fl::ReLU());
    case LeakyReLU:
      return std::make_unique<fl::LeakyReLU>(fl::LeakyReLU());
    case ELU:
      return std::make_unique<fl::ELU>(fl::ELU());
    case ThresholdReLU:
      return std::make_unique<fl::ThresholdReLU>(fl::ThresholdReLU());
    case GatedLinearUnit:
      return std::make_unique<fl::GatedLinearUnit>(fl::GatedLinearUnit());
    case LogSoftmax:
      return std::make_unique<fl::LogSoftmax>(fl::LogSoftmax());
    case Log:
      return std::make_unique<fl::Log>(fl::Log());
    default:
      throw std::exception();
  }
}

}  // namespace neurons
