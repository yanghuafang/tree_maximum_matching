#include <list>
#include <string>

#include "TreeNode.hpp"

// Define TreeWrapper struct that encapsulates a timestamp and tree nodes.
template <typename T>
struct TreeWrapper {
  uint64_t timestamp;
  std::vector<TreeNode<T>> tree;
};

template <typename T>
bool saveTreeToJson(const TreeWrapper<T>& treeWrapper,
                    const std::string& filename);

template <typename T>
bool loadTreeFromJson(TreeWrapper<T>& treeWrapper, const std::string& filename);

template <typename T>
bool saveTreesToJson(const std::list<TreeWrapper<T>>& trees,
                     const std::string& filename);

template <typename T>
bool loadTreesFromJson(std::list<TreeWrapper<T>>& trees,
                       const std::string& filename);