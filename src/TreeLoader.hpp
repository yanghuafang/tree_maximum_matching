#include <list>
#include <string>

#include "TreeNode.hpp"

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