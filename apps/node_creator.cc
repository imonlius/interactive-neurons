// Copyright (c) 2020 Simon Liu. All rights reserved.

#include <cinder/CinderImGui.h>
#include <imnodes.h>

#include "mnist-utilities.h"
#include "node_creator.h"

using neurons::Node;

namespace neurons::spawner {

bool SpawnMnistDataNode(neurons::Network& network,
    const std::string& data_directory, dim_t batch_size) {
  // Initialize DataNode of the Network
  af::array train_x;
  af::array train_y;
  af::array test_x;
  af::array test_y;
  // train_x and train_y are passed as references
  std::tie(train_x, train_y) =
      mnist_utilities::load_dataset(data_directory, false);
  std::tie(test_x, test_y) =
      mnist_utilities::load_dataset(data_directory,true);

  // Hold out the validation sets

  auto valid_x = train_x(af::span, af::span, 0,
      af::seq(0, mnist_utilities::kValSize - 1));
  train_x = train_x(af::span, af::span, 0,
      af::seq(mnist_utilities::kValSize, mnist_utilities::kTrainSize - 1));
  auto valid_y = train_y(
      af::seq(0, mnist_utilities::kValSize - 1));
  train_y = train_y(
      af::seq(mnist_utilities::kValSize,mnist_utilities::kTrainSize - 1));

  // Make the BatchDatasets
  auto train_set = fl::BatchDataset(std::make_shared<fl::TensorDataset>(
      std::vector<af::array>{train_x, train_y}), batch_size);
  auto valid_set = fl::BatchDataset(std::make_shared<fl::TensorDataset>(
      std::vector<af::array>{valid_x, valid_y}), batch_size);
  auto test_set = fl::BatchDataset(std::make_shared<fl::TensorDataset>(
      std::vector<af::array>{test_x, test_y}), batch_size);

  network.AddNode(std::make_unique<fl::BatchDataset>(train_set),
                  std::make_unique<fl::BatchDataset>(valid_set),
                  std::make_unique<fl::BatchDataset>(test_set));
  return true;
}

// Spawn an Activation Node of the passed NodeType. If type does not
// correspond to a valid activation node type, do nothing. Returns true
// if successful.
bool SpawnActivationNode(neurons::Network& network, neurons::NodeType type) {
  std::unique_ptr<fl::Module> module;
  switch (type) {
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
    default:
      // if no activation node is selected
      return false;
  }

  network.AddNode(type, std::move(module));
  return true;
}

// Spawn an Loss Node of the passed NodeType. If type does not
// correspond to a valid loss node type, do nothing. Returns true
// if successful.
bool SpawnLossNode(neurons::Network& network, neurons::NodeType type) {
  std::unique_ptr<fl::BinaryModule> module;

  switch (type) {
    case CategoricalCrossEntropy:
      module = std::make_unique<fl::CategoricalCrossEntropy>(
          fl::CategoricalCrossEntropy());
      break;
    case MeanAbsoluteError:
      module = std::make_unique<fl::MeanAbsoluteError>(fl::MeanAbsoluteError());
      break;
    case MeanSquaredError:
      module = std::make_unique<fl::MeanSquaredError>(fl::MeanSquaredError());
      break;
    default:
      // if no loss node is selected
      return false;
  }

  network.AddNode(type, std::move(module));
  return true;
}

// Starts pop-up configuration for a new BatchNorm Node.
// Returns true if spawning was successful. Sets freeze_editor to whether
// method is currently controlling the GUI.
bool SpawnBatchNormNode(neurons::Network& network, bool& freeze_editor) {
  bool node_created = false;

  ImGui::OpenPopup("BatchNorm");
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
        freeze_editor = false;
        ImGui::CloseCurrentPopup();
      }
    }
    ImGui::EndPopup();
  }
  return node_created;
}

// Starts pop-up configuration for a new LayerNorm Node.
// Returns true if spawning was successful. Sets freeze_editor to whether
// method is currently controlling the GUI.
bool SpawnLayerNormNode(neurons::Network& network, bool& freeze_editor) {
  bool node_created = false;

  ImGui::OpenPopup("LayerNorm");
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
        freeze_editor = false;
        ImGui::CloseCurrentPopup();
      }
    }
    ImGui::EndPopup();
  }
  return node_created;
}

// Starts pop-up configuration for a new Dropout Node.
// Returns true if spawning was successful. Sets freeze_editor to whether
// method is currently controlling the GUI.
bool SpawnDropoutNode(neurons::Network& network, bool& freeze_editor) {
  bool node_created = false;

  ImGui::OpenPopup("Dropout");
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
        freeze_editor = false;
        ImGui::CloseCurrentPopup();
      }
    }
    ImGui::EndPopup();
  }
  return node_created;
}

// Starts pop-up configuration for a new View Node.
// Returns true if spawning was successful. Sets freeze_editor to whether
// method is currently controlling the GUI.
bool SpawnViewNode(neurons::Network& network, bool& freeze_editor) {
  bool node_created = false;

  ImGui::OpenPopup("View");
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
        freeze_editor = false;
        ImGui::CloseCurrentPopup();
      }
    }
    ImGui::EndPopup();
  }
  return node_created;
}

// Starts pop-up configuration for a new Conv2D Node.
// Returns true if spawning was successful. Sets freeze_editor to whether
// method is currently controlling the GUI.
bool SpawnConv2DNode(neurons::Network& network, bool& freeze_editor) {
  bool node_created = false;

  ImGui::OpenPopup("Conv2D");
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
        freeze_editor = false;
        ImGui::CloseCurrentPopup();
      }
    }
    ImGui::EndPopup();
  }
  return node_created;
}

// Starts pop-up configuration for a new Pool2D Node.
// Returns true if spawning was successful. Sets freeze_editor to whether
// method is currently controlling the GUI.
bool SpawnPool2DNode(neurons::Network& network, bool& freeze_editor) {
  bool node_created = false;

  ImGui::OpenPopup("Pool2D");
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
        freeze_editor = false;
        ImGui::CloseCurrentPopup();
      }
    }
    ImGui::EndPopup();
  }
  return node_created;
}

// Starts pop-up configuration for a new Linear Node.
// Returns true if spawning was successful. Sets freeze_editor to whether
// method is currently controlling the GUI.
bool SpawnLinearNode(neurons::Network& network, bool& freeze_editor) {
  bool node_created = false;

  ImGui::OpenPopup("Linear");
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
        freeze_editor = false;
        ImGui::CloseCurrentPopup();
      }
    }
    ImGui::EndPopup();
  }
  return node_created;
}

bool SpawnModuleNode(neurons::Network& network, neurons::NodeType type,
                     bool& freeze_editor) {
  const std::vector config_types =
      {BatchNorm, LayerNorm, Dropout, View, Conv2D, Pool2D, Linear};

  // check if node requires a config pop up window.
  if (std::find(config_types.begin(), config_types.end(), type) !=
      config_types.end()) {
    freeze_editor = true;

    // default window spawn size is too small for editing
    ImVec2 window_size = ImVec2(300, 300);
    ImGui::SetNextWindowContentSize(window_size);
  }

  switch (type) {
    case Sigmoid:
    case Tanh:
    case HardTanh:
    case ReLU:
    case LeakyReLU:
    case ELU:
    case ThresholdReLU:
    case GatedLinearUnit:
    case LogSoftmax:
    case Log:
      return SpawnActivationNode(network, type);
    case CategoricalCrossEntropy:
    case MeanAbsoluteError:
    case MeanSquaredError:
      return SpawnLossNode(network, type);
    case BatchNorm:
      return SpawnBatchNormNode(network, freeze_editor);
    case LayerNorm:
      return SpawnLayerNormNode(network, freeze_editor);
    case Dropout:
      return SpawnDropoutNode(network, freeze_editor);
    case View:
      return SpawnViewNode(network, freeze_editor);
    case Conv2D:
      return SpawnConv2DNode(network, freeze_editor);
    case Pool2D:
      return SpawnPool2DNode(network, freeze_editor);
    case Linear:
      return SpawnLinearNode(network, freeze_editor);
    default:
      return false;
  }
}

}  // namespace neurons::spawner