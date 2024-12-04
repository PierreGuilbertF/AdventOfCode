#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

int CountXmasInString(const std::string &line) {
  if (line.size() < 4)
    return 0;

  int xmas_found = 0;
  for (int k = 0; k < line.size() - 3; ++k) {
    if (line[k] == 'X' && line[k + 1] == 'M' && line[k + 2] == 'A' &&
        line[k + 3] == 'S')
      xmas_found++;
  }
  return xmas_found;
}

std::string InverseString(const std::string &line) {
  std::string reverse_line;
  for (int k = 0; k < line.size(); ++k)
    reverse_line.push_back(line[line.size() - 1 - k]);
  return reverse_line;
}

bool IsWordXmas(const std::string &word) {
  return word[0] == 'M' && word[1] == 'A' && word[2] == 'S';
}

int main(int argc, char **argv) {
  // Read input file
  const std::string filename = "4/input.txt";
  std::ifstream input_file(filename);
  if (!input_file.is_open()) {
    std::cerr << "Could not read file: " << filename << std::endl;
    return EXIT_FAILURE;
  }

  // Parse input
  std::vector<std::string> array_of_words;
  std::string line;
  while (std::getline(input_file, line)) {
    std::stringstream line_data;
    line_data << line;
    array_of_words.push_back(line_data.str());
  }
  input_file.close();

  // We need to list all possible line:
  // 1 - vertical forward
  // 2 - vertical backward
  // 3 - horizontale forward
  // 4 - horizontal backward
  // 5 - diagonal forward
  // 6 - diagonal backward
  // 7 - antidiagonal forward
  // 8 - antidiagonal backward
  const int N = array_of_words.size();
  int total_xmas_found = 0;
  for (int i = 0; i < array_of_words.size(); ++i) {
    // vertical
    std::string vertical;
    // horizontal
    std::string horizontal;
    // diagonal
    std::string diagonal1;
    std::string diagonal2;
    // antidiagonal
    std::string antidiagonal1;
    std::string antidiagonal2;

    for (int j = 0; j < array_of_words.size(); ++j) {
      // vertical
      vertical.push_back(array_of_words[j][i]);
      // horizontal
      horizontal.push_back(array_of_words[i][j]);
      // diagonal
      if (i + j < N) {
        diagonal1.push_back(array_of_words[j][i + j]);
        diagonal2.push_back(array_of_words[i + j][j]);
      }
      // antidiagonal
      if (i + j < N) {
        antidiagonal1.push_back(array_of_words[N - 1 - j][i + j]);
      }
      if (i - j >= 0) {
        antidiagonal2.push_back(array_of_words[i - j][j]);
      }
    }

    total_xmas_found += CountXmasInString(vertical);
    total_xmas_found += CountXmasInString(InverseString(vertical));
    total_xmas_found += CountXmasInString(horizontal);
    total_xmas_found += CountXmasInString(InverseString(horizontal));
    total_xmas_found += CountXmasInString(diagonal1);
    total_xmas_found += CountXmasInString(InverseString(diagonal1));
    total_xmas_found += CountXmasInString(diagonal2);
    total_xmas_found += CountXmasInString(InverseString(diagonal2));
    total_xmas_found += CountXmasInString(antidiagonal1);
    total_xmas_found += CountXmasInString(InverseString(antidiagonal1));
    total_xmas_found += CountXmasInString(antidiagonal2);
    total_xmas_found += CountXmasInString(InverseString(antidiagonal2));

    // if i == 0 then diagonal1 == diagonal2
    if (i == 0) {
      total_xmas_found -= CountXmasInString(diagonal2);
      total_xmas_found -= CountXmasInString(InverseString(diagonal2));
    }

    // if i = N - 1 then antidiagonal2 == antidiagonal 1 when i == 0
    if (i == N - 1) {
      total_xmas_found -= CountXmasInString(antidiagonal2);
      total_xmas_found -= CountXmasInString(InverseString(antidiagonal2));
    }
  }

  // Second part
  // Unfortunately, my solution does not extend well to the second part,
  // so we will just create a method using a sliding window to check the pattern
  int count_x_mas = 0;
  for (int i = 1; i < N - 1; ++i) {
    for (int j = 1; j < N - 1; ++j) {
      const std::string line1 = {array_of_words[i - 1][j - 1],
                                 array_of_words[i][j],
                                 array_of_words[i + 1][j + 1]};
      const std::string line2 = {array_of_words[i + 1][j - 1],
                                 array_of_words[i][j],
                                 array_of_words[i - 1][j + 1]};
      const bool xmas_1 = IsWordXmas(line1) || IsWordXmas(InverseString(line1));
      const bool xmas_2 = IsWordXmas(line2) || IsWordXmas(InverseString(line2));
      if (xmas_1 && xmas_2)
        count_x_mas++;
    }
  }

  std::cout << "Found: " << total_xmas_found << " XMAS" << std::endl;
  std::cout << "Found: " << count_x_mas << " X-MAS" << std::endl;
  return EXIT_SUCCESS;
}