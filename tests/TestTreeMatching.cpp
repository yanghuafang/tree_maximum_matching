#include "TreeMatching.hpp"
#include "TreeMatchingTestHelper.hpp"

int main() {
  // Generate Tree A first.
  std::vector<Node<float>> treeA = generateTreeA<float>();
  printTree(treeA, "treeA");

  // Generate Tree B by adding drifts.
  std::vector<Node<float>> treeB = generateTreeB<float>(treeA);
  printTree(treeB, "treeB");

  std::vector<int> matching = matchTrees(treeA, treeB);
  printMatching(matching, "treeA", "treeB");

  return 0;
}