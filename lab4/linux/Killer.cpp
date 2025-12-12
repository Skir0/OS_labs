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

string obtainProcessName(pid_t processIdentifier)
{
    char filePath[256];
    snprintf(filePath, sizeof(filePath), "/proc/%d/comm", processIdentifier);
    ifstream nameFile(filePath);
    if (nameFile.is_open())
    {
        string processName;
        getline(nameFile, processName);
        return processName;
    }
    return "";
}

void terminateProcessById(pid_t processIdentifier)
{
    cout << "\n[Killer] Terminating by ID: " << processIdentifier << endl;
    if (processIdentifier <= 0 || processIdentifier == getpid())
    {
        cout << "[Killer] Invalid process ID or self-termination prevented." << endl;
        return;
    }
    string processName = obtainProcessName(processIdentifier);
    if (!processName.empty())
    {
        cout << "[Killer] Process name: " << processName << endl;
    }
    if (kill(processIdentifier, 0) == 0)
    {
        if (kill(processIdentifier, SIGTERM) == 0)
        {
            cout << "[Killer] Sent SIGTERM to process " << processIdentifier << endl;
            sleep(1);
            if (kill(processIdentifier, 0) == 0)
            {
                kill(processIdentifier, SIGKILL);
                cout << "[Killer] Sent SIGKILL to process " << processIdentifier << endl;
            }
        }
        cout << "[Killer] ID " << processIdentifier << " terminated." << endl;
    }
    else
    {
        cout << "[Killer] Process " << processIdentifier << " not found." << endl;
    }
}

void terminateProcessesByName(const string& targetProcessName)
{
    cout << "\n[Killer] Terminating by name: " << targetProcessName << endl;
    DIR* processDirectory = opendir("/proc");
    if (!processDirectory)
    {
        cerr << "[Killer] Cannot open /proc directory" << endl;
        return;
    }
    struct dirent* directoryEntry;
    vector<pid_t> processesToTerminate;
    while ((directoryEntry = readdir(processDirectory)) != nullptr)
    {
        pid_t processIdentifier = atoi(directoryEntry->d_name);
        if (processIdentifier > 0 && processIdentifier != getpid())
        {
            string currentProcessName = obtainProcessName(processIdentifier);
            if (!currentProcessName.empty() && currentProcessName == targetProcessName)
            {
                processesToTerminate.push_back(processIdentifier);
            }
        }
    }
    closedir(processDirectory);
    for (pid_t processIdentifier : processesToTerminate)
    {
        cout << "[Killer] Found " << targetProcessName << " (ID: " << processIdentifier << ")" << endl;
        terminateProcessById(processIdentifier);
    }
    if (processesToTerminate.empty())
    {
        cout << "[Killer] No processes found with name: " << targetProcessName << endl;
    }
}

void processEnvironmentVariable()
{
    const char* environmentValue = getenv("PROC_TO_KILL");
    if (environmentValue && strlen(environmentValue) > 0)
    {
        string environmentString(environmentValue);
        cout << "\n[Killer] Reading PROC_TO_KILL: " << environmentString << endl;
        stringstream stringStream(environmentString);
        string processName;
        while (getline(stringStream, processName, ','))
        {
            processName.erase(remove_if(processName.begin(), processName.end(), ::isspace), processName.end());
            if (!processName.empty())
            {
                terminateProcessesByName(processName);
            }
        }
    }
    else
    {
        cout << "\n[Killer] Environment variable PROC_TO_KILL not set." << endl;
    }
}

int main(int argumentCount, char* argumentValues[])
{
    cout << "================ KILLER APP STARTED ================" << endl;
    cout << "[Killer] My PID: " << getpid() << endl;
    bool argumentsHandled = false;
    for (int i = 1; i < argumentCount; ++i)
    {
        string currentArgument = argumentValues[i];
        if (currentArgument == "--id" && i + 1 < argumentCount)
        {
            pid_t processIdentifier = static_cast<pid_t>(atoi(argumentValues[++i]));
            terminateProcessById(processIdentifier);
            argumentsHandled = true;
        }
        else if (currentArgument == "--name" && i + 1 < argumentCount)
        {
            string processName = argumentValues[++i];
            terminateProcessesByName(processName);
            argumentsHandled = true;
        }
        else if (currentArgument == "--help" || currentArgument == "-h")
        {
            cout << "Usage:" << endl;
            cout << "  " << argumentValues[0] << " --id <PID>        Terminate process by ID" << endl;
            cout << "  " << argumentValues[0] << " --name <NAME>     Terminate processes by name" << endl;
            cout << "  " << argumentValues[0] << "                   Use PROC_TO_KILL environment variable" << endl;
            argumentsHandled = true;
        }
    }
    if (!argumentsHandled)
    {
        processEnvironmentVariable();
    }
    cout << "================ KILLER APP FINISHED ================" << endl;
    return 0;
}