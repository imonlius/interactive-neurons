// Copyright (c) 2020 CS126SP20. All rights reserved.

#ifndef FINALPROJECT_APPS_INTERACTIVE_NEURONS_H_
#define FINALPROJECT_APPS_INTERACTIVE_NEURONS_H_

#include <cinder/app/App.h>
#include <future>
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
  // Whether node editor should be frozen
  bool freeze_editor_;
  // Whether model training is in progress
  bool training_;
  // Training future
  std::future<void> train_result_;
  // Training log
  std::stringstream log_;
  // Training exception pointer
  std::exception_ptr exception_ptr;
  const std::string kDataDirectory = getAssetPath("mnist");
};

}  // namespace neurons

#endif  // FINALPROJECT_APPS_INTERACTIVE_NEURONS_H_
