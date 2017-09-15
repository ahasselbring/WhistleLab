/**
 * @file AHDetector.cpp implements methods of the AHDetector class
 */

#include <algorithm>
#include <cassert>
#include <cmath>
#include <complex>
#include <fstream>
#include <iostream>
#include <limits>

#include "AHDetector.hpp"


AHDetector::AHDetector()
  : realBuffer(bufferSize)
  , complexBuffer(bufferSize / 2 + 1)
  , amplitudeBuffer(bufferSize / 2 + 1)
  , fftPlan(fftw_plan_dft_r2c_1d(bufferSize, realBuffer.data(), reinterpret_cast<fftw_complex*>(complexBuffer.data()), FFTW_ESTIMATE))
  , training(false)
  , ann(nullptr)
{
  static_assert(bufferSize % 2 == 0, "The buffer size has to be even!");
  if (useNN)
  {
    ann = fann_create_from_file("../NeuralNetworks/AHDetector.net");
    if (ann == nullptr)
    {
      std::cerr << "AHDetector: Could not load neural network!\n";
    }
    else if (fann_get_num_input(ann) != numOfFeatures || fann_get_num_output(ann) != 1)
    {
      std::cerr << "AHDetector: Loaded neural network topology is not compatible!\n";
      fann_destroy(ann);
      ann = nullptr;
    }
    else
    {
      std::ifstream norm("../NeuralNetworks/AHDetector.norm");
      if (norm.is_open())
      {
        for (unsigned int i = 0; i < numOfFeatures; i++)
        {
          norm >> means[i] >> stddevs[i];
          if (!norm.good())
          {
            break;
          }
        }
        if (!norm.good())
        {
          std::cerr << "AHDetector: Could not load normalization parameters!\n";
          fann_destroy(ann);
          ann = nullptr;
        }
        norm.close();
      }
      else
      {
        std::cerr << "AHDetector: Could not load normalization parameters!\n";
        fann_destroy(ann);
        ann = nullptr;
      }
    }
  }
}

AHDetector::~AHDetector()
{
  if (useNN && ann != nullptr)
  {
    fann_save(ann, "../NeuralNetworks/AHDetector.net");
    fann_destroy(ann);
    ann = nullptr;

    std::ofstream norm("../NeuralNetworks/AHDetector.norm");
    if (norm.is_open())
    {
      for (unsigned int i = 0; i < numOfFeatures; i++)
      {
        norm << means[i] << ' ' << stddevs[i] << '\n';
      }
      norm.close();
    }
    else
    {
      std::cerr << "AHDetector: Could not save normalization parameters!\n";
    }
  }
  assert(ann == nullptr);
  fftw_destroy_plan(fftPlan);
}

void AHDetector::evaluate(EvaluationHandle& eh)
{
  if (useNN && !training && ann == nullptr)
  {
    return;
  }
  std::array<float, bufferSize> samples;
  const std::vector<std::complex<double>>& freqData = complexBuffer;
  const double freqResolution = static_cast<double>(bufferSize) / eh.getSampleRate();
  const unsigned int minFreqIndex = static_cast<unsigned int>(std::ceil(minFrequency * freqResolution));
  const unsigned int maxFreqIndex = static_cast<unsigned int>(std::ceil(maxFrequency * freqResolution));
  if (maxFreqIndex >= complexBuffer.size())
  {
    std::cerr << "AHDetector: maxFreqIndex " << maxFreqIndex << " is larger than the Nyquist frequency!\n";
    return;
  }

  while (eh.readSingleChannel(samples.data(), bufferSize) == bufferSize)
  {
    // 1. Perform discrete fourier (with Hann window) transform to obtain frequency spectrum.
    for (unsigned int i = 0; i < realBuffer.size(); i++)
    {
      realBuffer[i] = samples[i];
      realBuffer[i] *= std::pow(std::sin(static_cast<float>(M_PI) * static_cast<float>(i) / bufferSize), 2.0f);
    }
    fftw_execute(fftPlan);

    // 2. Precompute the absolute values of the spectrum (normalized by buffer size).
    for (unsigned int i = 0; i < freqData.size(); i++)
    {
      amplitudeBuffer[i] = std::abs(freqData[i]) / (bufferSize / 2);
    }

    // 3. Find the frequency at which the amplitude is highest in a configurable band.
    double maxAmplitude = minRequiredAmplitude;
    unsigned int maxAmplitudeFreqIndex = 0;
    for (unsigned int i = minFreqIndex; i < maxFreqIndex; i++)
    {
      const double amplitude = amplitudeBuffer[i];
      if (amplitude > maxAmplitude)
      {
        maxAmplitude = amplitude;
        maxAmplitudeFreqIndex = i;
      }
    }

    // 4. If a configurable amplitude has not been surpassed, the buffer is rejected.
    if (maxAmplitudeFreqIndex == 0)
    {
      continue;
    }

    // 5. Determine power in the range of the base frequency while detecting its boundaries.
    double whistlePower[2] = { amplitudeBuffer[maxAmplitudeFreqIndex], 0 };
    double stopBandPower[2] = { 0, 0 };
    const unsigned int i2 = (maxFreqIndex - minFreqIndex) / 2;
    const unsigned int lowerBoundLowerBound = std::max(maxAmplitudeFreqIndex - i2, 1U);
    const unsigned int upperBoundUpperBound = std::min(maxAmplitudeFreqIndex + i2, static_cast<unsigned int>(freqData.size()));
    unsigned int upperBound, lowerBound;
    unsigned int lowerBoundAtMinLowerPower = 0, upperBoundAtMinUpperPower = 0;
    double minLowerPower = std::numeric_limits<double>::max(), minUpperPower = std::numeric_limits<double>::max();
    double whistlePowerAtMinLowerPower = 0.f, whistlePowerAtMinUpperPower = 0.f;
    for (lowerBound = maxAmplitudeFreqIndex - 1; lowerBound > lowerBoundLowerBound; lowerBound--)
    {
      whistlePower[0] += amplitudeBuffer[lowerBound];
      if (amplitudeBuffer[lowerBound] < maxAmplitude * minAmplitudeOverMaxAmplitude)
      {
        break;
      }
      if (amplitudeBuffer[lowerBound] < minLowerPower)
      {
        minLowerPower = amplitudeBuffer[lowerBound];
        lowerBoundAtMinLowerPower = lowerBound;
        whistlePowerAtMinLowerPower = whistlePower[0];
      }
    }
    if (lowerBound == lowerBoundLowerBound)
    {
      lowerBound = lowerBoundAtMinLowerPower;
      whistlePower[0] = whistlePowerAtMinLowerPower;
    }
    for (upperBound = maxAmplitudeFreqIndex + 1; upperBound < upperBoundUpperBound; upperBound++)
    {
      whistlePower[0] += amplitudeBuffer[upperBound];
      if (amplitudeBuffer[upperBound] < maxAmplitude * minAmplitudeOverMaxAmplitude)
      {
        break;
      }
      if (amplitudeBuffer[upperBound] < minUpperPower)
      {
        minUpperPower = amplitudeBuffer[upperBound];
        upperBoundAtMinUpperPower = upperBound;
        whistlePowerAtMinUpperPower = whistlePower[0];
      }
    }
    if (upperBound == upperBoundUpperBound)
    {
      upperBound = upperBoundAtMinUpperPower;
      whistlePower[0] = whistlePowerAtMinUpperPower;
    }
    assert(upperBound > lowerBound);

    // 6. Determine power in the rest the second harmonic band and in between and above.
    const int minFreqIndex2 = 2 * maxAmplitudeFreqIndex - (upperBound - lowerBound) / 4;
    const int maxFreqIndex2 = 2 * maxAmplitudeFreqIndex + (upperBound - lowerBound) / 4;
    assert(minFreqIndex2 >= 0 && maxFreqIndex2 <= static_cast<int>(freqData.size()));
    for (int i = upperBound; i < minFreqIndex2; i++)
    {
      stopBandPower[0] += amplitudeBuffer[i];
    }
    for (int i = minFreqIndex2; i < maxFreqIndex2; i++)
    {
      whistlePower[1] += amplitudeBuffer[i];
    }
    for (int i = maxFreqIndex2; i < static_cast<int>(freqData.size()); i++)
    {
      stopBandPower[1] += amplitudeBuffer[i];
    }

    // 7. Normalize the power to their ranges.
    const double whistleBandRange = std::max(upperBound - lowerBound, 1U);
    const double stopBandRange = std::max(minFreqIndex2 - upperBound, 1U);
    const double whistleBandRange2 = std::max(maxFreqIndex2 - minFreqIndex2, 1);
    const double stopBandRange2 = std::max(static_cast<int>(freqData.size()) - maxFreqIndex2, 1);
    whistlePower[0] /= whistleBandRange;
    whistlePower[1] /= whistleBandRange2;
    stopBandPower[0] /= stopBandRange;
    stopBandPower[1] /= stopBandRange2;

    // 8. Compute feature vector.
    FeatureVector features;
    features[0] = maxAmplitude;
    features[1] = whistlePower[0] / stopBandPower[0];
    features[2] = whistlePower[1] / stopBandPower[0];
    features[3] = (whistlePower[0] + whistlePower[1]) / (stopBandPower[0] + stopBandPower[1]);

    // 8b. During training, store the feature vector including its annotated result.
    if (training)
    {
      const int label = eh.insideWhistle(-static_cast<int>(bufferSize) / 2);
      trainingExamples.emplace_back();
      TrainingExample& te = trainingExamples.back();
      te.input = features;
      te.output = (label != 0);
      continue;
    }

    // 9. Run classifier.
    const bool isWhistle = useNN ? classifyNN(features) : classifyJ48(features);
    if (isWhistle)
    {
      eh.report(-static_cast<int>(bufferSize) / 2);
    }
  }
}

void AHDetector::trainOnDatabase(const SampleDatabase& db)
{
  // 1. Evaluate this detector in training mode.
  training = true;
  evaluateOnDatabase(db);
  training = false;

  // 2. Call the classifier-specific training method.
  if (useNN)
  {
    trainNN();
  }
  else
  {
    trainJ48();
  }

  // 3. Clear the collected training examples.
  trainingExamples.clear();
}

bool AHDetector::classifyJ48(const FeatureVector& features) const
{
  if (features[0] <= 1079858)
  {
    if (features[3] <= 332.149 && features[2] > 2.99929)
    {
      return true;
    }
  }
  else
  {
    return true;
  }
  return false;
}

bool AHDetector::classifyNN(const FeatureVector& features) const
{
  assert(ann != nullptr);
  fann_type input[numOfFeatures];
  for (unsigned int i = 0; i < numOfFeatures; i++)
  {
    input[i] = static_cast<float>((features[i] - means[i]) / stddevs[i]);
  }
  fann_type* output = fann_run(ann, input);
  return *output > 0.5f;
}

void AHDetector::trainJ48()
{
  std::ofstream csv("../DecisionTrees/AHDetector.csv");
  if (!csv.is_open())
  {
    std::cerr << "AHDetector: Could not open CSV file for writing!\n";
    return;
  }
  for (unsigned int i = 0; i < numOfFeatures; i++)
  {
    csv << "feature" << i << ',';
  }
  csv << "whistle\n";
  for (auto& te : trainingExamples)
  {
    for (unsigned int i = 0; i < numOfFeatures; i++)
    {
      csv << te.input[i] << ',';
    }
    csv << (te.output ? "YES" : "NO") << '\n';
  }
  csv.close();

  std::ofstream names("../DecisionTrees/AHDetector.names");
  if (!names.is_open())
  {
    std::cerr << "AHDetector: Could not open C5.0 names file for writing!\n";
    return;
  }
  names << "whistle.\n";
  for (unsigned int i = 0; i < numOfFeatures; i++)
  {
    names << "feature" << i << ": continuous.\n";
  }
  names << "whistle: YES, NO.\n";
  names.close();

  std::ofstream data("../DecisionTrees/AHDetector.data");
  if (!data.is_open())
  {
    std::cerr << "AHDetector: Could not open C5.0 data file for writing!\n";
    return;
  }
  for (auto& te : trainingExamples)
  {
    for (unsigned int i = 0; i < numOfFeatures; i++)
    {
      data << te.input[i] << ',';
    }
    data << (te.output ? "YES" : "NO") << '\n';
  }
  data.close();

  std::ofstream costs("../DecisionTrees/AHDetector.costs");
  if (!costs.is_open())
  {
    std::cerr << "AHDetector: Could not open C5.0 costs file for writing!\n";
    return;
  }
  costs << "YES, NO: 10\n";
  costs << "NO, YES: 1\n";
  costs.close();
}

void AHDetector::trainNN()
{
  // 1. Find out mean and standard deviation of the features.
  means.fill(0);
  stddevs.fill(0);
  for (auto& te : trainingExamples)
  {
    for (unsigned int i = 0; i < numOfFeatures; i++)
    {
      means[i] += te.input[i];
    }
  }
  for (unsigned int i = 0; i < numOfFeatures; i++)
  {
    means[i] /= static_cast<double>(trainingExamples.size());
  }
  for (auto& te : trainingExamples)
  {
    for (unsigned int i = 0; i < numOfFeatures; i++)
    {
      stddevs[i] += (te.input[i] - means[i]) * (te.input[i] - means[i]);
    }
  }
  for (unsigned int i = 0; i < numOfFeatures; i++)
  {
    stddevs[i] = std::sqrt(stddevs[i] / static_cast<double>(trainingExamples.size()));
  }

  // 2. Create / overwrite neural network.
  if (ann != nullptr)
  {
    fann_destroy(ann);
  }
  ann = fann_create_standard(3, numOfFeatures, 4, 1);
  if (ann == nullptr)
  {
    std::cerr << "AHDetector: Could not create new neural network!\n";
    return;
  }
  fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
  fann_set_activation_function_output(ann, FANN_SIGMOID);
  fann_randomize_weights(ann, -1.0f, 1.0f);

  // 3. Setup data and train the neural network.
  fann_train_data* data = fann_create_train(static_cast<unsigned int>(trainingExamples.size()), numOfFeatures, 1);
  if (data == nullptr)
  {
    std::cerr << "AHDetector: Could not create train data!\n";
    return;
  }
  for (unsigned int i = 0; i < trainingExamples.size(); i++)
  {
    for (unsigned int j = 0; j < numOfFeatures; j++)
    {
      data->input[i][j] = static_cast<float>((trainingExamples[i].input[j] - means[j]) / stddevs[j]);
    }
    data->output[i][0] = trainingExamples[i].output ? 1.0f : 0.0f;
  }
  fann_train_on_data(ann, data, 10000, 100, 0.0f);
  fann_destroy_train(data);
}
