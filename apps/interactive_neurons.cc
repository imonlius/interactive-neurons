// Copyright (c) 2020 Simon Liu. All rights reserved.

#include "interactive_neurons.h"

#include <cinder/CinderImGui.h>
#include <cinder/gl/wrapper.h>

#include "imgui_adapter/link-adapter.h"
#include "imgui_adapter/node-adapter.h"

namespace neurons {

using cinder::app::KeyEvent;

namespace ed = ax::NodeEditor;

InteractiveNeurons::InteractiveNeurons() = default;

void InteractiveNeurons::setup() {
  ImGui::Initialize(ImGui::Options());
  context_ = ax::NodeEditor::CreateEditor();

  network_ = Network();
  network_.AddNode(NodeType::Conv2D, fl::Conv2D(1, 1, 1, 1));
  network_.AddNode(NodeType::Conv2D, fl::Conv2D(1, 1, 1, 1));
}

void InteractiveNeurons::update() { }

void InteractiveNeurons::DrawNodes(
    const std::vector<adapter::NodeAdapter>& nodes) {
  for (const adapter::NodeAdapter& node : nodes) {
    ed::BeginNode(node.id_);
    ImGui::Text("Node");

    ed::BeginPin(node.input_id_, ed::PinKind::Input);
    ImGui::Text("Input");
    ed::EndPin();

    ed::BeginPin(node.output_id_, ed::PinKind::Output);
    ImGui::Text("Output");
    ed::EndPin();

    ed::EndNode();
  }
}

void InteractiveNeurons::AttemptLink(std::vector<adapter::NodeAdapter>& nodes,
    std::vector<adapter::LinkAdapter>& links,
    const ed::PinId& startPinId, const ed::PinId& endPinId) {

  adapter::NodeAdapter* start = adapter::FindOwnerNode(nodes, startPinId);
  adapter::NodeAdapter* end = adapter::FindOwnerNode(nodes, endPinId);

  // nullptr safety, then check that the connection is between two diff. nodes
  if (start == nullptr || end == nullptr || start == end) {
    return;
  }

  adapter::NodeAdapter* input;
  adapter::NodeAdapter* output;

  // figure out the input and the output nodes
  if (start->output_id_ == startPinId && end->input_id_ == endPinId) {
    // connection dragged from output to input pin
    output = start;
    input = end;
  } else if (start->input_id_ == startPinId && end->output_id_ == endPinId) {
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
  ed::Link(links.back().id_, links.back().output_id_, links.back().input_id_);
}

void InteractiveNeurons::draw() {
  cinder::gl::clear(cinder:: Color( 0, 0, 0 ) );

  auto& io = ImGui::GetIO();

  ed::SetCurrentEditor(context_);

  // Following code derived from example:
  // github.com/thedmd/imgui-node-editor/blob/master/Examples/BasicInteraction
  // Start interaction with editor.
  ed::Begin("Node Editor", ImVec2(0.0, 0.0f));

  // Draw nodes
  auto nodes = adapter::BuildNodeAdapters(network_.GetNodes());
  DrawNodes(nodes);

  // Draw Links
  auto links = adapter::BuildLinkAdapters(network_.GetLinks());
  for (const adapter::LinkAdapter& link : links) {
    ed::Link(link.id_, link.input_id_, link.output_id_);
  }

  // Handle creation action, returns true if editor want to create new object (node or link)
  if (ed::BeginCreate()) {
    ed::PinId start_pin, end_pin;
    if (ed::QueryNewLink(&start_pin, &end_pin)) {
      // QueryNewLink returns true if editor want to create new link between pins.
      if (ed::AcceptNewItem()) {
        // ed::AcceptNewItem() return true when user release mouse button.
        AttemptLink(nodes, links, start_pin, end_pin);
      }
      // You may choose to reject connection between these nodes
      // by calling ed::RejectNewItem(). This will allow editor to give
      // visual feedback by changing link thickness and color.
    }
  }
  ed::EndCreate(); // Wraps up object creation action handling.


  // Handle deletion action
  if (ed::BeginDelete()) {
    // Loop over them links marked for deletion
    ed::LinkId deletedLinkId;
    while (ed::QueryDeletedLink(&deletedLinkId)) {
      // Accept deletion.
      if (ed::AcceptDeletedItem()) {
        // Then remove link from your data.
        for (auto it = links.begin(); it != links.end(); ++it) {
          if (it->id_ == deletedLinkId) {
            links.erase(it);
            break;
          }
        }
      }
      // You may reject link deletion by calling:
      // ed::RejectDeletedItem();
    }
  }
  ed::EndDelete(); // Wrap up deletion action

  // End of interaction with editor.
  ed::End();

  ed::SetCurrentEditor(nullptr);
}

void InteractiveNeurons::keyDown(KeyEvent event) { }

}  // namespace neurons
