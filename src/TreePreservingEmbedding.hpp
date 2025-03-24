#pragma once

#include <string>

#include "TreeNode.hpp"

template <typename T>
int getNodeLevel(const std::vector<Node<T>>& tree, int index);

template <typename T>
void generateTreePreservingEmbedding(std::vector<Node<T>>& tree);

template <typename T>
void printTreePreservingEmbedding(const std::vector<Node<T>>& tree,
                                  const std::string& treeName);