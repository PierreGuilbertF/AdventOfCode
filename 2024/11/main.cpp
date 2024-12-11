#include <cmath>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <vector>

#define INTERGER_TYPE uint64_t

INTERGER_TYPE
CountRocks(const std::map<INTERGER_TYPE, INTERGER_TYPE> &rock_to_count) {
  INTERGER_TYPE num_rocks = 0;
  for (const auto &rock : rock_to_count) {
    num_rocks += rock.second;
  }
  return num_rocks;
}

void RunBlink(std::map<INTERGER_TYPE, INTERGER_TYPE> &rock_to_count) {
  std::map<INTERGER_TYPE, INTERGER_TYPE> new_rock_to_count;
  for (const auto &rock : rock_to_count) {
    const INTERGER_TYPE rock_value = rock.first;
    const INTERGER_TYPE rock_count = rock.second;
    if (rock_value == 0) {
      if (new_rock_to_count.count(1) == 0)
        new_rock_to_count[1] = 0;
      new_rock_to_count.at(1) += rock_count;
    } else {
      const std::string string_rock_value = std::to_string(rock_value);
      if (string_rock_value.size() % 2 != 0) {
        if (new_rock_to_count.count(2024 * rock_value) == 0)
          new_rock_to_count[2024 * rock_value] = 0;
        new_rock_to_count.at(2024 * rock_value) += rock_count;
      } else {
        std::string num1, num2;
        for (int i = 0; i < string_rock_value.size() / 2; ++i) {
          num1.push_back(string_rock_value[i]);
          num2.push_back(string_rock_value[string_rock_value.size() / 2 + i]);
        }
        INTERGER_TYPE num1_int;
        INTERGER_TYPE num2_int;
        try {
          num1_int = std::stoi(num1);
          num2_int = std::stoi(num2);
        } catch (...) {
          std::cout << "num: " << num1 << " or: " << num2
                    << " coming from: " << string_rock_value
                    << " could not be splitted" << std::endl;
        }

        if (new_rock_to_count.count(num1_int) == 0)
          new_rock_to_count[num1_int] = 0;
        if (new_rock_to_count.count(num2_int) == 0)
          new_rock_to_count[num2_int] = 0;
        new_rock_to_count.at(num1_int) += rock_count;
        new_rock_to_count.at(num2_int) += rock_count;
      }
    }
  }
  rock_to_count = new_rock_to_count;
}

void BlinkEyes(std::map<INTERGER_TYPE, INTERGER_TYPE> &rock_to_count,
               int num_blink) {
  for (int k = 0; k < num_blink; ++k) {
    RunBlink(rock_to_count);
  }
}

int main(int argc, char **argv) {
  // Read input file
  const std::string filename = "11/input.txt";
  std::ifstream input_file(filename);
  if (!input_file.is_open()) {
    std::cerr << "Could not read file: " << filename << std::endl;
    return EXIT_FAILURE;
  }

  // Parse input
  std::map<INTERGER_TYPE, INTERGER_TYPE> rock_to_count;
  std::string line;
  while (std::getline(input_file, line)) {
    std::stringstream line_data(line);
    std::istringstream iss(line_data.str());

    INTERGER_TYPE number;
    while (iss >> number) {
      if (rock_to_count.count(number) == 0) {
        rock_to_count[number] = 1;
      } else {
        rock_to_count.at(number)++;
      }
    }
  }
  input_file.close();

  const int num_blinks = 75;
  BlinkEyes(rock_to_count, num_blinks);
  const uint64_t num_rocks = CountRocks(rock_to_count);
  std::cout << "We have: " << num_rocks << " after: " << num_blinks << " blinks"
            << std::endl;

  return EXIT_SUCCESS;
}