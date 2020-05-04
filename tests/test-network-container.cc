// Copyright (c) 2020 Simon Liu. All rights reserved.

#include <catch2/catch.hpp>

#include "neurons/data-node.h"
#include "neurons/network-container.h"

using Catch::Contains;
using neurons::DataNode;
using neurons::ModuleNode;
using neurons::NetworkContainer;
using neurons::Link;

/*
 * NetworkContainer(NodeDeque& nodes, const std::deque<Link>& links);
 */

TEST_CASE("NetworkContainer: Constructor", "[NetworkContainer][Constructor]") {

  SECTION("Passed nodes and links have inconsistent IDs") {
    auto node_one = std::make_shared<DataNode>(0, nullptr, nullptr, nullptr);
    auto node_two = std::make_shared<ModuleNode>(1, neurons::Conv2D,
        std::make_unique<fl::Conv2D>(fl::Conv2D(1, 1, 1, 1)));
    auto node_three = std::make_shared<ModuleNode>(2,
        neurons::CategoricalCrossEntropy,
        std::make_unique<fl::CategoricalCrossEntropy>());

    neurons::NodeDeque nodes = {node_one, node_two, node_three};
    std::deque<Link> links;
    links.emplace_back(3, node_one, node_two);
    links.emplace_back(3, node_two, node_three);
    REQUIRE_THROWS_WITH(NetworkContainer(nodes, links),
        Contains("Passed nodes and links are inconsistent."));
  }

  SECTION("Passed nodes and links do not have 1 connected component") {
    auto node_one = std::make_shared<DataNode>(0, nullptr, nullptr, nullptr);
    auto node_two = std::make_shared<ModuleNode>(1, neurons::Conv2D,
        std::make_unique<fl::Conv2D>(fl::Conv2D(1, 1, 1, 1)));
    auto node_three = std::make_shared<ModuleNode>(2,
        neurons::CategoricalCrossEntropy,
        std::make_unique<fl::CategoricalCrossEntropy>());

    neurons::NodeDeque nodes = {node_one, node_two, node_three};
    std::deque<Link> links;
    links.emplace_back(3, node_one, node_two);
    REQUIRE_THROWS_WITH(NetworkContainer(nodes, links),
        Contains("Graph does not consist of 1 component."));
  }

  SECTION("Passed nodes and links contains a directed cycle") {
    auto node_one = std::make_shared<DataNode>(0, nullptr, nullptr, nullptr);
    auto node_two = std::make_shared<ModuleNode>(1, neurons::Conv2D,
        std::make_unique<fl::Conv2D>(fl::Conv2D(1, 1, 1, 1)));
    auto node_four = std::make_shared<ModuleNode>(2, neurons::Conv2D,
        std::make_unique<fl::Conv2D>(fl::Conv2D(1, 1, 1, 1)));
    auto node_three = std::make_shared<ModuleNode>(3,
        neurons::CategoricalCrossEntropy,
        std::make_unique<fl::CategoricalCrossEntropy>());

    neurons::NodeDeque nodes = {node_one, node_two, node_three, node_four};
    std::deque<Link> links;
    links.emplace_back(4, node_one, node_two);
    links.emplace_back(5, node_two, node_three);
    links.emplace_back(6, node_two, node_four);
    links.emplace_back(7, node_four, node_two);
    REQUIRE_THROWS_WITH(NetworkContainer(nodes, links),
        Contains("Graph contains a directed cycle."));
  }

  SECTION("Passed nodes do not have satisfied inputs") {
    auto node_two = std::make_shared<ModuleNode>(1, neurons::Conv2D,
        std::make_unique<fl::Conv2D>(fl::Conv2D(1, 1, 1, 1)));
    auto node_three = std::make_shared<ModuleNode>(2,
        neurons::CategoricalCrossEntropy,
        std::make_unique<fl::CategoricalCrossEntropy>());

    neurons::NodeDeque nodes = {node_two, node_three};
    std::deque<Link> links;
    links.emplace_back(4, node_two, node_three);
    REQUIRE_THROWS_WITH(NetworkContainer(nodes, links),
        Contains("Graph node inputs are not satisfied."));
  }

  SECTION("Passed nodes do not have satisfied outputs") {
    auto node_one = std::make_shared<DataNode>(0, nullptr, nullptr, nullptr);
    auto node_two = std::make_shared<ModuleNode>(1, neurons::Conv2D,
        std::make_unique<fl::Conv2D>(fl::Conv2D(1, 1, 1, 1)));

    neurons::NodeDeque nodes = {node_one, node_two};
    std::deque<Link> links;
    links.emplace_back(3, node_one, node_two);
    REQUIRE_THROWS_WITH(NetworkContainer(nodes, links),
        Contains("Graph node outputs are not satisfied."));
  }

  SECTION("Passed nodes and links are valid.") {
    auto node_one = std::make_shared<DataNode>(0, nullptr, nullptr, nullptr);
    auto node_two = std::make_shared<ModuleNode>(1, neurons::Conv2D,
        std::make_unique<fl::Conv2D>(fl::Conv2D(1, 1, 1, 1)));
    auto node_three = std::make_shared<ModuleNode>(2,
        neurons::CategoricalCrossEntropy,
        std::make_unique<fl::CategoricalCrossEntropy>());

    neurons::NodeDeque nodes = {node_one, node_two, node_three};
    std::deque<Link> links;
    links.emplace_back(3, node_one, node_two);
    links.emplace_back(4, node_two, node_three);
    REQUIRE_NOTHROW(NetworkContainer(nodes, links));
  }

}

/*
 * std::vector<fl::Variable> forward(
 * const std::vector<fl::Variable>& input) override;
 */

TEST_CASE("NetworkContainer: Forward: Vector",
    "[NetworkContainer][Forward][Vector]") {

  SECTION("One input") {
    auto node_one = std::make_shared<DataNode>(0, nullptr, nullptr, nullptr);

    auto node_two = std::make_shared<ModuleNode>(1, neurons::Linear,
        std::make_unique<fl::Linear>(fl::Linear(10, 5, false)));
    node_two->setParams(fl::Variable(af::constant(1, 5, 10, 1), true), 0);

    auto node_three = std::make_shared<ModuleNode>(2, neurons::Linear,
        std::make_unique<fl::Linear>(fl::Linear(5, 1, false)));
    node_three->setParams(fl::Variable(af::constant(1, 1, 5, 1), true), 0);

    auto node_four = std::make_shared<ModuleNode>(3,
        neurons::CategoricalCrossEntropy,
        std::make_unique<fl::CategoricalCrossEntropy>());

    neurons::NodeDeque nodes = {node_one, node_two, node_three, node_four};
    std::deque<Link> links;
    links.emplace_back(4, node_one, node_two);
    links.emplace_back(5, node_two, node_three);
    links.emplace_back(6, node_three, node_four);

    auto network = NetworkContainer(nodes, links);
    std::vector<fl::Variable> data = {fl::input(af::constant(1, 10, 1, 1, 1))};
    auto output = network.forward(data);

    std::vector<fl::Variable> expected =
        {fl::Variable(af::constant(50, 1, 1, 1, 1), false)};

    REQUIRE(output.size() == expected.size());
    REQUIRE(output.front().dims() == expected.front().dims());
    REQUIRE(fl::allClose(output.front(), expected.front(), 1e-7));
  }

  SECTION("Multiple inputs") {
    auto node_one = std::make_shared<DataNode>(0, nullptr, nullptr, nullptr);

    auto node_two = std::make_shared<ModuleNode>(1, neurons::Linear,
        std::make_unique<fl::Linear>(fl::Linear(10, 5, false)));
    node_two->setParams(fl::Variable(af::constant(1, 5, 10, 1), true), 0);

    auto node_three = std::make_shared<ModuleNode>(2, neurons::Linear,
        std::make_unique<fl::Linear>(fl::Linear(5, 1, false)));
    node_three->setParams(fl::Variable(af::constant(1, 1, 5, 1), true), 0);

    auto node_four = std::make_shared<ModuleNode>(3,
        neurons::CategoricalCrossEntropy,
        std::make_unique<fl::CategoricalCrossEntropy>());

    neurons::NodeDeque nodes = {node_one, node_two, node_three, node_four};
    std::deque<Link> links;
    links.emplace_back(4, node_one, node_two);
    links.emplace_back(5, node_two, node_three);
    links.emplace_back(6, node_three, node_four);

    auto network = NetworkContainer(nodes, links);
    std::vector<fl::Variable> data = {fl::input(af::constant(1, 10, 1, 1, 1)),
                                      fl::input(af::constant(2, 10, 1, 1, 1))};
    REQUIRE_THROWS_WITH(network.forward(data),
        Contains("Network expects only one input"));
  }

}

/*
 * fl::Variable forward(const fl::Variable& input);
 */

TEST_CASE("NetworkContainer: Forward", "[NetworkContainer][Forward]") {

  SECTION("One path network") {
    auto node_one = std::make_shared<DataNode>(0, nullptr, nullptr, nullptr);

    auto node_two = std::make_shared<ModuleNode>(1, neurons::Linear,
        std::make_unique<fl::Linear>(fl::Linear(10, 5, false)));
    node_two->setParams(fl::Variable(af::constant(1, 5, 10, 1), true), 0);

    auto node_three = std::make_shared<ModuleNode>(2, neurons::Linear,
        std::make_unique<fl::Linear>(fl::Linear(5, 1, false)));
    node_three->setParams(fl::Variable(af::constant(1, 1, 5, 1), true), 0);

    auto node_four = std::make_shared<ModuleNode>(3,
        neurons::CategoricalCrossEntropy,
        std::make_unique<fl::CategoricalCrossEntropy>());

    neurons::NodeDeque nodes = {node_one, node_two, node_three, node_four};
    std::deque<Link> links;
    links.emplace_back(4, node_one, node_two);
    links.emplace_back(5, node_two, node_three);
    links.emplace_back(6, node_three, node_four);

    auto network = NetworkContainer(nodes, links);
    auto output = network.forward(
        fl::input(af::constant(1, 10, 1, 1, 1)));

    auto expected = fl::Variable(
        af::constant(50, 1, 1, 1, 1), false);

    REQUIRE(output.dims() == expected.dims());
    REQUIRE(fl::allClose(output, expected, 1e-7));
  }

  SECTION("Multi-path network") {
    auto node_one = std::make_shared<DataNode>(0, nullptr, nullptr, nullptr);

    auto node_two = std::make_shared<ModuleNode>(1, neurons::Linear,
        std::make_unique<fl::Linear>(fl::Linear(10, 5, false)));
    node_two->setParams(fl::Variable(af::constant(1, 5, 10, 1), true), 0);

    auto node_three = std::make_shared<ModuleNode>(2, neurons::Linear,
        std::make_unique<fl::Linear>(fl::Linear(5, 1, false)));
    node_three->setParams(fl::Variable(af::constant(1, 1, 5, 1), true), 0);

    auto node_four = std::make_shared<ModuleNode>(3, neurons::Linear,
        std::make_unique<fl::Linear>(fl::Linear(5, 1, false)));
    node_four->setParams(fl::Variable(af::constant(1, 1, 5, 1), true), 0);

    auto node_five = std::make_shared<ModuleNode>(4,
        neurons::CategoricalCrossEntropy,
        std::make_unique<fl::CategoricalCrossEntropy>());

    neurons::NodeDeque nodes =
        {node_one, node_two, node_three, node_four, node_five};
    std::deque<Link> links;
    links.emplace_back(5, node_one, node_two);
    links.emplace_back(6, node_two, node_three);
    links.emplace_back(7, node_two, node_four);
    links.emplace_back(8, node_three, node_five);
    links.emplace_back(9, node_four, node_five);

    auto network = NetworkContainer(nodes, links);
    auto output = network.forward(
        fl::input(af::constant(1, 10, 1, 1, 1)));

    auto expected = fl::Variable(
        af::constant(100, 1, 1, 1, 1), false);

    REQUIRE(output.dims() == expected.dims());
    REQUIRE(fl::allClose(output, expected, 1e-7));
  }
}

/*
 * fl::Variable operator()(const fl::Variable& input);
 */

TEST_CASE("NetworkContainer: operator()", "[NetworkContainer][operator]") {

  SECTION("One path network") {
    auto node_one = std::make_shared<DataNode>(0, nullptr, nullptr, nullptr);

    auto node_two = std::make_shared<ModuleNode>(1, neurons::Linear,
        std::make_unique<fl::Linear>(fl::Linear(10, 5, false)));
    node_two->setParams(fl::Variable(af::constant(1, 5, 10, 1), true), 0);

    auto node_three = std::make_shared<ModuleNode>(2, neurons::Linear,
        std::make_unique<fl::Linear>(fl::Linear(5, 1, false)));
    node_three->setParams(fl::Variable(af::constant(1, 1, 5, 1), true), 0);

    auto node_four = std::make_shared<ModuleNode>(3,
        neurons::CategoricalCrossEntropy,
        std::make_unique<fl::CategoricalCrossEntropy>());

    neurons::NodeDeque nodes = {node_one, node_two, node_three, node_four};
    std::deque<Link> links;
    links.emplace_back(4, node_one, node_two);
    links.emplace_back(5, node_two, node_three);
    links.emplace_back(6, node_three, node_four);

    auto network = NetworkContainer(nodes, links);
    auto output = network(fl::input(af::constant(1, 10, 1, 1, 1)));

    auto expected = fl::Variable(
        af::constant(50, 1, 1, 1, 1), false);

    REQUIRE(output.dims() == expected.dims());
    REQUIRE(fl::allClose(output, expected, 1e-7));
  }

  SECTION("Multi-path network") {
    auto node_one = std::make_shared<DataNode>(0, nullptr, nullptr, nullptr);

    auto node_two = std::make_shared<ModuleNode>(1, neurons::Linear,
        std::make_unique<fl::Linear>(fl::Linear(10, 5, false)));
    node_two->setParams(fl::Variable(af::constant(1, 5, 10, 1), true), 0);

    auto node_three = std::make_shared<ModuleNode>(2, neurons::Linear,
        std::make_unique<fl::Linear>(fl::Linear(5, 1, false)));
    node_three->setParams(fl::Variable(af::constant(1, 1, 5, 1), true), 0);

    auto node_four = std::make_shared<ModuleNode>(3, neurons::Linear,
        std::make_unique<fl::Linear>(fl::Linear(5, 1, false)));
    node_four->setParams(fl::Variable(af::constant(1, 1, 5, 1), true), 0);

    auto node_five = std::make_shared<ModuleNode>(4,
        neurons::CategoricalCrossEntropy,
        std::make_unique<fl::CategoricalCrossEntropy>());

    neurons::NodeDeque nodes =
        {node_one, node_two, node_three, node_four, node_five};
    std::deque<Link> links;
    links.emplace_back(5, node_one, node_two);
    links.emplace_back(6, node_two, node_three);
    links.emplace_back(7, node_two, node_four);
    links.emplace_back(8, node_three, node_five);
    links.emplace_back(9, node_four, node_five);

    auto network = NetworkContainer(nodes, links);
    auto output = network(fl::input(af::constant(1, 10, 1, 1, 1)));

    auto expected = fl::Variable(
        af::constant(100, 1, 1, 1, 1), false);

    REQUIRE(output.dims() == expected.dims());
    REQUIRE(fl::allClose(output, expected, 1e-7));
  }
}

/*
 * std::string prettyString() const override;
 */

TEST_CASE("NetworkContainer: prettyString",
    "[NetworkContainer][prettyString]") {

  SECTION("One path network") {
    auto node_one = std::make_shared<DataNode>(0, nullptr, nullptr, nullptr);

    auto node_two = std::make_shared<ModuleNode>(1, neurons::Linear,
        std::make_unique<fl::Linear>(fl::Linear(10, 5, false)));
    node_two->setParams(fl::Variable(af::constant(1, 5, 10, 1), true), 0);

    auto node_three = std::make_shared<ModuleNode>(2, neurons::Linear,
        std::make_unique<fl::Linear>(fl::Linear(5, 1, false)));
    node_three->setParams(fl::Variable(af::constant(1, 1, 5, 1), true), 0);

    auto node_four = std::make_shared<ModuleNode>(3,
        neurons::CategoricalCrossEntropy,
        std::make_unique<fl::CategoricalCrossEntropy>());

    neurons::NodeDeque nodes = {node_one, node_two, node_three, node_four};
    std::deque<Link> links;
    links.emplace_back(4, node_one, node_two);
    links.emplace_back(5, node_two, node_three);
    links.emplace_back(6, node_three, node_four);

    auto network = NetworkContainer(nodes, links);
    REQUIRE(network.prettyString() == "Network:\n"
                                      "[input -> (1), (1) -> (2), (2) -> output]\n"
                                      "(1): Linear (10->5) (without bias)\n"
                                      "(2): Linear (5->1) (without bias)\n");
  }

  SECTION("Multi-path network") {
    auto node_one = std::make_shared<DataNode>(0, nullptr, nullptr, nullptr);

    auto node_two = std::make_shared<ModuleNode>(1, neurons::Linear,
        std::make_unique<fl::Linear>(fl::Linear(10, 5, false)));
    node_two->setParams(fl::Variable(af::constant(1, 5, 10, 1), true), 0);

    auto node_three = std::make_shared<ModuleNode>(2, neurons::Linear,
        std::make_unique<fl::Linear>(fl::Linear(5, 1, false)));
    node_three->setParams(fl::Variable(af::constant(1, 1, 5, 1), true), 0);

    auto node_four = std::make_shared<ModuleNode>(3, neurons::Linear,
        std::make_unique<fl::Linear>(fl::Linear(5, 1, false)));
    node_four->setParams(fl::Variable(af::constant(1, 1, 5, 1), true), 0);

    auto node_five = std::make_shared<ModuleNode>(4,
        neurons::CategoricalCrossEntropy,
        std::make_unique<fl::CategoricalCrossEntropy>());

    neurons::NodeDeque nodes =
        {node_one, node_two, node_three, node_four, node_five};
    std::deque<Link> links;
    links.emplace_back(5, node_one, node_two);
    links.emplace_back(6, node_two, node_three);
    links.emplace_back(7, node_two, node_four);
    links.emplace_back(8, node_three, node_five);
    links.emplace_back(9, node_four, node_five);

    auto network = NetworkContainer(nodes, links);
    // topological sort creates the ordering 1, 3, 2 for nodes
    REQUIRE(network.prettyString() == "Network:\n"
        "[input -> (1), (1) -> (2), (1) -> (3), (2) -> output, (3) -> output]\n"
        "(1): Linear (10->5) (without bias)\n"
        "(3): Linear (5->1) (without bias)\n"
        "(2): Linear (5->1) (without bias)\n");
  }

}