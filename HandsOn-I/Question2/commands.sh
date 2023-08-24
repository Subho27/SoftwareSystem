
# To compile InfiniteLoop.c file and rename output file as InfiniteLoop
gcc -o InfiniteLoop InfiniteLoop.c

# To run InfiniteLoop file from previous commands output
./InfiniteLoop

# To get process id of InfiniteLoop
processID="$(pidof InfiniteLoop)"

# To get into process directory of proc 
cd /proc/"$processID"

# Now you are inside process directory

