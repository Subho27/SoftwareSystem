
# To compile CreateFile.c file and rename output file as CreateFile
gcc -o CreateFile CreateFile.c

# To run InfiniteLoop file from previous commands output
./CreateFile

# To get process id of InfiniteLoop
processID="$(pidof CreateFile)"

# To get into process directory of proc 
cd /proc/"$processID"/fd

# Now you are inside file descriptor table directory
