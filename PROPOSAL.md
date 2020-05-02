# interactive-neurons

**Author: Simon Liu**

---

#### What kind of project you want to do (obviously)

My project goal is to build an interactive GUI that allows users to design and 
train simple convolutional neural networks. Using the GUI, users can tweak the
network architecture, including features such as the number of neurons, layers, 
connections, and activation functions. Users can also customize training 
parameters such as the data split ratio, learning rate, training time, and optimizers. 

Through this project, the overall goal is to allow users to play around and 
experiment with neural network architectures to see what impact certain design 
changes have on the training process and final accuracy.

#### What background knowledge do you have on this kind of project, and why do you want to do this project? *(Put another way, in what way will you be motivated to see this project to completion?)*

I've done some reading on neural networks, specifically CNNs. 
I want to do this project because I want to deepen my understanding of how these 
networks operate and what their training procedures are. I also want to gain more 
experience in working with graphical interfaces, as many of our previous projects 
have been based on the command line. 

#### What specific external libraries will you need? Provide some links (either in the paragraphs or as footnotes) to these libraries — and make sure these libraries are compatible with your operating system/IDE!
Cinder-ImGui - https://github.com/simongeilfus/Cinder-ImGui. 
*This library will be used to create the GUI with which the user interacts.*

flashlight - https://github.com/facebookresearch/flashlight. 
*This library will be used to train the network.*

#### A rough timeline of what will get accomplished and when (*e.g, "By the end of Week 2, I will have implemented XYZ and will start working on ABC"*)
* Week 1: Start working on the GUI - specifically create the design menus for 
users to add neuron nodes, layers, connections, and customize their properties
 through property menus. 
* Week 2: Finish working on the GUI and start implementing a design validator, 
which checks that the user's created design is a valid network architecture. 
If I finish this, start implementing model training using the flashlight library.
* Week 3: Finish implementing model training using the flashlight library. 
If I finish this, start the "stretch goals."

#### If you were to finish the project early, what extra "stretch goals" could you implement?
- [ ] Add loss graphs during training
- [ ] Add support for serializing and deserializing models 
- [ ] Add support for custom image datasets specified by the user
- [ ] Add support to export model figure as image