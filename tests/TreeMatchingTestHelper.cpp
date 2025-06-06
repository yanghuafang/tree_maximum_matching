#include "TreeMatchingTestHelper.hpp"

#include <iostream>
#include <random>

// Recursive helper function to assign positions and attributes to each node.
template <typename T>
void assignPositions(std::vector<TreeNode<T>>& nodes, int nodeIdx, T x, T y,
                     int level,
                     const std::vector<std::vector<int>>& treeStructure,
                     std::mt19937& rng) {
  // Set the original (posX, posY) position.
  nodes[nodeIdx].posX = x;
  nodes[nodeIdx].posY = y;

  // Compute the offset (Euclidean norm) and angle (radians) from the origin.
  nodes[nodeIdx].offset = std::sqrt(x * x + y * y);
  nodes[nodeIdx].angle =
      (nodes[nodeIdx].offset == 0.0) ? 0.0 : std::atan2(y, x);

  // Set node type: For the root (index 0), type is 0; for others, alternate
  // based on level.
  if (nodeIdx == 0) {
    nodes[nodeIdx].type = 0;
  } else {
    nodes[nodeIdx].type = (level % 2 != 0) ? 1 : 2;
  }

  // Retrieve the children of the current node according to the predefined tree
  // structure.
  const std::vector<int>& childrenIndices = treeStructure[nodeIdx];

  if (!childrenIndices.empty()) {
    // Random distributions to determine child spacing and x offset.
    std::uniform_real_distribution<T> distChildSpacing(4.0, 24.0);
    std::uniform_real_distribution<T> distY(5.0, 30.0);

    T childSpacing = distChildSpacing(rng);
    int n = childrenIndices.size();
    // Compute a starting y position so that siblings are vertically spaced.
    T startX = x - (childSpacing * (n - 1) / 2.0);

    // For each child, calculate its new position and recursively assign its
    // attributes.
    for (size_t i = 0; i < childrenIndices.size(); ++i) {
      int childIdx = childrenIndices[i];
      T newY = y + distY(rng);
      T newX = startX + i * childSpacing;

      // Set hierarchy info: assign parent and register child.
      nodes[childIdx].parent = nodeIdx;
      nodes[nodeIdx].children.push_back(childIdx);

      // Recursive call for the child node.
      assignPositions(nodes, childIdx, newX, newY, level + 1, treeStructure,
                      rng);
    }
  }
}

// Function to generate Topology Tree A.
template <typename T>
TreeWrapper<T> generateTreeA(
    const std::vector<std::vector<int>>& treeStructure) {
  const int numNodes = treeStructure.size();
  TreeWrapper<T> tree;
  tree.nodes.resize(numNodes);

  // Initialize a random number generator.
  std::random_device rd;
  std::mt19937 rng(rd());

  // Start with the root node (index 0) at position (0,0) and level 0.
  assignPositions<T>(tree.nodes, 0, 0.0, 0.0, 0, treeStructure, rng);

  return tree;
}

// Generates Tree B from Tree A by applying drifts to position, offset, and
// angle.
template <typename T>
TreeWrapper<T> generateTreeB(const TreeWrapper<T>& treeA) {
  // Start from Tree A structure and attributes
  TreeWrapper<T> treeB = treeA;

  // Initialize random generators for distance jitter and angle jitter.
  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_real_distribution<T> jitterScaleDist(-0.3, 0.3);
  std::uniform_real_distribution<T> angleJitterDist(-60.0, 60.0);

  // Convert degrees to radians factor.
  const T deg2rad = M_PI / 180.0;

  // Process each node in treeB.
  for (size_t i = 0; i < treeB.nodes.size(); ++i) {
    // For the root node (index 0), fix the position at the origin.
    if (i == 0) {
      treeB.nodes[i].posX = 0.0;
      treeB.nodes[i].posY = 0.0;
      treeB.nodes[i].offset = 0.0;
      treeB.nodes[i].angle = 0.0;
      continue;
    }

    // Get the parent's original position from Tree A.
    int parentIdx = treeA.nodes[i].parent;
    T parentX = treeA.nodes[parentIdx].posX;
    T parentY = treeA.nodes[parentIdx].posY;

    // Get the current node's original position from Tree A.
    T currentX = treeA.nodes[i].posX;
    T currentY = treeA.nodes[i].posY;

    // Compute the original vector (from the parent to the current node)
    T vecX = currentX - parentX;
    T vecY = currentY - parentY;
    T originalDistance = std::sqrt(vecX * vecX + vecY * vecY);

    // Generate a random distance jitter (a factor of the original distance).
    T scale = jitterScaleDist(rng);
    T distanceJitter = originalDistance * scale;

    // Compute the original angle from the parent's position.
    T originalAngle = std::atan2(vecY, vecX);
    // Generate a random angle jitter (converted from degrees to radians).
    T angleJitter = angleJitterDist(rng) * deg2rad;
    T newAngle = originalAngle + angleJitter;

    // Compute the new position with jitter by adding the jitter vector.
    T newX = currentX + distanceJitter * std::cos(newAngle);
    T newY = currentY + distanceJitter * std::sin(newAngle);

    // Update Tree B with the new position.
    treeB.nodes[i].posX = newX;
    treeB.nodes[i].posY = newY;
    // Recalculate the offset and angle based on the new position.
    treeB.nodes[i].offset = std::sqrt(newX * newX + newY * newY);
    treeB.nodes[i].angle =
        (treeB.nodes[i].offset == 0.0 ? 0.0 : std::atan2(newY, newX));
  }

  return treeB;
}

// Explicit instantiations for type to use.
template void assignPositions<float>(
    std::vector<TreeNode<float>>& nodes, int nodeIdx, float x, float y,
    int level, const std::vector<std::vector<int>>& treeStructure,
    std::mt19937& rng);

template TreeWrapper<float> generateTreeA<float>(
    const std::vector<std::vector<int>>& treeStructure);

template TreeWrapper<float> generateTreeB<float>(
    const TreeWrapper<float>& treeA);