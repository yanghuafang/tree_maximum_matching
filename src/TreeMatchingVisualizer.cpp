#include "TreeMatchingVisualizer.hpp"

#include <Python.h>

#include <iostream>

#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

//------------------------------------------------------------------------------
// It draws edges (with the specified edge color), scatters nodes
// with different colors based on node type, and annotates each node with its
// index.
//
// Parameters:
//   tree         : The vector of nodes for the tree.
//   treeName: A prefix label (e.g. "TreeA" or "TreeB") used for scatter
//   labels.
//   edgeColor    : Color used to draw the tree's edges.
template <typename T>
void renderTree(const TreeWrapper<T> &tree, const std::string &treeName,
                const std::string &edgeColor) {
  // Draw edges from each node to its parent (if not a root)
  for (size_t i = 0; i < tree.nodes.size(); ++i) {
    if (tree.nodes[i].parent != -1) {
      T parentX = tree.nodes[tree.nodes[i].parent].posX;
      T parentY = tree.nodes[tree.nodes[i].parent].posY;
      T childX = tree.nodes[i].posX;
      T childY = tree.nodes[i].posY;
      std::vector<T> xEdge{parentX, childX};
      std::vector<T> yEdge{parentY, childY};
      plt::plot(xEdge, yEdge, {{"color", edgeColor}});
    }
  }

  // Group nodes by type for scatter plotting.
  std::vector<T> xType0, yType0;
  std::vector<T> xType1, yType1;
  std::vector<T> xType2, yType2;

  T delta = 0.1;
  std::vector<std::pair<std::pair<T, T>, int>> clusters;
  for (size_t i = 0; i < tree.nodes.size(); ++i) {
    T xVal = tree.nodes[i].posX;
    T yVal = tree.nodes[i].posY;
    if (tree.nodes[i].type == 0) {
      xType0.push_back(xVal);
      yType0.push_back(yVal);
    } else if (tree.nodes[i].type == 1) {
      xType1.push_back(xVal);
      yType1.push_back(yVal);
    } else if (tree.nodes[i].type == 2) {
      xType2.push_back(xVal);
      yType2.push_back(yVal);
    }

    bool foundOverlap = false;
    int overlapNum = 0;
    for (std::pair<std::pair<T, T>, int> &cluster : clusters) {
      T x = cluster.first.first;
      T y = cluster.first.second;
      T dist = std::sqrt(std::pow(x - xVal, 2) + std::pow(y - yVal, 2));
      if (dist < delta) {
        cluster.second += 1;
        foundOverlap = true;
        overlapNum = cluster.second;
        break;
      }
    }
    if (!foundOverlap) {
      clusters.push_back(std::make_pair(std::make_pair(xVal, yVal), 0));
    }

    // Annotate the node with its index.
    T xOffset = 2;
    plt::text(xVal - overlapNum * xOffset, yVal, "`" + std::to_string(i));
  }

  // Scatter points for each type.
  if (!xType0.empty())
    plt::scatter(xType0, yType0, 50.0,
                 {{"color", "red"}, {"label", treeName + " Type 0"}});
  if (!xType1.empty())
    plt::scatter(xType1, yType1, 50.0,
                 {{"color", "blue"}, {"label", treeName + " Type 1"}});
  if (!xType2.empty())
    plt::scatter(xType2, yType2, 50.0,
                 {{"color", "green"}, {"label", treeName + " Type 2"}});
}

//------------------------------------------------------------------------------
// Plots a single tree on the current matplotlib figure.
//
// Parameters:
//   tree         : The vector of nodes for the tree.
//   treeName: A prefix label (e.g. "TreeA" or "TreeB") used for scatter
//   labels.
//   edgeColor    : Color used to draw the tree's edges.
template <typename T>
void visualizeTree(const TreeWrapper<T> &tree, const std::string &treeName,
                   const std::string &edgeColor, int figure, bool block) {
  // Create and set up the figure.
  plt::figure(figure);
  plt::title("Tree Visualization");
  plt::xlabel("X-axis");
  plt::ylabel("Y-axis");
  plt::grid(true);

  renderTree(tree, treeName, edgeColor);

  // Enable interactive hover annotations via mplcursors.
  PyRun_SimpleString(
      "import matplotlib.pyplot as plt\n"
      "import mplcursors\n"
      "mplcursors.cursor(plt.gca().collections, hover=True).connect('add', "
      "lambda sel: sel.annotation.set_text('({:.2f}, "
      "{:.2f})'.format(sel.target[0], sel.target[1])))\n");

  plt::legend();
  plt::show(block);
  plt::pause(0.1);
}

//------------------------------------------------------------------------------
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
void renderMatching(const TreeWrapper<T> &treeA, const TreeWrapper<T> &treeB,
                    const std::vector<int> &matchRes,
                    const std::string &matchLineColor) {
  for (size_t i = 0; i < matchRes.size(); ++i) {
    int matchedIndex = matchRes[i];
    if (matchedIndex >= 0 &&
        static_cast<size_t>(matchedIndex) < treeB.nodes.size()) {
      T xA = treeA.nodes[i].posX;
      T yA = treeA.nodes[i].posY;
      T xB = treeB.nodes[matchedIndex].posX;
      T yB = treeB.nodes[matchedIndex].posY;
      std::vector<T> xMatch{xA, xB};
      std::vector<T> yMatch{yA, yB};
      plt::plot(xMatch, yMatch,
                {{"color", matchLineColor}, {"linestyle", "--"}});
    }
  }
}

//------------------------------------------------------------------------------
// Combines the visualization of two trees and their matching relationships.
// It creates a new figure, calls renderTree twice (once for each tree) and
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
void visualizeTreesMatching(
    const TreeWrapper<T> &treeA, const TreeWrapper<T> &treeB,
    const std::vector<int> &matchRes, const std::string &similarityType,
    const std::string &treeAEdgeColor, const std::string &treeBEdgeColor,
    const std::string &matchLineColor, int figure, bool block) {
  // Create and set up the figure.
  plt::figure(figure);
  plt::title("Tree Matching Visualization : " + similarityType);
  plt::xlabel("X-axis");
  plt::ylabel("Y-axis");
  plt::grid(true);

  // Visualize treeA (no horizontal shift, edge color as provided).
  renderTree<T>(treeA, "TreeA", treeAEdgeColor);

  // Visualize treeB with a horizontal shift.
  renderTree<T>(treeB, "TreeB", treeBEdgeColor);

  // Draw matching lines between treeA and treeB.
  renderMatching<T>(treeA, treeB, matchRes, matchLineColor);

  // Enable interactive hover annotations via mplcursors.
  PyRun_SimpleString(
      "import matplotlib.pyplot as plt\n"
      "import mplcursors\n"
      "mplcursors.cursor(plt.gca().collections, hover=True).connect('add', "
      "lambda sel: sel.annotation.set_text('({:.2f}, "
      "{:.2f})'.format(sel.target[0], sel.target[1])))\n");

  plt::legend();
  plt::show(block);
  plt::pause(0.1);
}

void pltShow() { plt::show(); }

// Explicit instantiations for type to use.
template void renderTree<float>(const TreeWrapper<float> &tree,
                                const std::string &treeName,
                                const std::string &edgeColor);

template void visualizeTree<float>(const TreeWrapper<float> &tree,
                                   const std::string &treeName,
                                   const std::string &edgeColor, int figure,
                                   bool block);

template void renderMatching<float>(const TreeWrapper<float> &treeA,
                                    const TreeWrapper<float> &treeB,
                                    const std::vector<int> &matchRes,
                                    const std::string &matchLineColor);

template void visualizeTreesMatching<float>(
    const TreeWrapper<float> &treeA, const TreeWrapper<float> &treeB,
    const std::vector<int> &matchRes, const std::string &similarityType,
    const std::string &treeAEdgeColor, const std::string &treeBEdgeColor,
    const std::string &matchLineColor, int figure1, bool block);