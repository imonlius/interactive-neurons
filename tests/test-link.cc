// Copyright (c) 2020 Simon Liu. All rights reserved.

#include <neurons/link.h>
#include <neurons/module-node.h>

#include <catch2/catch.hpp>

// TODO: Update test cases to reflect link criteria.

/*
 * static bool CanLink(const Node& input, const Node& output);
 */

TEST_CASE("Link: CanLink", "[Link][CanLink]") {
  // For now, all Nodes are allowed to link
  SECTION("Always Yes") {
    auto module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
    auto node = neurons::ModuleNode(1,
        neurons::NodeType::Conv2D, std::make_unique<fl::Conv2D>(module));
    REQUIRE(neurons::Link::CanLink(node, node));
  }
}

/*
 * static Link BuildLink(size_t id, Node& input, Node& output);
 */

TEST_CASE("Link: BuildLink", "[Link][BuildLink]") {
  SECTION("Always Yes") {
    auto input_module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
    auto input_node = neurons::ModuleNode(1,neurons::NodeType::Conv2D,
        std::make_unique<fl::Conv2D>(input_module));
    auto output_module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
    auto output_node = neurons::ModuleNode(1,neurons::NodeType::Conv2D,
        std::make_unique<fl::Conv2D>(output_module));
    REQUIRE(neurons::Link::CanLink(input_node, output_node));

    auto link = neurons::Link::BuildLink(3, input_node, output_node);
    REQUIRE(link.GetId() == 3);
    REQUIRE(link.input_ == &input_node);
    REQUIRE(link.output_ == &output_node);
  }
}

