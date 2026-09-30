# Multi-threaded sorting

In this assignment you will learn and practice developing a multithreaded application in C with Pthreads. 

For this project, the purpose of the program (sorting an array of numbers) will not be new.  In fact, you probably have implemented the selection sort algorithm in a previous class.  However, in this project you will implement the algorithm in C **and** make it multi-threaded. 

Selection sort isn't usually a divide-and-conquer algorithm (unlike merge sort or quick sort, for example).  Just to refresh your memory, here's an overview of that algorithm:
1. **Find the Minimum Element:** Start with the first element of the array. Scan the entire array to find the smallest element.
2. **Swap:** Swap the smallest element found with the first element of the array.
3. **Repeat:** Move to the next element and repeat the process for the remaining unsorted portion of the array. Continue this process until the entire array is sorted. 

However, this project will demonstrate how to divide work using threads.  This provides optimization in today's multi-core environment as each thread can run on a separate CPU.

### Description

Write a multithreaded sorting program that works as follows: An array of double values is divided into two smaller lists of equal size. Two separate threads (which we will term sorting threads) sort each sublist using selection sort. The two sublists are then merged by a third thread --- a merging thread -- which merges the two sorted sublists into a single sorted list.

Your program should take take an integer (say N)  from the command line. This number N represents the size of the array that needs to be sorted.  Accordingly, you should create an array of N double values and randomly select the values from the range of [1.0, 1000.0]. Then sort them using multhithreading as described above (there is some pseudocode below) and measure how long does it take to finish this sorting task. For the comparision purposes, you will also time a call to your sort function to sort the whole array and measure how long it takes if we do not use multuthreading (basically one (the main) thread is doing the sorting job).

Here is how your program should be executed and a sample output:
```
> main 1000
Sorting is done in 20.0ms when one thread is used
Sorting is done in 10.0ms when two threads are used
```
The numbers 10.0 and 20.0 here are just an example! Your actual numbers will be different and depend on the runs. (I have some more discussion at the end).

You need to figure out how to implement this program in C (Pthreads)! A main part of this program will be learning how to pass parameters to your threads, which can only have 1 parameter (a choice made in POSIX threads for simplicity and flexibility).  You will need to define and use structs carefully to help you consolidate multiple data (pointer to the array, the size of the array, perhaps a string to identify the array while debugging...) into one parameter. 

### Logistics
The Makefile supplied with the project already links against the `pthreads` library and so can be used as is.

Recall how to supply command line arguments to a given program:
```
prompt> ./executable arg1 arg2 arg3
```

This program uses random number generation.  Recall that you can make the program's behavior deterministic by specifying a seed to start the random number generation.  In C, use the `srand` function with a literal integer to ensure that your random number generation is the same for each run of your program, thus: `srand(42);`.  *Note: you should comment this line of code out before submitting your work, however!*

**DO NOT** use readily available sort functions/methods from any C libraries.  You need to implement a simple one by yourself!

### Part 1
In this part you will write your program in C with Pthreads.  I highly recommend you remember the fundamentals of incremental development, start small, and compile/run/test often with small arrays!  You should also read the pseudocode given at the bottom of this assignment and think about how to tackle this project.  Becoming an advanced developer means you often spend more time thinking than programming.

Here is a possible plan of action which breaks the work into small, manageable chunks and gets you started.  It should go without saying that you should test your program and verify its functionality after each of these steps using small arrays.
1. Write some code which processes the command line argument and prints error messages if it is not present.
2. Make an array of size N and populate it with random doubles between 1.0 and 1000.0.  You may want to write a function which takes a pointer to an array and an integer (size of array) and prints the array.  You'll use such a function frequently for debugging!
3. Write a selection sort function (and perhaps a helpful function `swap` to swap two elements of the array) which takes a pointer to an array and a size and sorts the array.  Test with small arrays with both an even and odd number of elements.
4. Create a struct to hold (at a minimum) a pointer to an array and the size of that array.  Change your sorting function to take a single struct parameter and sort.
5. Add code to copy/setup all necessary arrays before sorting any of them. (See pseudocode below.)
6. Add code to create a pthread which calls your sorting function and passes the struct parameter.  Don't forget to join the pthread after you create it!  You can now print out the times for a single thread to sort an array.
7. ... Continue with a function to merge two sorted arrays into a third array, create a struct to hold parameters for this function, and create/join a thread for that function...  (See pseudocode!)

### Part 2 - README.md
In the README.md file:
1. Be sure to include your collaboration statement.  There are not many language specifics given in this project, and I expect most students will utilize several (many?) outside sources.  **Be specific!** Don't just say, "I watched a video on YouTube."  Give me the exact URL. Remember that if you use GAI, you should give me a discussion of the prompts' purposes **AND** what you learned from it.
2. Do you think everything you did is correct? If not, give a brief description of what is working and what progress was made on the part that is not working.
3. Test your program with different values of N (1000, 5000, 10000,20000) and see how it performs. Make a table of your results and include it in your README.md.
4. Comment on your results using two threads vs. no threading.
5.  Comments (e.g., Approximate time spent on this assignment? Did you find this a challenging project? Do you feel like you could start a simple multi-threaded program now?):

### SUBMISSION:
Be sure to remove all debugging output! The only output should be the timing data. Zip the project directory, download, and upload to the Canvas assignment.

### GRADING:
This assignment is worth 100 points (Correct execution of the program is 70pts, programming style (comments, function utilization, naming, etc.) is 10pts, and the writeup is 20pts).

### Pseudocode
here is the high level idea!
```
struct sortingParams {...};
struct mergingParams {...};

selectionSort() {...}

main() {
	n will be given as command line argument
	print error messages as necessary
	exit with return 0; if needed

	struct timespec ts_begin, ts_end;
	double elapsed;  

	create array A (n double values) and randomly generate these values 

	also create arrays B and C with the same size of A
	create firstHalf and secondHalf with the half size of A

	//----------- ONE THREAD CASE --------------
	copy A into B
	clock_gettime(CLOCK_MONOTONIC, &ts_begin); 
	package params for sorting function
	create threadB sortThread to sort B 
	join threadB
	clock_gettime(CLOCK_MONOTONIC, &ts_end);  
	elapsed = ts_end.tv_sec - ts_begin.tv_sec; 
	elapsed += (ts_end.tv_nsec - ts_begin.tv_nsec) / 1000000000.0; 

	print sorting time:
	Sorting by ONE thread is done in elapsed*1000 ms   

	//------------------- TWO THREADS CASE ------------
	copy A into firstHalf  and secondHalf
	clock_gettime(CLOCK_MONOTONIC, &ts_begin);  
	package up params into structs
	create threadA1 to sort firstHalf  
	create threadA2 to sort secondHalf   
	join threadA1
	join threadA2

	package up params for merging
	create threadM to merge firsthalf and secondhalf into C
	/* make sure this just merges the sorted values from two arrays while keeping them sorted.  O(n)
Dont' copy these arrays and then call sort which will be like a single thread sorting the whole thing! */
	join threadM
	clock_gettime(CLOCK_MONOTONIC, &ts_end);  
	elapsed = ts_end.tv_sec - ts_begin.tv_sec; 
	elapsed += (ts_end.tv_nsec - ts_begin.tv_nsec) / 1000000000.0; 

	print timing information for 2 thread version:
	Sorting by TWO threads is done in elapsed*1000 ms   
}
``` 

### What should be the exection time ratio between two thread vs. single threads?

In theory, a single thread (using selection sort) takes $O(n^2)$, right? So if you divide the array into two parts, then sorting the first half takes $O((\frac{n}{2})^2)$.  Similarly, sorting the second half takes the same and merging takes $O(n)$.

If two parts are sorted by different CPU cores in parallel using kernel level threads, then (in theory) the execution time of two threads would be around $\frac{1}{4}$ that of a single thread, right? If only a single CPU is used with user level threads then the ratio could be around $\frac{1}{2}$.

But then there are a lot of other issues! For example, switching between threads (context switching cost) also adds some time as we learned last sprint. We also have to consider paging (more next sprint!).  The case of a single thread dealing with a large array may cause more page faults etc. If you first execute single thread and then two threads in the same program, the single thread case may be busy with getting all the pages and cause more page faults; but then  two threads may not result any page fault and use the existing data and cause fewer page faults. 

If you see wildly different execution times, try reversing your threading: time two threads first, then run single thread to see if there is any difference! 

Moreover, measuring execution time is not an easy job. Sometimes due to how clock is updated, you may get interesting execution times. That is why actually people run the same experiment several times and then take their averages, carefully calibrate their systems to reduce "interference" from other applications, etc.

In summary, it is possible to get wild execution times beyond the expected theortical ratios.  But if is too wild, make sure  your code is working correctly. Also verify that the sorted array is actually sorted at the end in both cases. Then report the execution times that you got and try to explain what you see and how/why they might be different than the theoretical expectations.


###### Assignment adapted from Dr. Turgay Korkmaz
