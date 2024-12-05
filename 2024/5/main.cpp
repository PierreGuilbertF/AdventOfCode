#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

std::pair<int, int> ParseRule(const std::string &rule) {
  std::string number_1, number_2;
  bool is_one = true;
  for (int k = 0; k < rule.size(); ++k) {
    if (rule[k] == '|') {
      is_one = false;
      continue;
    } else if (is_one) {
      number_1.push_back(rule[k]);
    } else {
      number_2.push_back(rule[k]);
    }
  }
  const int nbr_1 = std::stoi(number_1);
  const int nbr_2 = std::stoi(number_2);
  return std::make_pair(nbr_1, nbr_2);
}

std::vector<int> ParseUpdate(const std::string &update) {
  std::vector<int> numbers;
  std::stringstream ss(update);
  std::string token;
  while (std::getline(ss, token, ',')) {
    numbers.push_back(std::stoi(token));
  }
  return numbers;
}

bool IsUpdateValid(const std::vector<int> &update,
                   const std::vector<std::pair<int, int>> &rules) {
  // Initialize usefull maps to keep track of rules and respect of them
  std::map<int, bool> number_to_already_seen;
  std::map<int, std::vector<int>> x_to_ys;
  std::map<int, std::vector<int>> y_to_xs;
  std::map<int, bool> page_to_available;
  for (const auto &rule : rules) {
    number_to_already_seen[rule.first] = false;
    number_to_already_seen[rule.second] = false;
    x_to_ys[rule.first].push_back(rule.second);
    y_to_xs[rule.second].push_back(rule.first);
  }

  for (const auto &page : update) {
    page_to_available[page] = true;
  }

  bool is_valid = true;
  for (const auto &page : update) {
    const bool is_x = x_to_ys.count(page);
    const bool is_y = y_to_xs.count(page);

    // No rules for this page update
    if (!is_x && !is_y) {
      if (number_to_already_seen.count(page) != 0)
        number_to_already_seen.at(page) = true;
      continue;
    }

    // we need to check that the x page asosciated has already be seen
    if (is_y) {
      const std::vector<int> &xs = y_to_xs.at(page);
      for (const int &x : xs) {
        if (number_to_already_seen.at(x) == false &&
            page_to_available.count(x) != 0)
          is_valid = false;
      }
    }
    // we need to check that the y page associated has not been already seen
    if (is_x) {
      const std::vector<int> &ys = x_to_ys.at(page);
      for (const int &y : ys) {
        if (number_to_already_seen.at(y) == true &&
            page_to_available.count(y) != 0)
          is_valid = false;
      }
    }
    if (number_to_already_seen.count(page) != 0)
      number_to_already_seen.at(page) = true;
  }

  return is_valid;
}

std::map<int, std::pair<bool, int>>
ResetAlreadySeen(const std::vector<std::pair<int, int>> &rules) {
  std::map<int, std::pair<bool, int>> number_to_already_seen;
  for (const auto &rule : rules) {
    number_to_already_seen[rule.first] = std::make_pair(false, -1);
    number_to_already_seen[rule.second] = std::make_pair(false, -1);
  }
  return number_to_already_seen;
}

void SwapIndex(std::vector<int> &array, int idx0, int idx1) {
  const int temp = array[idx0];
  array[idx0] = array[idx1];
  array[idx1] = temp;
  return;
}

int GetMidWithReordering(const std::vector<int> &update,
                         const std::vector<std::pair<int, int>> &rules) {
  // Initialize usefull maps to keep track of rules and respect of them
  std::map<int, std::vector<int>> x_to_ys;
  std::map<int, std::vector<int>> y_to_xs;
  std::map<int, bool> page_to_available;
  for (const auto &rule : rules) {
    x_to_ys[rule.first].push_back(rule.second);
    y_to_xs[rule.second].push_back(rule.first);
  }
  for (const auto &page : update) {
    page_to_available[page] = true;
  }

  // this one will be reset at every changes
  std::map<int, std::pair<bool, int>> number_to_already_seen =
      ResetAlreadySeen(rules);

  bool is_valid = true;
  std::vector<int> copy_update = update;
  for (int k = 0; k < copy_update.size(); k++) {
    const auto page = copy_update[k];
    const bool is_x = x_to_ys.count(page);
    const bool is_y = y_to_xs.count(page);

    // No rules for this page update
    if (!is_x && !is_y) {
      if (number_to_already_seen.count(page) != 0)
        number_to_already_seen.at(page).first = true;
      continue;
    }

    // we need to check that the x page asosciated has already be seen
    bool as_been_swapped = false;
    if (is_y) {
      const std::vector<int> &xs = y_to_xs.at(page);
      for (const int &x : xs) {
        if (number_to_already_seen.at(x).first == false &&
            page_to_available.count(x) != 0) {
          // find the next x
          int next_x_idx = -1;
          for (int i = k + 1; i < copy_update.size(); ++i) {
            if (copy_update[i] == x) {
              next_x_idx = i;
              break;
            }
          }
          if (next_x_idx == -1) {
            std::cerr << "could not find next x: " << x
                      << " which is unexpected" << std::endl;
          }
          // swap x and y
          SwapIndex(copy_update, k, next_x_idx);
          // reset already seen
          number_to_already_seen = ResetAlreadySeen(rules);
          // reset k to -1 to counter the ++
          k = -1;
          as_been_swapped = true;
          break;
        }
      }
    }

    if (as_been_swapped) {
      continue;
    }

    // we need to check that the y page associated has not been already seen
    if (is_x) {
      const std::vector<int> &ys = x_to_ys.at(page);
      for (const int &y : ys) {
        // std::cout << "assciated y: " << y << std::endl;
        if (number_to_already_seen.at(y).first == true &&
            page_to_available.count(y) != 0) {
          // swap x and y
          SwapIndex(copy_update, k, number_to_already_seen.at(y).second);
          // reset already seen
          number_to_already_seen = ResetAlreadySeen(rules);
          // reset k to -1 to counter the ++
          k = -1;
          break;
        }
      }
    }
    if (number_to_already_seen.count(page) != 0) {
      number_to_already_seen.at(page).first = true;
      number_to_already_seen.at(page).second = k;
    }
  }

  return copy_update[(copy_update.size() - 1) / 2];
}

int main(int argc, char **argv) {
  // Read input file
  const std::string filename = "5/input.txt";
  std::ifstream input_file(filename);
  if (!input_file.is_open()) {
    std::cerr << "Could not read file: " << filename << std::endl;
    return EXIT_FAILURE;
  }

  // Parse input
  std::vector<std::pair<int, int>> order_rules;
  std::vector<std::vector<int>> updates;
  std::string line;
  bool is_rule = true;
  while (std::getline(input_file, line)) {
    std::stringstream line_data;
    line_data << line;
    if (line_data.str() == "") {
      is_rule = false;
      continue;
    } else if (is_rule) {
      order_rules.push_back(ParseRule(line_data.str()));
    } else {
      updates.push_back(ParseUpdate(line_data.str()));
    }
  }
  input_file.close();

  // iterate over updates
  int sum_of_mid_page_valid_update = 0;
  int sum_of_mid_page_not_valid_update = 0;
  for (const auto &update : updates) {
    // check if the update is valid
    const bool is_update_valid = IsUpdateValid(update, order_rules);
    if (!is_update_valid) {
      sum_of_mid_page_not_valid_update +=
          GetMidWithReordering(update, order_rules);
    } else {
      if (static_cast<int>(update.size()) % 2 != 1) {
        std::cout << "Update length is not impair: " << update.size()
                  << std::endl;
      }
      sum_of_mid_page_valid_update += update[(update.size() - 1) / 2];
    }
  }

  std::cout << "Sum of middle page of valid updates: "
            << sum_of_mid_page_valid_update << std::endl;
  std::cout << "Sum of middle page of not valid updates: "
            << sum_of_mid_page_not_valid_update << std::endl;
  return EXIT_SUCCESS;
}