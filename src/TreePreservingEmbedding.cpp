#include "TreePreservingEmbedding.hpp"

#include <cmath>
#include <iostream>
#include <limits>
#include <queue>

// Helper function to get the node level by following parent pointers.
template <typename T>
int getTreeNodeLevel(const TreeWrapper<T>& tree, int index) {
  int level = 0;
  int current = index;
  int parent = tree.nodes[current].parent;
  while (parent != -1) {
    const TreeNode<T>& curNode = tree.nodes[current];
    const TreeNode<T>& parentNode = tree.nodes[parent];
    T dist = std::sqrt(std::pow(curNode.posX - parentNode.posX, 2) +
                       std::pow(curNode.posY - parentNode.posY, 2));
    T delta = 0.1;
    if (dist > delta) {
      level++;
    }
    current = parent;
    parent = tree.nodes[current].parent;
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
void generateTreePreservingEmbedding(TreeWrapper<T>& tree) {
  if (tree.nodes.empty()) return;

  // Get maximum level of the tree.
  int maxLevel = 0;
  for (int i = 0; i < tree.nodes.size(); ++i) {
    int level = getTreeNodeLevel(tree, i);
    if (level > maxLevel) maxLevel = level;
  }

  // Initialize the root node.
  // For the root (index 0), assign the full angle range [0,360] degrees.
  tree.nodes[0].tpeMinAngle = 0.0;
  tree.nodes[0].tpeMaxAngle = 360.0;
  // Here we choose the midpoint as the tpeAngle (you can change this if
  // desired).
  tree.nodes[0].tpeAngle = 0.0;
  // Place the root at the center.
  tree.nodes[0].tpeRadius = 0.0;
  // Calculate Cartesian coordinates from polar coordinates.
  tree.nodes[0].tpeX =
      tree.nodes[0].tpeRadius * std::cos(tree.nodes[0].tpeAngle * M_PI / 180.0);
  tree.nodes[0].tpeY =
      tree.nodes[0].tpeRadius * std::sin(tree.nodes[0].tpeAngle * M_PI / 180.0);

  // Use a breadth-first-search (BFS) traversal.
  std::queue<int> q;
  q.push(0);

  while (!q.empty()) {
    int curIdx = q.front();
    q.pop();

    TreeNode<T>& parentNode = tree.nodes[curIdx];
    int numChildren = parentNode.children.size();
    if (numChildren == 0) continue;  // Leaf node, no children to process.

    // Parent's allocated angle range.
    T parentMin = parentNode.tpeMinAngle;
    T parentMax = parentNode.tpeMaxAngle;
    T parentRange = parentMax - parentMin;

    // Divide the parent's angle range equally among its children.
    for (int i = 0; i < numChildren; i++) {
      int childIdx = parentNode.children[i];
      TreeNode<T>& child = tree.nodes[childIdx];
      // Compute child's angle range.
      child.tpeMinAngle = parentMin + (parentRange * i) / numChildren;
      child.tpeMaxAngle = parentMin + (parentRange * (i + 1)) / numChildren;
      // Choose the midpoint of the child's angle range as its tpeAngle.
      child.tpeAngle = (child.tpeMinAngle + child.tpeMaxAngle) / 2.0;
      // Set the child's radius.
      child.tpeRadius = getTreeNodeLevel(tree, childIdx) / maxLevel;
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
void printTreePreservingEmbedding(const TreeWrapper<T>& tree,
                                  const std::string& treeName) {
  if (!kDebug) return;
  std::cout << "TPE of Tree: " << treeName << " Timestamp: " << tree.timestamp
            << std::endl;
  for (size_t i = 0; i < tree.nodes.size(); i++) {
    std::cout << "  Node " << i << ": "
              << " tpeX = " << tree.nodes[i].tpeX
              << ", tpeY = " << tree.nodes[i].tpeY
              << ", tpeRadius = " << tree.nodes[i].tpeRadius
              << ", tpeAngle = " << tree.nodes[i].tpeAngle
              << ", tpeMinAngle = " << tree.nodes[i].tpeMinAngle
              << ", tpeMaxAngle = " << tree.nodes[i].tpeMaxAngle << std::endl;
  }
}

// Explicit instantiations for type to use.
template int getTreeNodeLevel<float>(const TreeWrapper<float>& tree, int index);

template void generateTreePreservingEmbedding<float>(TreeWrapper<float>& tree);

template void printTreePreservingEmbedding<float>(
    const TreeWrapper<float>& tree, const std::string& treeName);