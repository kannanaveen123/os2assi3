#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <stdexcept>

std::atomic<int> counter(0);
std::atomic<int> lock(0);

std::vector<std::vector<int>> readMatrix(std::ifstream &inputFile, int n)
{
    std::vector<std::vector<int>> matrix(n, std::vector<int>(n, 0));

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if (!(inputFile >> matrix[i][j]))
            {
                throw std::runtime_error("Error reading matrix element at position (" + std::to_string(i) + ", " + std::to_string(j) + ").");
            }
        }
    }

    return matrix;
}

void TAS()
{
    while (std::atomic_exchange_explicit(&lock, 1, std::memory_order_acquire))
    {
    }
}

void CAS()
{
    while (true)
    {
        int expected = 0;
        if (lock.compare_exchange_strong(expected, 1, std::memory_order_acquire))
        {
            break;
        }
    }
}

void BoundedCAS()
{
    int expected = 0;
    while (true)
    {
        if (lock.compare_exchange_weak(expected, 1, std::memory_order_acquire))
        {
            break;
        }
        expected = 0; // Reset expected value for the next iteration
    }
}

void AtomicIncrement()
{
    
}

void matrixMultiplyChunk(const std::vector<std::vector<int>> &matrixA, std::vector<std::vector<int>> &resultMatrix, int n, int startRow, int endRow)
{
    for (int i = startRow; i < endRow; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            for (int k = 0; k < n; ++k)
            {
                resultMatrix[i][j] += matrixA[i][k] * matrixA[k][j];
            }
        }
    }
}

void matrixSquareChunk(const std::vector<std::vector<int>> &matrixA, std::vector<std::vector<int>> &resultMatrix, int n, int rowInc, void (*mutexFunc)(), double &timeTaken)
{
    auto start = std::chrono::high_resolution_clock::now();

    int startRow, endRow;
    while (true)
    {
        mutexFunc();
        startRow = counter;
        counter += rowInc;
        endRow = counter;
        lock.exchange(0);

        if (startRow >= n)
            break;

        if (endRow > n)
            endRow = n;

        matrixMultiplyChunk(matrixA, ref(resultMatrix), n, startRow, endRow);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    timeTaken = duration.count();
}

int main()
{
    int n, k, rowInc;
    std::ifstream inputFile("inp.txt");
    if (!inputFile.is_open())
    {
        std::cerr << "Error opening input file.\n";
        return 1;
    }

    inputFile >> n >> k >> rowInc;

    std::vector<std::vector<int>> matrixA;
    try
    {
        matrixA = readMatrix(inputFile, n);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        inputFile.close();
        return 1;
    }

    inputFile.close();

    std::vector<std::vector<int>> resultMatrixTAS(n, std::vector<int>(n, 0));
    std::vector<std::vector<int>> resultMatrixCAS(n, std::vector<int>(n, 0));
    std::vector<std::vector<int>> resultMatrixBoundedCAS(n, std::vector<int>(n, 0));
    std::vector<std::vector<int>> resultMatrixAtomicIncrement(n, std::vector<int>(n, 0));

    double timeTAS, timeCAS, timeBoundedCAS, timeAtomicIncrement;

    std::vector<std::thread> threads;

    // Launch threads using different mutual exclusion methods
    for (int i = 0; i < k; i++)
        threads.emplace_back(matrixSquareChunk, std::ref(matrixA), std::ref(resultMatrixTAS), n, rowInc, &TAS, std::ref(timeTAS));
    for (auto &thread : threads)
    {
        thread.join();
    }
    threads.clear();
    counter = 0;
    for (int i = 0; i < k; i++)
        threads.emplace_back(matrixSquareChunk, std::ref(matrixA), std::ref(resultMatrixCAS), n, rowInc, &CAS, std::ref(timeCAS));
    for (auto &thread : threads)
    {
        thread.join();
    }
    threads.clear();
    counter = 0;
    for (int i = 0; i < k; i++)
        threads.emplace_back(matrixSquareChunk, std::ref(matrixA), std::ref(resultMatrixBoundedCAS), n, rowInc, &BoundedCAS, std::ref(timeBoundedCAS));
    for (auto &thread : threads)
    {
        thread.join();
    }
    threads.clear();
    counter = 0;
    for (int i = 0; i < k; i++)
        threads.emplace_back(matrixSquareChunk, std::ref(matrixA), std::ref(resultMatrixAtomicIncrement), n, rowInc, &AtomicIncrement, std::ref(timeAtomicIncrement));
    for (auto &thread : threads)
    {
        thread.join();
    }
    threads.clear();

    // Wait for all threads to finish

    // Output the result and time taken for each mutual exclusion method
    std::ofstream outFile("out.txt");
    if (!outFile.is_open())
    {
        std::cerr << "Error opening output file.\n";
        return 1;
    }

    outFile << "Result Matrix (Square of matrix A) - TAS:\n";
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            outFile << resultMatrixTAS[i][j] << " ";
        }
        outFile << "\n";
    }
    outFile << "\nTime taken for TAS: " << timeTAS << " seconds\n";
    std::cout<<timeTAS<<std::endl;

    outFile << "\nResult Matrix (Square of matrix A) - CAS:\n";
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            outFile << resultMatrixCAS[i][j] << " ";
        }
        outFile << "\n";
    }
    outFile << "\nTime taken for CAS: " << timeCAS << " seconds\n";
    std::cout<<timeCAS<<std::endl;


    outFile << "\nResult Matrix (Square of matrix A) - Bounded CAS:\n";
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            outFile << resultMatrixBoundedCAS[i][j] << " ";
        }
        outFile << "\n";
    }
    outFile << "\nTime taken for Bounded CAS: " << timeBoundedCAS << " seconds\n";
    std::cout<<timeBoundedCAS<<std::endl;

    outFile << "\nResult Matrix (Square of matrix A) - Atomic Increment:\n";
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            outFile << resultMatrixAtomicIncrement[i][j] << " ";
        }
        outFile << "\n";
    }
    outFile << "\nTime taken for Atomic Increment: " << timeAtomicIncrement << " seconds\n";
    std::cout<<timeAtomicIncrement<<std::endl;

    outFile.close();

    return 0;
}