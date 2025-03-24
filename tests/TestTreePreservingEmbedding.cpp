#include "TreeMatchingTestHelper.hpp"
#include "TreePreservingEmbedding.hpp"
#include "TreePreservingEmbeddingVisualizer.hpp"

int main() {
  // Define the tree structure of nodeIdx : childrenIndices.
  // Note: Using 0-indexing: Node 0 is at index 0.
  // Structure:
  //   node 0: nodes 1 and 2
  //   node 1: none
  //   node 2: nodes 3,4 and 5
  //   node 3: none
  //   node 4: nodes 6 and 7
  //   node 5: none
  //   node 6: none
  //   node 7: none
  std::vector<std::vector<int>> treeStructure = {
      {1, 2},     // Node 0
      {},         // Node 1
      {3, 4, 5},  // Node 2
      {},         // Node 3
      {6, 7},     // Node 4
      {},         // Node 5
      {},         // Node 6
      {},         // Node 7
  };

  // Generate Tree A first.
  std::vector<Node<float>> treeA = generateTreeA<float>(treeStructure);
  printTree(treeA, "treeA");

  // Generate Tree B by adding drifts.
  std::vector<Node<float>> treeB = generateTreeB<float>(treeA);
  printTree(treeB, "treeB");

  // Generate TPE of treeA.
  generateTreePreservingEmbedding(treeA);
  printTreePreservingEmbedding(treeA, "treeA");

  // Generate TPE of treeB.
  generateTreePreservingEmbedding(treeB);
  printTreePreservingEmbedding(treeB, "treeB");

  // Display treeA TPE.
  visualizeTreePreservingEmbedding(treeA, "treeA");

  // Display treeB TPE.
  visualizeTreePreservingEmbedding(treeB, "treeB");

  return 0;
}