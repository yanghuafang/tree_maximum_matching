#include "TreeLoader.hpp"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Function to save the vector of Nodes (the tree) into a JSON file.
template <typename T>
bool saveTreeToJson(const std::vector<Node<T>>& tree,
                    const std::string& filename) {
  // Create a JSON array.
  json j = json::array();

  // Serialize each Node into a JSON object.
  for (const auto& node : tree) {
    json jNode;
    jNode["posX"] = node.posX;
    jNode["posY"] = node.posY;
    jNode["offset"] = node.offset;
    jNode["angle"] = node.angle;
    jNode["type"] = node.type;
    jNode["children"] = node.children;
    jNode["parent"] = node.parent;
    j.push_back(jNode);
  }

  // Write the JSON array to file.
  std::ofstream outFile(filename);
  if (!outFile.is_open()) {
    std::cerr << "Error: Could not open file for writing: " << filename
              << std::endl;
    return false;
  }
  // Dump with indent of 4 spaces.
  outFile << j.dump(4);
  return true;
}

// Function to load the tree from a JSON file into a vector of Nodes.
template <typename T>
bool loadTreeFromJson(std::vector<Node<T>>& tree, const std::string& filename) {
  // Open the file for reading.
  std::ifstream inFile(filename);
  if (!inFile.is_open()) {
    std::cerr << "Error: Could not open file for reading: " << filename
              << std::endl;
    return false;
  }

  // Parse the JSON file.
  json j;
  inFile >> j;

  // Clear the existing vector.
  tree.clear();

  // Rebuild the tree node-by-node.
  for (const auto& jNode : j) {
    Node<T> node;
    node.posX = jNode.at("posX").get<T>();
    node.posY = jNode.at("posY").get<T>();
    node.offset = jNode.at("offset").get<T>();
    node.angle = jNode.at("angle").get<T>();
    node.type = jNode.at("type").get<int>();
    node.children = jNode.at("children").get<std::vector<int>>();
    node.parent = jNode.at("parent").get<int>();
    tree.push_back(node);
  }
  return true;
}

// Explicit instantiations for type to use.
template bool saveTreeToJson<float>(const std::vector<Node<float>>& tree,
                                    const std::string& filename);

template bool loadTreeFromJson<float>(std::vector<Node<float>>& tree,
                                      const std::string& filename);