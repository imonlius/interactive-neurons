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

TODO: 
1. Add Dataloader class (with DataNode counterpart).
2. Add graph algorithms for network checking. 
3. Implement machine learning part.
4. Add graph display of machine learning loss.