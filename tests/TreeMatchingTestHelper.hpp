#include "TreeMatching.hpp"

template <typename T>
std::vector<TreeNode<T>> generateTreeA(
    const std::vector<std::vector<int>>& treeStructure);

template <typename T>
std::vector<TreeNode<T>> generateTreeB(const std::vector<TreeNode<T>>& treeA);