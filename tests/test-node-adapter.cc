// Copyright (c) 2020 Simon Liu. All rights reserved.

#include "imgui_adapter/node-adapter.h"
#include <catch2/catch.hpp>

/*
 * NodeAdapter::NodeAdapter(Node& node)
 */

TEST_CASE("NodeAdapter: Constructor", "[NodeAdapter][Constructor]") {

  const size_t node_id = 17;

  auto module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
  auto node = neurons::Node(node_id, neurons::NodeType::Conv2D,
      std::make_unique<fl::Conv2D>(module));

  auto node_adapter = neurons::adapter::NodeAdapter(node);
  REQUIRE(node_adapter.node_ == &node);

  REQUIRE(node_adapter.id_ == node_id * neurons::adapter::kIdMultiplier);
  REQUIRE(node_adapter.input_id_ ==
          node_id * neurons::adapter::kIdMultiplier + 1);
  REQUIRE(node_adapter.output_id_ ==
          node_id * neurons::adapter::kIdMultiplier + 2);
}

/*
 * std::vector<NodeAdapter> BuildNodeAdapters(std::vector<Node>& nodes)
 */

TEST_CASE("NodeAdapter: BuildNodeAdapters",
    "[NodeAdapter][BuildNodeAdapters]") {

  std::vector<neurons::Node> nodes;

  SECTION("Empty vector") {
    REQUIRE(neurons::adapter::BuildNodeAdapters(nodes).empty());
  }

  SECTION("Nonempty vector") {
    const size_t node_id = 17;
    const size_t node_id_two = 19;

    auto module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
    auto module_two = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);

    nodes.emplace_back(node_id, neurons::NodeType::Conv2D,
                       std::make_unique<fl::Conv2D>(module));
    nodes.emplace_back(node_id_two, neurons::NodeType::Conv2D,
                       std::make_unique<fl::Conv2D>(module_two));

    auto adapters = neurons::adapter::BuildNodeAdapters(nodes);

    REQUIRE(adapters.size() == 2);

    REQUIRE(adapters.at(0).node_ == &nodes.at(0));
    REQUIRE(adapters.at(0).id_ ==
            node_id * neurons::adapter::kIdMultiplier);
    REQUIRE(adapters.at(0).input_id_ ==
            node_id * neurons::adapter::kIdMultiplier + 1);
    REQUIRE(adapters.at(0).output_id_ ==
            node_id * neurons::adapter::kIdMultiplier + 2);

    REQUIRE(adapters.at(1).node_ == &nodes.at(1));
    REQUIRE(adapters.at(1).id_ ==
            node_id_two * neurons::adapter::kIdMultiplier);
    REQUIRE(adapters.at(1).input_id_ ==
            node_id_two * neurons::adapter::kIdMultiplier + 1);
    REQUIRE(adapters.at(1).output_id_ ==
            node_id_two * neurons::adapter::kIdMultiplier + 2);
  }
}

/*
 * NodeAdapter* FindPinOwner(std::vector<NodeAdapter>& nodes, size_t id)
 */

TEST_CASE("NodeAdapter: FindPinOwner", "[NodeAdapter][FindPinOwner]") {

  const size_t node_id = 17;
  const size_t node_id_two = 19;

  auto module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
  auto module_two = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);

  std::vector<neurons::Node> nodes;
  nodes.emplace_back(node_id, neurons::NodeType::Conv2D,
                     std::make_unique<fl::Conv2D>(module));
  nodes.emplace_back(node_id_two, neurons::NodeType::Conv2D,
                     std::make_unique<fl::Conv2D>(module_two));

  auto adapters = neurons::adapter::BuildNodeAdapters(nodes);

  SECTION("PinId is Node's Input") {
    auto pin = adapters.at(0).input_id_;
    REQUIRE(neurons::adapter::FindPinOwner(adapters, pin) == &adapters.at(0));
  }

  SECTION("PinId is Node's Output") {
    auto pin = adapters.at(1).output_id_;
    REQUIRE(neurons::adapter::FindPinOwner(adapters, pin) == &adapters.at(1));
  }

  SECTION("PinId belongs to no Node") {
    REQUIRE(neurons::adapter::FindPinOwner(adapters, 999) == nullptr);
  }

}

/*
 * NodeAdapter* FindOwnerNode(std::vector<NodeAdapter>& nodes, size_t id)
 */

TEST_CASE("NodeAdapter: FindOwnerNode", "[NodeAdapter][FindOwnerNode]") {

  const size_t node_id = 17;
  const size_t node_id_two = 19;

  auto module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
  auto module_two = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);

  std::vector<neurons::Node> nodes;
  nodes.emplace_back(node_id, neurons::NodeType::Conv2D,
                     std::make_unique<fl::Conv2D>(module));
  nodes.emplace_back(node_id_two, neurons::NodeType::Conv2D,
                     std::make_unique<fl::Conv2D>(module_two));

  auto adapters = neurons::adapter::BuildNodeAdapters(nodes);

  SECTION("Node is present") {
    auto id = adapters.at(1).id_;
    REQUIRE(neurons::adapter::FindOwnerNode(adapters, id) == &adapters.at(1));
  }

  SECTION("Node is not present") {
    REQUIRE(neurons::adapter::FindOwnerNode(adapters, 999) == nullptr);
  }

}