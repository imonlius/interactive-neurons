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

using neurons::adapter::NodeAdapter;
using neurons::adapter::LinkAdapter;
using neurons::Node;
using neurons::Network;
using neurons::Link;

InteractiveNeurons::InteractiveNeurons() {
  network_ = Network();
  freeze_editor_ = false;
}

void InteractiveNeurons::setup() {
  ImGui::Initialize(ImGui::Options());
  imnodes::Initialize();
}

void InteractiveNeurons::update() { }

void InteractiveNeurons::DrawNodes(
    const std::vector<NodeAdapter>& nodes) {
  for (const NodeAdapter& node : nodes) {
    imnodes::BeginNode(node.id_);

    imnodes::BeginNodeTitleBar();
    auto test = node.node_->prettyString();
    ImGui::TextWrapped("%s", node.node_->prettyString().c_str());
    imnodes::EndNodeTitleBar();

    imnodes::BeginInputAttribute(node.input_id_);
    ImGui::Text("Input");
    imnodes::EndAttribute();

    imnodes::BeginOutputAttribute(node.output_id_);
    ImGui::Indent(50);
    ImGui::Text("Output");
    imnodes::EndAttribute();

    imnodes::EndNode();
  }
}

void InteractiveNeurons::AttemptLink(std::vector<NodeAdapter>& nodes,
    std::vector<LinkAdapter>& links, size_t start_id, size_t end_id) {

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

  // see if Link can be added
  if (!Link::CanLink(*input->node_, *output->node_)) {
    return;
  }

  links.emplace_back(*network_.AddLink(*input->node_, *output->node_));
}

void InteractiveNeurons::HandleLinkDeletion(
    std::vector<LinkAdapter>& links) {
  // See if any links are selected
  const size_t num_links_selected = imnodes::NumSelectedLinks();

  if (num_links_selected > 0 &&
      ImGui::IsKeyReleased(ImGui::GetIO().KeyMap[ImGuiKey_Backspace])) {
    std::vector<int> selected_links;
    selected_links.resize(num_links_selected);
    // Load selected_nodes with IDs of selected nodes
    imnodes::GetSelectedLinks(selected_links.data());
    for (const int id : selected_links) {
      auto adapter = adapter::FindOwnerLink(links, id);
      network_.DeleteLink(*adapter->link_);
    }
  }
}

void InteractiveNeurons::HandleNodeDeletion(
    std::vector<NodeAdapter>& nodes) {
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
}

void InteractiveNeurons::HandleNodeCreation() {
  // If a non-node/link area is right-clicked
  if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
      !ImGui::IsAnyItemHovered() &&
      ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
    ImGui::OpenPopup("Add Node");
  }

  static ImVec2 mouse_position;

  // add_node will be filled with the NodeType to be added if desired
  NodeType add_node = NodeType::Dummy;
  if (ImGui::BeginPopup("Add Node")) {
    // record mouse position on right click
    mouse_position = ImGui::GetMousePosOnOpeningCurrentPopup();

    if (ImGui::MenuItem("Add Conv2D Node")) {
      add_node = NodeType::Conv2D;
    }
    if (ImGui::MenuItem("Add Linear Node")) {
      add_node = NodeType::Linear;
    }

    ImGui::EndPopup();
  }

  // open pop-up based on desired node type to be added
  switch(add_node) {
    case NodeType::Conv2D:
      ImGui::OpenPopup("Add Conv2D");
      freeze_editor_ = true;
      break;
    case NodeType::Linear:
      ImGui::OpenPopup("Add Linear");
      freeze_editor_ = true;
      break;
    default:
      break;
  }

  bool node_created = false;

  if (ImGui::BeginPopupModal("Add Conv2D")) {
      static int n_args[10]; // must be static to be preserved between draws
    static bool bias;
    const std::string labels[] = {"Input Channels", "Output Channels",
                                  "Kernel X-dim Size", "Kernel Y-dim Size",
                                  "Conv X-dim Stride", "Conv Y-dim Stride",
                                  "Zero-Padding X-dim", "Zero-Padding Y-dim",
                                  "Conv X-dim Dilation", "Conv Y-Dim Dilation"};
    for (size_t i = 0; i < 10; ++i) {
      ImGui::Text("%s", labels[i].c_str());
      std::string label = "##" + labels[i]; // ## makes invis. label
      ImGui::InputInt(label.c_str(), &n_args[i]);
    }

    ImGui::Checkbox("Learnable Bias: ", &bias);

    if (ImGui::Button("Cancel")) {
      freeze_editor_ = false;
      ImGui::CloseCurrentPopup();
    }

    if (ImGui::Button("Add Node")) {

      // Check that arguments are valid
      bool args_valid = true;
      for (size_t i = 0; i < 10; ++i) {
        // zero-padding can be zero+, everything else must be positive.
        if (!(n_args[i] > 0 || ((i == 6 || i == 7) && n_args[i] == 0))) {
          args_valid = false;
        }
      }

      if (args_valid) {
        network_.AddNode(NodeType::Conv2D,std::make_unique<fl::Conv2D>(
            fl::Conv2D(n_args[0], n_args[1], n_args[2],
                       n_args[3],n_args[4], n_args[5], n_args[6],
                       n_args[7], n_args[8],n_args[9], bias)));
        node_created = true;
        ImGui::CloseCurrentPopup();
      }
    }
    ImGui::EndPopup();
  }

  if (ImGui::BeginPopupModal("Add Linear")) {
    static int n_args[2]; // must be static to be preserved between draws
    static bool bias;
    const std::string labels[] = {"Input Size", "Output Size"};
    for (size_t i = 0; i < 2; ++i) {
      ImGui::Text("%s", labels[i].c_str());
      std::string label = "##" + labels[i]; // ## makes invis. label
      ImGui::InputInt(label.c_str(), &n_args[i]);
    }

    ImGui::Checkbox("Learnable Bias: ", &bias);

    if (ImGui::Button("Cancel")) {
      freeze_editor_ = false;
      ImGui::CloseCurrentPopup();
    }

    if (ImGui::Button("Add Node")) {
      // Check that arguments are valid
      bool args_valid = (n_args[0] > 0 && n_args[1] > 0);

      if (args_valid) {
        network_.AddNode(NodeType::Linear,std::make_unique<fl::Linear>(
            fl::Linear(n_args[0], n_args[1], bias)));

        node_created = true;
        ImGui::CloseCurrentPopup();
      }
    }
    ImGui::EndPopup();
  }

  if (node_created) {
    // set the position of the spawned node to the cursor position
    auto adapter = NodeAdapter(network_.GetNodes().back());
    imnodes::SetNodeScreenSpacePos(adapter.id_, mouse_position);

    // unfreeze the editor to allow graph editing
    freeze_editor_ = false;
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

  HandleNodeCreation();
  imnodes::EndNodeEditor();

  if (!freeze_editor_) {
    HandleNodeDeletion(nodes);
    HandleLinkDeletion(links);

    // See if any links were drawn
    int start_pin;
    int end_pin;
    if (imnodes::IsLinkCreated(&start_pin, &end_pin)) {
      AttemptLink(nodes, links, start_pin, end_pin);
    }
  }

  ImGui::End();
}

void InteractiveNeurons::keyDown(KeyEvent event) { }

void InteractiveNeurons::quit() {
  imnodes::Shutdown();
  ImGui::DestroyContext();
}

}  // namespace neurons
