#pragma once

#include <string>

#include "TreeNode.hpp"

template <typename T>
int getTreeNodeLevel(const std::vector<TreeNode<T>>& tree, int index);

template <typename T>
void generateTreePreservingEmbedding(std::vector<TreeNode<T>>& tree);

template <typename T>
void printTreePreservingEmbedding(const std::vector<TreeNode<T>>& tree,
                                  const std::string& treeName);