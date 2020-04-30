// Copyright (c) 2020 Simon Liu. All rights reserved.

#include <neurons/node.h>

namespace neurons {

size_t Node::GetId() const {
  return id_;
}

NodeType Node::GetNodeType() const {
  return type_;
}

}  // namespace neurons