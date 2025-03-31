#include <list>
#include <string>

#include "TreeNode.hpp"

template <typename T>
bool saveTreeToJson(const std::vector<TreeNode<T>>& tree,
                    const std::string& filename);

template <typename T>
bool loadTreeFromJson(std::vector<TreeNode<T>>& tree,
                      const std::string& filename);

template <typename T>
bool saveTreesToJson(const std::list<std::vector<TreeNode<T>>>& trees,
                     const std::string& filename);

template <typename T>
bool loadTreesFromJson(std::list<std::vector<TreeNode<T>>>& trees,
                       const std::string& filename);