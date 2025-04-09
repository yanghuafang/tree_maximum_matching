#include "TreeMatching.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <queue>
#include <utility>

#include "HungarianAlgorithm.hpp"
#include "TreePreservingEmbedding.hpp"

template <typename T>
void clockwiseRotate90Degrees(TreeWrapper<T>& tree) {
  std::for_each(tree.nodes.begin(), tree.nodes.end(), [](TreeNode<T>& node) {
    T x = node.posX;
    T y = node.posY;
    node.posX = -y;
    node.posY = x;
  });
}

// Compute angle of the vector from (x1, y1) to (x2, y2), and normalize the
// angle to range [-90, 270].
template <typename T>
T computeAngle(T x1, T y1, T x2, T y2) {
  T angle = std::atan2(y2 - y1, x2 - x1) * 180.0 / M_PI;
  if (angle < -90.0) angle += 360.0;
  if (angle > 270.0) angle -= 360.0;
  return angle;
}

/*
 * This function builds a new tree (sortedTree) from the original tree such
 * that:
 * 1. For each node in the original tree, its children are re-ordered by the
 * angle (computed from parent's (posX,posY) to child's (posX,posY)) in
 * ascending order.
 * 2. Each node in sortedTree has its parent and children fields updated so that
 *    they refer to indices within sortedTree.
 * 3. The vector sortedIndices holds the mapping of new indices to original
 * indices. That is, sortedTree[newIdx] originally came from tree[
 * sortedIndices[newIdx]].
 *
 * The algorithm uses a breadth-first traversal. We also maintain an auxiliary
 * vector (oldToNew) mapping each original index to its new index in sortedTree.
 */
template <typename T>
void sortTree(const TreeWrapper<T>& tree, TreeWrapper<T>& sortedTree,
              std::vector<int>& sortedIndices) {
  sortedTree.timestamp = tree.timestamp;
  sortedTree.nodes.clear();
  sortedIndices.clear();
  if (tree.nodes.empty()) return;

  // This vector maps an original node index to its new index in sortedTree.
  std::vector<int> oldToNew(tree.nodes.size(), -1);

  // Process the root separately.
  TreeNode<T> newRoot = tree.nodes[0];
  newRoot.parent = -1;       // root's parent is set to -1.
  newRoot.children.clear();  // children will be populated later.
  sortedTree.nodes.push_back(newRoot);
  sortedIndices.push_back(0);
  oldToNew[0] = 0;

  // Queue for processing nodes in BFS order (using original indices).
  std::queue<int> q;
  q.push(0);  // assume tree[0] is the root.

  // Process nodes in BFS order.
  while (!q.empty()) {
    int curIdx = q.front();
    q.pop();

    const TreeNode<T>& curNode = tree.nodes[curIdx];
    int numChildren = curNode.children.size();
    if (numChildren == 0) continue;

    // children stores pair of child index and angle of vector from parent to
    // child.
    std::vector<std::pair<int, float>> children;
    for (int i = 0; i < numChildren; ++i) {
      int childIdx = curNode.children[i];
      const TreeNode<T>& childNode = tree.nodes[childIdx];
      T angle = computeAngle(curNode.posX, curNode.posY, childNode.posX,
                             childNode.posY);
      children.push_back(std::make_pair(childIdx, angle));
    }

    // Sort children by angle of vector from parent to child.
    std::sort(children.begin(), children.end(),
              [](const std::pair<int, float>& child1,
                 const std::pair<int, float>& child2) {
                return child1.second < child2.second;
              });

    // Put sorted children to queue.
    std::for_each(
        children.begin(), children.end(),
        [&q](const std::pair<int, float>& child) { q.push(child.first); });

    // Look up the parent's index in sortedTree.
    int curNewIdx = oldToNew[curIdx];

    // Update sortedTree and sortedIndices.
    for (const std::pair<int, float>& childPair : children) {
      int childIdx = childPair.first;
      TreeNode<T> newChild = tree.nodes[childIdx];

      // Update the parent to the parent's new index.
      newChild.parent = curNewIdx;
      newChild.children.clear();  // Will add its children indices later.

      // The new node's index is the current size of sortedTree.
      int childNewIdx = static_cast<int>(sortedTree.nodes.size());
      sortedTree.nodes.push_back(newChild);
      sortedIndices.push_back(childIdx);
      oldToNew[childIdx] = childNewIdx;

      // Append the new child index to children vector.
      sortedTree.nodes[curNewIdx].children.push_back(childNewIdx);
    }
  }
}

template <typename T>
void printTree(const TreeWrapper<T>& tree, const std::string& treeName) {
  if (!kDebug) return;
  std::cout << "Tree: " << treeName << " Timestamp: " << tree.timestamp
            << std::endl;
  for (size_t i = 0; i < tree.nodes.size(); ++i) {
    std::cout << "  Node " << i << ": pos=(" << tree.nodes[i].posX << ", "
              << tree.nodes[i].posY << ")"
              << ", offset=" << tree.nodes[i].offset
              << ", angle=" << tree.nodes[i].angle
              << ", type=" << tree.nodes[i].type
              << ", parent=" << tree.nodes[i].parent << std::endl;
  }
}

// Function to generate final normalized feature vectors for each node.
// The final feature vector is constructed as follows:
// [TPE embedding x, TPE embedding y,
//  normalized posX, normalized posY,
//  normalized offset,
//  sin(angle), cos(angle),
//  one-hot encoding for type (3 dimensions)]
template <typename T>
std::vector<std::vector<T>> generateFeatureVectors(const TreeWrapper<T>& tree) {
  // Determine normalization parameters for tpeRadius.
  T tpeRadiusMin = std::numeric_limits<T>::max();
  T tpeRadiusMax = std::numeric_limits<T>::lowest();
  // Determine normalization parameters for posX, posY and offset.
  T posXMin = std::numeric_limits<T>::max();
  T posXMax = std::numeric_limits<T>::lowest();
  T posYMin = std::numeric_limits<T>::max();
  T posYMax = std::numeric_limits<T>::lowest();
  T offsetMax = 0.0;

  for (const auto& node : tree.nodes) {
    if (node.tpeRadius < tpeRadiusMin) tpeRadiusMin = node.tpeRadius;
    if (node.tpeRadius > tpeRadiusMax) tpeRadiusMax = node.tpeRadius;

    if (node.posX < posXMin) posXMin = node.posX;
    if (node.posX > posXMax) posXMax = node.posX;
    if (node.posY < posYMin) posYMin = node.posY;
    if (node.posY > posYMax) posYMax = node.posY;
    if (node.offset > offsetMax) offsetMax = node.offset;
  }

  // Allocate a vector to hold the final feature vectors.
  std::vector<std::vector<T>> finalFeatures;
  finalFeatures.reserve(tree.nodes.size());

  // For each node compute the final feature vector.
  // The order is: [TPE_x, TPE_y, norm_posX, norm_posY, norm_offset, sin(angle),
  // cos(angle), one-hot type]
  for (const auto& node : tree.nodes) {
    std::vector<T> featureVector;

    // Append precomputed TPE embedding.
    featureVector.push_back(node.tpeX);
    featureVector.push_back(node.tpeY);

    // Normalize tpeRadius using min-max scaling.
    T normRadius =
        (tpeRadiusMax - tpeRadiusMin == 0)
            ? 0.5
            : (node.tpeRadius - tpeRadiusMin) / (tpeRadiusMax - tpeRadiusMin);
    featureVector.push_back(normRadius);

    // Convert tpe angle to sine and cosine components.
    featureVector.push_back(std::sin(node.tpeAngle));
    featureVector.push_back(std::cos(node.tpeAngle));

    // Normalize original position using min-max scaling.
    T normPosX = (posXMax - posXMin == 0)
                     ? 0.5
                     : (node.posX - posXMin) / (posXMax - posXMin);
    T normPosY = (posYMax - posYMin == 0)
                     ? 0.5
                     : (node.posY - posYMin) / (posYMax - posYMin);
    featureVector.push_back(normPosX);
    featureVector.push_back(normPosY);

    // Normalize offset by dividing by the maximum offset.
    T normOffset = (offsetMax == 0) ? 0.0 : node.offset / offsetMax;
    featureVector.push_back(normOffset);

    featureVector.push_back(node.angle / (2 * M_PI));

    featureVector.push_back(node.type);

    finalFeatures.push_back(featureVector);
  }

  return finalFeatures;
}

template <typename T>
void printFeatureVectors(const std::vector<std::vector<T>>& featureVectors,
                         const std::string& treeName) {
  if (!kDebug) return;
  std::cout << "Feature vectors for Tree " << treeName << std::endl;
  for (size_t i = 0; i < featureVectors.size(); ++i) {
    std::cout << "  Node " << i + 1 << " final feature vector: ";
    for (T val : featureVectors[i]) {
      std::cout << val << " ";
    }
    std::cout << std::endl;
  }
}

// Computes the cosine similarity between two vectors.
template <typename T>
T computeCosineSimilarity(const std::vector<T>& vectorA,
                          const std::vector<T>& vectorB) {
  T dotProduct = 0.0;
  T normA = 0.0;
  T normB = 0.0;
  for (size_t i = 0; i < vectorA.size(); i++) {
    dotProduct += vectorA[i] * vectorB[i];
    normA += vectorA[i] * vectorA[i];
    normB += vectorB[i] * vectorB[i];
  }
  if (normA == 0 || normB == 0) {
    return 0.0;  // Handle zero vectors.
  }
  return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

// Computes the negative Euclidean distance between two vectors.
// (Using negative distance so that a higher value indicates a better match.)
template <typename T>
T computeEuclideanSimilarity(const std::vector<T>& vectorA,
                             const std::vector<T>& vectorB) {
  T sumSquares = 0.0;
  for (size_t i = 0; i < vectorA.size(); i++) {
    T diff = vectorA[i] - vectorB[i];
    sumSquares += diff * diff;
  }
  return -std::sqrt(sumSquares);
}

// Calculates the similarity matrix between two sets of feature vectors.
// Each row corresponds to a node in Tree A and each column to a node in Tree B.
// The metric parameter decides whether to use "euclidean" or "cosine".
template <typename T>
std::vector<std::vector<T>> createSimilarityMatrix(
    const std::vector<std::vector<T>>& featuresA,
    const std::vector<std::vector<T>>& featuresB,
    const std::string& metric = "euclidean") {
  size_t numNodesA = featuresA.size();
  size_t numNodesB = featuresB.size();
  std::vector<std::vector<T>> similarityMatrix(numNodesA,
                                               std::vector<T>(numNodesB, 0.0));

  for (size_t i = 0; i < numNodesA; i++) {
    for (size_t j = 0; j < numNodesB; j++) {
      if (metric == "euclidean") {
        similarityMatrix[i][j] =
            computeEuclideanSimilarity(featuresA[i], featuresB[j]);
      } else if (metric == "cosine") {
        similarityMatrix[i][j] =
            computeCosineSimilarity(featuresA[i], featuresB[j]);
      }
    }
  }
  return similarityMatrix;
}

template <typename T>
void printSimilarityMatrix(const std::vector<std::vector<T>>& similarityMatrix,
                           const std::string& similarityType) {
  if (!kDebug) return;
  std::cout << "Similarity Matrix (" << similarityType << ")" << std::endl;
  for (const auto& row : similarityMatrix) {
    for (T sim : row) {
      std::cout << sim << "  ";
    }
    std::cout << std::endl;
  }
}

// Convert a similarity matrix into a cost matrix.
// The cost matrix is computed as: cost[i][j] = -similarityMatrix[i][j].
template <typename T>
std::vector<std::vector<T>> convertSimilarityMatrix2CostMatrix(
    const std::vector<std::vector<T>>& similarityMatrix) {
  size_t numRows = similarityMatrix.size();
  if (numRows == 0) {
    return {};  // Return empty if the input matrix is empty.
  }

  size_t numCols = similarityMatrix[0].size();

  // Use the maximum similarity value to construct the cost matrix.
  std::vector<std::vector<T>> costMatrix(numRows, std::vector<T>(numCols, 0.0));
  for (size_t i = 0; i < numRows; i++) {
    for (size_t j = 0; j < numCols; j++) {
      costMatrix[i][j] = -similarityMatrix[i][j];
    }
  }

  return costMatrix;
}

template <typename T>
void printCostMatrix(const std::vector<std::vector<T>>& costMatrix,
                     const std::string& costType) {
  if (!kDebug) return;
  std::cout << "Cost Matrix (" << costType << ")" << std::endl;
  for (const auto& row : costMatrix) {
    for (T sim : row) {
      std::cout << sim << "  ";
    }
    std::cout << std::endl;
  }
}

template <typename T>
std::vector<int> matchTrees(TreeWrapper<T>& treeA, TreeWrapper<T>& treeB,
                            const std::string& similarityType) {
  // Generate TPE of treeA.
  generateTreePreservingEmbedding(treeA);
  printTreePreservingEmbedding(treeA, "treeA");

  // Generate TPE of treeB.
  generateTreePreservingEmbedding(treeB);
  printTreePreservingEmbedding(treeB, "treeB");

  // Generate feature vectors for treeA.
  std::vector<std::vector<T>> featureVectorsA = generateFeatureVectors(treeA);
  printFeatureVectors(featureVectorsA, "treeA");

  // Generate feature vectors for treeB.
  std::vector<std::vector<T>> featureVectorsB = generateFeatureVectors(treeB);
  printFeatureVectors(featureVectorsB, "treeB");

  std::pair<T, std::vector<int>> maxMatching;
  if (similarityType == "cosine") {
    // Calculate the similarity matrix for tree A and tree B by using cosine
    // similarity.
    std::vector<std::vector<T>> similarityMatrixCosine =
        createSimilarityMatrix(featureVectorsA, featureVectorsB, "cosine");
    printSimilarityMatrix(similarityMatrixCosine, "cosine");

    // Convert cosine similarity matrix to cost matrix.
    std::vector<std::vector<T>> costMatrixCosine =
        convertSimilarityMatrix2CostMatrix(similarityMatrixCosine);
    printCostMatrix(costMatrixCosine, "cosine");

    // Run Hungarian Algorithm on the cosine cost matrix to get best maximum
    // match.
    maxMatching = hungarianAlgorithm(costMatrixCosine);
  } else if (similarityType == "euclidean") {
    // Calculate the similarity matrix for tree A and tree B by using euclidean
    // similarity.
    std::vector<std::vector<T>> similarityMatrixEuclidean =
        createSimilarityMatrix(featureVectorsA, featureVectorsB);
    printSimilarityMatrix(similarityMatrixEuclidean, "euclidean");

    // Convert euclidean similarity matrix to cost matrix.
    std::vector<std::vector<T>> costMatrixEuclidean =
        convertSimilarityMatrix2CostMatrix(similarityMatrixEuclidean);
    printCostMatrix(costMatrixEuclidean, "euclidean");

    // Run Hungarian Algorithm on the euclidean cost matrix to get best maximum
    // match.
    maxMatching = hungarianAlgorithm(costMatrixEuclidean);
  } else {
    std::cout << "unknown similarityType " << similarityType << std::endl;
    exit(1);
  }

  return maxMatching.second;
}

void printMatching(const std::vector<int>& matchRes, const std::string& treeA,
                   const std::string& treeB) {
  if (!kDebug) return;
  std::cout << "Maximum matching between " << treeA << " and " << treeB << ":"
            << std::endl;
  for (int i = 0; i < matchRes.size(); ++i) {
    std::cout << "  " << i << " -> " << matchRes[i] << std::endl;
  }
}

// Explicit instantiations for type to use.

template void clockwiseRotate90Degrees<float>(TreeWrapper<float>& tree);

template float computeAngle<float>(float x1, float y1, float x2, float y2);

template void sortTree<float>(const TreeWrapper<float>& tree,
                              TreeWrapper<float>& sortedTree,
                              std::vector<int>& sortedIndices);

template void printTree<float>(const TreeWrapper<float>& tree,
                               const std::string& treeName);

template std::vector<std::vector<float>> generateFeatureVectors<float>(
    const TreeWrapper<float>& tree);

template void printFeatureVectors<float>(
    const std::vector<std::vector<float>>& featureVectors,
    const std::string& treeName);

template float computeCosineSimilarity<float>(
    const std::vector<float>& vectorA, const std::vector<float>& vectorB);

template float computeEuclideanSimilarity<float>(
    const std::vector<float>& vectorA, const std::vector<float>& vectorB);

template std::vector<std::vector<float>> createSimilarityMatrix<float>(
    const std::vector<std::vector<float>>& featuresA,
    const std::vector<std::vector<float>>& featuresB,
    const std::string& metric = "euclidean");

template void printSimilarityMatrix<float>(
    const std::vector<std::vector<float>>& similarityMatrix,
    const std::string& similarityType);

template std::vector<std::vector<float>> convertSimilarityMatrix2CostMatrix<
    float>(const std::vector<std::vector<float>>& similarityMatrix);

template void printCostMatrix<float>(
    const std::vector<std::vector<float>>& costMatrix,
    const std::string& costType);

template std::vector<int> matchTrees<float>(TreeWrapper<float>& treeA,
                                            TreeWrapper<float>& treeB,
                                            const std::string& similarityType);