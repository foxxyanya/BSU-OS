#include <cstdio>
#include <functional>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include "process_func.h"

bool is_running(int id) {
    return kill(id, SIGCONT) == 0;
}

// goes through all processes with this name
bool is_running(const char* name) {
    auto pids = get_ids_by_name(name);
    for (int pid : pids) {
        if (is_running(pid)) {
            return true;
        }
    }
    return false;
}

// runs executable program and returns it's process id
int run(const char* prog_name) {
    int pid = fork();
    if (pid == 0) {
        execlp(prog_name, prog_name, nullptr);
        exit(0);
    }
    cout << "Process '" << prog_name << "' runs with process id = " << pid << ".\n";
    return pid;
}

void run_and_check_by_id(const char* app_name, const char* kill_exec_name) {
    int app_pid = run(app_name);
    sleep(1);
    int pid = fork();
    if (pid == 0){
        execlp(kill_exec_name, kill_exec_name, "--id", to_string(app_pid).c_str(), nullptr);
    } else {
        waitpid(pid, nullptr, 0);
    }
    waitpid(app_pid, nullptr, 0);
    if(!is_running(app_pid)) {
        cout << "Process with id " << app_pid << " was killed.\n";
    };
}

void run_and_check_by_name(const char* app_name, const char* kill_exec_name) {
    int app_pid1 = run(app_name);
    int app_pid2 = run(app_name);
    sleep(1);
    if(is_running(app_name)) {
        cout << "Start running some processes with name " << string(app_name) << ".\n";
    };
    int pid = fork();
    if (pid == 0){
        execlp(kill_exec_name, kill_exec_name, "--name", app_name, nullptr);
    } else {
        waitpid(pid, nullptr, 0);
    }
    waitpid(app_pid1, nullptr, 0);
    waitpid(app_pid2, nullptr, 0);
    if(!is_running(app_name)) {
        cout << "All processes with name " << string(app_name) << " were killed.\n";
    };
}

int main() {

    string curr_path = get_current_dir_name();
    string killer_compile_cmd = "g++ " + curr_path + "/killer.cpp -o killer";
    system(killer_compile_cmd.c_str());
    // system("g++ /home/anna/course_2/OS/user/killer.cpp -o killer");

    setenv("PROC_TO_KILL", "gedit", true);

    cout << "\n*** Starting some processes from 'PROC_TO_KILL' ***\n";
    run("gedit");
    sleep(2);

    cout << "\n*** Testing killer with parameter --id ***\n";
    run_and_check_by_id("gnome-calculator", "./killer");

    cout << "\n*** Testing killer with parameter --name. ***\n";
    run_and_check_by_name("firefox", "./killer");

    cout << "\n*** Asserting processes from 'PROC_TO_KILL' were killed ***\n";
    if(!is_running("gedit")) {
        cout << "Environment processes were killed.\n";
    }

    unsetenv("PROC_TO_KILL");

}
