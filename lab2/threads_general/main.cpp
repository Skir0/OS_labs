#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <random>
#include <iomanip>
#include <chrono>

using namespace std;

class MatrixMultiplier {
private:
    vector<vector<int>> initializeMatrix(int rows, int cols, int minVal, int maxVal) {
        random_device randomDevice;
        mt19937 generator(randomDevice());
        uniform_int_distribution<> distribution(minVal, maxVal);
        vector<vector<int>> matrix(rows, vector<int>(cols));

        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                matrix[row][col] = distribution(generator);
            }
        }

        return matrix;
    }

    void displayMatrix(const vector<vector<int>> &matrix) {
        for (const auto &row : matrix) {
            for (int value : row) {
                cout << setw(5) << value << ' ';
            }
            cout << '\n';
        }
    }

    vector<vector<int>> computeBlockMultiplication(const vector<vector<int>> &matrixA,
                                                  const vector<vector<int>> &matrixB,
                                                  int blockRow, int blockCol, int blockK,
                                                  int blockSize) {
        int startRow = blockRow * blockSize;
        int startCol = blockCol * blockSize;
        int startK = blockK * blockSize;

        int rowLength = min(blockSize, (int)matrixA.size() - startRow);
        int colLength = min(blockSize, (int)matrixA.size() - startCol);
        int kLength = min(blockSize, (int)matrixA.size() - startK);

        vector<vector<int>> resultBlock(rowLength, vector<int>(colLength, 0));

        for (int i = 0; i < rowLength; i++) {
            for (int j = 0; j < colLength; j++) {
                int sum = 0;
                for (int k = 0; k < kLength; k++) {
                    sum += matrixA[startRow + i][startK + k] * matrixB[startK + k][startCol + j];
                }
                resultBlock[i][j] = sum;
            }
        }
        return resultBlock;
    }

    void mergeResultBlock(vector<vector<int>> &resultMatrix, const vector<vector<int>> &block,
                         int blockRow, int blockCol, int blockSize) {
        for (int i = 0; i < block.size(); i++) {
            for (int j = 0; j < block[0].size(); j++) {
                if (blockRow * blockSize + i < resultMatrix.size() &&
                    blockCol * blockSize + j < resultMatrix.size()) {
                    resultMatrix[blockRow * blockSize + i][blockCol * blockSize + j] += block[i][j];
                }
            }
        }
    }

    void executeBlockMultiplication(const vector<vector<int>> &matrixA, const vector<vector<int>> &matrixB,
                                   vector<vector<int>> &resultMatrix, int blockRow, int blockCol,
                                   int blockK, int blockSize, mutex &mutexLock) {
        vector<vector<int>> block = computeBlockMultiplication(matrixA, matrixB, blockRow, blockCol, blockK, blockSize);
        lock_guard<mutex> lock(mutexLock);
        mergeResultBlock(resultMatrix, block, blockRow, blockCol, blockSize);
    }

public:
    vector<vector<int>> multiplyMatrices(const vector<vector<int>> &matrixA, const vector<vector<int>> &matrixB,
                                        int numBlocks = 1) {
        if (numBlocks <= 0) {
            numBlocks = 1;
        }
        
        vector<thread> workerThreads;
        mutex mutexLock;
        vector<vector<int>> resultMatrix(matrixA.size(), vector<int>(matrixA.size(), 0));
        
        int blockSize = (matrixA.size() + numBlocks - 1) / numBlocks;

        for (int blockRow = 0; blockRow < numBlocks; ++blockRow) {
            for (int blockCol = 0; blockCol < numBlocks; ++blockCol) {
                for (int blockK = 0; blockK < numBlocks; ++blockK) {
                    workerThreads.emplace_back(&MatrixMultiplier::executeBlockMultiplication, this,
                                              cref(matrixA), cref(matrixB), ref(resultMatrix),
                                              blockRow, blockCol, blockK, blockSize, ref(mutexLock));
                }
            }
        }

        for (auto &thread : workerThreads) {
            thread.join();
        }

        return resultMatrix;
    }

    void run() {
        int matrixSize = 0;
        const int MIN_RANDOM = 1;
        const int MAX_RANDOM = 9;

        cout << "Matrix is formed with random integer numbers [" << MIN_RANDOM << ", " << MAX_RANDOM << "]\n";
        cout << "Enter matrix size: ";
        cin >> matrixSize;

        vector<vector<int>> matrixA = initializeMatrix(matrixSize, matrixSize, MIN_RANDOM, MAX_RANDOM);
        vector<vector<int>> matrixB = initializeMatrix(matrixSize, matrixSize, MIN_RANDOM, MAX_RANDOM);
        vector<vector<int>> resultMatrix(matrixSize, vector<int>(matrixSize, 0));

        cout << "Matrix A:\n";
        displayMatrix(matrixA);
        cout << "Matrix B:\n";
        displayMatrix(matrixB);

        vector<int> blockAmounts = {3, 2, 1};

        for (int numBlocks : blockAmounts) {
            cout << "\n\n";
            cout << "\tBlock amount: " << numBlocks << "\n";
            cout << "A * B:\n";

            auto startTime = chrono::high_resolution_clock::now();
            resultMatrix = multiplyMatrices(matrixA, matrixB, numBlocks);
            auto endTime = chrono::high_resolution_clock::now();
            auto duration = chrono::duration<double, milli>(endTime - startTime);

            displayMatrix(resultMatrix);
            cout << fixed << setprecision(6);
            cout << "Time taken: " << duration.count() << "ms\n";
        }
    }
};

int main(int argc, char *argv[]) {
    MatrixMultiplier multiplier;
    multiplier.run();
    return 0;
}