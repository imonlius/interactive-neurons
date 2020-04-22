// Copyright (c) 2020 Simon Liu. All rights reserved.

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <neurons/node.h>

/*
 * Node(size_t id, NodeType type, fl::ModulePtr&& module_ptr)
 */

TEST_CASE("Node: Constructor", "[Node][Constructor]") {
  SECTION("Linear NodeType") {
    auto module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
    auto node = neurons::Node(0, neurons::NodeType::Conv2D, std::move(module));
    REQUIRE(node.GetId() == 0);
    REQUIRE(node.GetNodeType() == neurons::NodeType::Conv2D);
  }
  SECTION("Conv2D NodeType") {
    auto module = fl::Linear(1, 1);
    auto node = neurons::Node(1, neurons::NodeType::Linear, std::move(module));
    REQUIRE(node.GetId() == 1);
    REQUIRE(node.GetNodeType() == neurons::NodeType::Linear);
  }
}