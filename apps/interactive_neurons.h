// Copyright (c) 2020 CS126SP20. All rights reserved.

#ifndef FINALPROJECT_APPS_INTERACTIVE_NEURONS_H_
#define FINALPROJECT_APPS_INTERACTIVE_NEURONS_H_

#include <cinder/app/App.h>
#include <imgui_adapter/link-adapter.h>
#include <imgui_adapter/node-adapter.h>
#include <neurons/network.h>

namespace neurons {

class InteractiveNeurons : public cinder::app::App {
 public:
  InteractiveNeurons();
  void setup() override;
  void update() override;
  void draw() override;
  void keyDown(cinder::app::KeyEvent) override;
  void quit() override;

 private:
  Network network_;
  bool freeze_editor_;

  // Draw all the nodes on the imgui-node-editor NodeEditor.
  void DrawNodes(const std::vector<adapter::NodeAdapter>& nodes);
  // Attempt to create a link between Nodes on the Network.
  void AttemptLink(std::vector<adapter::NodeAdapter>& nodes,
                   std::vector<adapter::LinkAdapter>& links,
                   size_t start, size_t end);

  // Handle Node deletion
  void HandleNodeDeletion(std::vector<adapter::NodeAdapter>& nodes);
  // Handle Link deletion
  void HandleLinkDeletion(std::vector<adapter::LinkAdapter>& links);
  // Handle node creation
  void HandleNodeCreation();
};

}  // namespace neurons

#endif  // FINALPROJECT_APPS_INTERACTIVE_NEURONS_H_
