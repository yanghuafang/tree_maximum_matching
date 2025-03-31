source conda.sh

cd ../../tree_maximum_matching_build/
if [ $# -eq 2 ]; then
    ./TreeMatchingTimeTest "$1" "$2"
fi