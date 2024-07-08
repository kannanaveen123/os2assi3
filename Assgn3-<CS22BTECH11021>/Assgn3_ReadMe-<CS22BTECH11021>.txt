                                                                     PROGRAMMING ASSIGNMENT - 3
                                                                     ESLAVATH NAVEEN NAIK
                                                                     CS22BTECH11021
                                                                     
                                                                     
                                                                     
 This C++ program performs matrix multiplication using multiple threads, employing various mutual exclusion methods to ensure data integrity and concurrency control. The program allows the user to specify the size of the matrices, the number of threads, and the increment of rows processed by each thread. Four different mutual exclusion methods are implemented and compared: Test-and-Set (TAS), Compare-and-Swap (CAS), Bounded CAS, and Atomic Increment.
 
 Make sure you have a C++ compiler installed on your system. Navigate to the directory where the source code file (name.cpp) is locate
 and write in terminal ./a.out
 Prepare Input File: Create an input file named inp.txt
 
 here below how my code runs

example(INPUT):


3 2 1
1 2 3
4 5 6
7 8 9




example(OUTPUT):

Result Matrix (Square of matrix A) - TAS:
30 36 42
66 81 96
102 126 150

Time taken for TAS: 0.123 seconds

Result Matrix (Square of matrix A) - CAS:
30 36 42
66 81 96
102 126 150

Time taken for CAS: 0.234 seconds

Result Matrix (Square of matrix A) - Bounded CAS:
30 36 42
66 81 96
102 126 150

Time taken for Bounded CAS: 0.345 seconds

Result Matrix (Square of matrix A) - Atomic Increment:
30 36 42
66 81 96
102 126 150

Time taken for Atomic Increment: 0.456 seconds

