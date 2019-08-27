// Copyright 2016 Open Source Robotics Foundation, Inc.
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

#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <random>
#include <cstdlib>

using namespace std::chrono_literals;

/* This example creates a subclass of Node and uses std::bind() to register a
 * member function as a callback from the timer. */

class MinimalPublisher : public rclcpp::Node
{
public:
  MinimalPublisher()
  : Node("kyle_publisher"), count_(0)
  {
    publisher_ = this->create_publisher<std_msgs::msg::String>("topic");
    timer_ = this->create_wall_timer(
      500ms, std::bind(&MinimalPublisher::timer_callback, this));
  }

private:

  std::string get_word(int limit, std::string filename)
  {
    std::string path;
    const char* env_p = std::getenv("SNAP_DATA");
    if ((env_p != NULL) && (env_p[0] == '\0'))
    { 
      path = std::string(env_p);
    }
    else
    {
      path = "/tmp/";
    }
    std::string adj;
    std::ifstream infile(path + filename);
    if (infile.fail())	
    {
	std::cout << "can't find file." << std::endl;
    }
    int idx = 0;
    for( std::string line; getline( infile, line ); )
    {
      idx++;
      if (idx < limit)
      {
        continue;
      }
      adj = line;
      std::cout << "in adj " << adj << std::endl;
      break;
    }
    return adj;
}

  void timer_callback()
  {
    auto message = std_msgs::msg::String();
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 eng(rd()); // seed the generator
    std::uniform_int_distribution<> distr(1, 55); // define the range

    std::string adj = get_word( distr(eng), "adj" );
    std::string noun = get_word( distr(eng), "noun" );
    message.data = adj + " " + noun;
    RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
    publisher_->publish(message);
  }
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  size_t count_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalPublisher>());
  rclcpp::shutdown();
  return 0;
}
