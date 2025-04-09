#include "TreeMatching.hpp"

template <typename T>
TreeWrapper<T> generateTreeA(
    const std::vector<std::vector<int>>& treeStructure);

template <typename T>
TreeWrapper<T> generateTreeB(const TreeWrapper<T>& treeA);