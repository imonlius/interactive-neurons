#ifndef FINALPROJECT_MNIST_UTILITIES_H_
#define FINALPROJECT_MNIST_UTILITIES_H_

#include <flashlight/flashlight.h>
#include <neurons/data-node.h>

#include "neurons/network-container.h"

namespace neurons::mnist_utilities {

// MNIST Data loading functions below.
// All functions from MNIST flashlight example:
// https://github.com/facebookresearch/flashlight/blob/master/examples/Mnist.cpp
// using the byte-form MNIST datasets: http://yann.lecun.com/exdb/mnist/

const int kTrainSize = 60000;
const int kValSize = 5000; /* Held-out from train. */
const int kTestSize = 10000;
const int kImDim = 28;
const int kPixelMax = 255;
const int kInputIdx = 0;
const int kTargetIdx = 1;

// Load data from MNIST files from data_dir.
std::pair<af::array, af::array> load_dataset(const std::string& data_dir,
    bool test = false);

// Return a pair of categorical cross entropy loss and
// error for the model evaluated on the passed dataset.
std::pair<double, double> eval_loop(neurons::NetworkContainer& model,
    fl::Dataset& dataset);

// Train the passed model with the passed data node with the optimizer
// for the passed number of epochs. Uses categorical cross entropy loss and
// writes model training log to output. Sets training to whether training
// is still in progress (allows for checking in multi-threaded programs).
// If training becomes false during run, it will stop the process.
// Writes any exceptions that happens to exception_ptr.
void train_model(neurons::NetworkContainer& model, neurons::DataNode& data,
    fl::FirstOrderOptimizer& optimizer, int epochs, std::ostream& output,
    bool& training, std::exception_ptr& exception_ptr);

}  // namespace neurons::mnist_utilities

#endif // FINALPROJECT_NEURONS_MNIST_UTILITIES_H_