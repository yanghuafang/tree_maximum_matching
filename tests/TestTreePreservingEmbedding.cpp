#include "TreeMatchingTestHelper.hpp"
#include "TreePreservingEmbedding.hpp"

int main() {
  // Generate Tree A first.
  std::vector<Node<float>> treeA = generateTreeA<float>();
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

  return 0;
}