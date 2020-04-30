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

  // Returns whether a Link is possible between the input and output nodes.
  static bool CanLink(const Node& input, const Node& output);

  // Returns a Link if a link is possible.
  // Throws an exception otherwise.
  static Link BuildLink(size_t id, Node& input, Node& output);

 private:

  size_t id_;

  // Constructor is private to allow usage by BuildLink() only
  // Guarantees that passed pointers at time of construction are not nullptr
  Link(size_t id, Node* input, Node* output):
      input_(input), output_(output), id_(id) { };
};

}  // namespace neurons

#endif  // FINALPROJECT_NEURONS_LINK_H_
