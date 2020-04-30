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

// Draw all the nodes on the imnodes NodeEditor.
void DrawNodes(const std::vector<NodeAdapter>& nodes) {
  for (const NodeAdapter& node : nodes) {
    imnodes::BeginNode(node.id_);

    imnodes::BeginNodeTitleBar();
    auto test = node.node_->prettyString();
    ImGui::TextWrapped("%s", node.node_->prettyString().c_str());
    imnodes::EndNodeTitleBar();

    imnodes::BeginInputAttribute(node.input_id_);
    ImGui::Text("Input");
    imnodes::EndAttribute();

    // loss nodes should not have an output pin
    NodeType type = node.node_->GetNodeType();
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

  // see if Link can be added
  if (Link::CanLink(*input->node_, *output->node_)) {
    links.emplace_back(*network.AddLink(*input->node_, *output->node_));
  }
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
      // could occur if user selects node and then presses delete twice
      selected_nodes.pop_back();
      if (adapter == nullptr) {
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

  // add_node will be filled with the NodeType to be added if desired
  NodeType add_node = Dummy;

  if (ImGui::BeginPopup("Add Node")) {
    // record mouse position on right click
    mouse_position = ImGui::GetMousePosOnOpeningCurrentPopup();

    // Activation nodes
    if (ImGui::BeginMenu("Activation")) {
      // Activation nodes require no parameters, so we can spawn it here
      // without any additional pop up menus
      const std::vector<NodeType> activations =
          {Sigmoid, Tanh, HardTanh, ReLU, LeakyReLU, ELU,
           ThresholdReLU, GatedLinearUnit, LogSoftmax, Log};
      NodeType spawn_type = Dummy;
      for (auto activation : activations) {
        if (ImGui::MenuItem(NodeTypeToString(activation).c_str())) {
          spawn_type = activation;
        }
      }

      std::unique_ptr<fl::Module> module;
      switch (spawn_type) {
        case Sigmoid:
          module = std::make_unique<fl::Sigmoid>(fl::Sigmoid());
          break;
        case Tanh:
          module = std::make_unique<fl::Tanh>(fl::Tanh());
          break;
        case HardTanh:
          module = std::make_unique<fl::HardTanh>(fl::HardTanh());
          break;
        case ReLU:
          module = std::make_unique<fl::ReLU>(fl::ReLU());
          break;
        case LeakyReLU:
          module = std::make_unique<fl::LeakyReLU>(fl::LeakyReLU());
          break;
        case ELU:
          module = std::make_unique<fl::ELU>(fl::ELU());
          break;
        case ThresholdReLU:
          module = std::make_unique<fl::ThresholdReLU>(fl::ThresholdReLU());
          break;
        case GatedLinearUnit:
          module = std::make_unique<fl::GatedLinearUnit>(fl::GatedLinearUnit());
          break;
        case LogSoftmax:
          module = std::make_unique<fl::LogSoftmax>(fl::LogSoftmax());
          break;
        case Log:
          module = std::make_unique<fl::Log>(fl::Log());
          break;
        case Dummy:
          // if no activation node is selected
          break;
        default:
          throw std::runtime_error("Unexpected activation type.");
      }
      if (module) {
        // if module has been created
        network.AddNode(spawn_type, std::move(module));
        node_created = true;
      }
      ImGui::EndMenu();
    }

    // Loss nodes. Only show if no Loss node exists (short-circuit eval.)
    if (network.GetLossNode() == nullptr && ImGui::BeginMenu("Loss")) {
      // Loss nodes require no parameters, so we can spawn it here
      // without any additional pop up menus
      const std::vector<NodeType> losses =
          {CategoricalCrossEntropy, MeanAbsoluteError, MeanSquaredError};
      NodeType spawn_type = Dummy;
      for (auto loss : losses) {
        if (ImGui::MenuItem(NodeTypeToString(loss).c_str())) {
          spawn_type = loss;
        }
      }
      std::unique_ptr<fl::BinaryModule> module;
      switch (spawn_type) {
        case CategoricalCrossEntropy:
          module = std::make_unique<fl::CategoricalCrossEntropy>(
              fl::CategoricalCrossEntropy());
          break;
        case MeanAbsoluteError:
          module = std::make_unique<fl::MeanAbsoluteError>(
              fl::MeanAbsoluteError());
          break;
        case MeanSquaredError:
          module = std::make_unique<fl::MeanSquaredError>(
              fl::MeanSquaredError());
          break;
        case Dummy:
          // if no loss node is selected
          break;
        default:
          throw std::runtime_error("Unexpected loss type.");
      }
      if (module) {
        // if module has been created
        network.AddNode(spawn_type, std::move(module));
        node_created = true;
      }
      ImGui::EndMenu();
    }

    // These nodes require parameters, so pop up a window for config
    const std::vector types = {BatchNorm, LayerNorm, Dropout, View,
                               Conv2D, Pool2D, Linear};
    for (auto type : types) {
      if (ImGui::MenuItem(NodeTypeToString(type).c_str())) {
        add_node = type;
      }
    }

    ImGui::EndPopup();
  }

  // open pop-up based on desired node type to be added
  if (add_node != Dummy) {
    ImGui::OpenPopup(NodeTypeToString(add_node).c_str());
    freeze_editor = true;
  }

  ImVec2 window_size = ImVec2(300, 300);

  ImGui::SetNextWindowContentSize(window_size);
  if (ImGui::BeginPopupModal("BatchNorm")) {
    static int int_args[2]; // must be static to be preserved between draws
    static double double_args[2];
    static bool bool_args[2];
    const std::string labels[] = {"Feature Axis", "Feature Size",
                                  "Momentum", "Epsilon", "Affine",
                                  "Track Stats"};
    for (size_t i = 0; i < 6; ++i) {
      ImGui::Text("%s", labels[i].c_str());
      std::string label = "##" + labels[i]; // ## makes invis. label
      if (i < 2) {
        ImGui::InputInt(label.c_str(), &int_args[i]);
      } else if (i < 4) {
        // subtract 2 from index to account for separate array
        ImGui::InputDouble(label.c_str(), &double_args[i - 2]);
      } else {
        // subtract 4 from index to account for separate array
        ImGui::Checkbox(label.c_str(), &bool_args[i - 4]);
      }
    }

    if (ImGui::Button("Cancel")) {
      freeze_editor = false;
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("Add Node")) {
      // arguments valid if Feature Axis >= 0 and Feature Size > 0
      bool args_valid = (int_args[0] >= 0 && int_args[1] > 0);

      if (args_valid) {
        network.AddNode(BatchNorm, std::make_unique<fl::BatchNorm>(
            fl::BatchNorm(int_args[0], int_args[1], double_args[0],
                double_args[1], bool_args[0], bool_args[1])));
        node_created = true;
        ImGui::CloseCurrentPopup();
      }
    }
    ImGui::EndPopup();
  }

  ImGui::SetNextWindowContentSize(window_size);
  if (ImGui::BeginPopupModal("LayerNorm")) {
    static int axis; // must be static to be preserved between draws
    static double epsilon;
    static bool affine;
    // for axisSize, uses fl::kLnVariableAxisSize
    const std::string labels[] = {"Normalization Axis", "Epsilon", "Affine"};

    ImGui::Text("%s", labels[0].c_str());
    ImGui::InputInt(("##" + labels[0]).c_str(), &axis);

    ImGui::Text("%s", labels[1].c_str());
    ImGui::InputDouble(("##" + labels[1]).c_str(), &epsilon);

    ImGui::Text("%s", labels[2].c_str());
    ImGui::Checkbox(("##" + labels[2]).c_str(), &affine);

    if (ImGui::Button("Cancel")) {
      freeze_editor = false;
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("Add Node")) {
      // arguments valid if Feature Axis >= 0
      bool args_valid = (axis >= 0);

      if (args_valid) {
        network.AddNode(LayerNorm, std::make_unique<fl::LayerNorm>(
            fl::LayerNorm(axis, epsilon, affine, fl::kLnVariableAxisSize)));
        node_created = true;
        ImGui::CloseCurrentPopup();
      }
    }
    ImGui::EndPopup();
  }

  ImGui::SetNextWindowContentSize(window_size);
  if (ImGui::BeginPopupModal("Dropout")) {
    static double ratio; // must be static to be preserved between draws
    const std::string labels[] = {"Dropout Ratio"};

    ImGui::Text("%s", labels[0].c_str());
    // ## makes invis. label
    ImGui::InputDouble(("##" +  labels[0]).c_str(), &ratio);

    if (ImGui::Button("Cancel")) {
      freeze_editor = false;
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("Add Node")) {
      // Check that arguments are valid
      bool args_valid = (ratio >= 0 && ratio < 1);

      if (args_valid) {
        network.AddNode(Dropout,std::make_unique<fl::Dropout>(
            fl::Dropout(ratio)));
        node_created = true;
        ImGui::CloseCurrentPopup();
      }
    }
    ImGui::EndPopup();
  }

  ImGui::SetNextWindowContentSize(window_size);
  if (ImGui::BeginPopupModal("View")) {
    static int dims[4];
    const std::string labels[] = {"Dimension 1", "Dimension 2",
                                  "Dimension 3", "Dimenison 4"};
    for (size_t i = 0; i < 4; ++i ) {
      ImGui::Text("%s", labels[i].c_str());
      std::string label = "##" + labels[i]; // ## makes invis. label
      ImGui::InputInt(label.c_str(), &dims[i]);
    }

    if (ImGui::Button("Cancel")) {
      freeze_editor = false;
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("Add Node")) {
      // Check that arguments are valid
      bool args_valid = true;

      // View must have either nonnegative or -1 arguments
      size_t num_negative_one = 0;
      for (int dim : dims) {
        if (dim == -1) {
          ++num_negative_one;
        } else if (dim < 0) {
          args_valid = false;
        }
      }

      // View can only have one -1 argument
      if (num_negative_one > 1) {
        args_valid = false;
      }

      if (args_valid) {
        network.AddNode(View, std::make_unique<fl::View>(
            fl::View(af::dim4(
                dims[0], dims[1], dims[2], dims[3]))));
        node_created = true;
        ImGui::CloseCurrentPopup();
      }
    }
    ImGui::EndPopup();
  }

  ImGui::SetNextWindowContentSize(window_size);
  if (ImGui::BeginPopupModal("Conv2D")) {

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
      freeze_editor = false;
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("Add Node")) {
      // Check that arguments are valid
      bool args_valid = true;
      for (size_t i = 0; i < 10; ++i) {
        // zero-padding can be >= -1, everything else must be positive.
        // zero-padding of -1 uses smallest possible padding such that
        // out_size = ceil(in_size / stride)
        if (!(n_args[i] > 0 || ((i == 6 || i == 7) && n_args[i] >= -1))) {
          args_valid = false;
        }
      }

      if (args_valid) {
        network.AddNode(Conv2D,std::make_unique<fl::Conv2D>(
            fl::Conv2D(n_args[0], n_args[1], n_args[2],
                       n_args[3],n_args[4], n_args[5], n_args[6],
                       n_args[7], n_args[8],n_args[9], bias)));
        node_created = true;
        ImGui::CloseCurrentPopup();
      }
    }
    ImGui::EndPopup();
  }

  ImGui::SetNextWindowContentSize(window_size);
  if (ImGui::BeginPopupModal("Pool2D")) {

    static int n_args[6]; // must be static to be preserved between draws
    const std::string labels[] = {
        "Pooling Window X-dim", "Pooling Window Y-dim",
        "Stride X-dim", "Stride Y-dim",
        "Zero-Padding X-dim", "Zero-Padding Y-dim", "Pooling Mode"};

    static std::string pooling_mode;
    const std::string modes[] = {"MAX", "AVG_INCLUDE_PADDING",
                                 "AVG_EXCLUDE_PADDING"};

    for (size_t i = 0; i < 6; ++i) {
      ImGui::Text("%s", labels[i].c_str());
      std::string label = "##" + labels[i]; // ## makes invis. label
      ImGui::InputInt(label.c_str(), &n_args[i]);
    }

    // Get the Pooling Mode from a dropdown selection
    // derived from github.com/ocornut/imgui/issues/1658
    ImGui::Text("%s", labels[6].c_str());
    if (ImGui::BeginCombo(("##" + labels[6]).c_str(), pooling_mode.data())) {
      for (const auto & mode : modes) {
        bool is_selected = (pooling_mode == mode);
        if (ImGui::Selectable(mode.c_str(), is_selected)) {
          pooling_mode = mode;
        }
        if (is_selected) {
          // sets the initial focus of the list when opened
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo();
    }

    // translate pooling_mode string into PoolingMode
    fl::PoolingMode pooling;
    if (pooling_mode == "MAX") {
      pooling = fl::PoolingMode::MAX;
    } else if (pooling_mode == "AVG_INCLUDE_POOLING") {
      pooling = fl::PoolingMode::AVG_INCLUDE_PADDING;
    } else {
      pooling = fl::PoolingMode::AVG_EXCLUDE_PADDING;
    }

    if (ImGui::Button("Cancel")) {
      freeze_editor = false;
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("Add Node")) {
      // Check that arguments are valid
      // All fields must be greater than 0, except padding which must be >= -1
      // zero-padding of -1 uses smallest possible padding such that
      // out_size = ceil(in_size / stride)
      bool args_valid = n_args[0] > 0 && n_args[1] > 0 && n_args[2] > 0 &&
          n_args[3] > 0 && n_args[4] >= -1 && n_args[5] >= -1;

      if (args_valid) {
        network.AddNode(Pool2D, std::make_unique<fl::Pool2D>(
            fl::Pool2D(n_args[0], n_args[1], n_args[2],
                       n_args[3],n_args[4], n_args[5], pooling)));
        node_created = true;
        ImGui::CloseCurrentPopup();
      }
    }
    ImGui::EndPopup();
  }

  ImGui::SetNextWindowContentSize(window_size);
  if (ImGui::BeginPopupModal("Linear")) {
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
      freeze_editor = false;
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("Add Node")) {
      // Check that arguments are valid
      bool args_valid = (n_args[0] > 0 && n_args[1] > 0);

      if (args_valid) {
        network.AddNode(Linear,std::make_unique<fl::Linear>(
            fl::Linear(n_args[0], n_args[1], bias)));
        node_created = true;
        ImGui::CloseCurrentPopup();
      }
    }
    ImGui::EndPopup();
  }

  if (node_created) {
    // set the position of the spawned node to the cursor position
    auto adapter = NodeAdapter(network.GetNodes().back());
    imnodes::SetNodeScreenSpacePos(adapter.id_, mouse_position);

    // unfreeze the editor to allow graph editing
    freeze_editor = false;
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

void InteractiveNeurons::keyDown(KeyEvent event) { }

void InteractiveNeurons::quit() {
  imnodes::Shutdown();
  ImGui::DestroyContext();
}

}  // namespace neurons
