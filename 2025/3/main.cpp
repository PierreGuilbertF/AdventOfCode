#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

int BiggestJoltInBank(const std::string &bank) {
  int biggest_tenth = 0;
  int biggest_unit = 0;
  for (int k = 0; k < bank.size(); ++k) {
    const auto &digit = bank[k];
    int value = std::stoi(std::string(1, digit));
    if (value > biggest_tenth && k != bank.size() - 1) {
      biggest_tenth = value;
      biggest_unit = 0;
    } else if (value > biggest_unit) {
      biggest_unit = value;
    }
  }

  return biggest_tenth * 10 + biggest_unit;
}

unsigned long long BiggestJoltInBankForDigits(const std::string &bank,
                                              int digit_number) {
  std::vector<int> digits(digit_number, 0);
  for (int k = 0; k < bank.size(); ++k) {
    const auto &digit = bank[k];
    const int value = std::stoi(std::string(1, digit));
    for (int i = 0; i < digit_number; ++i) {
      int need_remaining_digit = digit_number - i - 1;
      if (value > digits[i] && k < bank.size() - need_remaining_digit) {
        digits[i] = value;
        for (int j = i + 1; j < digit_number; ++j) {
          digits[j] = 0;
        }
        break;
      }
    }
  }

  unsigned long long result = 0;
  unsigned long long exponent = 1;
  for (int k = 0; k < digits.size(); ++k) {
    result += digits[digits.size() - 1 - k] * exponent;
    exponent *= 10;
  }
  return result;
}

int main(int argc, char **argv) {
  // Read the file
  std::ifstream input_file("3/input.txt");
  if (!input_file.is_open()) {
    std::cerr << "Could not open file" << std::endl;
    return EXIT_FAILURE;
  }

  std::vector<std::string> banks;
  // Load the banks
  std::string line;
  while (std::getline(input_file, line)) {
    banks.push_back(line);
  }

  int sum_of_jolt = 0;
  unsigned long long sum_of_jolt_12 = 0;
  for (const auto &bank : banks) {
    std::cout << "bank: " << bank << std::endl;
    const int jolt = BiggestJoltInBank(bank);
    const unsigned long long jolt_12 = BiggestJoltInBankForDigits(bank, 12);
    sum_of_jolt += jolt;
    sum_of_jolt_12 += jolt_12;
    std::cout << "biggest jolt: " << jolt << std::endl << std::endl;
    std::cout << "biggest jolt: " << jolt_12 << std::endl << std::endl;
  }
  std::cout << "Total joltage: " << sum_of_jolt << std::endl;
  std::cout << "Total joltage: " << sum_of_jolt_12 << std::endl;

  return EXIT_SUCCESS;
}