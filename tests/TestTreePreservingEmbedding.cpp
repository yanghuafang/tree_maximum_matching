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
  parser.add_argument("--rotate")
      .default_value(false)
      .implicit_value(true)
      .help("clockwise rotate 90 degrees");

  try {
    parser.parse_args(argc, argv);
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  std::string treeJson = parser.get<std::string>("--tree");
  bool rotate = parser.get<bool>("--rotate");

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

    // Convert point from vehicle coordinate system(x->forward, y->left) to
    // nomal coordinate system(x->right, y->forward).
    if (rotate) {
      clockwiseRotate90Degrees(treeA);
    }
    printTree(treeA, "treeA");
    visualizeTree(treeA, "treeA", "red", 1, block);

    std::vector<TreeNode<float>> sortedTreeA;
    std::vector<int> sortedTreeAIndices;
    sortTree(treeA, sortedTreeA, sortedTreeAIndices);
    printTree(sortedTreeA, "sortedTreeA");
    visualizeTree(sortedTreeA, "sortedTreeA", "red", 2, block);

    // Generate Tree B by adding drifts.
    std::vector<TreeNode<float>> treeB = generateTreeB<float>(treeA);
    // Convert point from vehicle coordinate system(x->forward, y->left) to
    // nomal coordinate system(x->right, y->forward).
    if (rotate) {
      clockwiseRotate90Degrees(treeB);
    }
    printTree(treeB, "treeB");
    visualizeTree(treeB, "treeB", "blue", 3, block);

    std::vector<TreeNode<float>> sortedTreeB;
    std::vector<int> sortedTreeBIndices;
    sortTree(treeB, sortedTreeB, sortedTreeBIndices);
    printTree(sortedTreeB, "sortedTreeB");
    visualizeTree(sortedTreeB, "sortedTreeB", "blue", 4, block);

    // Generate TPE of treeA.
    generateTreePreservingEmbedding(sortedTreeA);
    printTreePreservingEmbedding(sortedTreeA, "sortedTreeA");

    // Generate TPE of treeB.
    generateTreePreservingEmbedding(sortedTreeB);
    printTreePreservingEmbedding(sortedTreeB, "sortedTreeB");

    // Display treeA TPE.
    visualizeTreePreservingEmbedding(sortedTreeA, "sortedTreeA", 5, block);

    // Display treeB TPE.
    visualizeTreePreservingEmbedding(sortedTreeB, "sortedTreeB", 6, block);
  } else {
    std::vector<TreeNode<float>> tree;
    if (!loadTreeFromJson(tree, treeJson)) {
      std::cerr << "Failed to load tree from json file " << treeJson
                << std::endl;
      return -2;
    }

    // Convert point from vehicle coordinate system(x->forward, y->left) to
    // nomal coordinate system(x->right, y->forward).
    if (rotate) {
      clockwiseRotate90Degrees(tree);
    }
    printTree(tree, "tree");
    visualizeTree(tree, "tree", "red", 1, block);

    std::vector<TreeNode<float>> sortedTree;
    std::vector<int> sortedTreeIndices;
    sortTree(tree, sortedTree, sortedTreeIndices);
    printTree(sortedTree, "sortedTree");
    visualizeTree(sortedTree, "sortedTree", "red", 2, block);

    generateTreePreservingEmbedding(sortedTree);
    printTreePreservingEmbedding(sortedTree, "sortedTree");
    visualizeTreePreservingEmbedding(sortedTree, "sortedTree", 3, block);
  }

  if (!block) {
    pltShow();
  }

  return 0;
}