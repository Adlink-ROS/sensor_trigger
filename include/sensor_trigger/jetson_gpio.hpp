// Copyright 2022 Tier IV, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SENSOR_TRIGGER__JETSON_GPIO_HPP_
#define SENSOR_TRIGGER__JETSON_GPIO_HPP_

#include <fcntl.h>
#include <unistd.h>

#include <map>
#include <string>

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define BUFFER_SIZE 64
#define GPIO_OUTPUT 1
#define GPIO_INPUT 0
#define GPIO_HIGH 1
#define GPIO_LOW 0

typedef int gpio_direction;
typedef int gpio_state;

namespace jetson_gpio
{
// Mapping of GPIO number to pin number for RQX-59G
// Fsync N: <first, second>
// Fsync 0: <440, PP.00>
// Fsync 1: <397, PH.06>
// Fsync 2: <487, PAC.01>
// Fsync 3: <486, PAC.00>
static std::map<int, std::pair<int, std::string>> pin_gpio_mapping{{0, {440, "PP.00"}}, {1, {397, "PH.06"}}, {2, {487, "PAC.01"}}, {3, {486, "PAC.00"}}};

class JetsonGpio
{
public:
  JetsonGpio() : state_file_descriptor_(-1) {}
  ~JetsonGpio();
  bool init_gpio_pin(int fsync_index, gpio_direction direction);
  bool set_gpio_pin_state(gpio_state state);

protected:
  bool export_gpio();
  bool unexport_gpio();
  bool set_gpio_direction(gpio_direction direction);

  int state_file_descriptor_;
  std::pair<int, std::string> fsync_gpio_;
};
}  // namespace jetson_gpio
#endif  // SENSOR_TRIGGER__JETSON_GPIO_HPP_
