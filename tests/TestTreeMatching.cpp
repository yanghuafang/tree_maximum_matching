#include <argparse/argparse.hpp>
#include <chrono>
#include <iostream>

#include "TreeLoader.hpp"
#include "TreeMatching.hpp"
#include "TreeMatchingTestHelper.hpp"
#include "TreeMatchingVisualizer.hpp"

int main(int argc, char* argv[]) {
  argparse::ArgumentParser parser("tree_maximum_matching");
  parser.add_argument("--tree1").default_value("").help("json file of tree1");
  parser.add_argument("--tree2").default_value("").help("json file of tree2");
  parser.add_argument("--output-tree1")
      .default_value("")
      .help("output json file of tree1");
  parser.add_argument("--output-tree2")
      .default_value("")
      .help("output json file of tree2");
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

  std::string tree1json = parser.get<std::string>("--tree1");
  std::string tree2json = parser.get<std::string>("--tree2");
  bool rotate = parser.get<bool>("--rotate");

  TreeWrapper<float> treeA;
  TreeWrapper<float> treeB;

  if (tree1json.empty() && tree2json.empty()) {
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
    treeA = generateTreeA<float>(treeStructure);
    printTree(treeA, "treeA");

    // Generate Tree B by adding drifts.
    treeB = generateTreeB<float>(treeA);
    printTree(treeB, "treeB");
  } else {
    if (!loadTreeFromJson(treeA, tree1json)) {
      std::cerr << "Failed to load tree1 from json file " << tree1json
                << std::endl;
      return -2;
    }

    std::cout << "Succeed to load treeA of timestamp " << treeA.timestamp
              << " from json file " << tree1json << std::endl;

    // Convert point from vehicle coordinate system(x->forward, y->left) to
    // nomal coordinate system(x->right, y->forward).
    if (rotate) {
      clockwiseRotate90Degrees(treeA);
    }
    printTree(treeA, "treeA");

    if (!loadTreeFromJson(treeB, tree2json)) {
      std::cerr << "Failed to load tree2 from json file " << tree2json
                << std::endl;
      return -3;
    }

    std::cout << "Succeed to load treeB of timestamp " << treeB.timestamp
              << " from json file " << tree2json << std::endl;

    // Convert point from vehicle coordinate system(x->forward, y->left) to
    // nomal coordinate system(x->right, y->forward).
    if (rotate) {
      clockwiseRotate90Degrees(treeB);
    }
    printTree(treeB, "treeB");
  }

  bool block = false;

  TreeWrapper<float> sortedTreeA;
  std::vector<int> sortedTreeAIndices;
  sortTree(treeA, sortedTreeA, sortedTreeAIndices);

  visualizeTree(treeA, "treeA", "red", 1, block);
  visualizeTree(sortedTreeA, "sortedTreeA", "red", 2, block);

  TreeWrapper<float> sortedTreeB;
  std::vector<int> sortedTreeBIndices;
  sortTree(treeB, sortedTreeB, sortedTreeBIndices);

  visualizeTree(treeB, "treeB", "blue", 3, block);
  visualizeTree(sortedTreeB, "sortedTreeB", "blue", 4, block);

  // Cosine match
  auto start = std::chrono::high_resolution_clock::now();
  std::vector<int> cosMatchRes = matchTrees(sortedTreeA, sortedTreeB);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout << "matchTrees spent " << duration.count() << " microseconds!"
            << std::endl;
  printMatching(cosMatchRes, "sortedTreeA", "sortedTreeB",
                sortedTreeA.timestamp, sortedTreeB.timestamp);

  // Set parameters for edge colors and matching line color.
  std::string treeAEdgeColor = "red";
  std::string treeBEdgeColor = "blue";
  std::string matchLineColor = "green";

  // Visualize the trees and their cosine matching.
  visualizeTreesMatching(sortedTreeA, sortedTreeB, cosMatchRes, "cosine",
                         treeAEdgeColor, treeBEdgeColor, matchLineColor, 5,
                         block);

  // Euclidean match
  std::vector<int> euclideanMatchRes =
      matchTrees(sortedTreeA, sortedTreeB, "euclidean");
  printMatching(euclideanMatchRes, "sortedTreeA", "sortedTreeB",
                sortedTreeA.timestamp, sortedTreeB.timestamp);

  // Visualize the trees and their euclidean matching.
  visualizeTreesMatching(sortedTreeA, sortedTreeB, euclideanMatchRes,
                         "euclidean", treeAEdgeColor, treeBEdgeColor,
                         matchLineColor, 6, block);

  // Save tree1 and tree2.
  std::string outputTree1json = parser.get<std::string>("--output-tree1");
  std::string outputTree2json = parser.get<std::string>("--output-tree2");
  if (!outputTree1json.empty()) {
    if (!saveTreeToJson(sortedTreeA, outputTree1json)) {
      std::cerr << "Failed to save tree1 to json file " << outputTree1json
                << std::endl;
      return -4;
    }
  }

  if (!outputTree2json.empty()) {
    if (!saveTreeToJson(sortedTreeB, outputTree2json)) {
      std::cerr << "Failed to save tree2 to json file " << outputTree2json
                << std::endl;
      return -5;
    }
  }

  if (!block) {
    pltShow();
  }

  return 0;
}