cd ../../tree_maximum_matching_build/
if [ $# -eq 0 ]; then
    ./TreeMatchingTest
elif [ $# -eq 2 ]; then
    ./TreeMatchingTest "$1" "$2"
elif [ $# -eq 4 ]; then
    ./TreeMatchingTest "$1" "$2" "$3" "$4"
fi