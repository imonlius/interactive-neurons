// Copyright (c) 2020 Simon Liu. All rights reserved.
#ifndef FINALPROJECT_NEURONS_DATANODE_H_
#define FINALPROJECT_NEURONS_DATANODE_H_

#include <flashlight/flashlight.h>

#include "module-node.h"

namespace neurons {

class DataNode : public Node {

 public:
  // DataNode Constructor
  DataNode(size_t id, std::unique_ptr<fl::Dataset> train_set,
      std::unique_ptr<fl::Dataset> valid_set,
      std::unique_ptr<fl::Dataset> test_set);

  // Pretty string
  [[nodiscard]] std::string prettyString() const override;

  // datasets are public members as various functions such as DatasetIterator
  // need a non-const reference to it
  std::unique_ptr<fl::Dataset> train_dataset_;
  std::unique_ptr<fl::Dataset> valid_dataset_;
  std::unique_ptr<fl::Dataset> test_dataset_;

};

}  // namespace neurons

#endif // FINALPROJECT_NEURONS_NODE_H_