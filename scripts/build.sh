source conda.sh

# activate an env in which matplotlib is installed.
conda activate machine-learning

cmake -S ../ -B ../../tree_maximum_matching_build -DCMAKE_BUILD_TYPE=Debug
cmake --build ../../tree_maximum_matching_build

