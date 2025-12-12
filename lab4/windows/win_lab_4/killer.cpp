#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

using namespace std;

const int MAX_NAME_SIZE = 260;

string GetProcessNameFromId(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) return "";

    char exePath[MAX_NAME_SIZE];
    DWORD pathSize = GetModuleFileNameExA(hProcess, NULL, exePath, MAX_NAME_SIZE);
    CloseHandle(hProcess);

    if (pathSize == 0) return "";

    string fullPath(exePath);
    size_t lastSlash = fullPath.find_last_of("\\");
    return (lastSlash != string::npos) ? fullPath.substr(lastSlash + 1) : fullPath;
}

void CloseProcessHandle(HANDLE hProcess, DWORD pid) {
    TerminateProcess(hProcess, 0);
    CloseHandle(hProcess);
}

void TerminateProcessById(DWORD processId) {
    cout << "\n[Killer] Terminating process ID: " << processId << endl;

    string procName = GetProcessNameFromId(processId);
    if (!procName.empty()) {
        cout << "[Killer] Process name: " << procName << endl;
    }

    if (processId == GetCurrentProcessId()) {
        cout << "[Killer] Cannot terminate self" << endl;
        return;
    }

    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    if (hProcess) {
        CloseProcessHandle(hProcess, processId);
        cout << "[Killer] Process " << processId << " terminated successfully" << endl;
    } else {
        DWORD error = GetLastError();
        cout << "[Killer] Failed to terminate " << processId << " (Error: " << error << ")" << endl;
    }
}

void TerminateProcessesByName(const string& targetName) {
    cout << "\n[Killer] Terminating processes with name: " << targetName << endl;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        cout << "[Killer] Failed to create process snapshot" << endl;
        return;
    }

    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    vector<DWORD> processIds;

    if (Process32First(hSnapshot, &processEntry)) {
        do {
            if (_stricmp(processEntry.szExeFile, targetName.c_str()) == 0) {
                if (processEntry.th32ProcessID != GetCurrentProcessId()) {
                    processIds.push_back(processEntry.th32ProcessID);
                }
            }
        } while (Process32Next(hSnapshot, &processEntry));
    }

    CloseHandle(hSnapshot);

    if (processIds.empty()) {
        cout << "[Killer] No processes found with name: " << targetName << endl;
        return;
    }

    cout << "[Killer] Found " << processIds.size() << " process(es)" << endl;

    for (DWORD pid : processIds) {
        cout << "  -> Terminating PID: " << pid << endl;
        TerminateProcessById(pid);
    }
}

void ListAllProcesses() {
    cout << "\n[Killer] Current Process List:" << endl;
    cout << "===============================" << endl;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return;

    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    int count = 0;
    if (Process32First(hSnapshot, &processEntry)) {
        do {
            if (processEntry.th32ProcessID != 0) {
                cout << "PID: " << processEntry.th32ProcessID
                     << "\tName: " << processEntry.szExeFile << endl;
                count++;
            }
        } while (Process32Next(hSnapshot, &processEntry));
    }

    CloseHandle(hSnapshot);
    cout << "===============================" << endl;
    cout << "Total processes: " << count << endl;
}

void ProcessEnvVariable() {
    char envBuffer[4096];
    DWORD bufferLen = GetEnvironmentVariableA("PROC_TO_KILL", envBuffer, sizeof(envBuffer));

    if (bufferLen == 0) {
        cout << "\n[Killer] Environment variable PROC_TO_KILL not set" << endl;
        return;
    }

    if (bufferLen >= sizeof(envBuffer)) {
        cout << "\n[Killer] Environment variable too long" << endl;
        return;
    }

    string envContent(envBuffer);
    cout << "\n[Killer] Processing PROC_TO_KILL: " << envContent << endl;

    vector<string> targetNames;
    stringstream parser(envContent);
    string token;

    while (getline(parser, token, ',')) {
        token.erase(remove_if(token.begin(), token.end(), ::isspace), token.end());
        if (!token.empty()) {
            targetNames.push_back(token);
        }
    }

    for (const string& name : targetNames) {
        TerminateProcessesByName(name);
    }
}

void ShowHelp() {
    cout << "\nProcess Killer - Command Line Options" << endl;
    cout << "=====================================" << endl;
    cout << "--id PID       : Terminate process by ID" << endl;
    cout << "--name NAME    : Terminate processes by name" << endl;
    cout << "--list         : Display all running processes" << endl;
    cout << "--env          : Use PROC_TO_KILL environment variable" << endl;
    cout << "--help         : Show this help message" << endl;
    cout << "\nExamples:" << endl;
    cout << "  Killer.exe --id 1234" << endl;
    cout << "  Killer.exe --name notepad.exe" << endl;
    cout << "  Killer.exe --list" << endl;
    cout << "  Killer.exe --env" << endl;
}

int main(int argc, char* argv[]) {
    cout << "================ KILLER APPLICATION ================" << endl;
    cout << "[Killer] Running as PID: " << GetCurrentProcessId() << endl;

    if (argc < 2) {
        ShowHelp();
        return 0;
    }

    bool actionPerformed = false;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];

        if (arg == "--id" && i + 1 < argc) {
            DWORD pid = strtoul(argv[++i], NULL, 10);
            if (pid > 0) {
                TerminateProcessById(pid);
                actionPerformed = true;
            }
        }
        else if (arg == "--name" && i + 1 < argc) {
            string name = argv[++i];
            TerminateProcessesByName(name);
            actionPerformed = true;
        }
        else if (arg == "--list") {
            ListAllProcesses();
            actionPerformed = true;
        }
        else if (arg == "--env") {
            ProcessEnvVariable();
            actionPerformed = true;
        }
        else if (arg == "--help" || arg == "-h") {
            ShowHelp();
            return 0;
        }
    }

    if (!actionPerformed) {
        ProcessEnvVariable();
    }

    cout << "\n================ OPERATION COMPLETE ================" << endl;

    return 0;
}