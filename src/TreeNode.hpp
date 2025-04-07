#pragma once

#include <vector>

// Define the structure for nodes of the topology tree.
template <typename T>
struct TreeNode {
  // TPE embedding placeholder (to be computed in a later step)
  T tpeX = 0.0, tpeY = 0.0;
  T tpeRadius = 0.0;
  T tpeMinAngle = 0.0, tpeMaxAngle = 0.0;
  T tpeAngle = 0.0;

  // Original position in Cartesian coordinates.
  T posX = 0.0, posY = 0.0;

  // Offset is the distance from the origin; angle is stored in radians.
  T offset = 0.0, angle = 0.0;

  // Node type: 0 for root, otherwise 1 or 2 depending on level.
  int type = 0;

  // Hierarchical info.
  std::vector<int> children;  // indices of children in the node vector.
  int parent = -1;            // index of the parent (-1 for root).
};

// Toggle for debugging info output.
constexpr bool kDebug = true;