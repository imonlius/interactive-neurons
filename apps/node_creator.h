// Copyright (c) 2020 Simon Liu. All rights reserved.

#ifndef FINALPROJECT_APPS_NODE_CREATOR_H_
#define FINALPROJECT_APPS_NODE_CREATOR_H_

#include <imgui_adapter/link-adapter.h>
#include <imgui_adapter/node-adapter.h>
#include <neurons/network.h>

namespace neurons::spawner {

// Spawn an MNIST DataNode in the passed network with the
// passed data directory and batch size. Returns true on success.
bool SpawnMnistDataNode(neurons::Network& network,
    const std::string& data_directory, dim_t batch_size);

// Spawn a Node of the passed Node type. Returns true if successful. Freezes
// editor when called, unfreezes editor once action is completed.
// Currently supports: BatchNorm, LayerNorm, Dropout, View,
// Conv2D, Pool2D, Linear
bool SpawnModuleNode(neurons::Network& network, neurons::NodeType type,
    bool& freeze_editor);

}  // namespace neurons::spawner

#endif  // FINALPROJECT_APPS_NODE_CREATOR_H_
