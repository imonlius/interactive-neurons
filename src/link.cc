// Copyright (c) 2020 Simon Liu. All rights reserved.

#include <neurons/link.h>
#include <stdexcept>

namespace neurons {

size_t Link::GetId() const {
  return id_;
}

bool Link::CanLink(const Node& input, const Node& output) {
  // TODO: Implement Link checker.
  return true;
}

Link Link::BuildLink(size_t id, Node& input, Node& output) {
  if (!CanLink(input, output)) {
    throw std::exception();
  }
  return Link(id, &input, &output);
}

}  // namespace neurons
