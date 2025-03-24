#include "TreeMatching.hpp"

template <typename T>
std::vector<Node<T>> generateTreeA(
    const std::vector<std::vector<int>>& treeStructure);

template <typename T>
std::vector<Node<T>> generateTreeB(const std::vector<Node<T>>& treeA);

template <typename T>
void printTree(const std::vector<Node<T>>& tree, const std::string& treeName);