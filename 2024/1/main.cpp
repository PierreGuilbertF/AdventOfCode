#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

int main(int argc, char **argv) {
  // Read input file
  const std::string filename = "1/input.txt";
  std::ifstream input_file(filename);
  if (!input_file.is_open()) {
    std::cerr << "Could not read file: " << filename << std::endl;
    return EXIT_FAILURE;
  }

  // Parse input
  std::vector<int> left_list, right_list;
  std::string line;
  while (std::getline(input_file, line)) {
    std::istringstream iss(line);
    int left, right;
    // read the two integers
    if (iss >> left >> right) {
      left_list.push_back(left);
      right_list.push_back(right);
    } else {
      std::cerr << "Erreur : Format invalide dans la ligne \"" << line << "\""
                << std::endl;
    }
  }
  input_file.close();

  // Check that the two lists are of the same size
  if (left_list.size() != right_list.size()) {
    std::cerr << "Left list size: " << left_list.size() << " does"
              << " not match right list size: " << right_list.size();
    return EXIT_FAILURE;
  }

  // Sort the two list
  std::sort(left_list.begin(), left_list.end());
  std::sort(right_list.begin(), right_list.end());
  int sum_of_absolute_difference = 0;
  for (int k = 0; k < left_list.size(); ++k) {
    sum_of_absolute_difference += std::abs(left_list[k] - right_list[k]);
  }
  std::cout << "sum of absolute difference is: " << sum_of_absolute_difference
            << std::endl;

  // Part two:
  std::map<int, int> right_id_to_count;
  for (const auto &id : right_list) {
    if (right_id_to_count.count(id) == 0) {
      right_id_to_count[id] = 1;
    } else {
      right_id_to_count.at(id) += 1;
    }
  }

  int total_multiplication = 0;
  for (const auto &id : left_list) {
    const int number_time =
        (right_id_to_count.count(id) == 0) ? 0 : right_id_to_count.at(id);
    total_multiplication += id * number_time;
  }

  std::cout << "multiplication is: " << total_multiplication << std::endl;

  return EXIT_SUCCESS;
}