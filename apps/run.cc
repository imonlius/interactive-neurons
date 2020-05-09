// Copyright (c) 2020 Simon Liu. All rights reserved.

#include <cinder/app/App.h>
#include <cinder/app/RendererGl.h>

#include "interactive_neurons.h"

using cinder::app::App;
using cinder::app::RendererGl;


namespace neurons {

const int kSamples = 8;
const int kWidth = 800;
const int kHeight = 800;

void SetUp(App::Settings* settings) {
  settings->setWindowSize(kWidth, kHeight);
  settings->setTitle("Interactive Neurons");
}

}  // namespace neurons


// This is a macro that runs the application.
CINDER_APP(neurons::InteractiveNeurons,
           RendererGl(RendererGl::Options().msaa(neurons::kSamples)),
           neurons::SetUp)
