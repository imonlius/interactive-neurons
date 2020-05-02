// Copyright (c) 2020 Simon Liu. All rights reserved.

#include "neurons/utilities.h"

namespace neurons::utilities {

// Get index of element in deque, returns size of deque if not in deque
template <typename T>
size_t GetIndex(const std::deque<T>& container, const T& element) {
  // std::distance will always be non-negative as we start from
  // container.begin()
  return static_cast<size_t>(std::distance(container.begin(),
                    std::find(container.begin(), container.end(), element)));
}

// Get all links leading to or from the passed node
std::deque<Link> GetNodeLinks(const std::shared_ptr<Node>& node,
    const std::deque<Link>& links) {
  std::deque<Link> adjacent_links;
  for (const auto& link: links) {
    if (node == link.input_ || node == link.output_) {
      adjacent_links.push_back(link);
    }
  }
  return adjacent_links;
}


bool NodesAndLinksConsistent(const NodeDeque& nodes,
                             const std::deque<Link>& links) {

  // check all nodes are not nullptr and IDs are unique
  std::set<size_t> element_ids;
  for (const auto& node : nodes) {
    if (node == nullptr) {
      return false;
    }
    // check if ID already exists in element_ids
    if (element_ids.find(node->GetId()) != element_ids.end()) {
      return false;
    }
    element_ids.insert(node->GetId());
  }

  for (const auto& link : links) {

    // we require that node IDs and link IDs are mutually unique
    if (element_ids.find(link.GetId()) != element_ids.end()) {
      return false;
    }
    element_ids.insert(link.GetId());

    if (std::find(nodes.begin(), nodes.end(), link.input_) == nodes.end() ||
        std::find(nodes.begin(), nodes.end(), link.output_) == nodes.end()) {
      return false;
    }
  }

  return true;
}

// Helper method for Kosaraju's algorithm
void Visit(const std::shared_ptr<Node>& node, std::vector<bool>& visited,
           NodeDeque& stack, const NodeDeque& nodes,
           const std::deque<Link>& links) {
  size_t index = GetIndex(nodes, node);
  if (!visited.at(index)) {
    // mark node as visited
    visited.at(index) = true;
    // visit each of its out-neighbors
    for (const auto& link : links) {
      if (link.input_ == node) {
        Visit(link.output_, visited, stack, nodes, links);
      }
    }
    // throw that node onto the stack
    stack.push_front(node);
  }
}

// Helper method for Kosaraju's algorithm
void Assign(const std::shared_ptr<Node>& node, const std::shared_ptr<Node>& root,
    std::vector<bool>& is_assigned, std::vector<size_t>& assignments,
    const NodeDeque& nodes, const std::deque<Link>& links) {

  size_t index = GetIndex(nodes, node);
  // if node has not been assigned, assign it to the same component as root
  if (!is_assigned.at(index)) {

    is_assigned.at(index) = true;

    size_t root_index = GetIndex(nodes, root);
    assignments.at(root_index) += 1;

    // assign every input neighbor of the node to the root's component as well
    for (const auto& link : links) {
      if (link.output_ == node) {
        Assign(link.input_, root, is_assigned, assignments, nodes, links);
      }
    }
  }

}

// Uses implementation of Kosaraju's algorithm derived from:
// https://en.wikipedia.org/wiki/Kosaraju's_algorithm
NodeDeque TopologicalSort(const NodeDeque& nodes,
                          const std::deque<Link>& links) {
  // nodes are identified by index, as we don't trust node IDs
  auto visited = std::vector<bool>(nodes.size(), false);
  NodeDeque stack;

  for (const auto& node : nodes) {
    // will put all the nodes onto the stack in order of visitation
    // thus, the stack is ordered from input to output nodes
    Visit(node, visited, stack, nodes, links);
  }

  return stack;
}

// Uses Kosaraju's algorithm with implementation derived from:
// https://en.wikipedia.org/wiki/Kosaraju's_algorithm
bool ContainsDirectedCycle(const NodeDeque& nodes,
    const std::deque<Link>& links) {

  // topologically sorted list (nodes that belong to the same
  // strong component have arbitrary relative ordering)
  NodeDeque stack = TopologicalSort(nodes, links);

  // each position with a non-zero value corresponds to a component
  // the value is the number of nodes in the component
  auto assignments = std::vector<size_t>(nodes.size(), 0);
  auto is_assigned = std::vector<bool>(nodes.size(), false);

  for (const auto& node : stack) {
    // aggregate all the nodes to maximal components
    Assign(node, node, is_assigned, assignments, nodes, links);
  }

  // check if there are strong components
  // with multiple nodes (i.e. a directed cycle)
  for (auto assignment : assignments) {
    if (assignment != 1) {
      return true;
    }
  }
  return false;
}

// Algorithm (implementation detail):
// S: Set of connected components = { }
// For each node N in the set A, find all the nodes reachable from N. If any of
// those nodes are already in a component of S, then continue to next node in
// A (N is part of a previously discovered component). Otherwise, add this
// set to S. Returns the number of elements in S.
size_t CountConnectedComponents(const NodeDeque& nodes,
                                const std::deque<Link>& links) {

  size_t connected_components = 0;

  // keeps track of whether search has visited a node
  // nodes are identified by their index, as we don't trust node IDs
  std::vector<bool> visited(nodes.size(), false);

  for (size_t node = 0; node < nodes.size(); ++node) {

    if (visited.at(node)) {
      continue;
    }
    // if node is unvisited, is not in a previously found connected component
    ++connected_components;
    // queue of nodes that are connected to node
    auto node_queue = std::queue<size_t>();
    node_queue.push(node);
    visited[node] = true;

    // while there are still unvisited nodes connected to node
    while (!node_queue.empty()) {
      size_t current_node = node_queue.front();
      node_queue.pop();
      // traverse all neighboring nodes and see if they've been visited
      for (const auto& link : GetNodeLinks(nodes.at(current_node), links)) {
        size_t input_index = GetIndex(nodes, link.input_);
        size_t output_index = GetIndex(nodes, link.output_);

        // if not visited, then they are not part of a prev. component
        if (!visited.at(input_index)) {
          visited.at(input_index) = true;
          node_queue.push(input_index);
        }

        if (!visited.at(output_index)) {
          visited.at(output_index) = true;
          node_queue.push(output_index);
        }
      }
    }
  }
  return connected_components;
}

// Returns whether every node that requires an input has an input link.
bool AreNodeInputsSatisfied(const NodeDeque& nodes,
    const std::deque<Link>& links) {
  NodeDeque nodes_copy(nodes);

  // go through every link to eliminate nodes that have a link into them
  for (const auto& link : links) {
    auto it = nodes_copy.begin();
    while (it != nodes_copy.end()) {
      // this does not allow nodes to be satisfy themselves as their own input
      if (*it == link.output_ && *it != link.input_) {
        it = nodes_copy.erase(it);
      } else {
        ++it;
      }
    }
  }

  // remaining nodes in nodes_copy do not have a link leading to them
  // if they are not data nodes (which do not have input), they are unsatisfied
  for (const auto& node : nodes_copy) {
    if (node->GetNodeType() != Dataset) {
      return false;
    }
  }
  return true;
}

}  // namespace neurons::utilities