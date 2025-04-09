#pragma once

#include <string>

#include "TreeNode.hpp"

template <typename T>
void clockwiseRotate90Degrees(TreeWrapper<T>& tree);

template <typename T>
void sortTree(const TreeWrapper<T>& tree, TreeWrapper<T>& sorted,
              std::vector<int>& sortedIndices);

template <typename T>
void printTree(const TreeWrapper<T>& tree, const std::string& treeName);

// similarityType: "cosine" or "euclidean"
template <typename T>
std::vector<int> matchTrees(TreeWrapper<T>& treeA, TreeWrapper<T>& treeB,
                            const std::string& similarityType = "cosine");

void printMatching(const std::vector<int>& matching, const std::string& treeA,
                   const std::string& treeB);
