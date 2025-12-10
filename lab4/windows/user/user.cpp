#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include <tlhelp32.h>

using namespace std;

struct RunningProcess
{
    DWORD id;
    string name;
};

RunningProcess StartTestProcess(const string& appName)
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    cout << "\n[User] Launching: " << appName << endl;
    vector<char> cmdBuffer(appName.begin(), appName.end());
    cmdBuffer.push_back('\0');
    CreateProcessA(NULL, cmdBuffer.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    cout << "[User] Launched. ID: " << pi.dwProcessId << endl;
    return { pi.dwProcessId, appName };
}

bool IsProcessRunning(DWORD processId)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
    if (hProcess == NULL) return false;
    DWORD exitCode;
    bool isRunning = (GetExitCodeProcess(hProcess, &exitCode) && exitCode == STILL_ACTIVE);
    CloseHandle(hProcess);
    return isRunning;
}

void RunKiller(const string& args)
{
    string commandLine = "Killer.exe " + args;
    cout << "\n--- Calling Killer: " << commandLine << " ---" << endl;
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    vector<char> cmdBuffer(commandLine.begin(), commandLine.end());
    cmdBuffer.push_back('\0');
    CreateProcessA(NULL, cmdBuffer.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void CheckAndOutput(const RunningProcess& rp)
{
    if (rp.id == 0) return;
    cout << "Check " << rp.id << " (" << rp.name << "): ";
    if (IsProcessRunning(rp.id))
    {
        cout << "STILL RUNNING." << endl;
    }
    else
    {
        cout << "TERMINATED." << endl;
    }
}

int main()
{
    cout << "================ USER APP STARTED ================" << endl;
    SetEnvironmentVariableA("PROC_TO_KILL", "Notepad.exe,calc.exe");
    cout << "[User] Set env var PROC_TO_KILL = 'Notepad.exe,calc.exe'" << endl;
    RunningProcess p1 = StartTestProcess("Notepad.exe");
    if (p1.id != 0)
    {
        RunKiller("--id " + to_string(p1.id));
        CheckAndOutput(p1);
    }
    RunningProcess p2 = StartTestProcess("Notepad.exe");
    RunningProcess p3 = StartTestProcess("Notepad.exe");
    RunKiller("--name Notepad.exe");
    CheckAndOutput(p2);
    CheckAndOutput(p3);
    RunningProcess p4 = StartTestProcess("Notepad.exe");
    RunningProcess p5 = StartTestProcess("calc.exe");
    RunKiller("");
    CheckAndOutput(p4);
    CheckAndOutput(p5);
    SetEnvironmentVariableA("PROC_TO_KILL", NULL);
    cout << "\n[User] Removed environment variable PROC_TO_KILL." << endl;
    cout << "================ USER APP FINISHED ================" << endl;
    return 0;
}