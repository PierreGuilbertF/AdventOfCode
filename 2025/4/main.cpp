#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

int CountAccessibleRoll(std::vector<std::string> &grid) {
  std::vector<std::pair<int, int>> to_remove;
  int num = 0;
  for (int i = 0; i < grid.size(); ++i) {
    const int xmin = std::max(0, i - 1);
    const int xmax = std::min(static_cast<int>(grid.size()) - 1, i + 1);
    for (int j = 0; j < grid[i].size(); ++j) {
      // skip if it is not a roll
      if (grid[i][j] != '@')
        continue;

      const int ymin = std::max(0, j - 1);
      const int ymax = std::min(static_cast<int>(grid[i].size()) - 1, j + 1);
      int num_beighbor = 0;

      for (int u = xmin; u <= xmax; ++u) {
        for (int v = ymin; v <= ymax; ++v) {
          // do not consider current position
          if (u == 0 && v == 0)
            continue;
          if (grid[u][v] == '@')
            num_beighbor++;
        }
      }
      if (num_beighbor <= 4) {
        to_remove.push_back(std::make_pair(i, j));
        num++;
      }
    }
  }
  for (const auto &elem : to_remove)
    grid[elem.first][elem.second] = '.';
  return num;
}

int main(int argc, char **argv) {
  // Read the file
  std::ifstream input_file("4/input.txt");
  if (!input_file.is_open()) {
    std::cerr << "Could not open file" << std::endl;
    return EXIT_FAILURE;
  }

  // Load the instructions
  std::vector<std::string> grid;
  std::string line;
  while (std::getline(input_file, line)) {
    grid.push_back(line);
  }

  for (const auto &row : grid) {
    std::cout << row << std::endl;
  }

  int number_accessible_roll = 0;
  while (true) {
    int number_accessible_roll_current = CountAccessibleRoll(grid);
    number_accessible_roll += number_accessible_roll_current;
    if (number_accessible_roll_current == 0)
      break;
  }
  std::cout << "Num valid roll: " << number_accessible_roll << std::endl;

  return EXIT_SUCCESS;
}