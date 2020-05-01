// Copyright (c) 2020 Simon Liu. All rights reserved.

#include <neurons/data-node.h>

#include <catch2/catch.hpp>

TEST_CASE("DataNode: Constructor", "[DataNode][Constructor]") {
  auto train_X = af::randu(1, 2);
  auto train_Y = af::randu(1, 2);
  auto valid_X = af::randu(1, 3);
  auto valid_Y = af::randu(1, 3);
  auto test_X = af::randu(1, 4);
  auto test_Y = af::randu(1, 4);

  auto node = neurons::DataNode(
      0, std::make_unique<fl::TensorDataset>(
          fl::TensorDataset({train_X, train_Y})),
      std::make_unique<fl::TensorDataset>(
          fl::TensorDataset({valid_X, valid_Y})),
      std::make_unique<fl::TensorDataset>(
          fl::TensorDataset({test_X, test_Y})));

  REQUIRE(node.GetNodeType() == neurons::Dataset);
  REQUIRE(node.GetId() == 0);
  REQUIRE(node.prettyString() == "Dataset");
  REQUIRE(node.train_dataset_->size() == 2);
  REQUIRE(node.valid_dataset_->size() == 3);
  REQUIRE(node.test_dataset_->size() == 4);

}