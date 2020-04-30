// Copyright (c) 2020 Simon Liu. All rights reserved.

#include <neurons/link.h>
#include <neurons/module-node.h>
#include <neurons/network.h>

#include <catch2/catch.hpp>

/*
 * Node* AddNode(NodeType type, fl::Module&& module);
 */
TEST_CASE("Network: Add Nodes", "[Network][AddNode]") {

  auto network = neurons::Network();

  SECTION("Add one Conv2D node") {
    auto module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
    network.AddNode(neurons::NodeType::Conv2D,
        std::make_unique<fl::Conv2D>(module));

    REQUIRE(network.GetNodes().size() == 1);
    REQUIRE(network.GetNodes().at(0)->GetId() == 0);
    REQUIRE(network.GetNodes().at(0)->GetNodeType() ==
            neurons::NodeType::Conv2D);
  }

  SECTION("Add one Linear node") {
    auto module = fl::Linear(1, 1);
    network.AddNode(neurons::NodeType::Linear,
                    std::make_unique<fl::Linear>(module));

    REQUIRE(network.GetNodes().size() == 1);
    REQUIRE(network.GetNodes().at(0)->GetId() == 0);
    REQUIRE(network.GetNodes().at(0)->GetNodeType() ==
            neurons::NodeType::Linear);
  }

  SECTION("Add two nodes") {
    auto module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
    network.AddNode(neurons::NodeType::Conv2D,
                    std::make_unique<fl::Conv2D>(module));

    auto module_two = fl::Linear(1, 1);
    network.AddNode(neurons::NodeType::Linear,
                    std::make_unique<fl::Linear>(module_two));

    REQUIRE(network.GetNodes().size() == 2);

    REQUIRE(network.GetNodes().at(0)->GetId() == 0);
    REQUIRE(network.GetNodes().at(0)->GetNodeType() ==
            neurons::NodeType::Conv2D);

    REQUIRE(network.GetNodes().at(1)->GetId() == 1);
    REQUIRE(network.GetNodes().at(1)->GetNodeType() ==
            neurons::NodeType::Linear);
  }
}

/*
 * std::shared_ptr<Node> AddNode(std::unique_ptr<fl::Dataset> module);
 */
TEST_CASE("Network: Add Nodes: Dataset", "[Network][AddNode][DataNode]") {

  auto X = af::randu(1, 1);
  auto Y = af::randu(1, 1);

  SECTION("No DataNode present in Network yet") {
    auto network = neurons::Network();
    REQUIRE(network.GetNodes().empty());

    network.AddNode(std::make_unique<fl::TensorDataset>(
        fl::TensorDataset({X, Y})));

    REQUIRE(network.GetNodes().size() == 1);
    REQUIRE(network.GetNodes().front()->GetNodeType() == neurons::Dataset);
  }

  SECTION("DataNode is already present in Network") {
    auto network = neurons::Network();

    network.AddNode(std::make_unique<fl::TensorDataset>(
        fl::TensorDataset({X, Y})));

    REQUIRE(network.GetNodes().size() == 1);

    // should return the original DataNode
    REQUIRE(network.AddNode(std::make_unique<fl::TensorDataset>(
        fl::TensorDataset({X, Y})))->GetId() == 0);

    REQUIRE(network.GetNodes().size() == 1);
  }

  SECTION("DataNode is deleted and then re-added to Network") {
    auto network = neurons::Network();

    network.AddNode(std::make_unique<fl::TensorDataset>(
        fl::TensorDataset({X, Y})));
    REQUIRE(network.GetNodes().size() == 1);

    network.DeleteNode(*network.GetNodes().front());
    REQUIRE(network.GetNodes().empty());

    network.AddNode(std::make_unique<fl::TensorDataset>(
        fl::TensorDataset({X, Y})));
    REQUIRE(network.GetNodes().size() == 1);
  }
}

/*
 * void DeleteNode(const Node& node);
 */
TEST_CASE("Network: Delete Nodes", "[Network][DeleteNode]") {

  auto network = neurons::Network();

  SECTION("Delete the only node") {
    auto module = fl::Linear(1, 1);
    network.AddNode(neurons::NodeType::Linear,
                    std::make_unique<fl::Linear>(module));

    network.DeleteNode(*network.GetNodes().at(0));
    REQUIRE(network.GetNodes().empty());
  }

  SECTION("Delete a node") {
    auto module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
    network.AddNode(neurons::NodeType::Conv2D,
                    std::make_unique<fl::Conv2D>(module));

    auto module_two = fl::Linear(1, 1);
    network.AddNode(neurons::NodeType::Linear,
                    std::make_unique<fl::Linear>(module_two));

    REQUIRE(network.GetNodes().size() == 2);

    network.DeleteNode(*network.GetNodes().at(0));
    REQUIRE(network.GetNodes().size() == 1);

    // check remaining node is correct
    REQUIRE(network.GetNodes().at(0)->GetNodeType() ==
            neurons::NodeType::Linear);
  }

  SECTION("Delete a node not in the network") {
    auto module = fl::Linear(1, 1);
    auto module_copy = fl::Linear(module);
    network.AddNode(neurons::NodeType::Linear,
                    std::make_unique<fl::Linear>(module));

    network.DeleteNode(neurons::ModuleNode(5, neurons::NodeType::Linear,
        std::make_unique<fl::Linear>(module_copy)));

    // Deleting a node not in the Network should not do anything.
    REQUIRE(network.GetNodes().size() == 1);
  }

  SECTION("Deleting a node that is linked") {
    auto module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
    network.AddNode(neurons::NodeType::Conv2D,
                    std::make_unique<fl::Conv2D>(module));
    auto module_two = fl::Linear(1, 1);
    network.AddNode(neurons::NodeType::Linear,
                    std::make_unique<fl::Linear>(module_two));

    network.AddLink(*network.GetNodes().at(0),
        *network.GetNodes().at(1));

    REQUIRE(network.GetLinks().size() == 1);
    network.DeleteNode(*network.GetNodes().at(0));
    REQUIRE(network.GetLinks().empty());
  }
}

/*
 * Link* AddLink(Node& input, Node& output);
 */
TEST_CASE("Network: Add Links", "[Network][AddLink]") {

  neurons::Network network;

  auto module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
  network.AddNode(neurons::NodeType::Conv2D,
                  std::make_unique<fl::Conv2D>(module));
  auto module_two = fl::Linear(1, 1);
  network.AddNode(neurons::NodeType::Linear,
                  std::make_unique<fl::Linear>(module_two));

  SECTION("Adding a valid link between two nodes") {
    REQUIRE(network.AddLink(
        *network.GetNodes().at(0), *network.GetNodes().at(1)) != nullptr);
  }

  SECTION("Linking a node to itself (invalid)") {
    REQUIRE(network.AddLink(
        *network.GetNodes().at(0), *network.GetNodes().at(0)) == nullptr);
  }

  SECTION("Adding a link between a node not in the network (invalid)") {
    auto mod = fl::Linear(1, 1);
    auto not_networked_node = neurons::ModuleNode(5, neurons::NodeType::Linear,
        std::make_unique<fl::Linear>(mod));
    REQUIRE(network.AddLink(
        *network.GetNodes().at(0), not_networked_node) == nullptr);
  }

}

/*
 * void DeleteLink(const Link& link);
 */

TEST_CASE("Network: Deleting Links", "[Network][DeleteLink]") {
  neurons::Network network;

  auto module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
  network.AddNode(neurons::NodeType::Conv2D,
      std::make_unique<fl::Conv2D>(module));
  auto module_two = fl::Linear(1, 1);
  network.AddNode(neurons::NodeType::Linear,
                  std::make_unique<fl::Linear>(module_two));

  network.AddLink(*network.GetNodes().at(0),
      *network.GetNodes().at(1));

  SECTION("Removing an in-network link") {
    network.DeleteLink(network.GetLinks().at(0));
    REQUIRE(network.GetLinks().empty());
  }

  SECTION("Removing an out-of-network link") {
    auto link = neurons::Link::BuildLink(5, *network.GetNodes().at(0),
                                         *network.GetNodes().at(1));
    network.DeleteLink(link);
    // should not change
    REQUIRE(network.GetLinks().size() == 1);
  }
}

/*
 * const Node* Network::GetLossNode() const
 */

TEST_CASE("Network: Get Loss ModuleNode", "[Network][GetLossNode]") {

  auto network = neurons::Network();

  SECTION("No loss node") {
    auto module = fl::Linear(1, 1);
    network.AddNode(neurons::NodeType::Linear,
                    std::make_unique<fl::Linear>(module));
    REQUIRE(network.GetLossNode() == nullptr);
  }
  SECTION("Adding and deleting a loss node") {
    auto module = fl::CategoricalCrossEntropy();
    network.AddNode(neurons::NodeType::CategoricalCrossEntropy,
        std::make_unique<fl::CategoricalCrossEntropy>(module));
    REQUIRE(network.GetLossNode()->GetNodeType() ==
            neurons::NodeType::CategoricalCrossEntropy);
    network.DeleteNode(*network.GetLossNode());
    REQUIRE(network.GetLossNode() == nullptr);
  }
}