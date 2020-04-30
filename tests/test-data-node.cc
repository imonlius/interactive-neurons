// Copyright (c) 2020 Simon Liu. All rights reserved.

#include <neurons/data-node.h>

#include <catch2/catch.hpp>

TEST_CASE("DataNode: Constructor", "[DataNode][Constructor]") {
  auto X = af::randu(1, 1);
  auto Y = af::randu(1, 1);

  auto node = neurons::DataNode(
      0, std::make_unique<fl::TensorDataset>(fl::TensorDataset({X, Y})));

  REQUIRE(node.GetNodeType() == neurons::Dataset);
  REQUIRE(node.GetId() == 0);
  REQUIRE(node.prettyString() == "Dataset");
  REQUIRE(node.dataset_->size() == 1);

}