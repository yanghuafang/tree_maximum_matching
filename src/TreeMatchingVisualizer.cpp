#include "TreeMatchingVisualizer.hpp"

#include <Python.h>

#include <iostream>

#include "TreeNode.hpp"
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

//------------------------------------------------------------------------------
// First function: visualizeTree
//
// Plots a single tree on the current matplotlib figure using the nodes' TPE
// coordinates. It draws edges (with the specified edge color), scatters nodes
// with different colors based on node type, and annotates each node with its
// index.
//
// Parameters:
//   tree         : The vector of nodes for the tree.
//   labelPrefix  : A prefix label (e.g. "TreeA" or "TreeB") used for scatter
//   labels.
//   edgeColor    : Color used to draw the tree's edges.
template <typename T>
void visualizeTree(const std::vector<Node<T>> &tree,
                   const std::string &labelPrefix,
                   const std::string &edgeColor) {
  // Draw edges from each node to its parent (if not a root)
  for (size_t i = 0; i < tree.size(); ++i) {
    if (tree[i].parent != -1) {
      double parentX = tree[tree[i].parent].posX;
      double parentY = tree[tree[i].parent].posY;
      double childX = tree[i].posX;
      double childY = tree[i].posY;
      std::vector<double> xEdge{parentX, childX};
      std::vector<double> yEdge{parentY, childY};
      // Counterclockwise 90 degrees rotation, as vehicle coordinate system is:
      // axis x points to front, axis y points to left.
      plt::plot(yEdge, xEdge, {{"color", edgeColor}});
    }
  }

  // Group nodes by type for scatter plotting.
  std::vector<double> xType0, yType0;
  std::vector<double> xType1, yType1;
  std::vector<double> xType2, yType2;

  for (size_t i = 0; i < tree.size(); ++i) {
    double xVal = tree[i].posX;
    double yVal = tree[i].posY;
    if (tree[i].type == 0) {
      xType0.push_back(xVal);
      yType0.push_back(yVal);
    } else if (tree[i].type == 1) {
      xType1.push_back(xVal);
      yType1.push_back(yVal);
    } else if (tree[i].type == 2) {
      xType2.push_back(xVal);
      yType2.push_back(yVal);
    }
    // Annotate the node with its index.
    // Counterclockwise 90 degrees rotation, as vehicle coordinate system is:
    // axis x points to front, axis y points to left.
    plt::text(yVal, xVal, std::to_string(i));
  }

  // Scatter points for each type.
  // Counterclockwise 90 degrees rotation, as vehicle coordinate system is: axis
  // x points to front, axis y points to left.
  if (!xType0.empty())
    plt::scatter(yType0, xType0, 50.0,
                 {{"color", "red"}, {"label", labelPrefix + " Type 0"}});
  if (!xType1.empty())
    plt::scatter(yType1, xType1, 50.0,
                 {{"color", "green"}, {"label", labelPrefix + " Type 1"}});
  if (!xType2.empty())
    plt::scatter(yType2, xType2, 50.0,
                 {{"color", "blue"}, {"label", labelPrefix + " Type 2"}});
}

//------------------------------------------------------------------------------
// Second function: visualizeMatching
//
// Draws dashed lines connecting matching nodes between treeA and treeB. Assumes
// that matchRes is a vector whose i-th element gives the matching node index in
// treeB for treeA[i].
//
// Parameters:
//   treeA        : First tree.
//   treeB        : Second tree.
//   matchRes     : Matching vector (matchRes[i] is the index in treeB that
//   matches treeA[i]).
//   matchLineColor: The color used for drawing matching
//   lines (as dashed lines).
template <typename T>
void visualizeMatching(const std::vector<Node<T>> &treeA,
                       const std::vector<Node<T>> &treeB,
                       const std::vector<int> &matchRes,
                       const std::string &matchLineColor) {
  for (size_t i = 0; i < matchRes.size(); ++i) {
    int matchedIndex = matchRes[i];
    if (matchedIndex >= 0 && static_cast<size_t>(matchedIndex) < treeB.size()) {
      double xA = treeA[i].posX;
      double yA = treeA[i].posY;
      double xB = treeB[matchedIndex].posX;
      double yB = treeB[matchedIndex].posY;
      std::vector<double> xMatch{xA, xB};
      std::vector<double> yMatch{yA, yB};
      // Counterclockwise 90 degrees rotation, as vehicle coordinate system is:
      // axis x points to front, axis y points to left.
      plt::plot(yMatch, xMatch,
                {{"color", matchLineColor}, {"linestyle", "--"}});
    }
  }
}

//------------------------------------------------------------------------------
// Third function: visualizeTreesMatching
//
// Combines the visualization of two trees and their matching relationships.
// It creates a new figure, calls visualizeTree twice (once for each tree) and
// then overlays the matching lines. After drawing, it enables hover annotations
// via mplcursors and displays the final plot.
//
// Parameters:
//   treeA          : First tree (e.g., treeA).
//   treeB          : Second tree (e.g., treeB).
//   matchRes       : Matching vector: matchRes[i] gives the matching index in
//   treeB for treeA[i].
//   treeAEdgeColor : Edge color for treeA. treeBEdgeColor :
//   Edge color for treeB.
//   matchLineColor : Color for the dashed matching lines.
template <typename T>
void visualizeTreesMatching(const std::vector<Node<T>> &treeA,
                            const std::vector<Node<T>> &treeB,
                            const std::vector<int> &matchRes,
                            const std::string &similarityType,
                            const std::string &treeAEdgeColor,
                            const std::string &treeBEdgeColor,
                            const std::string &matchLineColor) {
  // Create and set up the figure.
  plt::figure();
  plt::title("Tree Matching Visualization : " + similarityType);
  // Counterclockwise 90 degrees rotation, as vehicle coordinate system is: axis
  // x points to front, axis y points to left.
  plt::xlabel("Y-axis");
  plt::ylabel("X-axis");
  plt::grid(true);

  // Visualize treeA (no horizontal shift, edge color as provided).
  visualizeTree<T>(treeA, "TreeA", treeAEdgeColor);

  // Visualize treeB with a horizontal shift.
  visualizeTree<T>(treeB, "TreeB", treeBEdgeColor);

  // Draw matching lines between treeA and treeB.
  visualizeMatching<T>(treeA, treeB, matchRes, matchLineColor);

  // Enable interactive hover annotations via mplcursors.
  PyRun_SimpleString(
      "import matplotlib.pyplot as plt\n"
      "import mplcursors\n"
      "mplcursors.cursor(plt.gca().collections, hover=True).connect('add', "
      "lambda sel: sel.annotation.set_text('({:.2f}, "
      "{:.2f})'.format(sel.target[0], sel.target[1])))\n");

  plt::legend();
  plt::show();
}

// Explicit instantiations for type to use.
template void visualizeTree<float>(const std::vector<Node<float>> &tree,
                                   const std::string &labelPrefix,
                                   const std::string &edgeColor);

template void visualizeMatching<float>(const std::vector<Node<float>> &treeA,
                                       const std::vector<Node<float>> &treeB,
                                       const std::vector<int> &matchRes,
                                       const std::string &matchLineColor);

template void visualizeTreesMatching<float>(
    const std::vector<Node<float>> &treeA,
    const std::vector<Node<float>> &treeB, const std::vector<int> &matchRes,
    const std::string &similarityType, const std::string &treeAEdgeColor,
    const std::string &treeBEdgeColor, const std::string &matchLineColor);