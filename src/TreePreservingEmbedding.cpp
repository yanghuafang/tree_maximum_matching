#include "TreePreservingEmbedding.hpp"

#include <cmath>
#include <iostream>
#include <limits>

// Define M_PI if it's not available
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Helper function to get the node level by following parent pointers.
template <typename T>
int getNodeLevel(const std::vector<Node<T>>& tree, int index) {
  int level = 0;
  int current = index;
  while (tree[current].parent != -1) {
    level++;
    current = tree[current].parent;
  }
  return level;
}

// Function to generate TPE(Topology Tree Preserving Embedding) for nodes in the
// tree.

// The embedding is computed via a radial layout, where nodes in the same level
// are evenly distributed around a circle of radius (level+1)*5.
template <typename T>
void generateTreePreservingEmbedding(std::vector<Node<T>>& tree) {
  int n = tree.size();
  std::vector<int> nodeLevels(n, 0);
  int maxLevel = 0;

  // Determine the level for each node.
  for (int i = 0; i < n; i++) {
    int lvl = getNodeLevel(tree, i);
    nodeLevels[i] = lvl;
    if (lvl > maxLevel) {
      maxLevel = lvl;
    }
  }

  // Group node indices by their level.
  std::vector<std::vector<int>> nodesByLevel(maxLevel + 1);
  for (int i = 0; i < n; i++) {
    nodesByLevel[nodeLevels[i]].push_back(i);
  }

  // For each level, evenly distribute nodes by assigning an angle.
  // For each node:
  //    radius = (level + 1) * kMaxFeatureVectorDimensions
  //    angle (in degrees) = (index within level) * (360 / count of nodes at
  //    that level)
  // Convert angle to radians and compute the embedding.
  for (int lvl = 0; lvl <= maxLevel; lvl++) {
    int count = nodesByLevel[lvl].size();
    if (count == 0) continue;
    T angleIncrement = 360.0 / count;

    for (int pos = 0; pos < count; pos++) {
      T angleDeg = pos * angleIncrement;
      T angleRad = angleDeg * (M_PI / 180.0);
      T radius = (lvl + 1) * kMaxFeatureVectorDimensions;
      T tpeX = radius * std::cos(angleRad);
      T tpeY = radius * std::sin(angleRad);

      int nodeIndex = nodesByLevel[lvl][pos];
      tree[nodeIndex].x = tpeX;
      tree[nodeIndex].y = tpeY;
    }
  }
}

template <typename T>
void printTreePreservingEmbedding(const std::vector<Node<T>>& tree,
                                  const std::string& treeName) {
  std::cout << "TPE of Tree " << treeName << std::endl;
  for (int i = 0; i < tree.size(); i++) {
    std::cout << "  Node " << i + 1 << ": TPE (x, y) = (" << tree[i].x << ", "
              << tree[i].y << "), Level = " << getNodeLevel(tree, i)
              << std::endl;
  }
}

// Explicit instantiations for type to use.
template int getNodeLevel<float>(const std::vector<Node<float>>& tree,
                                 int index);

template void generateTreePreservingEmbedding<float>(
    std::vector<Node<float>>& tree);

template void printTreePreservingEmbedding<float>(
    const std::vector<Node<float>>& tree, const std::string& treeName);