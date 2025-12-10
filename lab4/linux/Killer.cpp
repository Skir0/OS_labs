#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

string GetProcessName(pid_t pid)
{
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/comm", pid);
    ifstream commFile(path);
    if (commFile.is_open())
    {
        string name;
        getline(commFile, name);
        return name;
    }
    return "";
}

void KillProcessById(pid_t processId)
{
    cout << "\n[Killer] Killing by ID: " << processId << endl;
    if (processId <= 0 || processId == getpid())
    {
        cout << "[Killer] Invalid process ID or self-termination prevented." << endl;
        return;
    }
    string procName = GetProcessName(processId);
    if (!procName.empty())
    {
        cout << "[Killer] Process name: " << procName << endl;
    }
    if (kill(processId, 0) == 0)
    {
        if (kill(processId, SIGTERM) == 0)
        {
            cout << "[Killer] Sent SIGTERM to process " << processId << endl;
            sleep(1);
            if (kill(processId, 0) == 0)
            {
                kill(processId, SIGKILL);
                cout << "[Killer] Sent SIGKILL to process " << processId << endl;
            }
        }
        cout << "[Killer] ID " << processId << " terminated." << endl;
    }
    else
    {
        cout << "[Killer] Process " << processId << " not found." << endl;
    }
}

void KillProcessesByName(const string& processName)
{
    cout << "\n[Killer] Killing by name: " << processName << endl;
    DIR* procDir = opendir("/proc");
    if (!procDir)
    {
        cerr << "[Killer] Cannot open /proc directory" << endl;
        return;
    }
    struct dirent* entry;
    vector<pid_t> pidsToKill;
    while ((entry = readdir(procDir)) != nullptr)
    {
        pid_t pid = atoi(entry->d_name);
        if (pid > 0 && pid != getpid())
        {
            string procName = GetProcessName(pid);
            if (!procName.empty() && procName == processName)
            {
                pidsToKill.push_back(pid);
            }
        }
    }
    closedir(procDir);
    for (pid_t pid : pidsToKill)
    {
        cout << "[Killer] Found " << processName << " (ID: " << pid << ")" << endl;
        KillProcessById(pid);
    }
    if (pidsToKill.empty())
    {
        cout << "[Killer] No processes found with name: " << processName << endl;
    }
}

void HandleEnvironmentVariable()
{
    const char* envValue = getenv("PROC_TO_KILL");
    if (envValue && strlen(envValue) > 0)
    {
        string envStr(envValue);
        cout << "\n[Killer] Reading PROC_TO_KILL: " << envStr << endl;
        stringstream ss(envStr);
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
    else
    {
        cout << "\n[Killer] Environment variable PROC_TO_KILL not set." << endl;
    }
}

int main(int argc, char* argv[])
{
    cout << "================ KILLER APP STARTED ================" << endl;
    cout << "[Killer] My PID: " << getpid() << endl;
    bool argsProcessed = false;
    for (int i = 1; i < argc; ++i)
    {
        string arg = argv[i];
        if (arg == "--id" && i + 1 < argc)
        {
            pid_t id = static_cast<pid_t>(atoi(argv[++i]));
            KillProcessById(id);
            argsProcessed = true;
        }
        else if (arg == "--name" && i + 1 < argc)
        {
            string name = argv[++i];
            KillProcessesByName(name);
            argsProcessed = true;
        }
        else if (arg == "--help" || arg == "-h")
        {
            cout << "Usage:" << endl;
            cout << "  " << argv[0] << " --id <PID>        Kill process by ID" << endl;
            cout << "  " << argv[0] << " --name <NAME>     Kill processes by name" << endl;
            cout << "  " << argv[0] << "                   Use PROC_TO_KILL environment variable" << endl;
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