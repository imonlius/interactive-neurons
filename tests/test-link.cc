// Copyright (c) 2020 Simon Liu. All rights reserved.

#include <neurons/link.h>
#include <neurons/module-node.h>

#include <catch2/catch.hpp>


/*
 * Link(size_t id, Node& input, Node& output);
 */

TEST_CASE("Link: Constructor", "[Link][Constructor]") {
  auto input_module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
  auto input_node = neurons::ModuleNode(1,neurons::NodeType::Conv2D,
      std::make_unique<fl::Conv2D>(input_module));
  auto output_module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
  auto output_node = neurons::ModuleNode(1,neurons::NodeType::Conv2D,
      std::make_unique<fl::Conv2D>(output_module));

  auto link = neurons::Link(3, input_node, output_node);
  REQUIRE(link.GetId() == 3);
  REQUIRE(link.input_ == &input_node);
  REQUIRE(link.output_ == &output_node);
}

