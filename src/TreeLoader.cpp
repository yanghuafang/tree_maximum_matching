#include "TreeLoader.hpp"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::ordered_json;

//------------------------------------------------------------------------------
// JSON conversion functions for TreeNode<T>
template <typename T>
void to_json(json& j, const TreeNode<T>& node) {
  j = json{{"posX", node.posX},     {"posY", node.posY},
           {"offset", node.offset}, {"angle", node.angle},
           {"type", node.type},     {"children", node.children},
           {"parent", node.parent}};
}

template <typename T>
void from_json(const json& j, TreeNode<T>& node) {
  j.at("posX").get_to(node.posX);
  j.at("posY").get_to(node.posY);
  j.at("offset").get_to(node.offset);
  j.at("angle").get_to(node.angle);
  j.at("type").get_to(node.type);
  j.at("children").get_to(node.children);
  j.at("parent").get_to(node.parent);
}

//------------------------------------------------------------------------------
// Save a single tree to a JSON file.
template <typename T>
bool saveTreeToJson(const std::vector<TreeNode<T>>& tree,
                    const std::string& filename) {
  try {
    json j = tree;  // Automatically converts each TreeNode<T> to JSON.

    std::ofstream outFile(filename);
    if (!outFile) return false;

    outFile << j.dump(4);  // Pretty-print with an indent of 4 spaces.
    return true;           // Success
  } catch (...) {
    return false;  // Failure
  }
}

//------------------------------------------------------------------------------
// Load a single tree from a JSON file.
template <typename T>
bool loadTreeFromJson(std::vector<TreeNode<T>>& tree,
                      const std::string& filename) {
  try {
    std::ifstream inFile(filename);
    if (!inFile) return false;

    json j;
    inFile >> j;

    // Assume the file contains a direct JSON array representing a tree.
    tree = j.get<std::vector<TreeNode<T>>>();
    return true;  // Success
  } catch (...) {
    return false;  // Failure
  }
}

//------------------------------------------------------------------------------
// Save multiple trees to a JSON file.
template <typename T>
bool saveTreesToJson(const std::list<std::vector<TreeNode<T>>>& trees,
                     const std::string& filename) {
  try {
    json j;
    j["trees"] = json::array();
    for (const auto& tree : trees) {
      j["trees"].push_back(tree);  // Convert each tree to JSON.
    }

    std::ofstream outFile(filename);
    if (!outFile) return false;

    outFile << j.dump(4);  // Pretty-print with an indent of 4 spaces.
    return true;           // Success
  } catch (...) {
    return false;  // Failure
  }
}

//------------------------------------------------------------------------------
// Load multiple trees from a JSON file.
template <typename T>
bool loadTreesFromJson(std::list<std::vector<TreeNode<T>>>& trees,
                       const std::string& filename) {
  try {
    std::ifstream inFile(filename);
    if (!inFile) return false;

    json j;
    inFile >> j;

    trees.clear();  // Clear the list before loading.
    if (j.contains("trees") && j["trees"].is_array()) {
      for (const auto& treeJson : j["trees"]) {
        std::vector<TreeNode<T>> tree =
            treeJson.get<std::vector<TreeNode<T>>>();
        trees.push_back(tree);
      }
      return true;  // Success
    }
    return false;  // Invalid JSON format
  } catch (...) {
    return false;  // Failure
  }
}

// Explicit instantiations for type to use.
template bool saveTreeToJson<float>(const std::vector<TreeNode<float>>& tree,
                                    const std::string& filename);

template bool loadTreeFromJson<float>(std::vector<TreeNode<float>>& tree,
                                      const std::string& filename);

template bool saveTreesToJson<float>(
    const std::list<std::vector<TreeNode<float>>>& trees,
    const std::string& filename);

template bool loadTreesFromJson<float>(
    std::list<std::vector<TreeNode<float>>>& trees,
    const std::string& filename);