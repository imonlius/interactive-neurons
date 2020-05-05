#include "mnist-utilities.h"

#include <iomanip>

// MNIST-specific dataloading and training functions below.
// All methods from this file are derived from MNIST flashlight example:
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

std::pair<double, double> eval_loop(neurons::NetworkContainer& model,
    fl::Dataset& dataset) {

  fl::AverageValueMeter loss_meter;
  fl::FrameErrorMeter error_meter;

  // place model into evaluation mode
  model.eval();

  for (auto& example: dataset) {
    // for MNIST dataset
    auto inputs = fl::noGrad(example.at(mnist_utilities::kInputIdx));
    auto target = fl::noGrad(example.at(mnist_utilities::kTargetIdx));

    auto output = model(inputs);

    // retrieve the prediction from the output (maximum values)
    af::array max_vals, max_ids;
    af::max(max_vals, max_ids, output.array(), 0);

    // record prediction error
    error_meter.add(af::reorder(max_ids, 1, 0), target.array());

    // record loss
    loss_meter.add(
        fl::categoricalCrossEntropy(output, target).array().scalar<float>());
  }

  // back to training mode
  model.train();

  double error = error_meter.value();
  double loss = loss_meter.value().at(0);

  return std::make_pair(loss, error);
}

void train_model_inner(neurons::NetworkContainer& model, neurons::DataNode& data,
                       fl::FirstOrderOptimizer& optimizer,
                       int epochs, std::ostream& output, bool& training) {

  for (int epoch = 0; epoch < epochs; ++epoch) {

    fl::AverageValueMeter train_loss_meter;

    for (auto& example : *(data.train_dataset_)) {
      // if training has been halted, immediate return.
      if (!training) {
        return;
      }

      auto inputs = fl::noGrad(example.at(mnist_utilities::kInputIdx));
      auto targets = fl::noGrad(example.at(mnist_utilities::kTargetIdx));

      auto outputs = model(inputs);

      // compute loss
      auto loss = fl::categoricalCrossEntropy(outputs, targets);
      train_loss_meter.add(loss.array().scalar<float>());

      // backprop, update weights, then zero gradients
      loss.backward();
      optimizer.step();
      optimizer.zeroGrad();
    }

    double train_loss = train_loss_meter.value().at(0);

    // evaluate on validation set
    double val_loss, val_error;
    std::tie(val_loss, val_error) = eval_loop(model, *data.valid_dataset_);

    output << "Epoch " << epoch << std::setprecision(3)
           << ": Avg Train Loss: " << train_loss
           << " Validation Loss: " << val_loss
           << " Validation Error (%): " << val_error << std::endl;
  }

  // report test loss and error
  double test_loss, test_error;
  std::tie(test_loss, test_error) = eval_loop(model, *data.test_dataset_);
  output << "Test Loss: " << test_loss
         << " Test Error (%): " << test_error << std::endl;
}


// Wraps train_model_inner() call with a try-catch block to return any
// exceptions.
void train_model(neurons::NetworkContainer& model, neurons::DataNode& data,
                 fl::FirstOrderOptimizer& optimizer,
                 int epochs, std::ostream& output, bool& training,
                 std::exception_ptr& exception_ptr) {

  training = true;
  output << model.prettyString(); // print network before training

  try {
    train_model_inner(model, data, optimizer, epochs, output, training);
  } catch (std::exception& exception) {
    exception_ptr = std::current_exception();
  }

  // keep the training lock out of the try/catch so execution is guaranteed.
  training = false;

}

}  // namespace neurons::mnist_utilities