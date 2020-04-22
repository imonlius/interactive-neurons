// Copyright (c) 2020 CS126SP20. All rights reserved.

#ifndef FINALPROJECT_APPS_INTERACTIVE_NEURONS_H_
#define FINALPROJECT_APPS_INTERACTIVE_NEURONS_H_

#include <cinder/app/App.h>
#include <imgui_adapter/link-adapter.h>
#include <imgui_adapter/node-adapter.h>
#include <imgui_node_editor.h>
#include <neurons/network.h>

namespace neurons {

class InteractiveNeurons : public cinder::app::App {
 public:
  InteractiveNeurons();
  void setup() override;
  void update() override;
  void draw() override;
  void keyDown(cinder::app::KeyEvent) override;

 private:
  ax::NodeEditor::EditorContext* context_;
  Network network_;

  // Draw all the nodes on the imgui-node-editor NodeEditor.
  void DrawNodes(const std::vector<adapter::NodeAdapter>& nodes);
  // Attempt to create a link between Nodes on the Network.
  void AttemptLink(std::vector<adapter::NodeAdapter>& nodes,
                   std::vector<adapter::LinkAdapter>& links,
                   const ax::NodeEditor::PinId& startPinId,
                   const ax::NodeEditor::PinId& endPinId);
};

}  // namespace neurons

#endif  // FINALPROJECT_APPS_INTERACTIVE_NEURONS_H_
