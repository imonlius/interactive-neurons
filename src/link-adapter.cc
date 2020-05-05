// Copyright (c) 2020 Simon Liu. All rights reserved.

#include "imgui_adapter/link-adapter.h"

#include "imgui_adapter/node-adapter.h"

namespace neurons::adapter {

LinkAdapter::LinkAdapter(Link& link) {
  link_ = &link;

  id_ = kIdMultiplier * link.GetId();
  // will throw exception if output_ or input_ is nullptr
  end_id_ = kIdMultiplier * link.output_->GetId() + 1;
  start_id_ = kIdMultiplier * link.input_->GetId() + 2;
}

std::vector<LinkAdapter> BuildLinkAdapters(std::deque<Link>& links) {
  auto adapters = std::vector<LinkAdapter>();
  adapters.reserve(links.size());
  for (auto& link : links) {
    adapters.emplace_back(link);
  }
  return adapters;
}

LinkAdapter* FindOwnerLink(std::vector<LinkAdapter>& links, size_t id) {
  for (auto& link : links) {
    if (link.id_ == id) {
      return &link;
    }
  }
  return nullptr;
}

}  // namespace neurons::adapter