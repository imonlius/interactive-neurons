// Copyright (c) 2020 Simon Liu. All rights reserved.
#ifndef FINALPROJECT_NEURONS_LINK_H_
#define FINALPROJECT_NEURONS_LINK_H_

#include <utility>

#include "module-node.h"

namespace neurons {

class Link {

 public:

  // Public member variables.
  std::shared_ptr<Node> input_;
  std::shared_ptr<Node> output_;

  // Get link ID.
  [[nodiscard]] size_t GetId() const;

  // Public constructor. Pass shared_ptr by value as Links will share ownership.
  Link(size_t id, std::shared_ptr<Node> input, std::shared_ptr<Node> output):
      input_(std::move(input)), output_(std::move(output)), id_(id) { };

 private:

  // ID should not be edited after construction.
  size_t id_;

};

}  // namespace neurons

#endif  // FINALPROJECT_NEURONS_LINK_H_
