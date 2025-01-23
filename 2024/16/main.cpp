#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <unordered_map>
#include <vector>

struct ArrayHash {
  std::size_t operator()(const std::array<int, 2> &arr) const noexcept {
    std::size_t seed = 0;
    for (int i : arr) {
      seed ^= std::hash<int>{}(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};

struct ArrayEqual {
  bool operator()(const std::array<int, 2> &a,
                  const std::array<int, 2> &b) const {
    return a[0] == b[0] && a[1] == b[1];
  }
};

struct Path {
public:
  std::array<int, 2> last_position;
  std::vector<std::vector<bool>> already_visited;
  std::vector<std::array<int, 2>> ordered_path;
  int score = 0;
  int prev_direction = 2;
};

struct Query {
  std::array<int, 2> position;
  int direction_idx = 2;
  int score = 0;
};

std::array<int, 2> FindStart(const std::vector<std::string> &maze) {
  for (int i = 0; i < maze.size(); i++) {
    for (int j = 0; j < maze[i].size(); ++j) {
      if (maze[i][j] == 'S')
        return {i, j};
    }
  }
  std::cerr << "Could not find maze start" << std::endl;
  return {-1, -1};
}

std::array<int, 2> FindEnd(const std::vector<std::string> &maze) {
  for (int i = 0; i < maze.size(); i++) {
    for (int j = 0; j < maze[i].size(); ++j) {
      if (maze[i][j] == 'E')
        return {i, j};
    }
  }
  std::cerr << "Could not find maze end" << std::endl;
  return {-1, -1};
}

void SolveMazeFast(const std::vector<std::string> &maze) {
  const std::array<int, 2> start = FindStart(maze);
  const std::array<int, 2> end = FindEnd(maze);

  // Create a score map that will act like a minimal distance
  // map between a point of the maze and the start
  std::vector<std::vector<int>> score_map(
      maze.size(),
      std::vector<int>(maze[0].size(), std::numeric_limits<int>::max()));
  score_map[start[0]][start[1]] = 0;

  std::vector<std::array<int, 2>> directions = {
      {1, 0}, {-1, 0}, {0, 1}, {0, -1}};
  Query initial_query = {start, 2, 0};
  std::vector<Query> queue(1, initial_query);
  while (!queue.empty()) {
    const auto query = queue.back();
    queue.pop_back();

    for (int k = 0; k < directions.size(); ++k) {
      const auto &direction = directions[k];
      const std::array<int, 2> candidate_position = {
          query.position[0] + direction[0], query.position[1] + direction[1]};
      const int new_score =
          (k == query.direction_idx) ? query.score + 1 : query.score + 1001;

      // outside of the range
      if (candidate_position[0] < 0 || candidate_position[0] >= maze.size() ||
          candidate_position[1] < 0 || candidate_position[1] >= maze[0].size())
        continue;
      //  wall
      if (maze[candidate_position[0]][candidate_position[1]] == '#')
        continue;
      // score is higher
      if (new_score > score_map[candidate_position[0]][candidate_position[1]])
        continue;
      // we are good to go
      score_map[candidate_position[0]][candidate_position[1]] = new_score;

      Query next_query = {candidate_position, k, new_score};
      queue.push_back(next_query);
    }
  }

  std::cout << "Distance at end: " << score_map[end[0]][end[1]] << std::endl;

  // Now that we have the distance map, start from the end and return to
  // the input folowwing the best path
  std::vector<std::array<int, 2>> path;
  std::vector<std::array<int, 2>> queue_backward(1, end);
  while (!queue_backward.empty()) {
    const std::array<int, 2> position = queue_backward.back();
    queue_backward.pop_back();
    path.push_back(position);

    const int thresh_score = score_map[position[0]][position[1]];
    int minimal_score = thresh_score;
    std::vector<std::array<int, 2>> possible_positions;
    for (const auto &direction : directions) {
      const std::array<int, 2> next_position = {position[0] + direction[0],
                                                position[1] + direction[1]};
      // outside of the range
      if (next_position[0] < 0 || next_position[0] >= maze.size() ||
          next_position[1] < 0 || next_position[1] >= maze[0].size())
        continue;
      //  wall
      if (maze[next_position[0]][next_position[1]] == '#')
        continue;
      const int candidate_score = score_map[next_position[0]][next_position[1]];
      if (candidate_score >= thresh_score)
        continue;

      // now find the direction(s) with lowest score
      if (candidate_score < minimal_score) {
        minimal_score = candidate_score;
        possible_positions.clear();
        possible_positions.push_back(next_position);
      } else if (candidate_score == minimal_score)
        possible_positions.push_back(next_position);
    }
    if (possible_positions.size() > 1)
      std::cout << "adding: " << possible_positions.size() << " cross"
                << std::endl;
    for (const auto &next_candidate : possible_positions)
      queue_backward.push_back(next_candidate);
  }

  std::cout << "Solution length: " << path.size() << std::endl;

  // Draw solution
  auto solution_maze = maze;
  for (const auto &position : path) {
    solution_maze[position[0]][position[1]] = 'O';
  }
  for (const auto &maze_line : solution_maze) {
    std::cout << maze_line << std::endl;
  }
}

void SolveMazeFindAllPaths(const std::vector<std::string> &maze) {
  std::array<int, 2> position = FindStart(maze);
  std::cout << "Maze start at: " << position[0] << ", " << position[1]
            << std::endl;

  Path path;
  path.already_visited = std::vector<std::vector<bool>>(
      maze.size(), std::vector<bool>(maze[0].size(), false));
  path.already_visited[position[0]][position[1]] = true;
  path.last_position = position;
  path.ordered_path.push_back(position);

  std::vector<std::vector<int>> score_map(
      maze.size(),
      std::vector<int>(maze[0].size(), std::numeric_limits<int>::max()));

  // Complete paths from start to end
  std::vector<Path> solutions;

  // Uncomplete paths to be processed
  std::vector<Path> queue(1, path);

  // Different possible directions
  std::vector<std::array<int, 2>> directions = {
      {1, 0}, {-1, 0}, {0, 1}, {0, -1}};

  // Explore all the possible paths
  // The number of paths will grow with the
  // number of intersections in the maze
  int iteration = 0;
  while (!queue.empty()) {
    iteration++;
    if (iteration % 10000 == 0)
      std::cout << "Path in the queue: " << queue.size() << std::endl;
    // Get the last element of the queue and erase it
    Path current_path = queue.back();
    queue.pop_back();

    // now explore the vecinity to see if we can:
    // 1- add a new move to the path
    // 2- split the path
    // 3- complete the path
    for (int k = 0; k < directions.size(); ++k) {
      const auto &direction = directions[k];
      const int i = current_path.last_position[0] + direction[0];
      const int j = current_path.last_position[1] + direction[1];

      // outside of the range
      if (i < 0 || i >= maze.size() || j < 0 || j >= maze[0].size())
        continue;
      //  wall
      if (maze[i][j] == '#')
        continue;
      // point already visited
      const std::array<int, 2> candidate = {i, j};
      if (current_path.already_visited[candidate[0]][candidate[1]])
        continue;
      // check if we have another path with smaller score
      int score = current_path.score + 1;
      if (current_path.prev_direction != k)
        score += 1000;
      if (score_map[candidate[0]][candidate[1]] < score)
        continue;
      // otherwise we are good to go
      score_map[candidate[0]][candidate[1]] = score;
      Path candidate_path = current_path;
      candidate_path.last_position = candidate;
      candidate_path.already_visited[candidate[0]][candidate[1]] = true;
      candidate_path.ordered_path.push_back(candidate);
      candidate_path.score = score;

      // If the point is the end
      if (maze[i][j] == 'E') {
        solutions.push_back(candidate_path);
        continue;
      }
      // else continue to explore
      queue.push_back(candidate_path);
    }
  }

  // count score
  int count = 0;
  int min_score = std::numeric_limits<int>::max();
  std::vector<std::string> minimal_maze;
  for (const auto &solution : solutions) {
    int score = 0;
    // looking east
    int direction_idx = 2;
    auto maze2 = maze;

    for (int k = 0; k < solution.ordered_path.size() - 1; ++k) {
      const auto &point = solution.ordered_path[k];
      const auto &next_point = solution.ordered_path[k + 1];
      const std::array<int, 2> direction = {next_point[0] - point[0],
                                            next_point[1] - point[1]};
      int new_direction_idx = 0;
      for (int p = 0; p < directions.size(); ++p) {
        if (directions[p][0] == direction[0] &&
            directions[p][1] == direction[1]) {
          new_direction_idx = p;
          break;
        }
      }

      if (new_direction_idx == direction_idx)
        score += 1;
      if (new_direction_idx != direction_idx)
        score += 1001;
      direction_idx = new_direction_idx;

      char symbol;
      if (new_direction_idx == 0)
        symbol = 'V';
      if (new_direction_idx == 1)
        symbol = '^';
      if (new_direction_idx == 2)
        symbol = '>';
      if (new_direction_idx == 3)
        symbol = '<';

      maze2[point[0]][point[1]] = symbol;
    }
    maze2[solution.last_position[0]][solution.last_position[1]] = 'X';

    if (score < min_score) {
      minimal_maze = maze2;
      min_score = score;
    }
  }

  std::cout << "Total paths to solution: " << solutions.size() << std::endl;
  std::cout << "Min score is: " << min_score << std::endl;
  std::cout << "reached with solution: " << std::endl;
  for (const auto &line : minimal_maze) {
    std::cout << line << std::endl;
  }
}

int main(int argc, char **argv) {
  // Read input file
  const std::string filename = "16/input.txt";
  std::ifstream input_file(filename);
  if (!input_file.is_open()) {
    std::cerr << "Could not read file: " << filename << std::endl;
    return EXIT_FAILURE;
  }

  std::vector<std::string> maze;
  std::string line;

  while (getline(input_file, line)) {
    maze.push_back(line);
  }

  // Display the maze
  for (const auto &line : maze) {
    std::cout << line << std::endl;
  }

  // Solve it
  SolveMazeFast(maze);

  return EXIT_SUCCESS;
}