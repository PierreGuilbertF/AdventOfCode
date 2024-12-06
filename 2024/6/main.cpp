#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

enum SlotType { Obstacle = 1, EndOfMap = 2, Free = 3 };

struct GuardHistoricSlot {
  std::pair<int, int> position;
  int direction_idx;
};

SlotType WhatIsNextSlot(const std::pair<int, int> &guard_position,
                        const std::pair<int, int> &direction,
                        const std::vector<std::string> &map) {
  const int N = map.size();
  // next candidate position
  const std::pair<int, int> next_position(
      guard_position.first + direction.first,
      guard_position.second + direction.second);

  // end of the map is reached
  if (next_position.first < 0 || next_position.first >= N ||
      next_position.second < 0 || next_position.second >= N)
    return SlotType::EndOfMap;
  // Obstacle encountered
  if (map[next_position.first][next_position.second] == '#')
    return SlotType::Obstacle;
  // guard can move
  return SlotType::Free;
}

int IsGuard(const char input) {
  if (input == '^')
    return 0;
  if (input == '>')
    return 1;
  if (input == 'v')
    return 2;
  if (input == '<')
    return 3;

  return -1;
}

std::vector<GuardHistoricSlot>
GetGuardPath(const std::pair<int, int> &initial_guard_position,
             int initial_direction_idx, const std::vector<std::string> &map) {
  // keep track of historic element. If the guard is at the same position
  // with the same direction than before it means we are in a loop
  // in this case, return after completing one loop
  std::map<std::pair<std::pair<int, int>, int>, bool> unique_historic_element;
  // final guard path to exit the map
  std::vector<GuardHistoricSlot> guard_path;
  std::vector<std::pair<int, int>> directions = {
      {-1, 0}, {0, 1}, {1, 0}, {0, -1}};
  std::pair<int, int> guard_position = initial_guard_position;
  int direction_idx = initial_direction_idx;
  while (true) {
    GuardHistoricSlot historic_element = {guard_position, direction_idx};
    guard_path.push_back(historic_element);
    // If we are in a loop, exit the function
    const std::pair<std::pair<int, int>, int> historic_key(guard_position,
                                                           direction_idx);
    if (unique_historic_element.count(historic_key) != 0) {
      return guard_path;
    }
    unique_historic_element[historic_key] = true;

    const std::pair<int, int> direction = directions[direction_idx];
    const SlotType type = WhatIsNextSlot(guard_position, direction, map);
    if (type == SlotType::Free) {
      // Move the guard
      guard_position.first += direction.first;
      guard_position.second += direction.second;
    } else if (type == SlotType::Obstacle) {
      // Change the direction
      direction_idx = (direction_idx + 1) % 4;
    } else {
      // End the loop
      break;
    }
  }
  return guard_path;
}

int main(int argc, char **argv) {
  // Read input file
  const std::string filename = "6/input.txt";
  std::ifstream input_file(filename);
  if (!input_file.is_open()) {
    std::cerr << "Could not read file: " << filename << std::endl;
    return EXIT_FAILURE;
  }

  // Parse input
  std::vector<std::string> map;
  std::string line;
  while (std::getline(input_file, line)) {
    std::stringstream line_data;
    line_data << line;
    map.push_back(line_data.str());
  }
  input_file.close();

  // Get the guard position
  const int N = map.size();
  std::pair<int, int> guard_position;
  int direction_idx;
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      const int detected_direction_idx = IsGuard(map[i][j]);
      if (detected_direction_idx != -1) {
        guard_position = std::make_pair(i, j);
        direction_idx = detected_direction_idx;
        std::cout << "Guard located at: [" << i << "," << j
                  << "], direction is: " << direction_idx << std::endl;
        break;
      }
    }
  }

  // Part 1, simply get the unique position visited
  const auto &path = GetGuardPath(guard_position, direction_idx, map);
  std::map<std::pair<int, int>, bool> unique_position_visited;
  for (const auto historic_element : path) {
    unique_position_visited[historic_element.position] = true;
  }

  // Part 2, create all potential path by adding a single obstacle
  // in the map at the time, and filter path that ends with a loop
  int num_loop_possible = 0;
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      // already an obstacle or guard position so skip it
      if (map[i][j] != '.')
        continue;
      // create new map and add the obstacle
      std::vector<std::string> new_map = map;
      new_map[i][j] = '#';
      // Get the path made by the guard
      const auto &path = GetGuardPath(guard_position, direction_idx, new_map);
      // check if the path is a loop
      std::map<std::pair<std::pair<int, int>, int>, bool>
          unique_historic_element;
      for (const auto &path_element : path) {
        const std::pair<std::pair<int, int>, int> key(
            path_element.position, path_element.direction_idx);
        // the path is a loop
        if (unique_historic_element.count(key) != 0) {
          num_loop_possible++;
          break;
        }
        unique_historic_element[key] = true;
      }
    }
  }

  std::cout << "The guard visited: " << unique_position_visited.size()
            << " distinct positions" << std::endl;
  std::cout << "There are: " << num_loop_possible
            << " ways of creating a loop by adding an obstacle" << std::endl;

  return EXIT_SUCCESS;
}