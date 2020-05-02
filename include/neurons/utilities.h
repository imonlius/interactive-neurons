// Copyright (c) 2020 Simon Liu. All rights reserved.
#ifndef FINALPROJECT_NEURONS_UTILITIES_H_
#define FINALPROJECT_NEURONS_UTILITIES_H_

#include "link.h"
#include "node.h"

namespace neurons::utilities {

// Returns whether node IDs and link IDs are all unique
// and link input and outputs correspond to nodes in the passed NodeDeque.
bool NodesAndLinksConsistent(const NodeDeque& nodes,
    const std::deque<Link>& links);

// Returns a NodeDeque with nodes sorted topologically, i.e.
// for every pair of distinct nodes a and b, if a serves as the input for
// b, then a will appear before b in the returned NodeDeque.
// Nodes that belong to directed cycles will have arbitrary relative ordering.
NodeDeque TopologicalSort(const NodeDeque& nodes,
    const std::deque<Link>& links);

// Returns whether the graph constructed by the passed links contains
// a directed cycle.
bool ContainsDirectedCycle(const NodeDeque& nodes,
    const std::deque<Link>& links);

// Returns the number of connected components in the graph.
size_t CountConnectedComponents(const NodeDeque& nodes,
                                const std::deque<Link>& links);

// Returns whether every node that requires an input has an input link.
bool AreNodeInputsSatisfied(const NodeDeque& nodes,
                            const std::deque<Link>& links);


}  // namespace neurons::utilities

#endif  // FINALPROJECT_NEURONS_UTILITIES_H_
