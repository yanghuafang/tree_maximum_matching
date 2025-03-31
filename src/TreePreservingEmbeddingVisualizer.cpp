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
void visualizeTreePreservingEmbedding(const std::vector<TreeNode<T>> &tree,
                                      const std::string &treeName) {
  // Separate points by node type.
  std::vector<T> xType0, yType0;
  std::vector<T> xType1, yType1;
  std::vector<T> xType2, yType2;

  for (size_t i = 0; i < tree.size(); ++i) {
    const auto &node = tree[i];
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

  plt::figure();
  plt::title(treeName);
  plt::xlabel("X-axis");
  plt::ylabel("Y-axis");
  plt::grid(true);

  // First, plot edges.
  // For each node (except the root), draw a line from origin (0,0) to its
  // own (x,y).
  for (size_t i = 0; i < tree.size(); i++) {
    if (tree[i].parent != -1) {
      double childX = tree[i].tpeX;
      double childY = tree[i].tpeY;

      int level = getTreeNodeLevel(tree, i);
      std::string eColor = "gray";

      std::vector<double> edgeX{0.0, childX};
      std::vector<double> edgeY{0.0, childY};
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

  // Annotate each node with its index.
  for (size_t i = 0; i < tree.size(); i++) {
    plt::text(tree[i].tpeX, tree[i].tpeY, std::to_string(i));
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
  plt::show();
}

// Explicit instantiations for type to use.
template void visualizeTreePreservingEmbedding<float>(
    const std::vector<TreeNode<float>> &tree, const std::string &treeName);