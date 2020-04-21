// Copyright (c) 2020 Simon Liu. All rights reserved.

#include <flashlight/flashlight.h>
#include <neurons/node.h>

namespace neurons {

size_t Node::GetId() const {
  return id_;
}

NodeType Node::GetNodeType() const {
  return type_;
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

std::vector<fl::Variable> Node::forward(const std::vector<fl::Variable>& inputs) {
  return this->module_->forward(inputs);
}

std::string Node::prettyString() const {
  return this->module_->prettyString();
};

}  // namespace neurons
