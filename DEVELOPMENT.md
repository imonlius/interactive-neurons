# Development

---

2020-04-16: Added flashlight machine learning library.

2020-04-18: Implemented neuron Network classes (Node, Link, Network).

2020-04-19: Finished integrating imgui-node-editor into project as CinderBlock.

2020-04-21: Implemented Adapter classes (NodeAdapter, LinkAdapter) 
for imgui to interface with neuron Network. Included example code from
imgui-node-editor for demonstration.

2020-04-23: Switched graph library from imgui-node-editor to imnodes. Implemented
node/link adding and deleting. Fixed memory management of flashlight modules in Nodes.

2020-04-24: Implemented node adding configuration and activation function nodes.

2020-04-28: Added more Node types to the editor. 

2020-04-29: Refactored Node as an abstract class with DataNode and ModuleNode subclasses.

2020-04-30: Added MNIST data loading methods.

2020-05-02: Added utility graph methods (ContainsDirectedGraph,
CountConnectedComponents, AreNodeInputsSatisfied, NodesAndLinksConsistent,
ContainsTopologicalCycle) for use when building flashlight model from network.

2020-05-03: Implemented neurons::NetworkContainer class, which is an fl::Container
generated from a neurons::Network.

2020-05-05: Implemented model training and ImGui log display.

TODO: 
1. Investigate training statistics. Issue: loss is reporting negative values.