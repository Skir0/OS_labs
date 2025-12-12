#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include <tlhelp32.h>
#include <psapi.h>

using namespace std;

const int MAX_NAME_SIZE = 260;

struct ProcessData {
    DWORD processId;
    string processName;
    bool terminated;
};

string ExtractProcessName(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!hProcess) return "unknown";

    char exePath[MAX_NAME_SIZE];
    DWORD pathSize = GetModuleFileNameExA(hProcess, NULL, exePath, MAX_NAME_SIZE);
    CloseHandle(hProcess);

    if (pathSize == 0) return "unknown";

    string fullPath(exePath);
    size_t lastSlash = fullPath.find_last_of("\\");
    return (lastSlash != string::npos) ? fullPath.substr(lastSlash + 1) : fullPath;
}

ProcessData LaunchApplication(const string& commandLine) {
    cout << "\n[User] Launching application: " << commandLine << endl;

    STARTUPINFOA startupInfo;
    PROCESS_INFORMATION processInfo;

    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    ZeroMemory(&processInfo, sizeof(processInfo));

    char cmdCopy[1024];
    strncpy_s(cmdCopy, commandLine.c_str(), sizeof(cmdCopy) - 1);

    if (!CreateProcessA(NULL, cmdCopy, NULL, NULL, FALSE,
                       CREATE_NEW_CONSOLE, NULL, NULL, &startupInfo, &processInfo)) {
        DWORD error = GetLastError();
        cout << "[User] Failed to create process (Error: " << error << ")" << endl;
        return {0, "", false};
    }

    Sleep(500);

    string actualName = ExtractProcessName(processInfo.dwProcessId);
    cout << "[User] Launched successfully. PID: " << processInfo.dwProcessId
         << ", Name: " << actualName << endl;

    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);

    return {processInfo.dwProcessId, commandLine, false};
}

bool CheckProcessActive(DWORD processId) {
    if (processId == 0) return false;

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
    if (!hProcess) return false;

    DWORD exitCode;
    bool isActive = false;

    if (GetExitCodeProcess(hProcess, &exitCode)) {
        isActive = (exitCode == STILL_ACTIVE);
    }

    CloseHandle(hProcess);
    return isActive;
}

void ExecuteKiller(const string& arguments) {
    string fullCommand = "Killer.exe " + arguments;
    cout << "\n--- Executing Killer: " << fullCommand << " ---" << endl;

    STARTUPINFOA startupInfo;
    PROCESS_INFORMATION processInfo;

    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    ZeroMemory(&processInfo, sizeof(processInfo));

    char cmdLine[1024];
    strncpy_s(cmdLine, fullCommand.c_str(), sizeof(cmdLine) - 1);

    if (CreateProcessA(NULL, cmdLine, NULL, NULL, FALSE,
                      CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &processInfo)) {
        WaitForSingleObject(processInfo.hProcess, INFINITE);

        DWORD exitCode;
        GetExitCodeProcess(processInfo.hProcess, &exitCode);
        cout << "[User] Killer completed with exit code: " << exitCode << endl;

        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
    } else {
        DWORD error = GetLastError();
        cout << "[User] Failed to execute Killer (Error: " << error << ")" << endl;
    }
}

void VerifyProcessStatus(ProcessData& process) {
    if (process.processId == 0 || process.terminated) return;

    cout << "Checking PID " << process.processId << " (" << process.processName << "): ";

    if (CheckProcessActive(process.processId)) {
        string currentName = ExtractProcessName(process.processId);
        cout << "ACTIVE as '" << currentName << "'" << endl;
    } else {
        cout << "TERMINATED" << endl;
        process.terminated = true;
    }
}

void CleanupProcesses(vector<ProcessData>& processes) {
    cout << "\n=== Cleaning up remaining processes ===" << endl;
    int remainingCount = 0;

    for (auto& proc : processes) {
        if (!proc.terminated && CheckProcessActive(proc.processId)) {
            cout << "Terminating process " << proc.processId << endl;

            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, proc.processId);
            if (hProcess) {
                TerminateProcess(hProcess, 0);
                WaitForSingleObject(hProcess, 1000);
                CloseHandle(hProcess);

                if (!CheckProcessActive(proc.processId)) {
                    proc.terminated = true;
                }
                remainingCount++;
            }
        }
    }

    if (remainingCount == 0) {
        cout << "No remaining processes found" << endl;
    }
}

int main() {
    cout << "================ PROCESS TESTER APPLICATION ================" << endl;
    cout << "[User] Current PID: " << GetCurrentProcessId() << endl;

    SetEnvironmentVariableA("PROC_TO_KILL", "Notepad.exe,calc.exe");
    cout << "[User] Set environment variable: PROC_TO_KILL = 'Notepad.exe,calc.exe'" << endl;

    vector<ProcessData> testProcesses;

    cout << "\n=== Test Scenario 1: Terminate by Process ID ===" << endl;
    ProcessData proc1 = LaunchApplication("Notepad.exe");
    testProcesses.push_back(proc1);

    if (proc1.processId != 0) {
        Sleep(1500);
        ExecuteKiller("--id " + to_string(proc1.processId));
        VerifyProcessStatus(testProcesses[0]);
    }

    cout << "\n=== Test Scenario 2: Terminate by Process Name ===" << endl;
    ProcessData proc2 = LaunchApplication("Notepad.exe");
    ProcessData proc3 = LaunchApplication("Notepad.exe");
    testProcesses.push_back(proc2);
    testProcesses.push_back(proc3);

    Sleep(1500);
    ExecuteKiller("--name Notepad.exe");

    for (size_t i = 1; i < testProcesses.size(); i++) {
        VerifyProcessStatus(testProcesses[i]);
    }

    cout << "\n=== Test Scenario 3: Environment Variable ===" << endl;
    ProcessData proc4 = LaunchApplication("Notepad.exe");
    ProcessData proc5 = LaunchApplication("calc.exe");
    testProcesses.push_back(proc4);
    testProcesses.push_back(proc5);

    Sleep(1500);
    ExecuteKiller("--env");

    for (size_t i = 3; i < testProcesses.size(); i++) {
        VerifyProcessStatus(testProcesses[i]);
    }

    cout << "\n=== Test Scenario 4: List All Processes ===" << endl;
    ExecuteKiller("--list");

    CleanupProcesses(testProcesses);

    SetEnvironmentVariableA("PROC_TO_KILL", NULL);
    cout << "\n[User] Cleared environment variable" << endl;

    cout << "\n================ TESTING COMPLETE ================" << endl;

    cout << "\nPress Enter to exit...";
    cin.get();

    return 0;
}