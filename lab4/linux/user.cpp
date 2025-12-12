#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
#include <fstream>

using namespace std;

struct RunningProcess
{
    pid_t id;
    string name;
    bool killed;
};

void IgnoreChildSignal()
{
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGCHLD, &sa, NULL);
}

bool IsProcessRunning(pid_t processId)
{
    if (processId <= 0)
        return false;
    return (kill(processId, 0) == 0);
}

RunningProcess StartTestProcess(const string& appName)
{
    cout << "\n[User] Launching: " << appName << endl;
    pid_t pid = fork();
    if (pid == 0)
    {
        signal(SIGINT, SIG_IGN);
        signal(SIGTERM, SIG_IGN);
        vector<string> args;
        stringstream ss(appName);
        string token;
        while (ss >> token)
        {
            args.push_back(token);
        }
        vector<char*> argv;
        for (auto& arg : args)
        {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);
        execvp(argv[0], argv.data());
        perror("[User] execvp failed");
        exit(1);
    }
    else if (pid > 0)
    {
        usleep(500000);
        cout << "[User] Launched. ID: " << pid << endl;
        return { pid, appName, false };
    }
    else
    {
        perror("[User] fork failed");
        return { 0, "", false };
    }
}

void RunKiller(const string& args)
{
    string commandLine = "./killer " + args;
    cout << "\n--- Calling Killer: " << commandLine << " ---" << endl;
    pid_t pid = fork();
    if (pid == 0)
    {
        vector<string> argList;
        stringstream ss(commandLine);
        string token;
        while (ss >> token)
        {
            argList.push_back(token);
        }
        vector<char*> argv;
        for (auto& arg : argList)
        {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);
        execvp(argv[0], argv.data());
        perror("[User] execvp killer failed");
        exit(1);
    }
    else if (pid > 0)
    {
        int status;
        waitpid(pid, &status, 0);
        cout << "[User] Killer finished" << endl;
    }
}

void CheckAndOutput(RunningProcess& rp)
{
    if (rp.id == 0 || rp.killed) return;
    cout << "Check " << rp.id << " (" << rp.name << "): ";
    if (IsProcessRunning(rp.id))
    {
        cout << "STILL RUNNING." << endl;
    }
    else
    {
        cout << "TERMINATED." << endl;
        rp.killed = true;
    }
}

int main()
{
    IgnoreChildSignal();
    cout << "================ USER APP STARTED ================" << endl;
    cout << "[User] My PID: " << getpid() << endl;
    setenv("PROC_TO_KILL", "sleep", 1);
    cout << "[User] Set env var PROC_TO_KILL = 'sleep'" << endl;
    vector<RunningProcess> processes;
    cout << "\n=== Scenario 1: Kill by ID ===" << endl;
    RunningProcess p1 = StartTestProcess("sleep 10");
    processes.push_back(p1);
    if (p1.id != 0)
    {
        sleep(2);
        RunKiller("--id " + to_string(p1.id));
        CheckAndOutput(processes[0]);
    }
    cout << "\n=== Scenario 2: Kill by Name ===" << endl;
    RunningProcess p2 = StartTestProcess("sleep 15");
    RunningProcess p3 = StartTestProcess("sleep 20");
    processes.push_back(p2);
    processes.push_back(p3);
    sleep(2);
    RunKiller("--name sleep");
    for (int i = 1; i < 3; i++)
    {
        CheckAndOutput(processes[i]);
    }
    cout << "\n=== Scenario 3: Environment Variable ===" << endl;
    RunningProcess p4 = StartTestProcess("sleep 25");
    RunningProcess p5 = StartTestProcess("sleep 30");
    processes.push_back(p4);
    processes.push_back(p5);
    sleep(2);
    RunKiller("");
    for (int i = 3; i < 5; i++)
    {
        CheckAndOutput(processes[i]);
    }
    cout << "\n=== Cleaning up remaining processes ===" << endl;
    for (auto& proc : processes)
    {
        if (!proc.killed && IsProcessRunning(proc.id))
        {
            cout << "Killing remaining process " << proc.id << endl;
            kill(proc.id, SIGTERM);
            sleep(1);
            if (IsProcessRunning(proc.id))
            {
                kill(proc.id, SIGKILL);
            }
        }
    }
    unsetenv("PROC_TO_KILL");
    cout << "\n[User] Removed environment variable PROC_TO_KILL." << endl;
    cout << "================ USER APP FINISHED ================" << endl;
    return 0;
}