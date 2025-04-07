#include <iostream>

#include "HungarianAlgorithm.hpp"

int main() {
  std::vector<std::vector<float>> cost = {
      {4.0, 2.0, 8.0}, {4.0, 3.0, 7.0}, {3.0, 1.0, 6.0}};

  auto result = hungarianAlgorithm(cost);
  float totalCost = result.first;
  const std::vector<int>& assignment = result.second;

  std::cout << "Total minimum cost: " << totalCost << "\n";
  std::cout << "Assignments (row -> column):\n";
  for (size_t i = 0; i < assignment.size(); ++i) {
    std::cout << "  Row " << i << " -> Column " << assignment[i] << "\n";
  }

  return 0;
}