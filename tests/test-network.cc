// Copyright (c) 2020 Simon Liu. All rights reserved.

#include <catch2/catch.hpp>
#include <neurons/link.h>
#include <neurons/network.h>
#include <neurons/node.h>

/*
 * Node* AddNode(NodeType type, fl::Module&& module);
 */
TEST_CASE("Network: Add Nodes", "[Network][AddNode]") {

  auto network = neurons::Network();

  SECTION("Add one Conv2D node") {
    auto module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
    network.AddNode(neurons::NodeType::Conv2D, std::move(module));

    REQUIRE(network.GetNodes().size() == 1);
    REQUIRE(network.GetNodes().at(0).GetId() == 0);
    REQUIRE(network.GetNodes().at(0).GetNodeType() == neurons::NodeType::Conv2D);
  }

  SECTION("Add one Linear node") {
    auto module = fl::Linear(1, 1);
    network.AddNode(neurons::NodeType::Linear, std::move(module));

    REQUIRE(network.GetNodes().size() == 1);
    REQUIRE(network.GetNodes().at(0).GetId() == 0);
    REQUIRE(network.GetNodes().at(0).GetNodeType() == neurons::NodeType::Linear);
  }

  SECTION("Add two nodes") {
    auto module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
    network.AddNode(neurons::NodeType::Conv2D, std::move(module));

    auto module_two = fl::Linear(1, 1);
    network.AddNode(neurons::NodeType::Linear, std::move(module_two));

    REQUIRE(network.GetNodes().size() == 2);

    REQUIRE(network.GetNodes().at(0).GetId() == 0);
    REQUIRE(network.GetNodes().at(0).GetNodeType() == neurons::NodeType::Conv2D);

    REQUIRE(network.GetNodes().at(1).GetId() == 1);
    REQUIRE(network.GetNodes().at(1).GetNodeType() == neurons::NodeType::Linear);
  }
}

/*
 * void DeleteNode(const Node& node);
 */
TEST_CASE("Network: Delete Nodes", "[Network][DeleteNode]") {

  auto network = neurons::Network();

  SECTION("Delete the only node") {
    auto module = fl::Linear(1, 1);
    network.AddNode(neurons::NodeType::Linear, std::move(module));

    network.DeleteNode(network.GetNodes().at(0));
    REQUIRE(network.GetNodes().empty());
  }

  SECTION("Delete a node") {
    auto module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
    network.AddNode(neurons::NodeType::Conv2D, std::move(module));

    auto module_two = fl::Linear(1, 1);
    network.AddNode(neurons::NodeType::Linear, std::move(module_two));

    REQUIRE(network.GetNodes().size() == 2);

    network.DeleteNode(network.GetNodes().at(0));
    REQUIRE(network.GetNodes().size() == 1);

    // check remaining node is correct
    REQUIRE(network.GetNodes().at(0).GetNodeType() == neurons::NodeType::Linear);
  }

  SECTION("Delete a node not in the network") {
    auto module = fl::Linear(1, 1);
    auto module_copy = fl::Linear(module);
    network.AddNode(neurons::NodeType::Linear, std::move(module));

    network.DeleteNode(neurons::Node(
        5, neurons::NodeType::Linear, std::move(module_copy)));

    // Deleting a node not in the Network should not do anything.
    REQUIRE(network.GetNodes().size() == 1);
  }

  SECTION("Deleting a node that is linked") {
    auto module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
    network.AddNode(neurons::NodeType::Conv2D, std::move(module));
    auto module_two = fl::Linear(1, 1);
    network.AddNode(neurons::NodeType::Linear, std::move(module_two));

    network.AddLink(network.GetNodes().at(0), network.GetNodes().at(1));

    REQUIRE(network.GetLinks().size() == 1);
    network.DeleteNode(network.GetNodes().at(0));
    REQUIRE(network.GetLinks().empty());
  }
}

/*
 * Link* AddLink(Node& input, Node& output);
 */
TEST_CASE("Network: Add Links", "[Network][AddLink]") {

  neurons::Network network;

  auto module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
  network.AddNode(neurons::NodeType::Conv2D, std::move(module));
  auto module_two = fl::Linear(1, 1);
  network.AddNode(neurons::NodeType::Linear, std::move(module_two));

  SECTION("Adding a valid link between two nodes") {
    REQUIRE(network.AddLink(
        network.GetNodes().at(0), network.GetNodes().at(1)) != nullptr);
  }

  SECTION("Linking a node to itself (invalid)") {
    REQUIRE(network.AddLink(
        network.GetNodes().at(0), network.GetNodes().at(0)) == nullptr);
  }

  SECTION("Adding a link between a node not in the network (invalid)") {
    auto mod = fl::Linear(1, 1);
    auto not_networked_node = neurons::Node(
        5, neurons::NodeType::Linear, std::move(mod));
    REQUIRE(network.AddLink(
        network.GetNodes().at(0), not_networked_node) == nullptr);
  }

}

/*
 * void DeleteLink(const Link& link);
 */

TEST_CASE("Network: Deleting Links", "[Network][DeleteLink]") {
  neurons::Network network;

  auto module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
  network.AddNode(neurons::NodeType::Conv2D, std::move(module));
  auto module_two = fl::Linear(1, 1);
  network.AddNode(neurons::NodeType::Linear, std::move(module_two));

  network.AddLink(network.GetNodes().at(0), network.GetNodes().at(1));

  SECTION("Removing an in-network link") {
    network.DeleteLink(network.GetLinks().at(0));
    REQUIRE(network.GetLinks().empty());
  }

  SECTION("Removing an out-of-network link") {
    auto link = neurons::Link::BuildLink(5, network.GetNodes().at(0),
                                         network.GetNodes().at(1));
    network.DeleteLink(link);
    // should not change
    REQUIRE(network.GetLinks().size() == 1);
  }
}
