// Copyright (c) 2020 Simon Liu. All rights reserved.

#include "interactive_neurons.h"

#include <cinder/CinderImGui.h>
#include <cinder/gl/wrapper.h>
#include <imnodes.h>

#include "imgui_adapter/link-adapter.h"
#include "imgui_adapter/node-adapter.h"

namespace neurons {

using cinder::app::KeyEvent;
using cinder::app::MouseEvent;

InteractiveNeurons::InteractiveNeurons() {
  network_ = Network();
};

void InteractiveNeurons::setup() {
  ImGui::Initialize(ImGui::Options());
  imnodes::Initialize();

  network_.AddNode(NodeType::Conv2D, fl::Conv2D(1, 1, 1, 1));
  network_.AddNode(NodeType::Conv2D, fl::Conv2D(1, 1, 1, 1));
}

void InteractiveNeurons::update() { }

void InteractiveNeurons::DrawNodes(
    const std::vector<adapter::NodeAdapter>& nodes) {
  for (const adapter::NodeAdapter& node : nodes) {
    imnodes::BeginNode(node.id_);

    imnodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Node");
    imnodes::EndNodeTitleBar();

    imnodes::BeginInputAttribute(node.input_id_);
    ImGui::Text("Input");
    imnodes::EndAttribute();

    imnodes::BeginOutputAttribute(node.output_id_);
    ImGui::Indent(40);
    ImGui::Text("Output");
    imnodes::EndAttribute();

    imnodes::EndNode();
  }
}

void InteractiveNeurons::AttemptLink(std::vector<adapter::NodeAdapter>& nodes,
    std::vector<adapter::LinkAdapter>& links, size_t start_id, size_t end_id) {

  adapter::NodeAdapter* start = adapter::FindPinOwner(nodes, start_id);
  adapter::NodeAdapter* end = adapter::FindPinOwner(nodes, end_id);

  // nullptr safety, then check that the connection is between two diff. nodes
  if (start == nullptr || end == nullptr || start == end) {
    return;
  }

  adapter::NodeAdapter* input;
  adapter::NodeAdapter* output;

  // figure out the input and the output nodes
  if (start->output_id_ == start_id && end->input_id_ == end_id) {
    // connection dragged from output to input pin
    output = start;
    input = end;
  } else if (start->input_id_ == start_id && end->output_id_ == end_id) {
    // connection dragged from input to output pin
    output = end;
    input = start;
  } else {
    // pins are either both input or output pins
    // which we treat as an invalid connection
    return;
  }

  // see if Link can be added
  if (!Link::CanLink(*input->node_, *output->node_)) {
    return;
  }

  links.emplace_back(*network_.AddLink(*input->node_, *output->node_));
}

void InteractiveNeurons::draw() {
  cinder::gl::clear(cinder:: Color( 0, 0, 0 ) );

  ImGui::Begin("Network Editor");
  imnodes::BeginNodeEditor();

  // Draw nodes
  auto nodes = adapter::BuildNodeAdapters(network_.GetNodes());
  DrawNodes(nodes);

  // Draw Links
  auto links = adapter::BuildLinkAdapters(network_.GetLinks());
  for (const adapter::LinkAdapter& link : links) {
    imnodes::Link(link.id_, link.output_id_, link.input_id_);
  }

  imnodes::EndNodeEditor();

  // See if any links were drawn
  int start_pin;
  int end_pin;
  if (imnodes::IsLinkCreated(&start_pin, &end_pin)) {
    AttemptLink(nodes, links, start_pin, end_pin);
  }

  // See if any nodes are selected
  const size_t num_nodes_selected = imnodes::NumSelectedNodes();

  if (num_nodes_selected > 0 &&
      ImGui::IsKeyReleased(ImGui::GetIO().KeyMap[ImGuiKey_Backspace])) {
    std::vector<int> selected_nodes;
    selected_nodes.resize(num_nodes_selected);
    // Load selected_nodes with IDs of selected nodes
    imnodes::GetSelectedNodes(selected_nodes.data());
    for (const int id : selected_nodes) {
      auto adapter = neurons::adapter::FindOwnerNode(nodes, id);
      network_.DeleteNode(*adapter->node_);
    }
  }

  // See if any links are selected
  const size_t num_links_selected = imnodes::NumSelectedLinks();

  if (num_links_selected > 0 &&
      ImGui::IsKeyReleased(ImGui::GetIO().KeyMap[ImGuiKey_Backspace])) {
    std::vector<int> selected_links;
    selected_links.resize(num_links_selected);
    // Load selected_nodes with IDs of selected nodes
    imnodes::GetSelectedLinks(selected_links.data());
    for (const int id : selected_links) {
      auto adapter = neurons::adapter::FindOwnerLink(links, id);
      network_.DeleteLink(*adapter->link_);
    }
  }

  ImGui::End();
}

void InteractiveNeurons::keyDown(KeyEvent event) { }

}  // namespace neurons
