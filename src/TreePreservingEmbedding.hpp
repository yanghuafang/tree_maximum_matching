#pragma once

#include <string>

#include "TreeNode.hpp"

template <typename T>
int getTreeNodeLevel(const TreeWrapper<T>& tree, int index);

template <typename T>
void generateTreePreservingEmbedding(TreeWrapper<T>& tree);

template <typename T>
void printTreePreservingEmbedding(const TreeWrapper<T>& tree,
                                  const std::string& treeName);