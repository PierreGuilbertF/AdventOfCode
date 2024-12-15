#include <cmath>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <vector>

const double kShift = 10000000000000;
const double kEpsilon = 1e-8;
const int kCostA = 3;
const int kCostB = 1;
#define INTERGER_TYPE uint64_t

void DisplayProblem(const std::vector<std::pair<int, int>> & prizes, const std::vector<std::pair<int, int>> & button_a, const std::vector<std::pair<int, int>> & button_b) {
  for (int k = 0; k < prizes.size(); ++k) {
    std::cout << "Prize: [" << prizes[k].first << ", " << prizes[k].second << "] ";
    std::cout << "A: [" << button_a[k].first << ", " << button_a[k].second << "] ";
    std::cout << "B: [" << button_b[k].first << ", " << button_b[k].second << "] ";
    std::cout << std::endl;
  }
}

void DisplaySolution(const std::vector<std::pair<double, double>> & prizes, const std::vector<std::pair<double, double>> & button_a, const std::vector<std::pair<double, double>> & button_b, const std::vector<std::pair<double, double>> & solutions) {
  for (int k = 0; k < prizes.size(); ++k) {
    std::cout << "=================================" << std::endl;
    std::cout << "Prize: [" << prizes[k].first << ", " << prizes[k].second << "] ";
    std::cout << "A: [" << button_a[k].first << ", " << button_a[k].second << "] ";
    std::cout << "B: [" << button_b[k].first << ", " << button_b[k].second << "] ";
    std::cout << std::endl;
    std::cout << "solution: [" << solutions[k].first << ", " << solutions[k].second << "]" << std::endl;
  }
}

void ParseInput(std::ifstream & file, std::vector<std::pair<double, double>> & prizes, std::vector<std::pair<double, double>> & button_a, std::vector<std::pair<double, double>> & button_b) {
  std::string line;
    while (getline(file, line)) {
      std::pair<int, int> prize, a, b;
      if (line.find("Button A") != std::string::npos) {
          sscanf(line.c_str(), "Button A: X+%d, Y+%d", &a.first, &a.second);
          button_a.push_back(a);
      } else if (line.find("Button B") != std::string::npos) {
          sscanf(line.c_str(), "Button B: X+%d, Y+%d", &b.first, &b.second);
          button_b.push_back(b);
      } else if (line.find("Prize") != std::string::npos) {
          sscanf(line.c_str(), "Prize: X=%d, Y=%d", &prize.first, &prize.second);
          std::pair<double, double> double_prize(prize.first + kShift, prize.second + kShift);
          prizes.push_back(double_prize);
      }
    }
}

std::pair<double, double> normalize(const std::pair<double, double> & vector) {
  const double norm = std::sqrt(vector.first * vector.first + vector.second * vector.second);
  if (norm < kEpsilon) return vector;
  return std::make_pair(vector.first / norm, vector.second / norm);
}

double norm(const std::pair<double, double> & vector) {
  return std::sqrt(vector.first * vector.first + vector.second * vector.second);
}

double cross_product(const std::pair<double, double> & u, const std::pair<double, double> & v) {
  return u.first * v.second - v.first * u.second;
}

bool IsInteger(double value) {
  return std::abs(value - std::round(value)) < kEpsilon;
}

std::pair<uint64_t, int> ComputeCost(const std::vector<std::pair<double, double>> & solutions) {
  uint64_t cost = 0;
  int prizes_won = 0;
  for (const auto & solution : solutions) {
    if (IsInteger(solution.first) && IsInteger(solution.second)) {
      cost += kCostA * solution.first + kCostB * solution.second;
      prizes_won++;
    }
  }
  return std::make_pair(cost, prizes_won);
}

std::vector<std::pair<double, double>> GetCoordinate(const std::vector<std::pair<double, double>> & prizes, const std::vector<std::pair<double, double>> & button_a, const std::vector<std::pair<double, double>> & button_b) {
  std::vector<std::pair<double, double>> coordinates;
  for (int k = 0; k < prizes.size(); ++k) {
    // different scenarios:
    // - vector A and vector B are parallel and aligned with prizes
    // - vector A and vector B are parallel but not aligned with prizes
    // - vector A and vector form a Basis
    const std::pair<double, double> u = button_a[k];
    const std::pair<double, double> v = button_b[k];
    const std::pair<double, double> y = prizes[k];
    const auto normalized_u = normalize(u);
    const auto normalized_v = normalize(v);
    const auto normalized_y = normalize(y);

    // check if they are aligned or one of the two vector is zero
    const double det = cross_product(u, v);
    if(std::abs(cross_product(normalized_u, normalized_v)) < kEpsilon) {
      std::pair<double, double> x(0.0, 0.0);
      // check if u and y are aligned
      if (cross_product(normalized_u, normalized_y) < kEpsilon && norm(u) > kEpsilon) {
        x.first = (y.first / u.first + y.second / u.second) / 2.0;
      }

      // check if v and y are aligned
      if (cross_product(normalized_v, normalized_y) < kEpsilon && norm(v) > kEpsilon) {
        x.second = (y.first / v.first + y.second / v.second) / 2.0;
      }

      // if only one or none are valid, continue
      if (x.first < kEpsilon || x.second < kEpsilon) {
        coordinates.push_back(x);
        continue;
      }

      // if one is integer and not the other one, keep the integer one
      if (IsInteger(x.first) && !IsInteger(x.second)) {
        x.second = 0.0;
        coordinates.push_back(x);
        continue;
      }

      if (IsInteger(x.second) && !IsInteger(x.first)) {
        x.first = 0.0;
        coordinates.push_back(x);
        continue;
      }

      // Otherwise, keep the cheapest one
      const double cost_a = kCostA * x.first;
      const double cost_b = kCostB * x.second;
      if (cost_a < cost_b) {
        x.second = 0.0;
      } else {
        x.first = 0.0;
      }
      coordinates.push_back(x);
    } else {
      // u and v are a basis of R2, just solve the problem
      std::pair<double, double> x;
      x.first = (v.second * y.first - v.first * y.second) / det;
      x.second = (-u.second * y.first + u.first * y.second) / det;
      coordinates.push_back(x);
    }
  }
  return coordinates;
}

int main(int argc, char **argv) {
  // Read input file
  const std::string filename = "13/input.txt";
  std::ifstream input_file(filename);
  if (!input_file.is_open()) {
    std::cerr << "Could not read file: " << filename << std::endl;
    return EXIT_FAILURE;
  }

  std::vector<std::pair<double, double>> prizes, button_a, button_b;
  ParseInput(input_file, prizes, button_a, button_b);

  std::vector<std::pair<double, double>> coordinates = GetCoordinate(prizes, button_a, button_b);
  const std::pair<uint64_t, int> total_cost = ComputeCost(coordinates);

  std::cout << "Total token used: " << total_cost.first << " to win: " << total_cost.second << " prizes" << std::endl;

  return EXIT_SUCCESS;
}