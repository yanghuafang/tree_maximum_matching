#pragma once

#include <string>

#include "TreeNode.hpp"

template <typename T>
void generateTreePreservingEmbedding(std::vector<Node<T>>& tree);

template <typename T>
void printTreePreservingEmbedding(const std::vector<Node<T>>& tree,
                                  const std::string& treeName);