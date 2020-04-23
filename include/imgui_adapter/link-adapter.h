// Copyright (c) 2020 Simon Liu. All rights reserved.

#ifndef FINALPROJECT_IMGUI_ADAPTER_LINK_ADAPTER_H
#define FINALPROJECT_IMGUI_ADAPTER_LINK_ADAPTER_H

#include "neurons/link.h"
#include "imgui_node_editor.h"

namespace neurons {

namespace adapter {

// Adapter between the imgui-node-editor graph link and a neuron Link
// imgui-node-editor Link relies on pins
struct LinkAdapter {
  ax::NodeEditor::LinkId id_;
  ax::NodeEditor::PinId input_id_;
  ax::NodeEditor::PinId output_id_;

  Link* link_;

  // Constructor from Link
  explicit LinkAdapter(Link& link);
};

// Return a vector of LinkAdapters wrapped around the passed links
std::vector<LinkAdapter> BuildLinkAdapters(std::vector<Link>& links);

// Returns a pointer to the Link in the passed vector with the passed LinkId.
// If multiple Links have the same LinkId, will return the first one.
// Returns nullptr if none of the Pins match the criteria.
LinkAdapter* FindOwnerLink(std::vector<LinkAdapter>& links,
    const ax::NodeEditor::LinkId& id);

}  // namespace adapter

}  // namespace neurons

#endif  // FINALPROJECT_IMGUI_ADAPTER_LINK_ADAPTER_H
