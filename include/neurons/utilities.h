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

// Returns whether the graph constructed by the passed links contains
// a non-trivial (# nodes > 1) strong component, where a strong or
// strongly connected component is a component in which there is
// a path from every vertex to every other vertex, i.e. a loop.
bool ContainsNonTrivialStrongComponent(const NodeDeque& nodes,
    const std::deque<Link>& links);

// Returns the number of connected components in the graph.
size_t CountConnectedComponents(const NodeDeque& nodes,
                                const std::deque<Link>& links);

// Returns whether every node that requires an input has an input link.
bool AreNodeInputsSatisfied(const NodeDeque& nodes,
                            const std::deque<Link>& links);


}  // namespace neurons::utilities

#endif  // FINALPROJECT_NEURONS_UTILITIES_H_
