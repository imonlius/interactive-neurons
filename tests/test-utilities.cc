// Copyright (c) 2020 Simon Liu. All rights reserved.

#include "neurons/data-node.h"
#include "neurons/utilities.h"
#include <catch2/catch.hpp>

using neurons::Link;
using neurons::Node;
using neurons::DataNode;
using neurons::ModuleNode;
using neurons::utilities::AreNodeInputsSatisfied;
using neurons::utilities::ContainsDirectedCycle;
using neurons::utilities::CountConnectedComponents;
using neurons::utilities::NodesAndLinksConsistent;
using neurons::utilities::TopologicalSort;

/*
 * bool NodesAndLinksConsistent(const NodeDeque& nodes,
    const std::deque<Link>& links);
 */
TEST_CASE("Utilities: NodesAndLinksConsistent",
    "[Utilities][NodesAndLinksConsistent]") {

  SECTION("Duplicate Node IDs") {
    auto node_one =
        std::make_shared<ModuleNode>(0, neurons::Conv2D, nullptr);
    auto node_two =
        std::make_shared<ModuleNode>(0, neurons::Conv2D, nullptr);

    neurons::NodeDeque nodes = {node_one, node_two};
    std::deque<Link> links;
    REQUIRE_FALSE(NodesAndLinksConsistent(nodes, links));
  }

  SECTION("Duplicate Link IDs") {
    auto node_one =
        std::make_shared<ModuleNode>(0, neurons::Conv2D, nullptr);
    auto node_two =
        std::make_shared<ModuleNode>(1, neurons::Conv2D, nullptr);

    neurons::NodeDeque nodes = {node_one, node_two};
    std::deque<Link> links;
    links.emplace_back(2, node_one, node_two);
    links.emplace_back(2, node_one, node_two);
    REQUIRE_FALSE(NodesAndLinksConsistent(nodes, links));
  }

  SECTION("Link and Node Same ID") {
    auto node_one =
        std::make_shared<ModuleNode>(0, neurons::Conv2D, nullptr);
    auto node_two =
        std::make_shared<ModuleNode>(1, neurons::Conv2D, nullptr);

    neurons::NodeDeque nodes = {node_one, node_two};
    std::deque<Link> links;
    links.emplace_back(0, node_one, node_two);
    links.emplace_back(2, node_one, node_two);
    REQUIRE_FALSE(NodesAndLinksConsistent(nodes, links));
  }

  SECTION("Link with Foreign Node") {
    auto node_one =
        std::make_shared<ModuleNode>(0, neurons::Conv2D, nullptr);
    auto node_two =
        std::make_shared<ModuleNode>(1, neurons::Conv2D, nullptr);

    neurons::NodeDeque nodes = {node_one};
    std::deque<Link> links;
    links.emplace_back(2, node_one, node_two);
    REQUIRE_FALSE(NodesAndLinksConsistent(nodes, links));
  }

  SECTION("Link with nullptr Node") {
    auto node_one =
        std::make_shared<ModuleNode>(0, neurons::Conv2D, nullptr);
    auto node_two =
        std::make_shared<ModuleNode>(1, neurons::Conv2D, nullptr);

    neurons::NodeDeque nodes = {node_one, node_two};
    std::deque<Link> links;
    links.emplace_back(2, node_one, nullptr);
    REQUIRE_FALSE(NodesAndLinksConsistent(nodes, links));
  }

  SECTION("Nullptr Node") {
    auto node_one =
        std::make_shared<ModuleNode>(0, neurons::Conv2D, nullptr);
    auto node_two =
        std::make_shared<ModuleNode>(1, neurons::Conv2D, nullptr);

    neurons::NodeDeque nodes = {node_one, node_two, nullptr};
    std::deque<Link> links;
    REQUIRE_FALSE(NodesAndLinksConsistent(nodes, links));
  }

  SECTION("No nodes or links (valid)") {
    REQUIRE(NodesAndLinksConsistent(neurons::NodeDeque(), std::deque<Link>()));
  }

  SECTION("Populated network (valid)") {
    auto node_one =
        std::make_shared<ModuleNode>(0, neurons::Conv2D, nullptr);
    auto node_two =
        std::make_shared<ModuleNode>(1, neurons::Conv2D, nullptr);

    neurons::NodeDeque nodes = {node_one, node_two};
    std::deque<Link> links;
    links.emplace_back(2, node_one, node_two);
    REQUIRE(NodesAndLinksConsistent(nodes, links));
  }

}

/*
 * NodeDeque TopologicalSort(const NodeDeque& nodes,
    const std::deque<Link>& links);
 */
TEST_CASE("Utilities: TopologicalSort",
    "[Utilities][TopologicalSort]") {

  SECTION("Nodes without links") {
    const auto node_one =
        std::make_shared<ModuleNode>(0, neurons::Conv2D, nullptr);
    const auto node_two =
        std::make_shared<ModuleNode>(1, neurons::Conv2D, nullptr);
    const neurons::NodeDeque nodes = {node_one, node_two};

    const neurons::NodeDeque sorted =
        TopologicalSort(nodes, std::deque<Link>());
    REQUIRE(sorted.size() == 2);
    // check that sorted contains node_one and node_two, any order
    REQUIRE(std::find(sorted.begin(), sorted.end(), node_one) != sorted.end());
    REQUIRE(std::find(sorted.begin(), sorted.end(), node_two) != sorted.end());
  }

  SECTION("One prime dependency") {
    const auto node_one =
        std::make_shared<ModuleNode>(0, neurons::Conv2D, nullptr);
    const auto node_two =
        std::make_shared<ModuleNode>(1, neurons::Conv2D, nullptr);
    const auto node_three =
        std::make_shared<ModuleNode>(2, neurons::Conv2D, nullptr);
    const auto node_four =
        std::make_shared<ModuleNode>(3, neurons::Conv2D, nullptr);
    const neurons::NodeDeque nodes =
        {node_one, node_two, node_three, node_four};


    std::deque<Link> links;
    links.emplace_back(4, node_one, node_two);
    links.emplace_back(5, node_two, node_three);
    links.emplace_back(6, node_three, node_four);

    const neurons::NodeDeque sorted = TopologicalSort(nodes, links);
    REQUIRE(sorted.size() == 4);
    REQUIRE(sorted.at(0) == node_one);
    REQUIRE(sorted.at(1) == node_two);
    REQUIRE(sorted.at(2) == node_three);
    REQUIRE(sorted.at(3) == node_four);
  }

  SECTION("Two prime dependencies") {
    const auto node_one =
        std::make_shared<ModuleNode>(0, neurons::Conv2D, nullptr);
    const auto node_two =
        std::make_shared<ModuleNode>(1, neurons::Conv2D, nullptr);
    const auto node_three =
        std::make_shared<ModuleNode>(2, neurons::Conv2D, nullptr);
    const auto node_four =
        std::make_shared<ModuleNode>(3, neurons::Conv2D, nullptr);
    const neurons::NodeDeque nodes =
        {node_one, node_two, node_three, node_four};


    std::deque<Link> links;
    links.emplace_back(4, node_one, node_two);
    links.emplace_back(5, node_three, node_four);

    const neurons::NodeDeque sorted = TopologicalSort(nodes, links);
    // require that index_two > index_one and index_four > index_three
    // and that index_one and index_three >= 0 (any order)
    auto index_one = std::distance(sorted.begin(),
        std::find(sorted.begin(), sorted.end(), node_one));
    auto index_two = std::distance(sorted.begin(),
        std::find(sorted.begin(), sorted.end(), node_two));
    auto index_three = std::distance(sorted.begin(),
        std::find(sorted.begin(), sorted.end(), node_three));
    auto index_four = std::distance(sorted.begin(),
        std::find(sorted.begin(), sorted.end(), node_four));

    REQUIRE(index_one >= 0);
    REQUIRE(index_three >= 0);
    REQUIRE(index_two > index_one);
    REQUIRE(index_four > index_three);
  }

}

/*
 * bool ContainsDirectedCycle(const std::deque<Link>& links);
 */
TEST_CASE("Utilities: ContainsDirectedCycle",
    "[Utilities][ContainsDirectedCycle]") {

  SECTION("No components") {
    REQUIRE_FALSE(
        ContainsDirectedCycle(neurons::NodeDeque(), std::deque<Link>()));
  }

  SECTION("Contains a directed cycle") {
    auto node_one =
        std::make_shared<ModuleNode>(0, neurons::Conv2D, nullptr);
    auto node_two =
        std::make_shared<ModuleNode>(1, neurons::Conv2D, nullptr);

    neurons::NodeDeque nodes = {node_one, node_two};
    std::deque<Link> links;
    links.emplace_back(2, node_one, node_two);
    links.emplace_back(3, node_two, node_one);
    REQUIRE(ContainsDirectedCycle(nodes, links));
  }

  SECTION("Contains multiple directed cycles") {
    auto node_one =
        std::make_shared<ModuleNode>(0, neurons::Conv2D, nullptr);
    auto node_two =
        std::make_shared<ModuleNode>(1, neurons::Conv2D, nullptr);
    auto node_three =
        std::make_shared<ModuleNode>(2, neurons::Conv2D, nullptr);
    auto node_four =
        std::make_shared<ModuleNode>(3, neurons::Conv2D, nullptr);
    auto node_five =
        std::make_shared<ModuleNode>(4, neurons::Conv2D, nullptr);

    neurons::NodeDeque nodes =
        {node_one, node_two, node_three, node_four, node_five};
    std::deque<Link> links;
    links.emplace_back(5, node_one, node_two);
    links.emplace_back(6, node_two, node_three);
    links.emplace_back(7, node_four, node_five);
    links.emplace_back(8, node_five, node_four);
    REQUIRE(ContainsDirectedCycle(nodes, links));
  }

  SECTION("Contains only a weakly connected component") {
    auto node_one =
        std::make_shared<ModuleNode>(0, neurons::Conv2D, nullptr);
    auto node_two =
        std::make_shared<ModuleNode>(1, neurons::Conv2D, nullptr);
    auto node_three =
        std::make_shared<ModuleNode>(2, neurons::Conv2D, nullptr);

    neurons::NodeDeque nodes = {node_one, node_two, node_three};
    std::deque<Link> links;
    links.emplace_back(3, node_one, node_two);
    links.emplace_back(4, node_one, node_three);
    links.emplace_back(5, node_two, node_three);
    REQUIRE_FALSE(ContainsDirectedCycle(nodes, links));
  }
}

/*
 * size_t CountConnectedComponents(const NodeDeque& nodes,
                                const std::deque<Link>& links);
 */
TEST_CASE("Utilities: CountConnectedComponents",
    "[Utilities][CountConnectedComponents]") {

  SECTION("One connected component") {
    auto node_one =
        std::make_shared<ModuleNode>(0, neurons::Conv2D, nullptr);
    auto node_two =
        std::make_shared<ModuleNode>(1, neurons::Conv2D, nullptr);
    auto node_three =
        std::make_shared<ModuleNode>(2, neurons::Conv2D, nullptr);

    std::deque<Link> links;
    links.emplace_back(3, node_one, node_two);
    links.emplace_back(4, node_two, node_three);
    neurons::NodeDeque nodes = {node_one, node_two, node_three};
    REQUIRE(CountConnectedComponents(nodes, links) == 1);
  }

  SECTION("Multiple connected components") {
    auto node_one =
        std::make_shared<ModuleNode>(0, neurons::Conv2D, nullptr);
    auto node_two =
        std::make_shared<ModuleNode>(1, neurons::Conv2D, nullptr);
    auto node_three =
        std::make_shared<ModuleNode>(2, neurons::Conv2D, nullptr);

    std::deque<Link> links;
    links.emplace_back(3, node_one, node_two);
    neurons::NodeDeque nodes = {node_one, node_two, node_three};
    REQUIRE(CountConnectedComponents(nodes, links) == 2);
  }


  SECTION("All nodes are individual connected components") {
    auto node_one =
        std::make_shared<ModuleNode>(0, neurons::Conv2D, nullptr);
    auto node_two =
        std::make_shared<ModuleNode>(1, neurons::Conv2D, nullptr);
    auto node_three =
        std::make_shared<ModuleNode>(2, neurons::Conv2D, nullptr);

    std::deque<Link> links;
    neurons::NodeDeque nodes = {node_one, node_two, node_three};
    REQUIRE(CountConnectedComponents(nodes, links) == 3);
  }

}

/*
 * bool AreNodeInputsSatisfied(const NodeDeque& nodes,
                            const std::deque<Link>& links);
 */

TEST_CASE("Utilities: AreNodeInputsSatisfied",
    "[Utilities][AreNodeInputsSatisfied]") {

  SECTION("No nodes or links") {
    REQUIRE(AreNodeInputsSatisfied(neurons::NodeDeque(), std::deque<Link>()));
  }

  SECTION("All module nodes have valid inputs") {
    auto node_one =
        std::make_shared<ModuleNode>(0, neurons::Conv2D, nullptr);
    auto node_two =
        std::make_shared<ModuleNode>(1, neurons::Conv2D, nullptr);
    auto node_three =
        std::make_shared<ModuleNode>(2, neurons::Conv2D, nullptr);

    std::deque<Link> links;
    links.emplace_back(3, node_one, node_two);
    links.emplace_back(4, node_two, node_three);
    links.emplace_back(5, node_three, node_one);
    neurons::NodeDeque nodes = {node_one, node_two, node_three};

    REQUIRE(AreNodeInputsSatisfied(nodes, links));
  }

  SECTION("All module nodes have valid inputs + one input-less data node") {
    auto node_one =
        std::make_shared<ModuleNode>(0, neurons::Conv2D, nullptr);
    auto node_two =
        std::make_shared<ModuleNode>(1, neurons::Conv2D, nullptr);
    auto node_three =
        std::make_shared<ModuleNode>(2, neurons::Conv2D, nullptr);
    auto node_four =
        std::make_shared<DataNode>(3, nullptr, nullptr, nullptr);

    std::deque<Link> links;
    links.emplace_back(4, node_four, node_two);
    links.emplace_back(5, node_four, node_three);
    links.emplace_back(6, node_four, node_one);
    neurons::NodeDeque nodes = {node_one, node_two, node_three, node_four};

    REQUIRE(AreNodeInputsSatisfied(nodes, links));
  }

  SECTION("Some module nodes try to satisfy their own inputs") {
    auto node_one =
        std::make_shared<ModuleNode>(0, neurons::Conv2D, nullptr);
    auto node_two =
        std::make_shared<ModuleNode>(1, neurons::Conv2D, nullptr);
    auto node_three =
        std::make_shared<ModuleNode>(2, neurons::Conv2D, nullptr);

    std::deque<Link> links;
    links.emplace_back(4, node_two, node_three);
    links.emplace_back(5, node_three, node_three);
    links.emplace_back(6, node_one, node_one);
    neurons::NodeDeque nodes = {node_one, node_two, node_three};

    REQUIRE_FALSE(AreNodeInputsSatisfied(nodes, links));
  }

  SECTION("Some module nodes lack inputs") {
    auto node_one =
        std::make_shared<ModuleNode>(0, neurons::Conv2D, nullptr);
    auto node_two =
        std::make_shared<ModuleNode>(1, neurons::Conv2D, nullptr);
    auto node_three =
        std::make_shared<ModuleNode>(2, neurons::Conv2D, nullptr);

    std::deque<Link> links;
    links.emplace_back(4, node_two, node_three);
    links.emplace_back(5, node_three, node_three);
    neurons::NodeDeque nodes = {node_one, node_two, node_three};

    REQUIRE_FALSE(AreNodeInputsSatisfied(nodes, links));
  }

}