#include <iostream>
#include <sstream>
#include <csignal>
#include <fstream>
#include <cstring>
#include <vector>
#include <cstdlib>
#include "process_func.h"

using namespace std;

bool kill_process_by_id(int id) {
    return kill(id, SIGTERM) != -1;
}

void kill_process_by_name(const string& name) {
    auto ids = get_ids_by_name(name);
    for(int id: ids) {
        kill_process_by_id(id);
    }
}

void replace(string& s, char sep1, char sep2) {
    for (int i = 0; i < s.length(); ++i) {
        if(s[i] == sep1)
            s[i] = sep2;
    }
}

template<typename T>
vector<T> parse_str(const string& s) {
    istringstream iss(s);
    vector<T> v;
    T el;
    while (iss >> el) {
        v.push_back(el);
    }
    return v;
}

void kill_all_env_processes(const char* env_name, char separator) {
    string env_values = getenv(env_name);
    if(env_values.empty()) {
        cout << "Environment " << env_name << " is empty or doesn't exist.\n";
        cout.flush();
        return;
    }
    replace(env_values, separator, ' ');
    auto names = parse_str<string>(env_values);
    for(string& name : names) {
        kill_process_by_name(name);
    }
}

int main(int argc, char* argv[]) {

    if((argc < 3) || !((strcmp("--id", argv[1]) == 0) || (strcmp("--name", argv[1]) == 0))) {
        cout << "Correct format:\n" +
        string(argv[0]) + " --id <processes_ids> or\n" +
        string(argv[0]) + " --name <processes_names>\n";
        return 1;
    }

    if (strcmp("--id", argv[1]) == 0) {
        auto ids = parse_str<int>(argv[2]);
        for(int id : ids) {
            kill_process_by_id(id);
        }
    }
    if (strcmp("--name", argv[1]) == 0) {
        string name = argv[2];
        kill_process_by_name(name);
    }
    kill_all_env_processes("PROC_TO_KILL", ';');
    return 0;

}
