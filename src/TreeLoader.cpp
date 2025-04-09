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
// JSON conversion functions for TreeWrapper<T>
template <typename T>
void to_json(json& j, const TreeWrapper<T>& tree) {
  j = json{{"timestamp", tree.timestamp}, {"nodes", tree.nodes}};
}

template <typename T>
void from_json(const json& j, TreeWrapper<T>& tree) {
  j.at("timestamp").get_to(tree.timestamp);
  j.at("nodes").get_to(tree.nodes);
}

//------------------------------------------------------------------------------
// Save a single tree (wrapped in TreeWrapper) to a JSON file.
template <typename T>
bool saveTreeToJson(const TreeWrapper<T>& tree, const std::string& filename) {
  try {
    json j = tree;  // Automatically converts TreeWrapper<T> to JSON.

    std::ofstream outFile(filename);
    if (!outFile) return false;

    outFile << j.dump(4);  // Pretty-print with an indent of 4 spaces.
    return true;           // Success
  } catch (...) {
    return false;  // Failure
  }
}

//------------------------------------------------------------------------------
// Load a single tree (wrapped in TreeWrapper) from a JSON file.
template <typename T>
bool loadTreeFromJson(TreeWrapper<T>& tree, const std::string& filename) {
  try {
    std::ifstream inFile(filename);
    if (!inFile) return false;

    json j;
    inFile >> j;

    // Assume the file contains a JSON object representing a TreeWrapper.
    tree = j.get<TreeWrapper<T>>();
    return true;  // Success
  } catch (...) {
    return false;  // Failure
  }
}

//------------------------------------------------------------------------------
// Save multiple trees (wrapped in TreeWrapper) to a JSON file.
template <typename T>
bool saveTreesToJson(const std::list<TreeWrapper<T>>& trees,
                     const std::string& filename) {
  try {
    json j;
    j["trees"] = json::array();
    for (const auto& tree : trees) {
      j["trees"].push_back(tree);  // Convert each TreeWrapper to JSON.
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
// Load multiple trees (wrapped in TreeWrapper) from a JSON file.
template <typename T>
bool loadTreesFromJson(std::list<TreeWrapper<T>>& trees,
                       const std::string& filename) {
  try {
    std::ifstream inFile(filename);
    if (!inFile) return false;

    json j;
    inFile >> j;

    trees.clear();  // Clear the list before loading.
    if (j.contains("trees") && j["trees"].is_array()) {
      for (const auto& treeJson : j["trees"]) {
        TreeWrapper<T> tree = treeJson.get<TreeWrapper<T>>();
        trees.push_back(tree);
      }
      return true;  // Success
    }
    return false;  // Invalid JSON format
  } catch (...) {
    return false;  // Failure
  }
}

//------------------------------------------------------------------------------
// Explicit instantiations for type float.
template bool saveTreeToJson<float>(const TreeWrapper<float>& tree,
                                    const std::string& filename);
template bool loadTreeFromJson<float>(TreeWrapper<float>& tree,
                                      const std::string& filename);
template bool saveTreesToJson<float>(const std::list<TreeWrapper<float>>& trees,
                                     const std::string& filename);
template bool loadTreesFromJson<float>(std::list<TreeWrapper<float>>& trees,
                                       const std::string& filename);
