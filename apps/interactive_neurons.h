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
  void quit() override;

 private:
  Network network_;
  bool freeze_editor_;
  const std::string kDataDirectory = getAssetPath("mnist");
};

}  // namespace neurons

#endif  // FINALPROJECT_APPS_INTERACTIVE_NEURONS_H_
