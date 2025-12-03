#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

const int modulo_sum = 100 * 1000;

void PrintInstructions(
    const std::vector<std::pair<char, unsigned int>> &instructions) {
  for (const auto &instruction : instructions) {
    std::cout << "Instruction: " << std::string(1, instruction.first) << ", "
              << instruction.second << std::endl;
  }
}

int main(int argc, char **argv) {
  // Read the file
  std::ifstream input_file("1/input.txt");
  if (!input_file.is_open()) {
    std::cerr << "Could not open file" << std::endl;
    return EXIT_FAILURE;
  }

  // Load the instructions
  std::vector<std::pair<char, unsigned int>> instructions;
  std::string line;
  while (std::getline(input_file, line)) {
    const char direction = line[0];
    const unsigned int value =
        static_cast<unsigned int>(std::stoul(line.substr(1)));
    instructions.push_back(std::make_pair(direction, value));
  }
  input_file.close();

  // Solving puzzle
  int dial_position = 50;
  int number_left_at_zero = 0;
  int number_passing_at_zero = 0;
  for (const auto &instruction : instructions) {
    int sign = (instruction.first == 'L') ? -1 : 1;
    dial_position += (sign * instruction.second);
    if (dial_position > 99) {
      while (dial_position > 99) {
        dial_position -= 100;
        number_passing_at_zero++;
      }
    }
    if (dial_position < 0) {
      while (dial_position < 0) {
        dial_position += 100;
        number_passing_at_zero++;
      }
    }
    if (dial_position == 0)
      number_left_at_zero++;
  }

  std::cout << "Number of times dial was left at zero: " << number_left_at_zero
            << std::endl;
  std::cout << "Number of times dial clicked at zero: "
            << number_passing_at_zero << std::endl;
  return EXIT_SUCCESS;
}