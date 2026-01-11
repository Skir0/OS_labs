#include <windows.h>
#include <iostream>
#include <string>
#include <vector>

bool SetWorkingDirectoryToExePath()
{
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::string path(exePath);
    size_t pos = path.find_last_of("\\/");
    if (pos == std::string::npos)
    {
        return false;
    }
    std::string exeDir = path.substr(0, pos);
    return SetCurrentDirectoryA(exeDir.c_str());
}

HANDLE CreateChildProcess(const char* processName, HANDLE hChildStdIn, HANDLE hChildStdOut)
{
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFOA siStartInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFOA));
    siStartInfo.cb = sizeof(STARTUPINFOA);
    siStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    siStartInfo.hStdOutput = hChildStdOut;
    siStartInfo.hStdInput = hChildStdIn;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    std::string cmdLine = processName;

    if (!CreateProcessA(
        NULL,
        const_cast<char*>(cmdLine.c_str()),
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &siStartInfo,
        &piProcInfo
    ))
    {
        DWORD error = GetLastError();
        std::cerr << "CreateProcess failed for '" << processName << "': " << error << std::endl;

        if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
            std::cerr << "  File not found. Make sure " << processName << " is in the same directory." << std::endl;
        }
        return NULL;
    }

    CloseHandle(piProcInfo.hThread);
    return piProcInfo.hProcess;
}

int main()
{
    std::cout << "=== Pipe Chain Demonstration ===" << std::endl;
    std::cout << "Chain: Input -> M(x*7) -> A(x+11) -> P(x^3) -> S(sum) -> Output" << std::endl;

    if (!SetWorkingDirectoryToExePath())
    {
        std::cerr << "Failed to set working directory!" << std::endl;
        return 1;
    }

    std::vector<std::string> processFiles = {"process_m.exe", "process_a.exe", "process_p.exe", "process_s.exe"};
    for (const auto& file : processFiles) {
        if (GetFileAttributesA(file.c_str()) == INVALID_FILE_ATTRIBUTES) {
            std::cerr << "File not found: " << file << ". Please compile all .cpp files first." << std::endl;
        }
    }

    HANDLE hInputPipeRead, hInputPipeWrite;
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&hInputPipeRead, &hInputPipeWrite, &saAttr, 0))
    {
        std::cerr << "Input pipe creation failed: " << GetLastError() << std::endl;
        return 1;
    }

    HANDLE hPipeMtoA_Read, hPipeMtoA_Write;
    HANDLE hPipeAtoP_Read, hPipeAtoP_Write;
    HANDLE hPipePtoS_Read, hPipePtoS_Write;

    if (!CreatePipe(&hPipeMtoA_Read, &hPipeMtoA_Write, &saAttr, 0))
    {
        std::cerr << "Pipe M->A creation failed: " << GetLastError() << std::endl;
        return 1;
    }

    if (!CreatePipe(&hPipeAtoP_Read, &hPipeAtoP_Write, &saAttr, 0))
    {
        std::cerr << "Pipe A->P creation failed: " << GetLastError() << std::endl;
        return 1;
    }

    if (!CreatePipe(&hPipePtoS_Read, &hPipePtoS_Write, &saAttr, 0))
    {
        std::cerr << "Pipe P->S creation failed: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "\nCreating processes..." << std::endl;

    HANDLE hProcessM = CreateChildProcess("process_m.exe", hInputPipeRead, hPipeMtoA_Write);
    if (!hProcessM) {
        std::cerr << "Failed to create process_m.exe" << std::endl;
        return 1;
    }

    HANDLE hProcessA = CreateChildProcess("process_a.exe", hPipeMtoA_Read, hPipeAtoP_Write);
    if (!hProcessA) {
        std::cerr << "Failed to create process_a.exe" << std::endl;
        return 1;
    }

    HANDLE hProcessP = CreateChildProcess("process_p.exe", hPipeAtoP_Read, hPipePtoS_Write);
    if (!hProcessP) {
        std::cerr << "Failed to create process_p.exe" << std::endl;
        return 1;
    }

    HANDLE hProcessS = CreateChildProcess("process_s.exe", hPipePtoS_Read, GetStdHandle(STD_OUTPUT_HANDLE));
    if (!hProcessS) {
        std::cerr << "Failed to create process_s.exe" << std::endl;
        return 1;
    }

    CloseHandle(hInputPipeRead);
    CloseHandle(hPipeMtoA_Write);
    CloseHandle(hPipeMtoA_Read);
    CloseHandle(hPipeAtoP_Write);
    CloseHandle(hPipeAtoP_Read);
    CloseHandle(hPipePtoS_Write);
    CloseHandle(hPipePtoS_Read);

    DWORD bytesWritten;
    const char* data = "1 2 3 4 5\n";
    std::cout << "\nSending data to pipe: " << data;

    if (!WriteFile(hInputPipeWrite, data, (DWORD)strlen(data), &bytesWritten, NULL))
    {
        std::cerr << "Failed to write data to pipe: " << GetLastError() << std::endl;
    }

    CloseHandle(hInputPipeWrite);

    std::cout << "\nWaiting for processes to complete..." << std::endl;

    HANDLE processes[4] = {hProcessM, hProcessA, hProcessP, hProcessS};
    WaitForMultipleObjects(4, processes, TRUE, INFINITE);

    DWORD exitCodes[4];
    GetExitCodeProcess(hProcessM, &exitCodes[0]);
    GetExitCodeProcess(hProcessA, &exitCodes[1]);
    GetExitCodeProcess(hProcessP, &exitCodes[2]);
    GetExitCodeProcess(hProcessS, &exitCodes[3]);

    CloseHandle(hProcessM);
    CloseHandle(hProcessA);
    CloseHandle(hProcessP);
    CloseHandle(hProcessS);

    bool allSuccessful = true;
    for (int i = 0; i < 4; i++) {
        if (exitCodes[i] != 0) {
            std::cerr << "Process " << i << " exited with code " << exitCodes[i] << std::endl;
            allSuccessful = false;
        }
    }

    if (allSuccessful) {
        std::cout << "\nChain execution completed successfully!" << std::endl;
    } else {
        std::cerr << "\nChain execution completed with errors!" << std::endl;
    }

    return 0;
}