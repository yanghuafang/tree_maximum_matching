#include <Python.h>

#include <argparse/argparse.hpp>
#include <chrono>
#include <iostream>
#include <list>

#include "TreeLoader.hpp"
#include "TreeMatching.hpp"
#include "TreeMatchingTestHelper.hpp"
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

void visualizeTimeOfFrames(const std::list<float>& timeOfFrames,
                           const std::string& title) {
  std::vector<float> timeVector(timeOfFrames.begin(), timeOfFrames.end());

  std::vector<int> frameIndices(timeVector.size());
  for (size_t i = 0; i < timeVector.size(); ++i) {
    frameIndices[i] = static_cast<int>(i + 1);  // Frame indices start from 1
  }

  // Plot the time consumption
  plt::plot(frameIndices, timeVector);

  // Customize the plot
  plt::title("");
  plt::xlabel("Frame Index");
  plt::ylabel("Time (microseconds)");

  // Show the plot
  plt::show();
}

int main(int argc, char* argv[]) {
  argparse::ArgumentParser parser("tree_maximum_matching");
  parser.add_argument("--trees1").default_value("").help("json file of trees1");
  parser.add_argument("--trees2").default_value("").help("json file of trees2");

  try {
    parser.parse_args(argc, argv);
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  std::string trees1json = parser.get<std::string>("--trees1");
  std::string trees2json = parser.get<std::string>("--trees2");

  std::list<std::vector<TreeNode<float>>> treesA;
  std::list<std::vector<TreeNode<float>>> treesB;

  if (!loadTreesFromJson(treesA, trees1json)) {
    std::cerr << "Failed to load trees1 from json file " << trees1json
              << std::endl;
    return -2;
  }
  if (!loadTreesFromJson(treesB, trees2json)) {
    std::cerr << "Failed to load trees2 from json file " << trees2json
              << std::endl;
    return -3;
  }

  int size = 0;
  if (treesA.size() == treesB.size()) {
    size = treesA.size();
  } else {
    std::cerr << "treesA size " << treesA.size() << " != treesB size "
              << treesB.size() << std::endl;
    return -4;
  }

  std::list<float> timeOfFrames;
  std::list<std::vector<TreeNode<float>>>::iterator treesAIter = treesA.begin();
  std::list<std::vector<TreeNode<float>>>::iterator treesBIter = treesB.begin();

  // Cosine match
  while (treesAIter != treesA.end() && treesBIter != treesB.end()) {
    std::vector<TreeNode<float>>& treeA = *treesAIter;
    std::vector<TreeNode<float>>& treeB = *treesBIter;
    ++treesAIter;
    ++treesBIter;

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> cosMatchRes = matchTrees(treeA, treeB);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    timeOfFrames.push_back(duration.count());
    printMatching(cosMatchRes, "treeA", "treeB");
  }

  visualizeTimeOfFrames(timeOfFrames,
                        "Time consumption per frame of tree matching (cosine)");

  timeOfFrames.clear();
  treesAIter = treesA.begin();
  treesBIter = treesB.begin();

  // Euclidean match
  while (treesAIter != treesA.end() && treesBIter != treesB.end()) {
    std::vector<TreeNode<float>>& treeA = *treesAIter;
    std::vector<TreeNode<float>>& treeB = *treesBIter;
    ++treesAIter;
    ++treesBIter;

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> euclideanMatchRes = matchTrees(treeA, treeB, "euclidean");
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    timeOfFrames.push_back(duration.count());
    printMatching(euclideanMatchRes, "treeA", "treeB");
  }

  visualizeTimeOfFrames(
      timeOfFrames, "Time consumption per frame of tree matching (euclidean)");

  return 0;
}