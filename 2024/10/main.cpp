#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

void DisplayMap(const std::vector<std::vector<int>> &topography_map) {
  for (int i = 0; i < topography_map.size(); ++i) {
    for (int j = 0; j < topography_map.size(); ++j) {
      std::cout << topography_map[i][j] << "  ";
    }
    std::cout << std::endl << std::endl;
  }
}

std::vector<std::pair<int, int>>
FindStartingPoints(const std::vector<std::vector<int>> &topography_map) {
  std::vector<std::pair<int, int>> starting_points;
  for (int i = 0; i < topography_map.size(); ++i) {
    for (int j = 0; j < topography_map[i].size(); ++j) {
      if (topography_map[i][j] == 0)
        starting_points.push_back(std::make_pair(i, j));
    }
  }
  return starting_points;
}

struct Historic {
  std::vector<std::pair<int, int>> path;
};

int ComputePathsScore(const std::vector<std::vector<int>> &topography_map) {
  const int N = topography_map.size();
  // We will explore all the possible paths from this starting point with a
  // queue
  std::vector<std::pair<int, int>> directions = {
      {-1, 0}, {0, 1}, {1, 0}, {0, -1}};
  std::vector<Historic> valid_paths;
  std::vector<std::pair<int, int>> starting_points =
      FindStartingPoints(topography_map);

  // now, for each starting points, we will find all the path possible
  // from this starting point to an end point while respecting the rulse:
  // - uphill
  // - elevation always +1 betweeb two steps
  // - only moving in cross-direction
  for (const auto &start : starting_points) {
    std::vector<Historic> queue(0);
    Historic historic;
    historic.path = {start};
    queue.push_back(historic);

    while (queue.size() > 0) {
      Historic historic = queue[queue.size() - 1];
      queue.pop_back();

      // Check if we have reached the max
      const auto &point = historic.path[historic.path.size() - 1];
      const int old_height = topography_map[point.first][point.second];
      if (old_height == 9) {
        valid_paths.push_back(historic);
        continue;
      }

      for (const auto &direction : directions) {
        const int u = point.first + direction.first;
        const int v = point.second + direction.second;
        // Out of bond
        if (u < 0 || v < 0 || u >= N || v >= N)
          continue;
        // check that we are going uphill
        const int new_height = topography_map[u][v];
        if (new_height - old_height != 1)
          continue;
        // Check if we have reached the max
        if (new_height == 10)
          continue;
        // All good, we can add this new entry
        Historic new_historic = historic;
        new_historic.path.push_back(std::make_pair(u, v));
        queue.push_back(new_historic);
      }
    }
  }

  std::map<std::pair<std::pair<int, int>, std::pair<int, int>>, bool>
      unique_trail_head;
  for (const auto &path : valid_paths) {
    const auto &start = path.path[0];
    const auto &end = path.path[path.path.size() - 1];
    const auto key = std::make_pair(start, end);
    unique_trail_head[key] = true;
  }

  std::cout << "number valid paths: " << valid_paths.size() << std::endl;
  std::cout << "number unique paths: " << unique_trail_head.size() << std::endl;
  return unique_trail_head.size();
}

int main(int argc, char **argv) {
  // Read input file
  const std::string filename = "10/input.txt";
  std::ifstream input_file(filename);
  if (!input_file.is_open()) {
    std::cerr << "Could not read file: " << filename << std::endl;
    return EXIT_FAILURE;
  }

  // Parse input
  std::vector<std::vector<int>> topography_map;
  std::string line;
  while (std::getline(input_file, line)) {
    std::stringstream line_data(line);
    std::vector<int> row;
    for (int k = 0; k < line_data.str().size(); ++k) {
      const std::string int_str(1, line_data.str()[k]);
      if (int_str == ".")
        row.push_back(10);
      else {
        const int height = std::stoi(int_str);
        row.push_back(height);
      }
    }
    topography_map.push_back(row);
  }
  input_file.close();

  // Display map
  // DisplayMap(topography_map);

  const int score = ComputePathsScore(topography_map);
  std::cout << "score: " << score << std::endl;

  return EXIT_SUCCESS;
}