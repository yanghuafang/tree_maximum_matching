#pragma once

#include <string>

#include "TreeNode.hpp"

template <typename T>
void visualizeTree(const std::vector<TreeNode<T>> &tree,
                   const std::string &labelPrefix,
                   const std::string &edgeColor);

template <typename T>
void visualizeMatching(const std::vector<TreeNode<T>> &treeA,
                       const std::vector<TreeNode<T>> &treeB,
                       const std::vector<int> &matchRes,
                       const std::string &matchLineColor);

template <typename T>
void visualizeTreesMatching(const std::vector<TreeNode<T>> &treeA,
                            const std::vector<TreeNode<T>> &treeB,
                            const std::vector<int> &matchRes,
                            const std::string &similarityType,
                            const std::string &treeAEdgeColor,
                            const std::string &treeBEdgeColor,
                            const std::string &matchLineColor);