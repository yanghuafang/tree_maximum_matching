source conda.sh

cd ../../tree_maximum_matching_build/
if [ $# -eq 0 ]; then
    ./TreePreservingEmbeddingTest
elif [ $# -eq 1 ]; then
    ./TreePreservingEmbeddingTest "$1"
elif [ $# -eq 2 ]; then
    ./TreePreservingEmbeddingTest "$1" "$2"
fi