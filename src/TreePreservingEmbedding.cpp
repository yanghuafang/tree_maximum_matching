#include "TreePreservingEmbedding.hpp"

#include <cmath>
#include <iostream>
#include <limits>
#include <queue>

// Define M_PI if it's not available
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Helper function to get the node level by following parent pointers.
template <typename T>
int getTreeNodeLevel(const std::vector<TreeNode<T>>& tree, int index) {
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

// -------------------------------------------------------------------------
// Function: generateTreePreservingEmbedding
//
// Traverses the tree (represented as a vector of TreeNode<T>) in breadth-first
// order and assigns each node a tree-preserving embedding based on a radial
// layout.
// - The root (index 0) is assigned the full angle range [0, 360]. Its TPE
// radius
//   is 0, and its TPE angle is chosen as the midpoint (180 degrees).
// - For each node, its TPE angle range is divided equally among its children,
//   and each child receives its tpeMinAngle, tpeMaxAngle, and tpeAngle computed
//   accordingly. The TPE radius of a child is set to its parent's TPE radius
//   plus one.
// - Finally, tpeX and tpeY for each node are computed from the polar
// coordinates.
template <typename T>
void generateTreePreservingEmbedding(std::vector<TreeNode<T>>& tree) {
  if (tree.empty()) return;

  // Initialize the root node.
  // For the root (index 0), assign the full angle range [0,360] degrees.
  tree[0].tpeMinAngle = 0.0;
  tree[0].tpeMaxAngle = 360.0;
  // Here we choose the midpoint as the tpeAngle (you can change this if
  // desired).
  tree[0].tpeAngle = (tree[0].tpeMinAngle + tree[0].tpeMaxAngle) / 2.0;
  // Place the root at the center.
  tree[0].tpeRadius = 0.0;
  // Calculate Cartesian coordinates from polar coordinates.
  tree[0].tpeX = tree[0].tpeRadius * std::cos(tree[0].tpeAngle * M_PI / 180.0);
  tree[0].tpeY = tree[0].tpeRadius * std::sin(tree[0].tpeAngle * M_PI / 180.0);

  // Use a breadth-first-search (BFS) traversal.
  std::queue<int> q;
  q.push(0);

  while (!q.empty()) {
    int curIdx = q.front();
    q.pop();

    TreeNode<T>& parentNode = tree[curIdx];
    int numChildren = parentNode.children.size();
    if (numChildren == 0) continue;  // Leaf node, no children to process.

    // Parent's allocated angle range.
    T parentMin = parentNode.tpeMinAngle;
    T parentMax = parentNode.tpeMaxAngle;
    T parentRange = parentMax - parentMin;

    // Divide the parent's angle range equally among its children.
    for (int i = 0; i < numChildren; i++) {
      int childIdx = parentNode.children[i];
      TreeNode<T>& child = tree[childIdx];
      // Compute child's angle range.
      child.tpeMinAngle = parentMin + (parentRange * i) / numChildren;
      child.tpeMaxAngle = parentMin + (parentRange * (i + 1)) / numChildren;
      // Choose the midpoint of the child's angle range as its tpeAngle.
      child.tpeAngle = (child.tpeMinAngle + child.tpeMaxAngle) / 2.0;
      // Set the child's radius to be parent's radius + fixed step.
      child.tpeRadius = parentNode.tpeRadius + 1;
      // Calculate Cartesian coordinates: note conversion from degrees to
      // radians.
      T angleRad = child.tpeAngle * M_PI / 180.0;
      child.tpeX = child.tpeRadius * std::cos(angleRad);
      child.tpeY = child.tpeRadius * std::sin(angleRad);

      // Push the child index onto the queue to process its children.
      q.push(childIdx);
    }
  }
}

template <typename T>
void printTreePreservingEmbedding(const std::vector<TreeNode<T>>& tree,
                                  const std::string& treeName) {
  if (!kDebug) return;
  std::cout << "TPE of Tree " << treeName << std::endl;
  for (size_t i = 0; i < tree.size(); i++) {
    std::cout << "  Node " << i << ": "
              << " tpeX = " << tree[i].tpeX << ", tpeY = " << tree[i].tpeY
              << ", tpeRadius = " << tree[i].tpeRadius
              << ", tpeAngle = " << tree[i].tpeAngle
              << ", tpeMinAngle = " << tree[i].tpeMinAngle
              << ", tpeMaxAngle = " << tree[i].tpeMaxAngle << std::endl;
  }
}

// Explicit instantiations for type to use.
template int getTreeNodeLevel<float>(const std::vector<TreeNode<float>>& tree,
                                     int index);

template void generateTreePreservingEmbedding<float>(
    std::vector<TreeNode<float>>& tree);

template void printTreePreservingEmbedding<float>(
    const std::vector<TreeNode<float>>& tree, const std::string& treeName);