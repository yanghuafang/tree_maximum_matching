#pragma once

#include <string>

#include "TreeNode.hpp"

// similarityType: "cosine" or "euclidean"
template <typename T>
std::vector<int> matchTrees(std::vector<Node<T>>& treeA,
                            std::vector<Node<T>>& treeB,
                            const std::string& similarityType = "cosine");

void printMatching(const std::vector<int>& matching, const std::string& treeA,
                   const std::string& treeB);
