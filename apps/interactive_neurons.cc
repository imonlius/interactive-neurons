// Copyright (c) 2020 Simon Liu. All rights reserved.

#include "interactive_neurons.h"

#include <cinder/CinderImGui.h>
#include <cinder/gl/wrapper.h>
#include <imnodes.h>

#include "imgui_adapter/link-adapter.h"
#include "imgui_adapter/node-adapter.h"
#include "node_creator.h"

namespace neurons {

using cinder::app::KeyEvent;
using cinder::app::MouseEvent;

using neurons::adapter::NodeAdapter;
using neurons::adapter::LinkAdapter;
using neurons::Network;
using neurons::Link;

InteractiveNeurons::InteractiveNeurons() {
  network_ = Network();
  freeze_editor_ = false;

  dim_t kBatchSize = 64;
  spawner::SpawnMnistDataNode(network_, kDataDirectory, kBatchSize);
}

void InteractiveNeurons::setup() {
  ImGui::Initialize(ImGui::Options());
  imnodes::Initialize();
}

void InteractiveNeurons::update() { }

// Draw all the nodes on the imnodes NodeEditor.
void DrawNodes(const std::vector<NodeAdapter>& nodes) {
  for (const NodeAdapter& node : nodes) {
    imnodes::BeginNode(node.id_);

    imnodes::BeginNodeTitleBar();
    auto test = node.node_->prettyString();
    ImGui::TextWrapped("%s", node.node_->prettyString().c_str());
    imnodes::EndNodeTitleBar();

    NodeType type = node.node_->GetNodeType();

    // data nodes should not have an input pin
    if (type != Dataset) {
      imnodes::BeginInputAttribute(node.input_id_);
      ImGui::Text("Input");
      imnodes::EndAttribute();
    }

    // loss nodes should not have an output pin
    if (!(type == CategoricalCrossEntropy || type == MeanSquaredError ||
          type == MeanAbsoluteError)) {
      imnodes::BeginOutputAttribute(node.output_id_);
      ImGui::Indent(50);
      ImGui::Text("Output");
      imnodes::EndAttribute();
    }

    imnodes::EndNode();
  }
}

// Attempt to create a link between Nodes on the Network.
void AttemptLink(std::vector<NodeAdapter>& nodes,
    std::vector<LinkAdapter>& links, Network& network,
    size_t start_id, size_t end_id) {

  NodeAdapter* start = adapter::FindPinOwner(nodes, start_id);
  NodeAdapter* end = adapter::FindPinOwner(nodes, end_id);

  // nullptr safety, then check that the connection is between two diff. nodes
  if (start == nullptr || end == nullptr || start == end) {
    return;
  }

  NodeAdapter* input;
  NodeAdapter* output;

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

  // attempt to add link
  network.AddLink(input->node_, output->node_);
}

// Handle Link deletion
void HandleLinkDeletion(std::vector<LinkAdapter>& links, Network& network) {
  // See if any links are selected
  const size_t num_links_selected = imnodes::NumSelectedLinks();

  if (num_links_selected > 0 &&
      ImGui::IsKeyReleased(ImGui::GetIO().KeyMap[ImGuiKey_Backspace])) {
    std::vector<int> selected_links;
    selected_links.resize(num_links_selected);
    // Load selected_nodes with IDs of selected nodes
    imnodes::GetSelectedLinks(selected_links.data());
    while(!selected_links.empty()) {
      auto adapter = neurons::adapter::FindOwnerLink(links,
          selected_links.back());
      selected_links.pop_back();
      // could occur if user selects node and then presses delete twice
      if (adapter == nullptr) {
        continue;
      }
      network.DeleteLink(*adapter->link_);
    }
  }
}

// Handle Node deletion
void HandleNodeDeletion(std::vector<NodeAdapter>& nodes, Network& network) {
  // See if any nodes are selected
  const size_t num_nodes_selected = imnodes::NumSelectedNodes();

  if (num_nodes_selected > 0 &&
      ImGui::IsKeyReleased(ImGui::GetIO().KeyMap[ImGuiKey_Backspace])) {
    std::vector<int> selected_nodes;
    selected_nodes.resize(num_nodes_selected);
    // Load selected_nodes with IDs of selected nodes
    imnodes::GetSelectedNodes(selected_nodes.data());
    while(!selected_nodes.empty()) {
      auto adapter = neurons::adapter::FindOwnerNode(nodes,
          selected_nodes.back());
      selected_nodes.pop_back();
      // nullptr could occur if user selects node and then presses delete twice
      // do not allow deletion of Dataset node either
      if (adapter == nullptr || adapter->node_->GetNodeType() == Dataset) {
        continue;
      }
      network.DeleteNode(*adapter->node_);
    }
  }
}

// Handle node creation
void HandleNodeCreation(Network& network, bool& freeze_editor) {
  // If a non-node/link area is right-clicked
  if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
      !ImGui::IsAnyItemHovered() &&
      ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
    ImGui::OpenPopup("Add Node");
  }

  static ImVec2 mouse_position;
  bool node_created = false;
  static NodeType spawn_node = Dummy;

  if (ImGui::BeginPopup("Add Node")) {
    // record mouse position on right click
    mouse_position = ImGui::GetMousePosOnOpeningCurrentPopup();

    // Activation nodes
    if (ImGui::BeginMenu("Activation")) {
      const std::vector<NodeType> activations =
          {Sigmoid, Tanh, HardTanh, ReLU, LeakyReLU, ELU,
           ThresholdReLU, GatedLinearUnit, LogSoftmax, Log};
      for (auto activation : activations) {
        if (ImGui::MenuItem(NodeTypeToString(activation).c_str())) {
          spawn_node = activation;
        }
      }
      ImGui::EndMenu();
    }

    // Loss nodes. Only show if no Loss node exists (short-circuit eval.)
    if (network.GetLossNode() == nullptr && ImGui::BeginMenu("Loss")) {
      const std::vector<NodeType> losses =
          {CategoricalCrossEntropy, MeanAbsoluteError, MeanSquaredError};
      for (auto loss : losses) {
        if (ImGui::MenuItem(NodeTypeToString(loss).c_str())) {
          spawn_node = loss;
        }
      }
      ImGui::EndMenu();
    }

    // Every other Node type
    const std::vector<NodeType> types =
        {BatchNorm, LayerNorm, Dropout, View, Conv2D, Pool2D, Linear};
    for (auto type : types) {
      if (ImGui::MenuItem(NodeTypeToString(type).c_str())) {
        spawn_node = type;
      }
    }
    ImGui::EndPopup();
  }

  if (spawn_node != Dummy) {
    node_created = spawner::SpawnModuleNode(network, spawn_node, freeze_editor);
  }

  // if Node Creator has relinquished control, reset the spawn node
  if (!freeze_editor) {
    spawn_node = Dummy;
  }

  if (node_created) {
    // set the position of the spawned node to the cursor position
    auto adapter = NodeAdapter(network.GetNodes().back());
    imnodes::SetNodeScreenSpacePos(adapter.id_, mouse_position);
  }
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
  for (const LinkAdapter& link : links) {
    imnodes::Link(link.id_, link.output_id_, link.input_id_);
  }

  HandleNodeCreation(network_, freeze_editor_);
  imnodes::EndNodeEditor();

  if (!freeze_editor_) {
    HandleNodeDeletion(nodes, network_);
    HandleLinkDeletion(links, network_);

    // See if any links were drawn
    int start_pin;
    int end_pin;
    if (imnodes::IsLinkCreated(&start_pin, &end_pin)) {
      AttemptLink(nodes, links, network_, start_pin, end_pin);
    }
  }

  ImGui::End();
}

void InteractiveNeurons::quit() {
  imnodes::Shutdown();
  ImGui::DestroyContext();
}

}  // namespace neurons
