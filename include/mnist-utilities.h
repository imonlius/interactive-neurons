#ifndef FINALPROJECT_MNIST_UTILITIES_H_
#define FINALPROJECT_MNIST_UTILITIES_H_

#include <flashlight/flashlight.h>

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

size_t read_int(std::ifstream& f);

template <typename T>
af::array load_data(const std::string& im_file,
    const std::vector<long long int>& dims);

std::pair<af::array, af::array> load_dataset(const std::string& data_dir,
    bool test /* = false */);

}  // namespace neurons::mnist_utilities

#endif // FINALPROJECT_NEURONS_MNIST_UTILITIES_H_