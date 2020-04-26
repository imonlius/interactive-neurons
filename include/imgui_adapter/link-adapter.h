// Copyright (c) 2020 Simon Liu. All rights reserved.

#ifndef FINALPROJECT_IMGUI_ADAPTER_LINK_ADAPTER_H
#define FINALPROJECT_IMGUI_ADAPTER_LINK_ADAPTER_H

#include "neurons/link.h"

namespace neurons::adapter {

// Adapter between a graph link and a neuron Link
// Graph link relies on pins
struct LinkAdapter {
  size_t id_;
  size_t input_id_;
  size_t output_id_;

  Link* link_;

  // Constructor from Link
  explicit LinkAdapter(Link& link);
};

// Return a vector of LinkAdapters wrapped around the passed links
std::vector<LinkAdapter> BuildLinkAdapters(std::deque<Link>& links);

// Returns a pointer to the Link in the passed vector with the passed link ID.
// If multiple Links have the same link ID, will return the first one.
// Returns nullptr if none of the Pins match the criteria.
LinkAdapter* FindOwnerLink(std::vector<LinkAdapter>& links, size_t id);

}  // namespace neurons::adapter

#endif  // FINALPROJECT_IMGUI_ADAPTER_LINK_ADAPTER_H
