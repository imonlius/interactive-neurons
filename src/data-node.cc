// Copyright (c) 2020 Simon Liu. All rights reserved.

#include <flashlight/flashlight.h>
#include <neurons/data-node.h>

namespace neurons {

DataNode::DataNode(size_t id,
    std::unique_ptr<fl::Dataset> dataset) : Node(id, Dataset) {
  dataset_ = std::move(dataset);
}

// Pretty string
std::string DataNode::prettyString() const {
  return "Dataset";
}

}  // namespace neurons