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
  SECTION("Sigmoid") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::NodeType::Sigmoid) == "Sigmoid");
  }
  SECTION("Tanh") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::NodeType::Tanh) == "Tanh");
  }
  SECTION("HardTanh") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::NodeType::HardTanh) == "HardTanh");
  }
  SECTION("ReLU") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::NodeType::ReLU) == "ReLU");
  }
  SECTION("LeakyReLU") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::NodeType::LeakyReLU) == "LeakyReLU");
  }
  SECTION("ELU") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::NodeType::ELU) == "ELU");
  }
  SECTION("ThresholdReLU") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::NodeType::ThresholdReLU) == "ThresholdReLU");
  }
  SECTION("GatedLinearUnit") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::NodeType::GatedLinearUnit) == "GatedLinearUnit");
  }
  SECTION("LogSoftmax") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::NodeType::LogSoftmax) == "LogSoftmax");
  }
  SECTION("Log") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::NodeType::Log) == "Log");
  }
  SECTION("Dropout") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::NodeType::Dropout) == "Dropout");
  }
  SECTION("Pool2D") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::NodeType::Pool2D) == "Pool2D");
  }
  SECTION("View") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::NodeType::View) == "View");
  }
  SECTION("LayerNorm") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::NodeType::LayerNorm) == "LayerNorm");
  }
  SECTION("BatchNorm") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::NodeType::BatchNorm) == "BatchNorm");
  }
  SECTION("CategoricalCrossEntropy") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::NodeType::CategoricalCrossEntropy) ==
        "CategoricalCrossEntropy");
  }
  SECTION("MeanAbsoluteError") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::NodeType::MeanAbsoluteError) == "MeanAbsoluteError");
  }
  SECTION("MeanSquaredError") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::NodeType::MeanSquaredError) == "MeanSquaredError");
  }
  SECTION("CategoricalCrossEntropy") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::NodeType::CategoricalCrossEntropy) ==
        "CategoricalCrossEntropy");
  }
  SECTION("MeanAbsoluteError") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::NodeType::MeanAbsoluteError) == "MeanAbsoluteError");
  }
  SECTION("MeanSquaredError") {
    REQUIRE(neurons::NodeTypeToString(
        neurons::NodeType::MeanSquaredError) == "MeanSquaredError");
  }
}
