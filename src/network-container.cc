// Copyright (c) 2020 Simon Liu. All rights reserved.

#include "neurons/network-container.h"

#include "neurons/utilities.h"

namespace neurons {

NetworkContainer::NetworkContainer(NodeDeque& nodes,
    const std::deque<Link>& links) : links_(links) {

  // check graph requirements.
  if (!utilities::NodesAndLinksConsistent(nodes, links)) {
    throw std::invalid_argument("Passed nodes and links are inconsistent.");
  }
  if (utilities::CountConnectedComponents(nodes, links) != 1) {
    throw std::invalid_argument("Graph does not consist of 1 component.");
  }
  if (utilities::ContainsDirectedCycle(nodes, links)) {
    throw std::invalid_argument("Graph contains a directed cycle.");
  }
  if (!utilities::AreNodeInputsSatisfied(nodes, links)) {
    throw std::invalid_argument("Graph node inputs are not satisfied.");
  }
  if (!utilities::AreNodeOutputsSatisfied(nodes, links)) {
    throw std::invalid_argument("Graph node outputs are not satisfied.");
  }

  // get a topologically sorted list of nodes (guaranteed to exist now)
  // the shared_ptrs in sorted are identical to that of nodes
  NodeDeque sorted = utilities::TopologicalSort(nodes, links);

  // if previous graph conditions are satisfied, then first element
  // of sorted is a DataNode and last element is a loss node
  // pop those off to get the elements that are ModuleNodes
  data_node_id_ = sorted.front()->GetId();
  sorted.pop_front();
  loss_node_id_ = sorted.back()->GetId();
  sorted.pop_back();

  for (auto& node : sorted) {
    // by graph conditions, nodes should all be ModuleNodes
    auto module_node = std::dynamic_pointer_cast<ModuleNode>(node);
    // module_node is a shared_ptr controlled by the passed nodes argument
    // if it is nullptr (dynamic cast fails), then add will throw an exception.
    add(module_node);
  }
}

// Adds two vectors of fl::Variables element-wise.
// Throws exception if sizes of vectors don't match or Variable dimensions
// don't match elementwise.
std::vector<fl::Variable> Add(const std::vector<fl::Variable>& lhs,
    const std::vector<fl::Variable>& rhs) {

  if (lhs.size() != rhs.size()) {
    throw std::invalid_argument("Vector sizes do not match!");
  }

  std::vector<fl::Variable> result;
  for (size_t i = 0; i < lhs.size(); ++i) {
    result.push_back(lhs.at(i) + rhs.at(i));
  }

  return result;
}

std::vector<fl::Variable> NetworkContainer::forward(
    const std::vector<fl::Variable>& input) {
  // network comprises of UnaryModules, so only input is allowed
  if (input.size() != 1) {
    throw std::invalid_argument("Network expects only one input");
  }

  // maps each node to its module output
  // if a node has multiple inputs, sums them all into a single input
  auto output_maps = std::map<fl::ModulePtr, std::vector<fl::Variable>>();

  // modules_ is topologically sorted, so the input to each module
  // is guaranteed to have been processed by the time they're reached
  for (const auto& module : modules_) {
    // create an empty module with the correct dimensions
    std::vector<fl::Variable> module_input;
    // find the input links to module
    for (const auto& link : links_) {
      // comparison method from stackoverflow.com/questions/37489209
      if (std::dynamic_pointer_cast<void>(link.output_) !=
          std::dynamic_pointer_cast<void>(module)) {
        continue;
      }
      // if link input is data node, use the input directly, otherwise get
      // from output map
      std::vector<fl::Variable> link_input;
      if (link.input_->GetId() == data_node_id_) {
        link_input = input;
      } else {
        auto module_ptr = std::dynamic_pointer_cast<fl::Module>(link.input_);
        // check must be done as maps allow nullptr insertion
        if (module_ptr == nullptr) {
          throw std::runtime_error("Network links are invalid.");
        }
        link_input = output_maps.at(module_ptr);
      }

      // throws an exception if any dimensions do not match
      module_input = module_input.empty() ?
          link_input : Add(module_input, link_input);
    }
    auto module_output = module->forward(module_input);
    output_maps.insert({module, module_output});
  }

  // links to the loss node represent the final model output
  std::vector<fl::Variable> output;
  for (const auto& link : links_) {
    if (link.output_->GetId() == loss_node_id_) {
      auto module_ptr = std::dynamic_pointer_cast<fl::Module>(link.input_);
      if (module_ptr == nullptr) {
        throw std::runtime_error("Network container modules are invalid.");
      }
      auto link_output = output_maps.at(module_ptr);
      // will throw an exception if dimensions are a mismatch
      output = output.empty() ? link_output : Add(output, link_output);
    }
  }

  if (input.size() != output.size()) {
    throw std::runtime_error("Unexpected container output size.");
  }
  return output;
}

fl::Variable NetworkContainer::forward(const fl::Variable& input) {
  std::vector<fl::Variable> output = {input};
  output = forward(output);
  return output.front();
}

fl::Variable NetworkContainer::operator()(const fl::Variable& input) {
  return forward(input);
}

std::string NetworkContainer::prettyString() const {
  std::ostringstream output;
  output << "Network:" << std::endl;
  output << "[";
  bool first_link = true;
  for (const auto& link : links_) {

    if (!first_link) {
      output << ", ";
    } else {
      first_link = false;
    }

    if (link.input_->GetId() == data_node_id_) {
      output << "input";
    } else {
      output << "(" << link.input_->GetId() << ")";
    }
    output << " -> ";
    if (link.output_->GetId() == loss_node_id_) {
      output << "output";
    } else {
      output << "(" << link.output_->GetId() << ")";
    }
  }
  output << "]" << std::endl;

  for (const auto& module : modules_) {
    output << "("
           << std::dynamic_pointer_cast<ModuleNode>(module)->GetId() << "): ";
    output << module->prettyString() << std::endl;
  }
  return output.str();
}

}  // namespace neurons