#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#define INTEGER_TYPE uint64_t

const int kNumberOperation = 3;

enum Operation {
  addition = 1,
  multiplication = 2,
  concatenation = 3,
  unknown = 4
};

Operation IndexToOperation(int operation_idx) {
  if (operation_idx == 0)
    return Operation::addition;
  if (operation_idx == 1)
    return Operation::multiplication;
  if (operation_idx == 2)
    return Operation::concatenation;
  return Operation::unknown;
}

void UpdateOperation(std::vector<int> &per_location_operation_idx) {
  // We count in base cardinal(Operations). Increase the last element and
  // increase next operator if we are above the #operations
  per_location_operation_idx[per_location_operation_idx.size() - 1]++;
  for (int k = per_location_operation_idx.size() - 1; k >= 1; --k) {
    if (per_location_operation_idx[k] >= kNumberOperation) {
      per_location_operation_idx[k] = 0;
      per_location_operation_idx[k - 1]++;
    }
  }
}

std::vector<std::vector<int>>
FindMultiplicationChain(const std::vector<int> &per_location_operation_idx) {
  std::vector<std::vector<int>> chains;
  std::vector<int> current_chain(0);
  for (int k = 0; k < per_location_operation_idx.size(); ++k) {
    if (IndexToOperation(per_location_operation_idx[k]) ==
        Operation::multiplication) {
      current_chain.push_back(k);
    } else if (IndexToOperation(per_location_operation_idx[k]) ==
               Operation::addition) {
      if (current_chain.size() > 0)
        chains.push_back(current_chain);
      current_chain.clear();
      current_chain.resize(0);
    }
  }
  if (current_chain.size() > 0)
    chains.push_back(current_chain);
  return chains;
}

void PrintOperation(const std::vector<INTEGER_TYPE> &operands,
                    const std::vector<int> &per_location_operation_idx) {
  std::cout << operands[0];
  for (int k = 0; k < per_location_operation_idx.size(); ++k) {
    if (IndexToOperation(per_location_operation_idx[k]) ==
        Operation::multiplication) {
      std::cout << " * ";
    } else if (IndexToOperation(per_location_operation_idx[k]) ==
               Operation::addition) {
      std::cout << " + ";
    } else if (IndexToOperation(per_location_operation_idx[k]) ==
               Operation::concatenation) {
      std::cout << "||";
    }
    std::cout << operands[k + 1];
  }
  std::cout << std::endl;
}

INTEGER_TYPE ComputeOperationsNoMathOrder(
    INTEGER_TYPE result, const std::vector<INTEGER_TYPE> &operands,
    const std::vector<int> &per_location_operation_idx) {
  const int operation_location = operands.size() - 1;
  INTEGER_TYPE operation_result = operands[0];
  for (int k = 0; k < operation_location; ++k) {
    if (IndexToOperation(per_location_operation_idx[k]) ==
        Operation::multiplication) {
      operation_result *= operands[k + 1];
    } else if (IndexToOperation(per_location_operation_idx[k]) ==
               Operation::addition) {
      operation_result += operands[k + 1];
    } else if (IndexToOperation(per_location_operation_idx[k]) ==
               Operation::concatenation) {
      // Converting both numbers to string and then concatenating the string
      // instead of concatenating them using base 10 operations
      const std::string num_1 = std::to_string(operation_result);
      const std::string num_2 = std::to_string(operands[k + 1]);
      const std::string concatenated = num_1 + num_2;
      operation_result = std::stoull(concatenated);
    }
  }

  if (result == operation_result) {
    std::cout << "result: " << result << " = ";
    PrintOperation(operands, per_location_operation_idx);
  }

  return operation_result;
}

INTEGER_TYPE
ComputeOperations(INTEGER_TYPE result,
                  const std::vector<INTEGER_TYPE> &operands,
                  const std::vector<int> &per_location_operation_idx) {
  const int operation_location = operands.size() - 1;
  // We will compute the opration and we need to ensure
  // to respect the math operations order
  // First add the multiplication
  std::vector<std::vector<int>> chain_of_multiplication =
      FindMultiplicationChain(per_location_operation_idx);

  INTEGER_TYPE operation_result = 0;
  std::vector<bool> already_processed(operands.size(), false);
  for (const auto &chain : chain_of_multiplication) {
    INTEGER_TYPE current_multiplication = operands[chain[0]];
    already_processed[chain[0]] = true;
    for (const auto &index : chain) {
      current_multiplication = current_multiplication * operands[index + 1];
      already_processed[index + 1] = true;
    }
    // std::cout << "current mult: " << current_multiplication << std::endl;
    operation_result += current_multiplication;
  }
  // Finally add the additions
  for (int k = 0; k < already_processed.size(); ++k) {
    if (!already_processed[k])
      operation_result += operands[k];
  }

  return operation_result;
}

void PrintOperationCode(const std::vector<int> &per_location_operation_idx) {
  std::cout << "[";
  for (const auto idx : per_location_operation_idx) {
    std::cout << idx << ", ";
  }
  std::cout << "]" << std::endl;
}

bool CheckIfEquationCanBeTrue(INTEGER_TYPE result,
                              const std::vector<INTEGER_TYPE> &operands,
                              bool respect_operation_order) {
  const int operation_location = operands.size() - 1;
  // only one operand, check if the operand is equal to the result
  if (operation_location == 0) {
    return operands[0] == result;
  }

  // iterate over all possible operation at each location
  std::vector<int> per_location_operation_idx(operation_location, 0);
  while (true) {
    const INTEGER_TYPE operation_result =
        respect_operation_order
            ? ComputeOperations(result, operands, per_location_operation_idx)
            : ComputeOperationsNoMathOrder(result, operands,
                                           per_location_operation_idx);

    // We found the right combination of operands
    if (operation_result == result)
      return true;

    // Update operation combination
    UpdateOperation(per_location_operation_idx);
    // We have reached the end of the exhaustive try
    if (per_location_operation_idx[0] >= kNumberOperation)
      break;
  }
  return false;
}

int main(int argc, char **argv) {
  // Read input file
  const std::string filename = "7/input.txt";
  std::ifstream input_file(filename);
  if (!input_file.is_open()) {
    std::cerr << "Could not read file: " << filename << std::endl;
    return EXIT_FAILURE;
  }

  // Parse input
  std::vector<INTEGER_TYPE> results;
  std::vector<std::vector<INTEGER_TYPE>> operands;
  std::string line;
  while (std::getline(input_file, line)) {
    std::istringstream lineStream(line);
    INTEGER_TYPE result;
    char colon;
    // Parse left number and discard the colon
    lineStream >> result >> colon;
    results.push_back(result);

    std::vector<INTEGER_TYPE> operand;
    INTEGER_TYPE num;
    while (lineStream >> num) {
      operand.push_back(num);
    }
    operands.push_back(operand);
  }
  input_file.close();

  INTEGER_TYPE sum_of_valid_operation = 0;
  for (int k = 0; k < results.size(); ++k) {
    if (CheckIfEquationCanBeTrue(results[k], operands[k], false)) {
      INTEGER_TYPE old = sum_of_valid_operation;
      sum_of_valid_operation += results[k];
    }
  }

  std::cout << "sum of valid operations: " << sum_of_valid_operation
            << std::endl;

  return EXIT_SUCCESS;
}