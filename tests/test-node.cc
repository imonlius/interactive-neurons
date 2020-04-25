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
    auto node = neurons::Node(0, neurons::NodeType::Conv2D,
                              std::make_unique<fl::Conv2D>(module));
    REQUIRE(node.GetId() == 0);
    REQUIRE(node.GetNodeType() == neurons::NodeType::Conv2D);
  }
  SECTION("Conv2D NodeType") {
    auto module = fl::Linear(1, 1);
    auto node = neurons::Node(1, neurons::NodeType::Linear,
                              std::make_unique<fl::Linear>(module));
    REQUIRE(node.GetId() == 1);
    REQUIRE(node.GetNodeType() == neurons::NodeType::Linear);
  }
}

/*
 * NodeTypeToString(NodeType type)
 */
TEST_CASE("NodeTypeToString", "[NodeTypeToString][NodeType]") {
  SECTION("Dummy") {
    REQUIRE(neurons::NodeTypeToString(neurons::NodeType::Dummy) == "Dummy");
  }
  SECTION("Conv2D") {
    REQUIRE(neurons::NodeTypeToString(neurons::NodeType::Conv2D) == "Conv2D");
  }
  SECTION("Linear") {
    REQUIRE(neurons::NodeTypeToString(neurons::NodeType::Linear) == "Linear");
  }
  SECTION("Activation") {
    REQUIRE(neurons::NodeTypeToString(neurons::NodeType::Activation) ==
            "Activation");
  }
}

/*
 * NodeTypeToString(ActivationNodeType type)
 */
TEST_CASE("NodeTypeToString: Activation", "[NodeTypeToString][ActivationNodeType]") {
  SECTION("Sigmoid") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::ActivationNodeType::Sigmoid) == "Sigmoid");
  }
  SECTION("Tanh") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::ActivationNodeType::Tanh) == "Tanh");
  }
  SECTION("HardTanh") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::ActivationNodeType::HardTanh) == "HardTanh");
  }
  SECTION("ReLU") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::ActivationNodeType::ReLU) == "ReLU");
  }
  SECTION("LeakyReLU") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::ActivationNodeType::LeakyReLU) == "LeakyReLU");
  }
  SECTION("ELU") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::ActivationNodeType::ELU) == "ELU");
  }
  SECTION("ThresholdReLU") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::ActivationNodeType::ThresholdReLU) == "ThresholdReLU");
  }
  SECTION("GatedLinearUnit") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::ActivationNodeType::GatedLinearUnit) == "GatedLinearUnit");
  }
  SECTION("LogSoftmax") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::ActivationNodeType::LogSoftmax) == "LogSoftmax");
  }
  SECTION("Log") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::ActivationNodeType::Log) == "Log");
  }
}

/*
 * std::unique_ptr<fl::Module> SpawnActivation(ActivationNodeType type);
 */
TEST_CASE("Node: SpawnActivation", "[SpawnActivation]") {
  SECTION("Sigmoid") {
    REQUIRE(neurons::SpawnActivation(
        neurons::ActivationNodeType::Sigmoid)->prettyString() == "Sigmoid");
  }
  SECTION("Tanh") {
    REQUIRE(neurons::SpawnActivation(
        neurons::ActivationNodeType::Tanh)->prettyString() == "Tanh");
  }
  SECTION("HardTanh") {
    REQUIRE(neurons::SpawnActivation(
        neurons::ActivationNodeType::HardTanh)->prettyString() == "HardTanh");
  }
  SECTION("ReLU") {
    REQUIRE(neurons::SpawnActivation(
        neurons::ActivationNodeType::ReLU)->prettyString() == "ReLU");
  }
  SECTION("LeakyReLU") {
    REQUIRE(neurons::SpawnActivation(
        neurons::ActivationNodeType::LeakyReLU)->prettyString() ==
        "LeakyReLU (0.000000)");
  }
  SECTION("ELU") {
    REQUIRE(neurons::SpawnActivation(
        neurons::ActivationNodeType::ELU)->prettyString() == "ELU (1.000000)");
  }
  SECTION("ThresholdReLU") {
    REQUIRE(neurons::SpawnActivation(
        neurons::ActivationNodeType::ThresholdReLU)->prettyString() ==
        "ThresholdReLU");
  }
  SECTION("GatedLinearUnit") {
    REQUIRE(neurons::SpawnActivation(
        neurons::ActivationNodeType::GatedLinearUnit)->prettyString() ==
        "GatedLinearUnit (0)");
  }
  SECTION("LogSoftmax") {
    REQUIRE(neurons::SpawnActivation(
        neurons::ActivationNodeType::LogSoftmax)->prettyString() ==
        "LogSoftmax (0)");
  }
  SECTION("Log") {
    REQUIRE(neurons::SpawnActivation(
        neurons::ActivationNodeType::Log)->prettyString() == "Log");
  }
}
