#pragma once

#include <string>

#include "TreeNode.hpp"

template <typename T>
void sortTree(const std::vector<TreeNode<T>>& tree,
              std::vector<TreeNode<T>>& sortedTree,
              std::vector<int>& sortedIndices);

template <typename T>
void printTree(const std::vector<TreeNode<T>>& tree,
               const std::string& treeName);

// similarityType: "cosine" or "euclidean"
template <typename T>
std::vector<int> matchTrees(std::vector<TreeNode<T>>& treeA,
                            std::vector<TreeNode<T>>& treeB,
                            const std::string& similarityType = "cosine");

void printMatching(const std::vector<int>& matching, const std::string& treeA,
                   const std::string& treeB);
