#pragma once

#include <utility>
#include <vector>

template <typename T>
std::pair<T, std::vector<int>> hungarianAlgorithm(
    const std::vector<std::vector<T>>& costMatrix);