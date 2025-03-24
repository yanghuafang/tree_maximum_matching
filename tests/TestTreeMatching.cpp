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

  // Generate Tree A first.
  std::vector<Node<float>> treeA = generateTreeA<float>(treeStructure);
  printTree(treeA, "treeA");

  // Generate Tree B by adding drifts.
  std::vector<Node<float>> treeB = generateTreeB<float>(treeA);
  printTree(treeB, "treeB");

  std::vector<int> matchRes = matchTrees(treeA, treeB);
  printMatching(matchRes, "treeA", "treeB");

  // Set parameters for edge colors and matching line color.
  std::string treeAEdgeColor = "red";
  std::string treeBEdgeColor = "blue";
  std::string matchLineColor = "green";

  // Visualize the trees and their matching.
  visualizeTreesMatching(treeA, treeB, matchRes, treeAEdgeColor, treeBEdgeColor,
                         matchLineColor);

  return 0;
}