#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <chrono>
#include <windows.h>

using namespace std;

// ????????? ??? ???????? ?????? ? ?????
struct ThreadData {
    const vector<vector<int>>* matrixA;
    const vector<vector<int>>* matrixB;
    vector<vector<int>>* resultMatrix;
    int blockRow;
    int blockCol;
    int blockK;
    int blockSize;
    HANDLE mutex;
};

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

    void displayMatrix(const vector<vector<int>>& matrix) {
        for (const auto& row : matrix) {
            for (int value : row) {
                cout << setw(5) << value << ' ';
            }
            cout << '\n';
        }
    }

    vector<vector<int>> computeBlockMultiplication(const vector<vector<int>>& matrixA,
        const vector<vector<int>>& matrixB,
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

    void mergeResultBlock(vector<vector<int>>& resultMatrix, const vector<vector<int>>& block,
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

    // ??????????? ??????? ??? ?????? Windows
    static DWORD WINAPI executeBlockMultiplicationWrapper(LPVOID arg) {
        ThreadData* data = static_cast<ThreadData*>(arg);

        // ????????? ????????? ?????
        vector<vector<int>> block = computeBlockMultiplicationStatic(*data->matrixA, *data->matrixB,
            data->blockRow, data->blockCol,
            data->blockK, data->blockSize);

        // ????????? ??????? ??? ?????? ??????????
        WaitForSingleObject(data->mutex, INFINITE);
        mergeResultBlockStatic(*data->resultMatrix, block, data->blockRow, data->blockCol, data->blockSize);
        ReleaseMutex(data->mutex);

        // ??????????? ??????, ?????????? ??? ????????? ??????
        delete data;
        return 0;
    }

    // ??????????? ?????? ??? ????????????? ? ??????? ??????
    static vector<vector<int>> computeBlockMultiplicationStatic(const vector<vector<int>>& matrixA,
        const vector<vector<int>>& matrixB,
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

    static void mergeResultBlockStatic(vector<vector<int>>& resultMatrix, const vector<vector<int>>& block,
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

public:
    vector<vector<int>> multiplyMatrices(const vector<vector<int>>& matrixA, const vector<vector<int>>& matrixB,
        int blockSize = 0) {
        if (blockSize == 0) {
            blockSize = matrixA.size();
        }

        vector<HANDLE> workerThreads;
        HANDLE mutexHandle;
        vector<vector<int>> resultMatrix(matrixA.size(), vector<int>(matrixA.size(), 0));
        int totalBlocks = (matrixA.size() + blockSize - 1) / blockSize;

        // ??????? ???????
        mutexHandle = CreateMutex(NULL, FALSE, NULL);
        if (mutexHandle == NULL) {
            cerr << "Error creating mutex" << endl;
            return resultMatrix;
        }

        // ??????? ?????? ??? ??????? ?????
        for (int blockRow = 0; blockRow < totalBlocks; ++blockRow) {
            for (int blockCol = 0; blockCol < totalBlocks; ++blockCol) {
                for (int blockK = 0; blockK < totalBlocks; ++blockK) {
                    // ??????? ????????? ?????? ??? ??????
                    ThreadData* data = new ThreadData{
                        &matrixA, &matrixB, &resultMatrix,
                        blockRow, blockCol, blockK, blockSize,
                        mutexHandle
                    };

                    // ??????? ?????
                    HANDLE threadHandle = CreateThread(
                        NULL,                   // default security attributes
                        0,                      // default stack size
                        executeBlockMultiplicationWrapper, // thread function
                        data,                   // argument to thread function
                        0,                      // default creation flags
                        NULL                    // receive thread identifier
                    );

                    if (threadHandle != NULL) {
                        workerThreads.push_back(threadHandle);
                    }
                    else {
                        cerr << "Error creating thread" << endl;
                        delete data; // ??????????? ?????? ? ?????? ??????
                    }
                }
            }
        }

        // ??????? ?????????? ???? ???????
        WaitForMultipleObjects(workerThreads.size(), workerThreads.data(), TRUE, INFINITE);

        // ????????? handles ???????
        for (HANDLE thread : workerThreads) {
            CloseHandle(thread);
        }

        // ????????? handle ????????
        CloseHandle(mutexHandle);

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

        // Test with different block sizes
        vector<int> blockSizes = { 3, 2, 1 };

        for (int blockSize : blockSizes) {
            cout << "\n\n";
            cout << "\tBlock amount " << blockSize << " (~" << blockSize * blockSize << " threads)\n";
            cout << "A * B:\n";

            auto startTime = chrono::high_resolution_clock::now();
            resultMatrix = multiplyMatrices(matrixA, matrixB, blockSize);
            auto endTime = chrono::high_resolution_clock::now();
            auto duration = chrono::duration<double>(endTime - startTime);

            displayMatrix(resultMatrix);
            cout << fixed << setprecision(6);
            cout << "Time taken: " << duration.count() << " seconds\n";
        }
    }
};

int main(int argc, char* argv[]) {
    MatrixMultiplier multiplier;
    multiplier.run();
    return 0;
}