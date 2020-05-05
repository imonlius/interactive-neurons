// Copyright (c) 2020 Simon Liu. All rights reserved.

#include "imgui_adapter/link-adapter.h"

#include "imgui_adapter/node-adapter.h"
#include <catch2/catch.hpp>

/*
 * LinkAdapter::LinkAdapter(Link& link);
 */

TEST_CASE("LinkAdapter: Constructor", "[LinkAdapter][Constructor]") {

  const size_t input_id = 1;
  const size_t output_id = 2;
  const size_t link_id = 3;

  auto input_module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
  auto input_node = std::make_shared<neurons::ModuleNode>(
      neurons::ModuleNode(input_id, neurons::NodeType::Conv2D,
      std::make_unique<fl::Conv2D>(input_module)));
  auto output_module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
  auto output_node = std::make_shared<neurons::ModuleNode>(
      neurons::ModuleNode(output_id, neurons::NodeType::Conv2D,
      std::make_unique<fl::Conv2D>(output_module)));

  auto link = neurons::Link(link_id, input_node, output_node);

  auto link_adapter = neurons::adapter::LinkAdapter(link);

  REQUIRE(link_adapter.link_ == &link);

  REQUIRE(link_adapter.id_ ==
          link_id * neurons::adapter::kIdMultiplier);
  REQUIRE(link_adapter.start_id_ ==
          input_id * neurons::adapter::kIdMultiplier + 2);
  REQUIRE(link_adapter.end_id_ ==
          output_id * neurons::adapter::kIdMultiplier + 1);
}

/*
 * std::vector<LinkAdapter> BuildLinkAdapters(std::deque<Link>& links);
 */

TEST_CASE("LinkAdapter: BuildLinkAdapters",
    "[LinkAdapter][BuildLinkAdapters]") {

  SECTION("No Links") {
    std::deque<neurons::Link> links;
    REQUIRE(neurons::adapter::BuildLinkAdapters(links).empty());
  }

  SECTION("Multiple Links") {
    const size_t input_id = 1;
    const size_t output_id = 2;
    const size_t link_id = 3;
    const size_t reverse_link_id = 4;

    auto input_module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
    auto input_node = std::make_shared<neurons::ModuleNode>(
        neurons::ModuleNode(input_id, neurons::NodeType::Conv2D,
            std::make_unique<fl::Conv2D>(input_module)));
    auto output_module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
    auto output_node = std::make_shared<neurons::ModuleNode>(
        neurons::ModuleNode(output_id, neurons::NodeType::Conv2D,
        std::make_unique<fl::Conv2D>(output_module)));

    auto link = neurons::Link(link_id, input_node, output_node);
    auto link_reverse = neurons::Link(reverse_link_id, output_node, input_node);

    std::deque<neurons::Link> links = {link, link_reverse};
    auto link_adapters = neurons::adapter::BuildLinkAdapters(links);
    REQUIRE(link_adapters.size() == 2);

    REQUIRE(link_adapters.at(0).link_ == &links.at(0));

    REQUIRE(link_adapters.at(0).id_ ==
            link_id * neurons::adapter::kIdMultiplier);
    REQUIRE(link_adapters.at(0).start_id_ ==
            input_id * neurons::adapter::kIdMultiplier + 2);
    REQUIRE(link_adapters.at(0).end_id_ ==
            output_id * neurons::adapter::kIdMultiplier + 1);

    REQUIRE(link_adapters.at(1).link_ == &links.at(1));

    REQUIRE(link_adapters.at(1).id_ ==
            reverse_link_id * neurons::adapter::kIdMultiplier);
    REQUIRE(link_adapters.at(1).start_id_ ==
            output_id * neurons::adapter::kIdMultiplier + 2);
    REQUIRE(link_adapters.at(1).end_id_ ==
            input_id * neurons::adapter::kIdMultiplier + 1);
  }
}

/*
 * LinkAdapter* FindOwnerLink(std::vector<LinkAdapter>& links, size_t id)
 */

TEST_CASE("LinkAdapter: FindOwnerLink", "[LinkAdapter][FindOwnerLink]") {

  const size_t input_id = 1;
  const size_t output_id = 2;
  const size_t link_id = 3;
  const size_t reverse_link_id = 4;

  auto input_module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
  auto input_node = std::make_shared<neurons::ModuleNode>(
      neurons::ModuleNode(input_id, neurons::NodeType::Conv2D,
      std::make_unique<fl::Conv2D>(input_module)));
  auto output_module = fl::Conv2D(1, 1, 1, 1, 1, 1, 1, 1);
  auto output_node = std::make_shared<neurons::ModuleNode>(
      neurons::ModuleNode(output_id, neurons::NodeType::Conv2D,
      std::make_unique<fl::Conv2D>(output_module)));

  auto link = neurons::Link(link_id, input_node, output_node);
  auto link_reverse = neurons::Link(reverse_link_id, output_node, input_node);

  std::deque<neurons::Link> links = {link, link_reverse};
  auto link_adapters = neurons::adapter::BuildLinkAdapters(links);

  SECTION("Link is present") {
    const auto result = neurons::adapter::FindOwnerLink(link_adapters,
        link_adapters.at(0).id_);
    REQUIRE(result == &link_adapters.at(0));
  }

  SECTION("Link is not represent") {
    const auto result = neurons::adapter::FindOwnerLink(link_adapters,
        83472);
    REQUIRE(result == nullptr);
  }
}