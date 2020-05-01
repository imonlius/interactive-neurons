// Copyright (c) 2020 Simon Liu. All rights reserved.
#ifndef FINALPROJECT_NEURONS_LINK_H_
#define FINALPROJECT_NEURONS_LINK_H_

#include "module-node.h"

namespace neurons {

class Link {

 public:

  // Public member variables.
  Node* input_;
  Node* output_;

  // Get link ID.
  [[nodiscard]] size_t GetId() const;

  // Public constructor. Nodes are passed as references to ensure
  // they are not nullptr at time of construction.
  Link(size_t id, Node& input, Node& output):
      input_(&input), output_(&output), id_(id) { };

 private:

  // ID should not be edited after construction.
  size_t id_;

};

}  // namespace neurons

#endif  // FINALPROJECT_NEURONS_LINK_H_
