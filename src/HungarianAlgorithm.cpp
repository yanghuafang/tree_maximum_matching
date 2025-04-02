#include "HungarianAlgorithm.hpp"

#include <algorithm>
#include <limits>

/*
 * Function: padCostMatrix
 * -----------------------
 * Converts a possibly rectangular cost matrix into a square matrix.
 * Cells outside the original matrix are filled with INF so that they
 * are not chosen in an optimal assignment.
 *
 * Parameters:
 *  - costMatrix: The original cost matrix.
 *  - size: The target dimension for the square matrix.
 *  - INF: A large value used to discourage the selection of padded cells.
 *
 * Returns:
 *  A square matrix of dimensions size x size.
 */
template <typename T>
std::vector<std::vector<T>> padCostMatrix(
    const std::vector<std::vector<T>>& costMatrix, int size, T INF) {
  int numRows = costMatrix.size();
  int numCols = (numRows > 0 ? costMatrix[0].size() : 0);

  // Create a square matrix filled with INF.
  std::vector<std::vector<T>> paddedCost(size, std::vector<T>(size, INF));

  // Copy over the original values; indices beyond the original dimensions
  // remain as INF.
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (i < numRows && j < numCols) paddedCost[i][j] = costMatrix[i][j];
    }
  }
  return paddedCost;
}

/*
 * Function: buildAssignment
 * -------------------------
 * Constructs the final assignment from the computed matching.
 * It maps the 1-indexed matching results back to 0-indexed assignments
 * for the original matrix dimensions and ignores dummy rows or columns.
 *
 * Parameters:
 *  - columnMatching: The 1-indexed matching vector from the algorithm.
 *  - numRows: Number of rows in the original cost matrix.
 *  - numCols: Number of columns in the original cost matrix.
 *  - size: The dimension of the padded cost matrix.
 *
 * Returns:
 *  A vector where each element is the assigned column (0-indexed) for that row.
 */
std::vector<int> buildAssignment(const std::vector<int>& columnMatching,
                                 int numRows, int numCols, int size) {
  std::vector<int> assignment(numRows, -1);
  // Iterate over the padded matching results (starting at index 1).
  for (int j = 1; j <= size; j++) {
    int matchedRow = columnMatching[j];
    // Validate that the match is within the original dimensions.
    if (matchedRow <= numRows && j <= numCols) {
      // Convert from 1-indexed to 0-indexed.
      assignment[matchedRow - 1] = j - 1;
    }
  }
  return assignment;
}

/*
 * Function: exploreColumns
 * ------------------------
 * From a given column (associated with a row), this function examines
 * each unvisited column to update the minimal "reduced cost" for reaching that
 * column.
 *
 * The reduced cost for a given cell is computed as:
 *   cost[row][col] - rowDuals[row] - colDuals[col]
 *
 * While scanning, it records the predecessor of each column for path
 * construction.
 *
 * Parameters:
 *  - currentColumn: The column from which to start the exploration.
 *  - size: The size of the square matrix.
 *  - cost: The padded cost matrix.
 *  - rowDuals: Dual variables for rows.
 *  - colDuals: Dual variables for columns.
 *  - minReducedCost: Array holding the current best reduced costs for each
 * column.
 *  - visitedColumns: Marks columns already processed during this augmentation.
 *  - columnMatching: Current matching (1-indexed) indicating which row is
 * linked to a column.
 *  - previousColumn: Tracks the chain/path of column choices.
 *  - INF: A large value representing an un-updated cost.
 *
 * Returns:
 *  A pair consisting of:
 *    candidateColumn - the next column to be added to the augmenting path,
 *    delta - the minimum additional cost adjustment found.
 */
template <typename T>
std::pair<int, T> exploreColumns(int currentColumn, int size,
                                 const std::vector<std::vector<T>>& cost,
                                 const std::vector<T>& rowDuals,
                                 const std::vector<T>& colDuals,
                                 std::vector<T>& minReducedCost,
                                 const std::vector<bool>& visitedColumns,
                                 const std::vector<int>& columnMatching,
                                 std::vector<int>& previousColumn, T INF) {
  // Retrieve the row currently matched with the current column.
  // columnMatching[currentColumn]: The row associated with currentColumn.
  int rowIdx = columnMatching[currentColumn];

  // Candidate column with minimal cost.
  int candidateColumn = 0;

  // Minimal adjustment value.
  T delta = INF;

  // Explore all columns to update their minimal reduced costs, columns are
  // regarded with 1-based indexing.
  for (int j = 1; j <= size; j++) {
    if (!visitedColumns[j]) {
      // cost[rowIdx - 1][j - 1]: 1-based indexing to 0-based indexing.
      T reducedCost = cost[rowIdx - 1][j - 1] - rowDuals[rowIdx] - colDuals[j];

      // For each unvisited column j, if a better reduced cost is found.
      if (reducedCost < minReducedCost[j]) {
        minReducedCost[j] = reducedCost;
      }

      // Update delta and candidate if this cell's cost is the best so far.
      if (minReducedCost[j] < delta) {
        delta = minReducedCost[j];
        candidateColumn = j;
      }
    }
  }

  // Record that column candidateColumn was reached from currentColumn during
  // exploration.
  // This establishes a path for backtracking when reconstructing the
  // augmenting path.
  previousColumn[candidateColumn] = currentColumn;
  return std::make_pair(candidateColumn, delta);
}

/*
 * Function: updateDualVariables
 * -----------------------------
 * Adjusts dual variables after each exploration round.
 * For every visited column along the augmented path, update:
 *   - The row dual associated with that column.
 *   - The column dual itself.
 * For non-visited columns, decrease their stored minReducedCost.
 *
 * These updates maintain the feasibility condition for the dual variables.
 *
 * Parameters:
 *  - size: The dimension of the square matrix.
 *  - rowDuals: The row dual variables (updated in place).
 *  - colDuals: The column dual variables (updated in place).
 *  - minReducedCost: Array of current minimal reduced costs (updated in place).
 *  - visitedColumns: Marks which columns contribute to the current augmenting
 * path.
 *  - columnMatching: The current matching which also indicates associated rows.
 *  - delta: The minimal adjustment value from the current exploration.
 */
template <typename T>
void updateDualVariables(int size, std::vector<T>& rowDuals,
                         std::vector<T>& colDuals,
                         std::vector<T>& minReducedCost,
                         const std::vector<bool>& visitedColumns,
                         const std::vector<int>& columnMatching, T delta) {
  // Update dual variables for all columns, index 0 is a holder for path
  // construction.
  for (int j = 0; j <= size; j++) {
    if (visitedColumns[j]) {
      // For visited columns, adjust the dual variables associated with the
      // matching.
      rowDuals[columnMatching[j]] += delta;
      colDuals[j] -= delta;
    } else {
      // For unvisited columns, update the temporary cost estimate.
      minReducedCost[j] -= delta;
    }
  }
}

/*
 * Function: reconstructMatching
 * -----------------------------
 * After finding an unmatched (free) column, this function backtracks through
 * the augmenting path using the 'previousColumn' array and updates the
 * matching.
 *
 * Essentially, it "flips" the matching along the found path.
 *
 * Parameters:
 *  - currentColumn: The unmatched (free) column that ended the augmenting path.
 *  - columnMatching: The matching vector to update (modified in place).
 *  - previousColumn: The array containing the backtracking information.
 */
void reconstructMatching(int currentColumn, std::vector<int>& columnMatching,
                         const std::vector<int>& previousColumn) {
  // Backtrack until the dummy index (0) is reached.
  while (currentColumn != 0) {
    int temp = previousColumn[currentColumn];
    // Redirect matching: assign the row from the previous column.
    columnMatching[currentColumn] = columnMatching[temp];
    currentColumn = temp;
  }
}

/*
 * Function: augmentRowAssignment
 * ------------------------------
 * For a given row (currentRow), builds an augmenting path to improve the
 * overall matching. It repeatedly explores columns and adjusts dual variables
 * until an unmatched (free) column is found.
 *
 * Parameters:
 *  - currentRow: The row for which the assignment is being improved.
 *  - size: Dimension of the padded square matrix.
 *  - cost: The padded cost matrix.
 *  - rowDuals: Row dual variables (updated in the process).
 *  - colDuals: Column dual variables (updated in the process).
 *  - columnMatching: The matching vector (1-indexed, updated in place).
 *  - previousColumn: Array used to store the path for reconstructing the
 * matching.
 *  - INF: A large value representing infinity.
 */
template <typename T>
void augmentRowAssignment(int currentRow, int size,
                          const std::vector<std::vector<T>>& cost,
                          std::vector<T>& rowDuals, std::vector<T>& colDuals,
                          std::vector<int>& columnMatching,
                          std::vector<int>& previousColumn, T INF) {
  // Begin the augmenting path with currentRow assigned at the special index 0.
  columnMatching[0] = currentRow;

  // Initialize the minimal reduced cost for each column.
  std::vector<T> minReducedCost(size + 1, INF);

  // Keep track of which columns are included in the current augmenting path.
  std::vector<bool> visitedColumns(size + 1, false);

  int currentColumn = 0;
  // Build the augmenting path until an unmatched(free) column is reached.
  do {
    // Mark the current column as visited.
    visitedColumns[currentColumn] = true;

    // Explore all unvisited columns from the current column.
    std::pair<int, T> result = exploreColumns(
        currentColumn, size, cost, rowDuals, colDuals, minReducedCost,
        visitedColumns, columnMatching, previousColumn, INF);

    // Best candidate to extend the path.
    int candidateColumn = result.first;

    // Minimum cost adjustment found.
    T delta = result.second;

    // Update dual variables with the computed delta; this step facilitates
    // feasible progress.
    updateDualVariables(size, rowDuals, colDuals, minReducedCost,
                        visitedColumns, columnMatching, delta);

    // Move onto the next column candidate.
    currentColumn = candidateColumn;
  } while (columnMatching[currentColumn] != 0);

  // Once an unmatched (free) column is found, previousColumn contains the
  // indices of the columns that form the augmenting path from the starting row
  // to this free column. update the matching along the constructed augmenting
  // path.
  reconstructMatching(currentColumn, columnMatching, previousColumn);
}

/*
 * Function: hungarianAlgorithm
 * ----------------------------
 * Main function to solve the assignment problem using the Hungarian Algorithm.
 * It performs the following steps:
 *   1. Pads the original cost matrix to be square.
 *   2. Initializes dual variables and necessary bookkeeping arrays.
 *   3. Iteratively constructs augmenting paths for each row.
 *   4. Builds the final assignment and computes the optimal cost.
 *
 * Parameters:
 *  - costMatrix: The original cost matrix for the assignment problem.
 *
 * Returns:
 *  A pair where:
 *    - The first element is the total minimum cost.
 *    - The second element is the assignment vector mapping original rows to
 * columns.
 */
template <typename T>
std::pair<T, std::vector<int>> hungarianAlgorithm(
    const std::vector<std::vector<T>>& costMatrix) {
  int numRows = costMatrix.size();
  if (numRows == 0) return std::make_pair(T(0), std::vector<int>());
  int numCols = costMatrix[0].size();

  // Determine the dimension for the square cost matrix.
  int size = std::max(numRows, numCols);
  // Use a fraction of the maximum value to define INF, reducing risk of
  // overflow.
  const T INF = std::numeric_limits<T>::max() / 4;

  // Pad the cost matrix to form a square matrix where dummy cells have INF
  // cost.
  std::vector<std::vector<T>> cost = padCostMatrix(costMatrix, size, INF);

  // size + 1: 1-based indexing, valid range is [1, size]. Extra slot at index
  // 0.

  // Row and column dual variables used to adjust the cost matrix during
  // optimization.
  std::vector<T> rowDuals(size + 1, 0), colDuals(size + 1, 0);

  // Tracks the current matching of columns to rows.
  std::vector<int> columnMatching(size + 1, 0);

  // Used to trace back the path while constructing an augmenting path.
  // previousColumn is used to store the column indices that form the augmenting
  // path during the search for an unmatched (free) column. It essentially acts
  // as a breadcrumb trail, allowing the algorithm to trace back the path once
  // an unmatched (free) column is found.
  std::vector<int> previousColumn(size + 1, 0);

  // For each row (considering padded dimension), attempt to improve the
  // matching.
  for (int i = 1; i <= size; i++) {
    augmentRowAssignment(i, size, cost, rowDuals, colDuals, columnMatching,
                         previousColumn, INF);
  }

  // Map the computed matching back to an assignment for the original matrix
  // dimensions.
  std::vector<int> assignment =
      buildAssignment(columnMatching, numRows, numCols, size);

  // The optimal cost is derived from the processed dual variables.
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