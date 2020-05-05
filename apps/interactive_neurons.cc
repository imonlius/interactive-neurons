// Copyright (c) 2020 Simon Liu. All rights reserved.

#include "interactive_neurons.h"

#include <cinder/CinderImGui.h>
#include <cinder/gl/wrapper.h>
#include <imnodes.h>
#include <thread>

#include "imgui_adapter/link-adapter.h"
#include "imgui_adapter/node-adapter.h"
#include "mnist-utilities.h"
#include "neurons/network-container.h"
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
  training_ = false;

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

  // figure out the input and the output nodes
  if (start->output_id_ == start_id && end->input_id_ == end_id) {
    // connection dragged from output to input pin
    // do nothing
  } else if (start->input_id_ == start_id && end->output_id_ == end_id) {
    // connection dragged from input to output pin
    std::swap(start, end);
  } else {
    // pins are either both input or output pins
    // which we treat as an invalid connection
    return;
  }

  // attempt to add link
  network.AddLink(start->node_, end->node_);
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
      // MNIST dataset only allows use of CategoricalCrossEntropy loss
      const std::vector<NodeType> losses = {CategoricalCrossEntropy};
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
    node_created = spawner::SpawnModuleNode(
            network, spawn_node, freeze_editor);
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

// Modifies the passed shared_ptr<fl::FirstOrderOptimizer> and epochs
// with the configuration. Returns true if modification successful.
// Prints any errors/exceptions to output.
bool GetTrainConfiguration(std::shared_ptr<NetworkContainer>& container,
    bool& freeze_editor, std::shared_ptr<fl::FirstOrderOptimizer>& optim,
    int& epochs) {

  freeze_editor = true;
  ImGui::OpenPopup("Train Model");

  bool configured = false;

  if (ImGui::BeginPopupModal("Train Model")) {
    static int config_epochs;
    ImGui::Text("Training Epochs:");
    ImGui::InputInt("##Epochs", &config_epochs);

    static std::string optimizer_str;
    std::string optim_options[] = {"AdadeltaOptimizer", "AdagradOptimizer",
                                "AdamOptimizer", "AMSgradOptimizer",
                                "NovogradOptimizer", "RMSPropOptimizer",
                                "SGDOptimizer"};

    // Get the Optimizer from a drop-down selection
    // derived from github.com/ocornut/imgui/issues/1658
    ImGui::Text("Optimizer:");
    if (ImGui::BeginCombo("##Optimizer", optimizer_str.data())) {
      for (const auto & option : optim_options) {
        bool is_selected = (optimizer_str == option);
        if (ImGui::Selectable(option.c_str(), is_selected)) {
          optimizer_str = option;
        }
        if (is_selected) {
          // sets the initial focus of the list when opened
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo();
    }

    static std::shared_ptr<fl::FirstOrderOptimizer> optimizer;

    static bool optim_valid = false;

    // Load rest of required arguments based on selected optimizer
    if (optimizer_str == "AdadeltaOptimizer" ||
        optimizer_str == "RMSPropOptimizer") {
      static float args[4]; // must be static to be preserved between draws
      const std::string labels[] =
          {"Learning Rate", "Rho", "Epsilon", "Weight Decay"};

      for (size_t i = 0; i < 4; ++i) {
        ImGui::Text("%s", labels[i].c_str());
        std::string label = "##" + labels[i]; // ## makes invis. label
        ImGui::InputFloat(label.c_str(), &args[i]);
      }

      optim_valid = args[0] > 0 && args[1] > 0 && args[2] > 0 && args[3] >= 0;

      if (optim_valid) {
        if (optimizer_str == "AdadeltaOptimizer") {
          optimizer = std::make_shared<fl::AdadeltaOptimizer>(
              fl::AdadeltaOptimizer(container->params(),
                  args[0], args[1], args[2], args[3]));
        } else {
          optimizer = std::make_shared<fl::RMSPropOptimizer>(
              fl::RMSPropOptimizer(container->params(),
                  args[0], args[1], args[2], args[3]));
        }
      }
    }

    if (optimizer_str == "AdagradOptimizer") {
      static float args[3]; // must be static to be preserved between draws
      const std::string labels[] =
          {"Learning Rate", "Epsilon", "Weight Decay"};

      for (size_t i = 0; i < 3; ++i) {
        ImGui::Text("%s", labels[i].c_str());
        std::string label = "##" + labels[i]; // ## makes invis. label
        ImGui::InputFloat(label.c_str(), &args[i]);
      }

      optim_valid = args[0] > 0 && args[1] > 0 && args[2] >= 0;

      if (optim_valid) {
        optimizer = std::make_shared<fl::AdagradOptimizer>(
            fl::AdagradOptimizer(container->params(),
                args[0], args[1], args[2]));
      }
    }

    if (optimizer_str == "AdamOptimizer" ||
        optimizer_str == "AMSgradOptimizer" ||
        optimizer_str == "NovogradOptimizer") {
      static float args[5]; // must be static to be preserved between draws
      const std::string labels[] =
          {"Learning Rate", "Beta1", "Beta2", "Epsilon", "Weight Decay"};

      for (size_t i = 0; i < 5; ++i) {
        ImGui::Text("%s", labels[i].c_str());
        std::string label = "##" + labels[i]; // ## makes invis. label
        ImGui::InputFloat(label.c_str(), &args[i]);
      }

      optim_valid = args[0] > 0 && args[1] > 0 && args[2] > 0 && args[3] > 0 &&
          args[4] >= 0;

      if (optim_valid) {
        if (optimizer_str == "AdamOptimizer") {
          optimizer = std::make_shared<fl::AdamOptimizer>(
              fl::AdamOptimizer(container->params(),args[0],
                  args[1], args[2], args[3], args[4]));
        } else if (optimizer_str == "AMSgradOptimizer") {
          optimizer = std::make_shared<fl::AMSgradOptimizer>(
              fl::AMSgradOptimizer(container->params(),args[0],
                  args[1], args[2], args[3], args[4]));
        } else {
          optimizer = std::make_shared<fl::NovogradOptimizer>(
              fl::NovogradOptimizer(container->params(),args[0],
                  args[1], args[2], args[3], args[4]));

        }
      }
    }

    if (optimizer_str == "SGDOptimizer") {
      static float args[3]; // must be static to be preserved between draws
      static bool use_nesterov;
      const std::string labels[] =
          {"Learning Rate", "Momentum", "Weight Decay", "Use Nesterov"};

      for (size_t i = 0; i < 3; ++i) {
        ImGui::Text("%s", labels[i].c_str());
        std::string label = "##" + labels[i]; // ## makes invis. label
        ImGui::InputFloat(label.c_str(), &args[i]);
      }

      ImGui::Text("%s", labels[3].c_str());
      ImGui::Checkbox(("##" + labels[3]).c_str(), &use_nesterov);

      optim_valid = args[0] > 0 && args[1] > 0 && args[2] >= 0;

      if (optim_valid) {
        optimizer = std::make_shared<fl::SGDOptimizer>(
            fl::SGDOptimizer(container->params(),args[0],
                args[1], args[2], use_nesterov));
      }
    }

    if (ImGui::Button("Cancel")) {
      ImGui::CloseCurrentPopup();
      // Clear the container
      container = nullptr;
      freeze_editor = false;
    }

    if (ImGui::Button("Train")) {
      if (optim_valid && config_epochs > 0) {
        ImGui::CloseCurrentPopup();
        freeze_editor = false;

        // set the values for caller to have access
        epochs = config_epochs;
        optim = optimizer;

        configured = true;
      }
    }
    ImGui::EndPopup();
  }
  return configured;
}

void DrawLog(const std::stringstream& log) {
  ImGui::Begin("Training Log");
  ImGui::BeginChild("Scrolling");
  // make a copy of log_ so we can read the log without erasing it
  std::stringstream log_copy;
  log_copy << log.str();

  std::string line;
  while (std::getline(log_copy, line)) {
    ImGui::Text("%s", line.c_str());
  }
  ImGui::EndChild();
  ImGui::End();
}

void InteractiveNeurons::draw() {
  cinder::gl::clear(cinder:: Color( 0, 0, 0 ) );

  static bool menu_bar_active = false;
  ImGui::Begin("Network Editor", &menu_bar_active, ImGuiWindowFlags_MenuBar);
  imnodes::BeginNodeEditor();

  // Draw nodes
  auto nodes = adapter::BuildNodeAdapters(network_.GetNodes());
  DrawNodes(nodes);

  // Draw Links
  auto links = adapter::BuildLinkAdapters(network_.GetLinks());
  for (const LinkAdapter& link : links) {
    imnodes::Link(link.id_, link.start_id_, link.end_id_);
  }

  HandleNodeCreation(network_, freeze_editor_);
  imnodes::EndNodeEditor();

  static std::shared_ptr<NetworkContainer> container;

  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("Model Actions")) {
      if (!training_ && !freeze_editor_ && exception_ptr == nullptr &&
          ImGui::MenuItem("Train Model")) {
        // this block will throw exceptions if model is invalid architecture
        try {
          container = std::make_shared<NetworkContainer>(
              NetworkContainer(network_.GetNodes(), network_.GetLinks()));
        } catch (std::exception& exception) {
          exception_ptr = std::current_exception();
        }
      }
      if (training_ && !freeze_editor_ && ImGui::MenuItem("Stop Training")) {
          training_ = false;
      }
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }

  if (!training_ && container != nullptr && exception_ptr == nullptr) {
    int epochs;
    std::shared_ptr<fl::FirstOrderOptimizer> optim;
    if (GetTrainConfiguration(container, freeze_editor_, optim, epochs)) {
      // use multi-threading to allow Cinder to run while training
      // train_model has a void return type, but store value so that
      // it operates as an asynchronous thread otherwise it will block main
      train_result_ =
          std::async(std::launch::async, mnist_utilities::train_model,
              std::ref(*container), std::ref(*network_.GetDataNode()),
              std::ref(*optim), epochs, std::ref(log_),
              std::ref(training_), std::ref(exception_ptr));
    }
  }

  if (!freeze_editor_ && !training_) {
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

  // check training_thread for any exceptions
  if (exception_ptr != nullptr) {
    try {
      std::rethrow_exception(exception_ptr);
    } catch (const std::exception& exception) {
      log_ << exception.what() << std::endl;
    }
    exception_ptr = nullptr;
    // halt training in the event of an exception
    training_ = false;
    container = nullptr;
  }

  DrawLog(log_);
}

void InteractiveNeurons::quit() {
  imnodes::Shutdown();
  ImGui::DestroyContext();
}

}  // namespace neurons
