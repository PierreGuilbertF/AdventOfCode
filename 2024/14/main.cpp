#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <vector>

const int kWidth = 101;
const int kHeight = 103;
const int kMaxIteration = 20000;

void DisplayProblem(const std::vector<std::pair<int, int>> &positions,
                    const std::vector<std::pair<int, int>> &velocities) {
  for (int k = 0; k < positions.size(); ++k) {
    std::cout << "robot: " << k << " position: [" << positions[k].first << ", "
              << positions[k].second << "]";
    std::cout << " velocity: [" << velocities[k].first << ", "
              << velocities[k].second << "]" << std::endl;
  }
}

void UpdateRobotPositions(std::vector<std::pair<int, int>> &positions,
                          const std::vector<std::pair<int, int>> &velocities,
                          int time) {
  for (int k = 0; k < positions.size(); ++k) {
    // move the robots for the required amount of time
    // not taking into acocunt the teleportation
    int x = (positions[k].first + time * velocities[k].first);
    int y = (positions[k].second + time * velocities[k].second);

    // use modulo operator for teleportation effect
    x = x % kWidth;
    y = y % kHeight;

    // because numbers can be negative, we will shift it
    x = (x + kWidth) % kWidth;
    y = (y + kHeight) % kHeight;
    positions[k] = std::make_pair(x, y);
  }
  return;
}

uint64_t
QuadrantMultiplication(const std::vector<std::pair<int, int>> &positions) {
  std::array<uint64_t, 4> per_quadrant_robots = {0, 0, 0, 0};
  for (const auto &position : positions) {
    // we do not take into account the robots on the middle
    if (position.first == kWidth / 2 || position.second == kHeight / 2)
      continue;
    // first quadrant
    if (position.first < kWidth / 2 && position.second < kHeight / 2)
      per_quadrant_robots[0]++;
    // second quadrant
    if (position.first < kWidth / 2 && position.second > kHeight / 2)
      per_quadrant_robots[1]++;
    // third quadrant
    if (position.first > kWidth / 2 && position.second < kHeight / 2)
      per_quadrant_robots[2]++;
    // fourth quadrant
    if (position.first > kWidth / 2 && position.second > kHeight / 2)
      per_quadrant_robots[3]++;
  }
  return per_quadrant_robots[0] * per_quadrant_robots[1] *
         per_quadrant_robots[2] * per_quadrant_robots[3];
}

double
ComputeAverageNeighbhor(const std::vector<std::pair<int, int>> &positions) {
  std::map<std::pair<int, int>, bool> position_to_valid;
  for (const auto &position : positions) {
    position_to_valid[position] = true;
  }

  double result = 0;
  std::vector<std::pair<int, int>> directions = {
      {-1, 0}, {0, 1}, {1, 0}, {0, -1}};
  for (const auto &position : positions) {
    double neighbor = 0;
    for (const auto &direction : directions) {
      const std::pair<int, int> neighbor(position.first + direction.first,
                                         position.second + direction.second);
      if (position_to_valid.count(neighbor) != 0)
        result = result + 1.0;
    }
  }

  return result / std::max(1.0, static_cast<double>(positions.size()));
}

int main(int argc, char **argv) {
  // Read input file
  const std::string filename = "14/input.txt";
  std::ifstream input_file(filename);
  if (!input_file.is_open()) {
    std::cerr << "Could not read file: " << filename << std::endl;
    return EXIT_FAILURE;
  }

  std::vector<std::pair<int, int>> positions;
  std::vector<std::pair<int, int>> velocities;
  std::string line;

  while (getline(input_file, line)) {
    std::istringstream lineStream(line);
    std::string part;
    int px, py, vx, vy;

    getline(lineStream, part, ' ');
    sscanf(part.c_str(), "p=%d,%d", &px, &py);

    getline(lineStream, part);
    sscanf(part.c_str(), "v=%d,%d", &vx, &vy);

    positions.emplace_back(px, py);
    velocities.emplace_back(vx, vy);
  }

  // first problem
  int time = 100;
  UpdateRobotPositions(positions, velocities, time);
  const uint64_t result = QuadrantMultiplication(positions);
  std::cout << "Result is: " << result << std::endl;

  // second problem
  double max_avg = std::numeric_limits<double>::min();
  int max_index = -1;
  for (int k = 0; k < kMaxIteration; ++k) {
    // update the position of the robots for one second
    UpdateRobotPositions(positions, velocities, 1);
    // compute the averahe number of neighbor
    const double average_neigbhor = ComputeAverageNeighbhor(positions);
    if (average_neigbhor > max_avg) {
      max_avg = average_neigbhor;
      max_index = k;
    }
  }
  std::cout << "outlier stats reached at: " << max_index + 101 << std::endl;

  return EXIT_SUCCESS;
}