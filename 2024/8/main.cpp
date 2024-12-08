#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

std::map<std::string, std::vector<std::pair<int, int>>>
GetAntennaLocation(const std::vector<std::string> &antenna_map) {
  std::map<std::string, std::vector<std::pair<int, int>>> antenna_to_location;
  for (int i = 0; i < antenna_map.size(); ++i) {
    for (int j = 0; j < antenna_map[i].size(); ++j) {
      if (antenna_map[i][j] != '.') {
        const std::string key(1, antenna_map[i][j]);
        antenna_to_location[key].push_back(std::make_pair(i, j));
      }
    }
  }
  return antenna_to_location;
}

bool IsInMap(const std::pair<int, int> slot, int N) {
  return (slot.first >= 0) && (slot.second >= 0) && (slot.first <= N) &&
         (slot.second <= N);
}

void AddNodes(std::vector<std::pair<int, int>> &list_of_nodes,
              const std::pair<int, int> &xy1, const std::pair<int, int> &xy2,
              bool with_resonance, int map_size) {
  // Get their difference of coordinate in each dimension
  const int dx = xy1.first - xy2.first;
  const int dy = xy1.second - xy2.second;
  if (!with_resonance) {
    // add the nodes created
    const std::pair<int, int> node1(xy1.first + dx, xy1.second + dy);
    const std::pair<int, int> node2(xy2.first - dx, xy2.second - dy);
    list_of_nodes.push_back(node1);
    list_of_nodes.push_back(node2);
  } else {
    // add the nodes until they are out
    std::pair<int, int> current_node = xy1;
    // First point outside line
    while (IsInMap(current_node, map_size)) {
      list_of_nodes.push_back(current_node);
      current_node.first += dx;
      current_node.second += dy;
    }
    // Second point outside line
    current_node = xy1;
    while (IsInMap(current_node, map_size)) {
      list_of_nodes.push_back(current_node);
      current_node.first -= dx;
      current_node.second -= dy;
    }
  }
}

int ComputeUniqueAntinodes(const std::vector<std::string> &antenna_map,
                           bool with_resonance) {
  const std::map<std::string, std::vector<std::pair<int, int>>>
      antenna_to_location = GetAntennaLocation(antenna_map);

  std::vector<std::pair<int, int>> list_of_nodes;
  // Now, iterate over all pair of antenna of the same type
  for (const auto &antenna_type : antenna_to_location) {
    const auto &antennas = antenna_type.second;
    // we need at least two antennas of the same type
    if (antennas.size() < 2)
      continue;
    for (int antenna_idx_1 = 0; antenna_idx_1 < antennas.size() - 1;
         antenna_idx_1++) {
      const auto &xy1 = antennas[antenna_idx_1];
      for (int antenna_idx_2 = antenna_idx_1 + 1;
           antenna_idx_2 < antennas.size(); antenna_idx_2++) {
        const auto &xy2 = antennas[antenna_idx_2];
        AddNodes(list_of_nodes, xy1, xy2, with_resonance, antenna_map.size());
      }
    }
  }

  // now, filter the nodes to have:
  // - unique location
  // - within the map
  // - not occupied by antenna

  // Add the antenna
  std::map<std::pair<int, int>, bool> occupied_spot;
  // for (const auto &antennas : antenna_to_location) {
  //   for (const auto &spot : antennas.second) {
  //     occupied_spot[spot] = true;
  //   }
  // }

  // Add the nodes
  const int N = antenna_map.size();
  int number_unique_nodes = 0;
  for (const auto &node : list_of_nodes) {
    // out of the map
    if (node.first < 0 || node.second < 0 || node.first >= N ||
        node.second >= N)
      continue;
    // already occupied
    if (occupied_spot.count(node) != 0)
      continue;
    // All good
    number_unique_nodes++;
    occupied_spot[node] = true;
  }
  return number_unique_nodes;
}

int main(int argc, char **argv) {
  // Read input file
  const std::string filename = "8/input.txt";
  std::ifstream input_file(filename);
  if (!input_file.is_open()) {
    std::cerr << "Could not read file: " << filename << std::endl;
    return EXIT_FAILURE;
  }

  // Parse input
  std::vector<std::string> antenna_map;
  std::string line;
  while (std::getline(input_file, line)) {
    std::stringstream line_data(line);
    antenna_map.push_back(line_data.str());
  }
  input_file.close();

  // Without resonance
  const int number_antinodes_without_resonance =
      ComputeUniqueAntinodes(antenna_map, false);
  std::cout << "Number of unique nodes without resonance is: "
            << number_antinodes_without_resonance << std::endl;
  const int number_antinodes_with_resonance =
      ComputeUniqueAntinodes(antenna_map, true);
  std::cout << "Number of unique nodes without resonance is: "
            << number_antinodes_with_resonance << std::endl;

  return EXIT_SUCCESS;
}