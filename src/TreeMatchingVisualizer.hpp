#pragma once

#include <string>

#include "TreeNode.hpp"

template <typename T>
void visualizeTree(const TreeWrapper<T> &tree, const std::string &treeName,
                   const std::string &edgeColor, int figure = -1,
                   bool block = true);

template <typename T>
void visualizeTreesMatching(
    const TreeWrapper<T> &treeA, const TreeWrapper<T> &treeB,
    const std::vector<int> &matchRes, const std::string &similarityType,
    const std::string &treeAEdgeColor, const std::string &treeBEdgeColor,
    const std::string &matchLineColor, int figure = -1, bool block = true);

void pltShow();