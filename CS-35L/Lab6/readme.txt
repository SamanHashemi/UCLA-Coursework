Name: Saman Hashemipour
ID: 904903562



This section of the assignment (the homework) mainly focused on multithreading in a simpler sense. In order to make the assignment work I had to read, re-read, re-re-read the code to understand what was happening before I was to make any changes. I noticed that the multithreading had to happen in the nested loops of the function inside of main.

I then faced the problem of how to make sure that these ran the appropriate number of times and split the tasks accordingly. At first I thought about passing a parameter to a separate function and having that function take care of returning the value that was needed, but I realized that this would result in some rather messy code. My next decision was to then just put the section of the main that I needed into a separate file and deal with everything in there. I noticed rather quickly this was a much cleaner, and much simpler, more direct approach. 

This "solution" however, resulted in some rather unexpected errors. Since the nthreads (and other variables) was defined in the main I had to define it globally so that I could use it in the function I was going to create and the main function as well. The same went for the other variables that I had to create globally.

Lastly I had to deal with printing the color values of each location of the picture. This was easy to solve. I simply made an array that contained all the values of the picture and was able to print them easily.

My remaining errors were simple compilation errors (missing semicolon, forgotten return statements, misused variables, etc..)

My time comparisons were as follows:


time ./srt 1-test.ppm >1-test.ppm.tmp

real	0m47.688s
user	0m47.681s
sys	0m0.001s
mv 1-test.ppm.tmp 1-test.ppm
time ./srt 2-test.ppm >2-test.ppm.tmp

real	0m24.308s
user	0m48.579s
sys	0m0.001s
mv 2-test.ppm.tmp 2-test.ppm
time ./srt 4-test.ppm >4-test.ppm.tmp

real	0m12.277s
user	0m48.376s
sys	0m0.004s
mv 4-test.ppm.tmp 4-test.ppm
time ./srt 8-test.ppm >8-test.ppm.tmp

real	0m6.467s
user	0m50.172s
sys	0m0.002s


It is clear to see that the multithreading not only works, but it works to the point that we can receive results that are correlated to the number of threads that we decide to create. This is because the image is split into smaller images where each smaller image is easier to deal with and can split the time in half as seen above.