#include <chrono>
#include <iostream>

#include "TreeMatching.hpp"
#include "TreeMatchingTestHelper.hpp"
#include "TreeMatchingVisualizer.hpp"

int main() {
  // Define the tree structure of nodeIdx : childrenIndices.
  // Note: Using 0-indexing: Node 0 is at index 0.
  // Structure:
  //   node 0: nodes 1 and 2
  //   node 1: nodes 3 and 4
  //   node 2: nodes 5 and 6
  //   node 3: node 7
  //   node 4: none
  //   node 5: node 8 and 9
  //   node 6: none
  //   node 7: none
  //   node 8: none
  //   node 9: none
  std::vector<std::vector<int>> treeStructure = {
      {1, 2},  // node 0
      {3, 4},  // node 1
      {5, 6},  // node 2
      {7},     // node 3
      {},      // node 4
      {8, 9},  // node 5
      {},      // node 6
      {},      // node 7
      {},      // node 8
      {}       // node 9
  };

  // Set parameters for edge colors and matching line color.
  std::string treeAEdgeColor = "red";
  std::string treeBEdgeColor = "blue";
  std::string matchLineColor = "green";

  // Generate Tree A first.
  std::vector<Node<float>> treeA = generateTreeA<float>(treeStructure);
  printTree(treeA, "treeA");

  // Generate Tree B by adding drifts.
  std::vector<Node<float>> treeB = generateTreeB<float>(treeA);
  printTree(treeB, "treeB");

  // Cosine match
  auto start = std::chrono::high_resolution_clock::now();
  std::vector<int> cosMatchRes = matchTrees(treeA, treeB);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout << "matchTrees spent " << duration.count() << " microseconds!"
            << std::endl;
  printMatching(cosMatchRes, "treeA", "treeB");

  // Visualize the trees and their cosine matching.
  visualizeTreesMatching(treeA, treeB, cosMatchRes, "cosine", treeAEdgeColor,
                         treeBEdgeColor, matchLineColor);

  // Euclidean match
  std::vector<int> euclideanMatchRes = matchTrees(treeA, treeB, "euclidean");
  printMatching(euclideanMatchRes, "treeA", "treeB");

  // Visualize the trees and their euclidean matching.
  visualizeTreesMatching(treeA, treeB, euclideanMatchRes, "euclidean",
                         treeAEdgeColor, treeBEdgeColor, matchLineColor);

  return 0;
}