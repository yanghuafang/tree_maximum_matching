#include "TreeMatching.hpp"

#include <iostream>

#include "HungarianAlgorithm.hpp"
#include "TreePreservingEmbedding.hpp"

// Function to generate final normalized feature vectors for each node.
// The final feature vector is constructed as follows:
// [TPE embedding x, TPE embedding y,
//  normalized posX, normalized posY,
//  normalized offset,
//  sin(angle), cos(angle),
//  one-hot encoding for type (3 dimensions)]
template <typename T>
std::vector<std::vector<T>> generateFeatureVectors(
    const std::vector<Node<T>>& tree) {
  // Determine normalization parameters for tpeRadius.
  T tpeRadiusMin = std::numeric_limits<T>::max();
  T tpeRadiusMax = std::numeric_limits<T>::lowest();
  // Determine normalization parameters for posX, posY and offset.
  T posXMin = std::numeric_limits<T>::max();
  T posXMax = std::numeric_limits<T>::lowest();
  T posYMin = std::numeric_limits<T>::max();
  T posYMax = std::numeric_limits<T>::lowest();
  T offsetMax = 0.0;

  for (const auto& node : tree) {
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
  finalFeatures.reserve(tree.size());

  // For each node compute the final feature vector.
  // The order is: [TPE_x, TPE_y, norm_posX, norm_posY, norm_offset, sin(angle),
  // cos(angle), one-hot type]
  for (const auto& node : tree) {
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

    // Convert angle to sine and cosine components.
    featureVector.push_back(std::sin(node.angle));
    featureVector.push_back(std::cos(node.angle));

    // Append one-hot encoding for the node type.
    // Assuming node.type is one of {0, 1, 2}:
    for (int t = 0; t < 3; t++) {
      featureVector.push_back((node.type == t) ? 1.0 : 0.0);
    }

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
  std::cout << "Similarity Matrix (" << similarityType << ")" << std::endl
            << "  ";
  for (const auto& row : similarityMatrix) {
    for (T sim : row) {
      std::cout << sim << "\t";
    }
    std::cout << std::endl;
  }
}

// Convert a similarity matrix into a cost matrix.
// The cost matrix is computed as: cost[i][j] = maxSimilarity -
// similarityMatrix[i][j], where maxSimilarity is the maximum value in the
// similarity matrix.
template <typename T>
std::vector<std::vector<T>> convertSimilarityMatrix2CostMatrix(
    const std::vector<std::vector<T>>& similarityMatrix) {
  size_t numRows = similarityMatrix.size();
  if (numRows == 0) {
    return {};  // Return empty if the input matrix is empty.
  }

  size_t numCols = similarityMatrix[0].size();

  // Find the maximum similarity value in the entire matrix.
  T maxSimilarity = -std::numeric_limits<T>::infinity();
  for (size_t i = 0; i < numRows; i++) {
    for (size_t j = 0; j < numCols; j++) {
      if (similarityMatrix[i][j] > maxSimilarity) {
        maxSimilarity = similarityMatrix[i][j];
      }
    }
  }

  // Use the maximum similarity value to construct the cost matrix.
  std::vector<std::vector<T>> costMatrix(numRows, std::vector<T>(numCols, 0.0));
  for (size_t i = 0; i < numRows; i++) {
    for (size_t j = 0; j < numCols; j++) {
      costMatrix[i][j] = maxSimilarity - similarityMatrix[i][j];
    }
  }

  return costMatrix;
}

template <typename T>
void printCostMatrix(const std::vector<std::vector<T>>& costMatrix,
                     const std::string& costType) {
  if (!kDebug) return;
  std::cout << "Cost Matrix (" << costType << ")" << std::endl << "  ";
  for (const auto& row : costMatrix) {
    for (T sim : row) {
      std::cout << sim << "\t";
    }
    std::cout << std::endl;
  }
}

template <typename T>
std::vector<int> matchTrees(std::vector<Node<T>>& treeA,
                            std::vector<Node<T>>& treeB,
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
template std::vector<std::vector<float>> generateFeatureVectors(
    const std::vector<Node<float>>& tree);

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

template std::vector<int> matchTrees<float>(std::vector<Node<float>>& treeA,
                                            std::vector<Node<float>>& treeB,
                                            const std::string& similarityType);