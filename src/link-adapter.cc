// Copyright (c) 2020 Simon Liu. All rights reserved.

#include "imgui_adapter/link-adapter.h"

#include "imgui_adapter/node-adapter.h"

namespace neurons {

namespace adapter {

LinkAdapter::LinkAdapter(Link& link) {
  link_ = &link;

  id_ = kIdMultiplier * link.GetId();
  // will throw exception if output_ or input_ is nullptr
  output_id_ = kIdMultiplier * link.output_->GetId() + 2;
  input_id_ = kIdMultiplier * link.input_->GetId() + 1;
}

std::vector<LinkAdapter> BuildLinkAdapters(std::vector<Link>& links) {
  auto adapters = std::vector<LinkAdapter>();
  for (auto& link : links) {
    adapters.emplace_back(link);
  }
  return adapters;
}

LinkAdapter* FindOwnerLink(std::vector<LinkAdapter>& links,
                           const ax::NodeEditor::LinkId& id) {
  for (auto& link : links) {
    if (link.id_ == id) {
      return &link;
    }
  }
  return nullptr;
}

}  // namespace adapter

}  // namespace neurons