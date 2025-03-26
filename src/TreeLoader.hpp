#include <string>

#include "TreeNode.hpp"

template <typename T>
bool saveTreeToJson(const std::vector<Node<T>>& tree,
                    const std::string& filename);

template <typename T>
bool loadTreeFromJson(std::vector<Node<T>>& tree, const std::string& filename);