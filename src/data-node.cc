// Copyright (c) 2020 Simon Liu. All rights reserved.

#include <flashlight/flashlight.h>
#include <neurons/data-node.h>

namespace neurons {

DataNode::DataNode(size_t id, std::unique_ptr<fl::Dataset> train_set,
         std::unique_ptr<fl::Dataset> valid_set,
         std::unique_ptr<fl::Dataset> test_set) : Node(id, Dataset) {
  train_dataset_ = std::move(train_set);
  valid_dataset_ = std::move(valid_set);
  test_dataset_ = std::move(test_set);
}

// Pretty string
std::string DataNode::prettyString() const {
  return "Dataset";
}

}  // namespace neurons