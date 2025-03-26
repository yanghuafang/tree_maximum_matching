cd ../../tree_maximum_matching_build/
if [ $# -eq 0 ]; then
    ./HungarianAlgorithmTest
else
    ./HungarianAlgorithmTest "$1" "$2"
fi