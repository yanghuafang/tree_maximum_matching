#include "TreePreservingEmbeddingVisualizer.hpp"

#include <cmath>

#include "TreePreservingEmbedding.hpp"
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

// Visualization function using matplotlib-cpp.
// Displays:
//   1. The tree name as title.
//   2. Nodes with their index as text annotations.
//   3. Points are colored according to node type.
//   4. Labeled x-axis and y-axis with a grid.
//   5. Interactive (hover) display of coordinates via mplcursors.
//   6. Edges are drawn between each node and its parent with color according to
//   edge level.
template <typename T>
void visualizeTreePreservingEmbedding(const TreeWrapper<T> &tree,
                                      const std::string &treeName, int figure,
                                      bool block) {
  // Separate points by node type.
  std::vector<T> xType0, yType0;
  std::vector<T> xType1, yType1;
  std::vector<T> xType2, yType2;

  for (size_t i = 0; i < tree.nodes.size(); ++i) {
    const auto &node = tree.nodes[i];
    if (node.type == 0) {
      xType0.push_back(node.tpeX);
      yType0.push_back(node.tpeY);
    } else if (node.type == 1) {
      xType1.push_back(node.tpeX);
      yType1.push_back(node.tpeY);
    } else if (node.type == 2) {
      xType2.push_back(node.tpeX);
      yType2.push_back(node.tpeY);
    }
  }

  plt::figure(figure);
  plt::title(treeName + " at timestamp " + std::to_string(tree.timestamp));
  plt::xlabel("X-axis");
  plt::ylabel("Y-axis");
  plt::grid(true);

  // First, plot edges.
  // For each node (except the root), draw a line from origin (0,0) to its
  // own (x,y).
  for (size_t i = 0; i < tree.nodes.size(); i++) {
    if (tree.nodes[i].parent != -1) {
      T childX = tree.nodes[i].tpeX;
      T childY = tree.nodes[i].tpeY;

      std::string eColor = "gray";

      std::vector<T> edgeX{0.0, childX};
      std::vector<T> edgeY{0.0, childY};
      plt::plot(edgeX, edgeY, {{"color", eColor}});
    }
  }

  // Plot the nodes (scatter plot) with different colors for each type.
  if (!xType0.empty())
    plt::scatter(xType0, yType0, 50.0, {{"color", "red"}, {"label", "Type 0"}});
  if (!xType1.empty())
    plt::scatter(xType1, yType1, 50.0,
                 {{"color", "green"}, {"label", "Type 1"}});
  if (!xType2.empty())
    plt::scatter(xType2, yType2, 50.0,
                 {{"color", "blue"}, {"label", "Type 2"}});

  T delta = 0.1;
  std::vector<std::pair<std::pair<T, T>, int>> clusters;
  for (size_t i = 0; i < tree.nodes.size(); i++) {
    T posX = tree.nodes[i].posX;
    T posY = tree.nodes[i].posY;

    bool foundOverlap = false;
    int overlapNum = 0;
    for (std::pair<std::pair<T, T>, int> &cluster : clusters) {
      T x = cluster.first.first;
      T y = cluster.first.second;
      T dist = std::sqrt(std::pow(x - posX, 2) + std::pow(y - posY, 2));
      if (dist < delta) {
        cluster.second += 1;
        foundOverlap = true;
        overlapNum = cluster.second;
        break;
      }
    }
    if (!foundOverlap) {
      clusters.push_back(std::make_pair(std::make_pair(posX, posY), 0));
    }

    // Annotate each node with its index.
    T xOffset = 0.2;
    plt::text(tree.nodes[i].tpeX - xOffset * overlapNum, tree.nodes[i].tpeY,
              "`" + std::to_string(i));
  }

  plt::legend();

  // Enable hover annotations using mplcursors.
  // Since matplotlib-cpp doesn't offer an "exec" function, we manually call
  // the Python C API function PyRun_SimpleString to execute a snippet.
  PyRun_SimpleString(
      "import matplotlib.pyplot as plt\n"
      "import mplcursors\n"
      "mplcursors.cursor(plt.gca().collections, hover=True).connect('add', "
      "lambda sel: sel.annotation.set_text('({:.2f}, "
      "{:.2f})'.format(sel.target[0], sel.target[1])))\n");

  // Show the plot.
  plt::show(block);
  plt::pause(0.1);
}

// Explicit instantiations for type to use.
template void visualizeTreePreservingEmbedding<float>(
    const TreeWrapper<float> &tree, const std::string &treeName, int figure,
    bool block);