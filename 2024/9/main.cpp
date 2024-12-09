#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

std::vector<std::string>
CreateExpandedMemory(const std::vector<int> &input_memory) {
  std::vector<std::string> expanded_memory;
  for (int k = 0; k < input_memory.size(); ++k) {
    const bool is_file = k % 2 == 0;
    const int id = k / 2;
    for (int i = 0; i < input_memory[k]; ++i) {
      if (is_file) {
        expanded_memory.push_back(std::to_string(id));
      } else {
        expanded_memory.push_back(std::string(1, '.'));
      }
    }
  }
  return expanded_memory;
}

int FindNextFreeLocationIdx(const std::vector<std::string> &expanded_memory) {
  for (int k = 0; k < expanded_memory.size(); ++k) {
    if (expanded_memory[k] == std::string(1, '.'))
      return k;
  }
  return expanded_memory.size() - 1;
}

void PrintExpandedMemory(const std::vector<std::string> &expanded_memory) {
  for (const auto &value : expanded_memory) {
    std::cout << value;
  }
  std::cout << std::endl;
}

void SortMemoryFileByFile(std::vector<std::string> &expanded_memory) {
  int next_free_location_idx = 0;
  while (true) {
    if (expanded_memory[expanded_memory.size() - 1] == std::string(1, '.')) {
      expanded_memory.pop_back();
    } else {
      next_free_location_idx = FindNextFreeLocationIdx(expanded_memory);
      if (next_free_location_idx >= expanded_memory.size() - 1)
        break;
      expanded_memory[next_free_location_idx] =
          expanded_memory[expanded_memory.size() - 1];
      expanded_memory.pop_back();
    }
  }
}

std::pair<int, int>
FindNextBlockToMove(const std::vector<std::string> &expanded_memory,
                    int from_index) {
  int k = from_index;
  // skip "." values
  while (expanded_memory[k] == "." && k >= 0)
    k--;

  std::pair<int, int> block;
  block.second = k;
  const std::string block_value = expanded_memory[block.second];
  for (; k >= 0; --k) {
    if (expanded_memory[k] != block_value) {
      break;
    }
    block.first = k;
  }
  return block;
}

std::pair<int, int>
FindNextFreeLocationBlock(const std::vector<std::string> &expanded_memory,
                          int free_space_needed) {
  std::pair<int, int> block(0, 0);
  for (int k = 0; k < expanded_memory.size(); ++k) {
    if (expanded_memory[k] == ".") {
      block.second = k;
      int free_space_size = block.second - block.first;
      if (free_space_needed == free_space_size) {
        block.first++;
        return block;
      }
    } else {
      block.first = k;
    }
  }
  // nothing found, return default value;
  return std::make_pair(-1, -1);
}

void PrintBlock(const std::vector<std::string> &expanded_memory,
                const std::pair<int, int> &block) {
  for (int k = block.first; k <= block.second; ++k) {
    std::cout << expanded_memory[k] << " ";
  }
  std::cout << std::endl;
}

void RemoveElement(std::vector<std::string> &expanded_memory,
                   const std::pair<int, int> &block) {
  const auto old_memory = expanded_memory;
  expanded_memory.clear();
  expanded_memory.resize(0);
  for (int k = 0; k < old_memory.size(); ++k) {
    if (k >= block.first && k <= block.second)
      continue;
    expanded_memory.push_back(old_memory[k]);
  }
}

void ReplaceElement(std::vector<std::string> &expanded_memory,
                    const std::pair<int, int> &block) {
  for (int k = block.first; k <= block.second; ++k) {
    expanded_memory[k] = ".";
  }
}

void SortMemoryBlockByBlock(std::vector<std::string> &expanded_memory) {
  std::pair<int, int> next_free_block;
  // keep in mind last visited block
  std::pair<int, int> next_block_to_move(expanded_memory.size(),
                                         expanded_memory.size());
  int count = 0;
  while (next_block_to_move.second >= 0) {
    if (expanded_memory[next_block_to_move.second] == std::string(1, '.')) {
      next_block_to_move.second--;
    } else {
      next_block_to_move =
          FindNextBlockToMove(expanded_memory, next_block_to_move.first - 1);

      const int free_space_needed =
          next_block_to_move.second - next_block_to_move.first + 1;
      next_free_block =
          FindNextFreeLocationBlock(expanded_memory, free_space_needed);
      if (next_free_block.first == -1)
        continue;
      if (next_free_block.first >= next_block_to_move.second)
        continue;

      // moving next block to move to the found free location
      for (int k = 0;
           k < next_block_to_move.second - next_block_to_move.first + 1; ++k) {
        expanded_memory[next_free_block.first + k] =
            expanded_memory[next_block_to_move.first + k];
      }

      // replace the old elements
      ReplaceElement(expanded_memory, next_block_to_move);
    }
  }
}

uint64_t ComputeChecksum(const std::vector<std::string> &expanded_memory) {
  uint64_t checksum = 0;
  for (int k = 0; k < expanded_memory.size(); ++k) {
    try {
      const int number = std::stoi(expanded_memory[k]);
      checksum += static_cast<uint64_t>(number) * static_cast<uint64_t>(k);
    } catch (const std::invalid_argument &e) {
    } catch (const std::out_of_range &e) {
    }
  }
  return checksum;
}

int main(int argc, char **argv) {
  // Read input file
  const std::string filename = "9/input.txt";
  std::ifstream input_file(filename);
  if (!input_file.is_open()) {
    std::cerr << "Could not read file: " << filename << std::endl;
    return EXIT_FAILURE;
  }

  // Parse input
  std::vector<int> input_memory;
  std::string line;
  while (std::getline(input_file, line)) {
    std::stringstream line_data(line);
    for (int k = 0; k < line_data.str().size(); ++k) {
      const std::string int_str(1, line_data.str()[k]);
      input_memory.push_back(std::stoi(int_str));
    }
  }
  input_file.close();

  // Create the expanded memory
  std::vector<std::string> expanded_memory = CreateExpandedMemory(input_memory);
  std::cout << "memory size: " << input_memory.size() << std::endl;
  std::cout << "expanded memory size: " << expanded_memory.size() << std::endl;

  // Solve the problem one
  std::vector<std::string> memory_file_by_file = expanded_memory;
  SortMemoryFileByFile(memory_file_by_file);
  uint64_t checksum_1 = ComputeChecksum(memory_file_by_file);

  // Solve the problem two
  std::vector<std::string> memory_block_by_block = expanded_memory;
  SortMemoryBlockByBlock(memory_block_by_block);
  PrintExpandedMemory(memory_block_by_block);
  uint64_t checksum_2 = ComputeChecksum(memory_block_by_block);

  std::cout << "checksum is when sorted file by file: " << checksum_1
            << std::endl;
  std::cout << "checksum is when sorted block by block: " << checksum_2
            << std::endl;

  return EXIT_SUCCESS;
}