source conda.sh

cd ../../tree_maximum_matching_build/
if [ $# -eq 0 ]; then
    ./TreePreservingEmbeddingTest
else
    ./TreePreservingEmbeddingTest "$1"
fi