#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

bool AreAdjacentLevelsSafe(int level_1, int level_2, bool increasing) {
  if (increasing && level_2 <= level_1) {
    return false;
  } else if (!increasing && level_1 <= level_2) {
    return false;
  } else if (std::abs(level_2 - level_1) > 3) {
    return false;
  }
  return true;
}

void EvaluateErrors(const std::vector<int> &report,
                    const std::vector<int> &error_idx_if_increasing,
                    const std::vector<int> &error_idx_if_decreasing,
                    int &count_zero_error_reports,
                    int &count_one_error_reports) {
  const std::vector<int> smallest_error_idx =
      (error_idx_if_increasing.size() < error_idx_if_decreasing.size())
          ? error_idx_if_increasing
          : error_idx_if_decreasing;
  const bool increasing =
      (error_idx_if_increasing.size() < error_idx_if_decreasing.size()) ? true
                                                                        : false;
  // If no error, it is fine
  if (smallest_error_idx.size() == 0) {
    count_zero_error_reports++;
    count_one_error_reports++;
    return;
  } else {
    for (int index_to_remove = 0; index_to_remove < report.size();
         ++index_to_remove) {
      // Create new report without the index to remove
      std::vector<int> new_report;
      for (int k = 0; k < report.size(); ++k) {
        if (k == index_to_remove)
          continue;
        new_report.push_back(report[k]);
      }

      // Loop over the report
      std::vector<int> new_report_error_idx_if_increasing;
      std::vector<int> new_report_error_idx_if_decreasing;
      for (int k = 0; k < new_report.size() - 1; ++k) {
        if (!AreAdjacentLevelsSafe(new_report[k], new_report[k + 1], true))
          new_report_error_idx_if_increasing.push_back(k);
        if (!AreAdjacentLevelsSafe(new_report[k], new_report[k + 1], false))
          new_report_error_idx_if_decreasing.push_back(k);
      }
      if (new_report_error_idx_if_increasing.size() == 0 ||
          new_report_error_idx_if_decreasing.size() == 0) {
        count_one_error_reports++;
        return;
      }
    }
  }
}

int main(int argc, char **argv) {
  // Read input file
  const std::string filename = "2/input.txt";
  std::ifstream input_file(filename);
  if (!input_file.is_open()) {
    std::cerr << "Could not read file: " << filename << std::endl;
    return EXIT_FAILURE;
  }

  // Parse input
  std::vector<std::vector<int>> reports;
  std::string line;
  while (std::getline(input_file, line)) {
    std::vector<int> current_report;
    std::istringstream iss(line);
    int number;
    while (iss >> number) {
      current_report.push_back(number);
    }
    reports.push_back(current_report);
  }
  input_file.close();

  // Check if reports are safe
  int count_zero_error_reports = 0;
  int count_one_error_reports = 0;
  for (const auto &report : reports) {
    // skip report of only one value
    if (report.size() < 2) {
      count_zero_error_reports++;
      count_one_error_reports++;
      continue;
    }

    // Loop over the reports
    std::vector<int> error_idx_if_increasing, error_idx_if_decreasing;
    for (int k = 0; k < report.size() - 1; ++k) {
      if (!AreAdjacentLevelsSafe(report[k], report[k + 1], true))
        error_idx_if_increasing.push_back(k);
      if (!AreAdjacentLevelsSafe(report[k], report[k + 1], false))
        error_idx_if_decreasing.push_back(k);
    }

    EvaluateErrors(report, error_idx_if_increasing, error_idx_if_decreasing,
                   count_zero_error_reports, count_one_error_reports);
  }

  std::cout << "There are: " << count_zero_error_reports
            << " reports with no errors" << std::endl;
  std::cout << "There are: " << count_one_error_reports
            << " reports with less than one errors" << std::endl;

  return EXIT_SUCCESS;
}