#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

void LoadData(const std::string &line,
              std::vector<std::pair<std::string, std::string>> &id_ranges) {
  std::string first_id;
  std::string second_id;
  int mode = 0;
  for (int i = 0; i < line.size(); ++i) {
    if (line[i] == '-') {
      mode = 1;
    } else if (line[i] == ',') {
      id_ranges.push_back(std::make_pair(first_id, second_id));
      mode = 0;
      first_id.clear();
      second_id.clear();
    } else {
      if (mode == 0)
        first_id.push_back(line[i]);
      else
        second_id.push_back(line[i]);
    }
  }
  id_ranges.push_back(std::make_pair(first_id, second_id));
}

// Using "string" addition in case input contains huge number inputs
// larger than whan can be stored in a long long unsigned int
std::string NextNumber(std::string number) {
  int reminder = 1;
  std::string result;
  for (int i = number.size() - 1; i >= 0; --i) {
    std::string digit = std::string(1, number[i]);
    int digit_int = std::stoi(digit);
    digit_int += reminder;
    reminder = 0;
    if (digit_int > 9) {
      reminder += 1;
      digit_int -= 10;
    }
    result.push_back(std::to_string(digit_int)[0]);
  }
  if (reminder != 0) {
    result.push_back(std::to_string(reminder)[0]);
  }
  std::string sum_string(result.size(), '0');
  for (int i = 0; i < result.size(); ++i) {
    sum_string[result.size() - 1 - i] = result[i];
  }
  return sum_string;
}

unsigned long long ReturnValueIfRepetitive(const std::string &number) {
  unsigned long long value = 0;
  if (static_cast<int>(number.size()) % 2 == 1)
    return value;
  std::string val_left, val_right;
  int middle = number.size() / 2;
  for (int i = 0; i < number.size(); ++i) {
    if (i < middle)
      val_left.push_back(number[i]);
    else
      val_right.push_back(number[i]);
  }

  if (val_left == val_right) {
    return std::stoull(number);
  }
  return value;
}

unsigned long long ReturnValueIfAnyRepetitive(const std::string &number) {
  for (int i = 2; i <= number.size(); ++i) {
    if (number.size() % i != 0)
      continue;
    int split_idx = number.size() / i;
    std::vector<std::string> chunk(i);
    int shift_idx = 0;
    for (int j = 0; j < number.size(); ++j) {
      chunk[shift_idx].push_back(number[j]);
      if ((j + 1) % split_idx == 0)
        shift_idx++;
    }
    bool is_ok = true;
    for (int j = 1; j < chunk.size(); ++j) {
      if (chunk[j] != chunk[0]) {
        is_ok = false;
        break;
      }
    }
    if (is_ok) {
      return std::stoull(number);
    }
  }

  return 0;
}

int main(int argc, char **argv) {
  // Read the file
  std::ifstream input_file("2/input.txt");
  if (!input_file.is_open()) {
    std::cerr << "Could not open file" << std::endl;
    return EXIT_FAILURE;
  }

  // Load the instructions
  std::vector<std::pair<std::string, std::string>> id_ranges;
  std::string line;
  std::getline(input_file, line);
  LoadData(line, id_ranges);

  // 64 bits unsigned int
  unsigned long long sum_of_repetitive_id = 0;
  unsigned long long sum_of_any_repetitive_id = 0;
  for (const auto &range : id_ranges) {
    std::string id = range.first;
    while (id != range.second) {
      sum_of_repetitive_id += ReturnValueIfRepetitive(id);
      sum_of_any_repetitive_id += ReturnValueIfAnyRepetitive(id);
      id = NextNumber(id);
    }
    sum_of_repetitive_id += ReturnValueIfRepetitive(id);
    sum_of_any_repetitive_id += ReturnValueIfAnyRepetitive(id);
  }

  std::cout << "Total: " << sum_of_repetitive_id << std::endl;
  std::cout << "Total: " << sum_of_any_repetitive_id << std::endl;

  return EXIT_SUCCESS;
}