#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char const *argv[]) {
  size_t n = 5;

  std::vector<int> data = {101, 102, 103, 104, 105};

  for (size_t k = 1; k < n; k++) {
    std::cout << "k=" << k << "\n";
    std::string mask(k, 'a');
    mask.resize(n, 'b');
    std::vector<int> selected;

    do {
      for (unsigned int i = 0; i < mask.size(); i++) {
        if (mask[i] == 'a') {
          selected.push_back(data[i]);
        }
      }

      for (unsigned int i = 0; i < mask.size(); i++) {
        std::cout << mask[i];
      }
      std::cout << "\n";
      for (unsigned int i = 0; i < selected.size(); i++) {
        std::cout << selected[i] << " ";
      }
      std::cout << "\n";
      selected.clear();

    } while (std::next_permutation(mask.begin(), mask.end()));
  }
  return 0;
}
