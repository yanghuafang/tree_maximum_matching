#pragma once

#include <string>

#include "TreeNode.hpp"

template <typename T>
void visualizeTreePreservingEmbedding(const TreeWrapper<T> &tree,
                                      const std::string &treeName,
                                      int figure = -1, bool block = true);