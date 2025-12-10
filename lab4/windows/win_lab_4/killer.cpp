#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <windows.h>
#include <tlhelp32.h>

using namespace std;

void TerminateProcessHandle(HANDLE hProcess, DWORD id)
{
    TerminateProcess(hProcess, 0);
    CloseHandle(hProcess);
}

void KillProcessById(DWORD processId)
{
    cout << "\n[Killer] Killing by ID: " << processId << endl;
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    if (hProcess != NULL)
    {
        TerminateProcessHandle(hProcess, processId);
        cout << "[Killer] ID " << processId << " terminated." << endl;
    }
    else
    {
        cout << "[Killer] ID " << processId << " not found." << endl;
    }
}

void KillProcessesByName(const string& processName)
{
    cout << "\n[Killer] Killing by name: " << processName << endl;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hSnapshot, &pe32))
    {
        do
        {
            if (string(pe32.szExeFile) == processName)
            {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
                if (hProcess != NULL)
                {
                    TerminateProcessHandle(hProcess, pe32.th32ProcessID);
                    cout << "[Killer] Terminated " << pe32.szExeFile << " (ID: " << pe32.th32ProcessID << ")." << endl;
                }
            }
        } while (Process32Next(hSnapshot, &pe32));
    }
    CloseHandle(hSnapshot);
}

void HandleEnvironmentVariable()
{
    char buffer[32767];
    DWORD bufferSize = GetEnvironmentVariableA("PROC_TO_KILL", buffer, sizeof(buffer));
    if (bufferSize > 0)
    {
        string envValue(buffer);
        cout << "\n[Killer] Reading PROC_TO_KILL: " << envValue << endl;
        stringstream ss(envValue);
        string name;
        while (getline(ss, name, ','))
        {
            name.erase(remove_if(name.begin(), name.end(), ::isspace), name.end());
            if (!name.empty())
            {
                KillProcessesByName(name);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    cout << "================ KILLER APP STARTED ================" << endl;
    bool argsProcessed = false;
    for (int i = 1; i < argc; ++i)
    {
        string arg = argv[i];
        if (arg == "--id" && i + 1 < argc)
        {
            DWORD id = strtoul(argv[++i], NULL, 10);
            KillProcessById(id);
            argsProcessed = true;
        }
        else if (arg == "--name" && i + 1 < argc)
        {
            string name = argv[++i];
            KillProcessesByName(name);
            argsProcessed = true;
        }
    }
    if (!argsProcessed)
    {
        HandleEnvironmentVariable();
    }
    cout << "================ KILLER APP FINISHED ================" << endl;
    return 0;
}