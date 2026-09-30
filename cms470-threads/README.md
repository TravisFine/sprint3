# Multi-threaded sorting

## Name
Travis Fine

## Collaboration statement
In terms of human collaboration, I was the only one who worked on this. I did use chat gpt's new browser thing to help me go through zybooks and work with me to understand the material better. It gave some suggestions on how to code parts of the program which I did take into account, but the work here is all my own interpretation and code.

## Running the program
run:

~~~sh
make
./main 1000
~~~

Replace 1000 with the size you want to test. N needs to be a positive whole number. The program normally prints just the two sorting times. If the input is invalid, it prints an error to stderr.

## How it works
The program makes an array of random doubles between 1.0 and 1000.0. Both sorting methods get copies of the same numbers, and those copies are made before the timer starts. The selection sort uses regular loops.

For the one-thread version, one thread sorts the whole array. This follows the pseudocode, even though the earlier description mentions sorting in main. For the two-thread version, each thread sorts one half. After both finish, another thread merges the halves. That timer includes starting the threads, waiting for them, and merging. If N is odd, the second half gets the extra number.

## Does it work?
Everything seems to be working based on the tests so far. After sorting, the program checks that the one-thread result is in order and that both methods produced the same numbers in the same order. This check happens after timing and does not print any debugging output. Each sorting thread has its own array, and the arrays and structs stay around until the threads finish.

The tests covered N = 1, 2, 3, 9, 10, and 101, plus the four sizes below. A separate test checked duplicates, decimals, 1.0 and 1000.0, an empty half, and numbers that were already sorted against known answers. Bad inputs were checked too, including missing input, zero, negative numbers, letters, decimals, numbers that were too large, and extra arguments.

## Sorting times
These are the actual times from one run at each size in the Codespace, using the provided Makefile. They are not averages. The program uses the current time for the random seed, and srand(42) is commented out.

| N | One sorting thread (ms) | Two sorting threads + merge (ms) |
| --- | ---: | ---: |
| 1000 | 1.223 | 0.687 |
| 5000 | 21.943 | 10.447 |
| 10000 | 89.176 | 40.528 |
| 20000 | 354.324 | 154.845 |

## What the results show
The two-thread version was faster for all four sizes. At 20,000 numbers, it took about 44% of the one-thread time, making it about 2.29 times faster. For the larger arrays, doubling the size made the one-thread time roughly four times longer. That makes sense for selection sort, since its work grows at O(n^2).

In an ideal case, sorting two halves at the same time could take about a quarter of the time, plus the O(n) merge. These results were closer to half. Starting threads, waiting for them, and sharing CPU time in the Codespace could help explain that, but the timings do not tell us the exact reason. Splitting the array also means fewer comparisons overall, so the improvement is not just from running on different cores.

For very small arrays, the two-thread version was actually slower. There is barely anything to sort, so the extra thread work can take more time than it saves. Running each size several times would give a better idea of the average performance.

## Questions
- About how long I spent: about 4-ish hours but I had a movie on too, so part of that was probably wasted time.
- What was challenging: understanding the material itself
- Could I start a simple multithreaded program now? I think so. This project has really helped me understand how multithreading actually works in the code itself.

