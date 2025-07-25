/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <cstdint>
#include "velox/common/base/Exceptions.h"
#include "velox/common/base/IOUtils.h"

namespace facebook::velox::functions::aggregate {

class NoisySumAccumulator {
 public:
  NoisySumAccumulator(double sum, double noiseScale)
      : sum_{sum}, noiseScale_{noiseScale} {}

  NoisySumAccumulator() = default;

  void checkAndSetNoiseScale(double noiseScale) {
    VELOX_USER_CHECK_GE(
        noiseScale, 0.0, "Noise scale must be non-negative value.");
    this->noiseScale_ = noiseScale;
  }

  // This function is used to update the sum
  void update(double value) {
    this->sum_ += value;
  }

  double getSum() const {
    return this->sum_;
  }

  double getNoiseScale() const {
    return this->noiseScale_;
  }

  static size_t serializedSize() {
    return sizeof(double) + sizeof(double);
  }

  void serialize(char* buffer) {
    common::OutputByteStream stream(buffer);
    stream.appendOne(sum_);
    stream.appendOne(noiseScale_);
  }

  static NoisySumAccumulator deserialize(const char* intermediate) {
    common::InputByteStream stream(intermediate);
    auto sum = stream.read<double>();
    auto noiseScale = stream.read<double>();

    return NoisySumAccumulator{sum, noiseScale};
  }

 private:
  double sum_{0.0};
  // Initial noise scale is an invalid noise scale,
  // indicating that we have not updated it yet
  double noiseScale_{-1.0};
};

} // namespace facebook::velox::functions::aggregate
