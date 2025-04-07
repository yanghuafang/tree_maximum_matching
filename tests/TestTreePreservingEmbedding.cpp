#include <argparse/argparse.hpp>
#include <iostream>

#include "TreeLoader.hpp"
#include "TreeMatchingTestHelper.hpp"
#include "TreeMatchingVisualizer.hpp"
#include "TreePreservingEmbedding.hpp"
#include "TreePreservingEmbeddingVisualizer.hpp"

int main(int argc, char* argv[]) {
  argparse::ArgumentParser parser("tree_maximum_matching");
  parser.add_argument("--tree").default_value("").help("json file of tree");

  try {
    parser.parse_args(argc, argv);
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  std::string treeJson = parser.get<std::string>("--tree");

  bool block = false;

  if (treeJson.empty()) {
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
    std::vector<TreeNode<float>> treeA = generateTreeA<float>(treeStructure);
    printTree(treeA, "treeA");

    // Generate Tree B by adding drifts.
    std::vector<TreeNode<float>> treeB = generateTreeB<float>(treeA);
    printTree(treeB, "treeB");

    // Generate TPE of treeA.
    generateTreePreservingEmbedding(treeA);
    printTreePreservingEmbedding(treeA, "treeA");

    // Generate TPE of treeB.
    generateTreePreservingEmbedding(treeB);
    printTreePreservingEmbedding(treeB, "treeB");

    // Display treeA TPE.
    visualizeTreePreservingEmbedding(treeA, "treeA", 1, block);

    // Display treeB TPE.
    visualizeTreePreservingEmbedding(treeB, "treeB", 2, block);
  } else {
    std::vector<TreeNode<float>> tree;
    if (!loadTreeFromJson(tree, treeJson)) {
      std::cerr << "Failed to load tree from json file " << treeJson
                << std::endl;
      return -2;
    }
    printTree(tree, "tree");

    generateTreePreservingEmbedding(tree);
    printTreePreservingEmbedding(tree, "tree");
    visualizeTreePreservingEmbedding(tree, "tree", 1, block);
  }

  if (!block) {
    pltShow();
  }

  return 0;
}