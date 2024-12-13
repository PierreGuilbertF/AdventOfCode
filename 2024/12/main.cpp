#include <cmath>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <vector>

#define INTERGER_TYPE uint64_t

void DisplayMap(const std::vector<std::string> &map) {
  std::cout << "Map is: " << std::endl;
  for (const auto &line : map) {
    std::cout << line << std::endl;
  }
}

std::vector<std::vector<std::pair<int, int>>>
SegmentRegion(const std::vector<std::string> &map) {
  // keep track of already processed data
  const int N = map.size();
  std::vector<std::vector<bool>> already_processed(N,
                                                   std::vector<bool>(N, false));

  // Solve problem with region growth
  std::vector<std::pair<int, int>> directions = {
      {-1, 0}, {0, 1}, {1, 0}, {0, -1}};
  std::vector<std::vector<std::pair<int, int>>> regions;
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      if (already_processed[i][j])
        continue;
      const char token = map[i][j];

      // remainin data to process
      std::vector<std::pair<int, int>> queue;
      queue.push_back(std::make_pair(i, j));
      already_processed[i][j] = true;

      std::vector<std::pair<int, int>> current_region;
      while (!queue.empty()) {
        const std::pair<int, int> location = queue[queue.size() - 1];
        queue.pop_back();
        current_region.push_back(location);
        for (const auto &direction : directions) {
          const std::pair<int, int> candidate_location(
              location.first + direction.first,
              location.second + direction.second);

          // check if the new_location is out of bounds
          if (candidate_location.first < 0 || candidate_location.second < 0 ||
              candidate_location.first >= N || candidate_location.second >= N)
            continue;
          // check if the new location has not been processed
          if (already_processed[candidate_location.first]
                               [candidate_location.second])
            continue;
          // check if the new location matches the token
          const char candidate_token =
              map[candidate_location.first][candidate_location.second];
          if (candidate_token != token)
            continue;
          // If the location is free and match the token, add it to the region
          queue.push_back(candidate_location);
          already_processed[candidate_location.first]
                           [candidate_location.second] = true;
        }
      }
      regions.push_back(current_region);
    }
  }
  return regions;
}

std::map<std::pair<int, int>, bool>
GetBorder(const std::map<std::pair<int, int>, bool> &locations) {
  std::map<std::pair<int, int>, bool> border;
  std::vector<std::pair<int, int>> directions = {
      {-1, 0}, {0, 1}, {1, 0}, {0, -1}};
  for (const auto &location : locations) {
    const auto &uv = location.first;
    int num_neighbor = 0;
    for (const auto &direction : directions) {
      std::pair<int, int> neighbor(uv.first + direction.first,
                                   uv.second + direction.second);
      if (locations.count(neighbor) != 0)
        num_neighbor++;
    }
    if (num_neighbor != 4)
      border[uv] = true;
  }
  return border;
}

int ComputePerimeter(const std::map<std::pair<int, int>, bool> &locations) {
  // now iterate over all locations and see if it is:
  // - isolated => perimeter + 4
  // - advencement => perimeter + 3
  // - corner => perimeter +2
  // - border => perimeter + 1
  // - inside => perimeter + 0
  std::vector<std::pair<int, int>> directions = {
      {-1, 0}, {0, 1}, {1, 0}, {0, -1}};
  int perimeter = 0;
  for (const auto &location : locations) {
    const auto &uv = location.first;
    int num_neighbor = 0;
    for (const auto &direction : directions) {
      std::pair<int, int> neighbor(uv.first + direction.first,
                                   uv.second + direction.second);
      if (locations.count(neighbor) != 0)
        num_neighbor++;
    }
    perimeter += 4 - num_neighbor;
  }
  return perimeter;
}

int ComputeNumberSide(const std::map<std::pair<int, int>, bool> &locations) {
  int number_side = 0;
  std::map<std::pair<int, int>, bool> border = GetBorder(locations);
  std::vector<std::pair<int, int>> directions = {
      {-1, 0}, {0, 1}, {1, 0}, {0, -1}};
  int direction_idx = 0;

  for (auto &border_element : border) {
    if (!border_element.second)
      continue;

    // find directions
    std::vector<std::pair<int, int>> possible_directions;
    for (const auto &direction : directions) {
      const std::pair<int, int> neighbor(
          border_element.first.first + direction.first,
          border_element.first.second + direction.second);
      if (border.count(neighbor) != 0)
        possible_directions.push_back(direction);
    }

    // isolated
    if (possible_directions.size() == 0) {
      number_side += 4;
    } else if (possible_directions.size() == 1) {
      number_side += 3;
    } else if (possible_directions.size() == 2) {
      number_side += 2;
    } else if (possible_directions.size() == 1) {
    }
  }
  return number_side;
}

std::pair<int, int>
ComputeAreaPerimeter(const std::vector<std::string> &map,
                     const std::vector<std::pair<int, int>> &region) {
  std::pair<int, int> area_perimeter(0, 0);
  area_perimeter.first = region.size();
  // convert representation
  std::map<std::pair<int, int>, bool> locations;
  for (const auto &location : region)
    locations[location] = true;
  // Compute perimeter
  area_perimeter.second = ComputePerimeter(locations);
  // Compute number of side
  const int number_of_side = ComputeNumberSide(locations);
  std::cout << "Number side: " << number_of_side << std::endl;

  return area_perimeter;
}

uint64_t ComputePrice(const std::vector<std::string> &map) {
  // First segment the regions
  std::vector<std::vector<std::pair<int, int>>> regions = SegmentRegion(map);
  std::cout << "found: " << regions.size() << " regions" << std::endl;
  // Now compute their perimeters and their area
  uint64_t price = 0;
  for (const auto &region : regions) {
    const std::pair<int, int> area_perimeter =
        ComputeAreaPerimeter(map, region);
    price += area_perimeter.first * area_perimeter.second;
    std::cout << "area: " << area_perimeter.first
              << ", perimeter: " << area_perimeter.second << std::endl;
  }
  return price;
}

int main(int argc, char **argv) {
  // Read input file
  const std::string filename = "12/input.txt";
  std::ifstream input_file(filename);
  if (!input_file.is_open()) {
    std::cerr << "Could not read file: " << filename << std::endl;
    return EXIT_FAILURE;
  }

  // Parse input
  std::vector<std::string> map;
  std::string line;
  while (std::getline(input_file, line)) {
    std::stringstream line_data(line);
    map.push_back(line_data.str());
  }
  input_file.close();

  const uint64_t price = ComputePrice(map);
  std::cout << "total price: " << price << std::endl;
  DisplayMap(map);

  return EXIT_SUCCESS;
}