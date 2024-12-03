#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

std::vector<int> FindDoPattern(const std::string &memory) {
  std::vector<int> do_pattern_idx;
  for (int k = 0; k < memory.size() - 4; ++k) {
    if (memory[k] == 'd' && memory[k + 1] == 'o' && memory[k + 2] == '(' &&
        memory[k + 3] == ')')
      do_pattern_idx.push_back(k);
  }
  return do_pattern_idx;
}

std::vector<int> FindDontPattern(const std::string &memory) {
  std::vector<int> dont_pattern_idx;
  for (int k = 0; k < memory.size() - 7; ++k) {
    if (memory[k] == 'd' && memory[k + 1] == 'o' && memory[k + 2] == 'n' &&
        memory[k + 3] == '\'' && memory[k + 4] == 't' && memory[k + 5] == '(' &&
        memory[k + 6] == ')')
      dont_pattern_idx.push_back(k);
  }
  return dont_pattern_idx;
}

std::vector<std::pair<int, int>> FindMulPattern(const std::string &memory) {
  std::vector<std::pair<int, int>> mul_pattern_idx;
  for (int k = 0; k < memory.size() - 3; ++k) {
    // Find mul( pattern
    if (memory[k] == 'm' && memory[k + 1] == 'u' && memory[k + 2] == 'l' &&
        memory[k + 3] == '(') {
      // Find the next )
      int next_parenthesis = -1;
      for (int j = k + 3; j < memory.size(); ++j) {
        if (memory[j] == ')') {
          next_parenthesis = j;
          break;
        }
      }
      if (next_parenthesis == -1)
        continue;
      mul_pattern_idx.push_back(std::make_pair(k, next_parenthesis));
    }
  }
  return mul_pattern_idx;
}

std::pair<bool, std::pair<int, int>>
CheckIfCommandIsValid(const std::string &command) {
  // Find comma index
  int comma_idx = -1;
  for (int k = 0; k < command.size(); ++k) {
    if (command[k] == ',') {
      comma_idx = k;
      break;
    }
  }
  if (comma_idx == -1)
    return std::make_pair(false, std::make_pair(-1, -1));

  // Now get two numbers as string
  std::string number_1_str, number_2_str;
  for (int k = 4; k < comma_idx; ++k) {
    number_1_str.push_back(command[k]);
  }
  for (int k = comma_idx + 1; k < command.size() - 1; ++k) {
    number_2_str.push_back(command[k]);
  }

  // Try to interpret these strings as integer
  int number_1, number_2;
  try {
    size_t pos_1, pos_2;
    number_1 = std::stoi(number_1_str, &pos_1);
    number_2 = std::stoi(number_2_str, &pos_2);

    // Check that all the chars have been consumed
    if (pos_1 != number_1_str.size() || pos_2 != number_2_str.size())
      return std::make_pair(false, std::make_pair(-1, -1));
  } catch (const std::invalid_argument &e) {
    return std::make_pair(false, std::make_pair(-1, -1));
    std::cout << "Erreur : La chaîne ne contient pas de nombre valide."
              << std::endl;
  } catch (const std::out_of_range &e) {
    return std::make_pair(false, std::make_pair(-1, -1));
    std::cout
        << "Erreur : Le nombre est hors de la plage des valeurs d'un entier."
        << std::endl;
  }

  return std::make_pair(true, std::make_pair(number_1, number_2));
}

int main(int argc, char **argv) {
  // Read input file
  const std::string filename = "3/input.txt";
  std::ifstream input_file(filename);
  if (!input_file.is_open()) {
    std::cerr << "Could not read file: " << filename << std::endl;
    return EXIT_FAILURE;
  }

  // Parse input
  std::stringstream memory;
  std::string line;
  while (std::getline(input_file, line)) {
    std::vector<int> current_report;
    memory << line;
  }
  input_file.close();

  // Find the potential correct operation using the "mul(****)" pattern
  const std::vector<std::pair<int, int>> mul_pattern_idx =
      FindMulPattern(memory.str());
  const std::vector<int> do_pattern = FindDoPattern(memory.str());
  const std::vector<int> dont_pattern = FindDontPattern(memory.str());

  // Determine if operations are valid
  std::vector<bool> valid_operation(memory.str().size(), true);
  std::vector<bool> already_initialized(memory.str().size(), false);
  for (const auto &do_idx : do_pattern) {
    valid_operation[do_idx] = true;
    already_initialized[do_idx] = true;
  }
  for (const auto &dont_idx : dont_pattern) {
    valid_operation[dont_idx] = false;
    already_initialized[dont_idx] = true;
  }
  already_initialized[0] = true;

  // Now propagate
  for (int k = 1; k < valid_operation.size(); ++k) {
    if (already_initialized[k])
      continue;
    valid_operation[k] = valid_operation[k - 1];
  }

  // Now, among these candidate, use the one where ***** = number,number
  int sum_of_multiplications = 0;
  int sum_of_multiplications_only_if_valid = 0;
  for (const auto pattern : mul_pattern_idx) {
    std::string command;
    for (int i = pattern.first; i <= pattern.second; ++i) {
      command.push_back(memory.str()[i]);
    }
    const auto &command_info = CheckIfCommandIsValid(command);
    if (!command_info.first)
      continue;
    sum_of_multiplications +=
        command_info.second.first * command_info.second.second;
    if (valid_operation[pattern.first])
      sum_of_multiplications_only_if_valid +=
          command_info.second.first * command_info.second.second;
  }
  std::cout << "Potential pattern found: " << mul_pattern_idx.size()
            << std::endl;
  std::cout << "Result of sum of multiplication: " << sum_of_multiplications
            << std::endl;
  std::cout << "Result of sum of multiplication when operation are valids: "
            << sum_of_multiplications_only_if_valid << std::endl;

  return EXIT_SUCCESS;
}