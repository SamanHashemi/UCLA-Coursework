#! /usr/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#	8. lock wait time
#
# output:
#	lab2_list-1.png ... cost per operation vs threads and iterations
#	lab2_list-2.png ... threads and iterations that run (un-protected) w/o failure
#	lab2_list-3.png ... threads and iterations that run (protected) w/o failure
#	lab2_list-4.png ... cost per operation vs number of threads
#
# Note:
#	Managing data is simplified by keeping all of the results in a single
#	file.  But this means that the individual graphing commands have to
#	grep to select only the data they want.
#
#	Early in your implementation, you will not have data for all of the
#	tests, and the later sections may generate errors for missing data.
#

# general plot parameters
set terminal png
set datafile separator ","

# how many threads/iterations we can run without failure (w/o yielding)
set title "List-1: Throughput for Spin and Mutex Locked vs # of Threads"
set xlabel "# of Threads"
set logscale x 2
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_1.png'

# grep out only single threaded, un-protected, non-yield results
plot \
     "< grep -E 'list-none-m,[0-9][0-9]?,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'mutex' with linespoints lc rgb 'blue', \
     "< grep -E 'list-none-s,[0-9][0-9]?,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'spinlock' with linespoints lc rgb 'green'

set title "List-2: Average time per operation and wait-for-lock vs # of Threads"
set xlabel "Threads"
set logscale x 2
set ylabel "Time in nanoseconds (ns)"
set logscale y 10
set output 'lab2b_2.png'
# note that unsuccessful runs should have produced no output
plot \
     "< grep -E 'list-none-m,[0-9][0-9]?,1000,1,' lab2b_list.csv" using ($2):($7) \
	title 'avg time per operation' with linespoints lc rgb 'green', \
     "< grep -E 'list-none-m,[0-9][0-9]?,1000,1,' lab2b_list.csv" using ($2):($8) \
	title 'time waiting for lock' with linespoints lc rgb 'red'     


set title "List-3: Successful iterations vs Threads for each sync method"
unset logscale x
set logscale x 2
set xlabel "Threads"
set ylabel "successful iterations"
set logscale y 10
set output 'lab2b_3.png'
plot \
    "< grep -E 'list-id-none,[0-9]+,[0-9]+,4' lab2b_list.csv" using ($2):($3) \
	with points lc rgb "red" title "unprotected", \
    "< grep -E 'list-id-m,[0-9]+,[0-9]+,4' lab2b_list.csv" using ($2):($3) \
	with points lc rgb "green" title "Posix Mutex", \
    "< grep -E 'list-id-s,[0-9]+,[0-9]+,4' lab2b_list.csv" using ($2):($3) \
	with points lc rgb "blue" title "Spinlock"

#
# "no valid points" is possible if even a single iteration can't run
#

set title "List-4: Aggregated throughput vs. threads (mutex)"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [0.75:]
set ylabel "Throughput"
set logscale y
set output 'lab2b_4.png'
set key left top
plot \
     "< grep -E 'list-none-m,[0-9][0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'Lists: 1' with linespoints lc rgb 'blue', \
     "< grep -E 'list-none-m,[0-9][0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'Lists: 4' with linespoints lc rgb 'green', \
     "< grep -E 'list-none-m,[0-9][0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'Lists: 8' with linespoints lc rgb 'red', \
     "< grep -E 'list-none-m,[0-9][0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'Lists: 16' with linespoints lc rgb 'orange'


set title "List-5: Aggregated throughput vs. threads (spinlock)"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [0.75:]
set ylabel "Throughput"
set logscale y
set output 'lab2b_5.png'
set key left top
plot \
     "< grep -E 'list-none-s,[0-9][0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'Lists: 1' with linespoints lc rgb 'blue', \
     "< grep -E 'list-none-s,[0-9][0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'Lists: 4' with linespoints lc rgb 'green', \
     "< grep -E 'list-none-s,[0-9][0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'Lists: 8' with linespoints lc rgb 'red', \
     "< grep -E 'list-none-s,[0-9][0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'Lists: 16' with linespoints lc rgb 'orange'