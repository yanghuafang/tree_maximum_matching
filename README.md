# tree_maximum_matching
Tree Maximum Matching through Tree Preserving Embedding and Hungarian Algorithm.

## Environment preparation
conda install matplotlib mplcursors numpy  

conda install -c conda-forge nlohmann_json  
or  
brew install nlohmann-json  

## Build
cd scripts  
./build.sh

## Test Hungarian Algorithm
./runHungarianAlgorithmTest.sh

## Test Tree PreservingEmbedding
// Calculate feature vectors for nodes of trees generated randomly by Tree Preserving Embedding algorithm.  
./runTreePreservingEmbeddingTest.sh  

// Load tree from a json file, then calculate feature vectors of the tree by Tree Preserving Embedding algorithm.  
./runTreePreservingEmbeddingTest --tree=tree1.json  

## Test Tree Matching
// Get the maximum matching between two trees generated randomly.  
./runTreeMatchingTest.sh  

// Get the maximum matching between two trees generated randomly, save the two generated trees to json files.  
./runTreeMatchingTest.sh --output-tree1=tree1.json --output-tree2=tree2.json  

// Load two trees from json files, get the maximum matching between the loaded trees.  
./runTreeMatchingTest.sh --tree1=tree1.json --tree2=tree2.json  

// Load two trees from json files, get the maximum matching between the loaded trees, save two trees to json files.  
./runTreeMatchingTest.sh --tree1=tree1.json --tree2=tree2.json --output-tree1=treeA.json --output-tree2=treeB.json   