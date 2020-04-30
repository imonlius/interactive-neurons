#include "mnist-utilities.h"

// MNIST Data loading functions below.
// All functions from MNIST flashlight example:
// https://github.com/facebookresearch/flashlight/blob/master/examples/Mnist.cpp

namespace neurons::mnist_utilities {

size_t read_int(std::ifstream& f) {
  size_t d = 0;
  size_t c;
  for (size_t i = 0; i < sizeof(int); i++) {
    c = 0;
    f.read((char*)&c, 1);
    d |= (c << (8 * (sizeof(int) - i - 1)));
  }
  return d;
}

template <typename T>
af::array load_data(const std::string& im_file,
    const std::vector<long long int>& dims) {
  std::ifstream file(im_file, std::ios::binary);
  if (!file.is_open()) {
    throw af::exception("[mnist:load_data] Can't find MNIST file.");
  }
  read_int(file); // unused magic
  size_t elems = 1;
  for (auto d : dims) {
    size_t read_d = read_int(file);
    elems *= read_d;
    if (read_d != d) {
      throw af::exception("[mnist:load_data] Unexpected MNIST dimension.");
    }
  }

  std::vector<T> data;
  data.reserve(elems);
  for (size_t i = 0; i < elems; i++) {
    unsigned char tmp;
    file.read((char*)&tmp, sizeof(tmp));
    data.push_back(tmp);
  }

  std::vector<long long int> rdims(dims.rbegin(), dims.rend());
  // af is column-major
  af::dim4 af_dims(rdims.size(), rdims.data());
  return af::array(af_dims, data.data());
}

std::pair<af::array, af::array> load_dataset(
    const std::string& data_dir,
    bool test /* = false */) {
  std::string f = test ? "t10k" : "train";
  int size = test ? kTestSize : kTrainSize;

  std::string image_file = data_dir + "/" + f + "-images-idx3-ubyte";
  af::array ims = load_data<float>(image_file, {size, kImDim, kImDim});
  ims = moddims(ims, {kImDim, kImDim, 1, size});
  // Rescale to [-0.5,  0.5]
  ims = (ims - kPixelMax / 2) / kPixelMax;

  std::string label_file = data_dir + "/" + f + "-labels-idx1-ubyte";
  af::array labels = load_data<int>(label_file, {size});

  return std::make_pair(ims, labels);
}

}  // namespace neurons::mnist_utilities