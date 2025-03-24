#include "HungarianAlgorithm.hpp"

#include <algorithm>
#include <limits>

// ---------------------------------------------------------------------------
// Function: padCostMatrix
//
// Pads a possibly rectangular cost matrix into a square matrix of dimension
// 'size' using the given INF value. This is required because the algorithm
// works on square matrices.
// ---------------------------------------------------------------------------
template <typename T>
std::vector<std::vector<T>> padCostMatrix(
    const std::vector<std::vector<T>>& costMatrix, int size, T INF) {
  int numRows = costMatrix.size();
  int numCols = (numRows > 0 ? costMatrix[0].size() : 0);
  std::vector<std::vector<T>> paddedCost(size, std::vector<T>(size, INF));
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (i < numRows && j < numCols) paddedCost[i][j] = costMatrix[i][j];
    }
  }
  return paddedCost;
}

// ---------------------------------------------------------------------------
// Function: buildAssignment
//
// Based on the matching vector 'columnMatching' produced by the algorithm
// (which is defined over the padded square matrix), this function builds the
// final assignment vector for the original matrix dimensions. Any assignment
// outside the range of the original matrix is discarded.
// ---------------------------------------------------------------------------
std::vector<int> buildAssignment(const std::vector<int>& columnMatching,
                                 int numRows, int numCols, int size) {
  std::vector<int> assignment(numRows, -1);
  for (int j = 1; j <= size; j++) {
    int matchedRow = columnMatching[j];
    if (matchedRow <= numRows && j <= numCols) {
      assignment[matchedRow - 1] = j - 1;
    }
  }
  return assignment;
}

// ---------------------------------------------------------------------------
// Function: exploreColumns
//
// Given the current column (currentColumn), this function examines all
// unvisited columns to update their minimal reduced cost (minReducedCost).
// It records the path using the previousColumn vector. Returns a pair
// containing the candidate column with the smallest reduced cost and the
// corresponding delta value.
// ---------------------------------------------------------------------------
template <typename T>
std::pair<int, T> exploreColumns(int currentColumn, int size,
                                 const std::vector<std::vector<T>>& cost,
                                 const std::vector<T>& rowDuals,
                                 const std::vector<T>& colDuals,
                                 std::vector<T>& minReducedCost,
                                 const std::vector<bool>& visitedColumns,
                                 const std::vector<int>& columnMatching,
                                 std::vector<int>& previousColumn, T INF) {
  int rowIdx =
      columnMatching[currentColumn];  // The row associated with currentColumn.
  int candidateColumn = 0;            // Candidate column with minimal cost.
  T delta = INF;                      // Minimal adjustment value.

  // --- Explore all columns to update their minimal reduced costs ---
  for (int j = 1; j <= size; j++) {
    if (!visitedColumns[j]) {
      T reducedCost = cost[rowIdx - 1][j - 1] - rowDuals[rowIdx] - colDuals[j];
      if (reducedCost < minReducedCost[j]) {
        minReducedCost[j] = reducedCost;
        previousColumn[j] = currentColumn;
      }
      if (minReducedCost[j] < delta) {
        delta = minReducedCost[j];
        candidateColumn = j;
      }
    }
  }
  return std::make_pair(candidateColumn, delta);
}

// ---------------------------------------------------------------------------
// Function: updateDualVariables
//
// Given the computed delta, updates the dual variables for all columns. For
// visited columns, it adjusts the corresponding row and column duals; for
// unvisited columns, it decrements minReducedCost by delta.
// ---------------------------------------------------------------------------
template <typename T>
void updateDualVariables(int size, std::vector<T>& rowDuals,
                         std::vector<T>& colDuals,
                         std::vector<T>& minReducedCost,
                         const std::vector<bool>& visitedColumns,
                         const std::vector<int>& columnMatching, T delta) {
  // --- Update dual variables for all columns ---
  for (int j = 0; j <= size; j++) {
    if (visitedColumns[j]) {
      rowDuals[columnMatching[j]] += delta;
      colDuals[j] -= delta;
    } else {
      minReducedCost[j] -= delta;
    }
  }
}

// ---------------------------------------------------------------------------
// Function: reconstructMatching
//
// Once an unmatched column (free column) is found, this function retraces the
// augmenting path using the previousColumn vector to update the matching in
// columnMatching.
// ---------------------------------------------------------------------------
void reconstructMatching(int currentColumn, std::vector<int>& columnMatching,
                         const std::vector<int>& previousColumn) {
  while (currentColumn != 0) {
    int temp = previousColumn[currentColumn];
    columnMatching[currentColumn] = columnMatching[temp];
    currentColumn = temp;
  }
}

// ---------------------------------------------------------------------------
// Function: augmentRowAssignment
//
// Uses exploreColumns() and updateDualVariables() to build an augmenting path
// for the current row. After reaching an unmatched column, the function
// reconstructs the matching using reconstructMatching().
// ---------------------------------------------------------------------------
template <typename T>
void augmentRowAssignment(int currentRow, int size,
                          const std::vector<std::vector<T>>& cost,
                          std::vector<T>& rowDuals, std::vector<T>& colDuals,
                          std::vector<int>& columnMatching,
                          std::vector<int>& previousColumn, T INF) {
  columnMatching[0] = currentRow;
  std::vector<T> minReducedCost(size + 1, INF);
  std::vector<bool> visitedColumns(size + 1, false);

  int currentColumn = 0;
  // --- Build the augmenting path until a free column is reached ---
  do {
    visitedColumns[currentColumn] = true;
    std::pair<int, T> result = exploreColumns(
        currentColumn, size, cost, rowDuals, colDuals, minReducedCost,
        visitedColumns, columnMatching, previousColumn, INF);
    int candidateColumn = result.first;
    T delta = result.second;
    updateDualVariables(size, rowDuals, colDuals, minReducedCost,
                        visitedColumns, columnMatching, delta);
    currentColumn = candidateColumn;
  } while (columnMatching[currentColumn] != 0);

  // --- Reconstruct the matching along the computed augmenting path ---
  reconstructMatching(currentColumn, columnMatching, previousColumn);
}

// ---------------------------------------------------------------------------
// Function: hungarianAlgorithm
//
// Sets up the problem by padding the cost matrix and initializing dual
// variables and helper arrays. Processes each row with augmentRowAssignment()
// to build the optimal matching, and finally constructs the assignment for the
// original matrix dimensions.
// ---------------------------------------------------------------------------
template <typename T>
std::pair<T, std::vector<int>> hungarianAlgorithm(
    const std::vector<std::vector<T>>& costMatrix) {
  int numRows = costMatrix.size();
  if (numRows == 0) return std::make_pair(T(0), std::vector<int>());
  int numCols = costMatrix[0].size();

  // Determine the dimension for the square cost matrix.
  int size = std::max(numRows, numCols);
  const T INF = std::numeric_limits<T>::max() / 4;

  // --- Pad the cost matrix into a square matrix ---
  std::vector<std::vector<T>> cost = padCostMatrix(costMatrix, size, INF);

  // Initialize dual variables and matching-related arrays.
  std::vector<T> rowDuals(size + 1, 0), colDuals(size + 1, 0);
  std::vector<int> columnMatching(size + 1, 0), previousColumn(size + 1, 0);

  // Process each row to build the matching.
  for (int i = 1; i <= size; i++) {
    augmentRowAssignment(i, size, cost, rowDuals, colDuals, columnMatching,
                         previousColumn, INF);
  }

  // --- Build the final assignment based on the original matrix dimensions ---
  std::vector<int> assignment =
      buildAssignment(columnMatching, numRows, numCols, size);

  T optimalCost = -colDuals[0];
  return std::make_pair(optimalCost, assignment);
}

// Explicit instantiations for type to use.
template std::vector<std::vector<float>> padCostMatrix<float>(
    const std::vector<std::vector<float>>& costMatrix, int size, float INF);

template std::pair<int, float> exploreColumns<float>(
    int currentColumn, int size, const std::vector<std::vector<float>>& cost,
    const std::vector<float>& rowDuals, const std::vector<float>& colDuals,
    std::vector<float>& minReducedCost, const std::vector<bool>& visitedColumns,
    const std::vector<int>& columnMatching, std::vector<int>& previousColumn,
    float INF);

template void updateDualVariables<float>(
    int size, std::vector<float>& rowDuals, std::vector<float>& colDuals,
    std::vector<float>& minReducedCost, const std::vector<bool>& visitedColumns,
    const std::vector<int>& columnMatching, float delta);

template void augmentRowAssignment<float>(
    int currentRow, int size, const std::vector<std::vector<float>>& cost,
    std::vector<float>& rowDuals, std::vector<float>& colDuals,
    std::vector<int>& columnMatching, std::vector<int>& previousColumn,
    float INF);

template std::pair<float, std::vector<int>> hungarianAlgorithm<float>(
    const std::vector<std::vector<float>>& costMatrix);