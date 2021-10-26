#ifndef KILLER_PROCESS_FUNC_H
#define KILLER_PROCESS_FUNC_H

#include <iostream>
#include <fstream>
#include <vector>
#include <dirent.h>

using namespace std;

string extract_proc_name(const string& id_info_line) {
    string name;
    size_t pos = id_info_line.find('\0');
    if (pos != string::npos)
        name = id_info_line.substr(0, pos);
    pos = name.rfind('/');
    if (pos != string::npos)
        name = name.substr(pos + 1);
    return name;
}

vector<int> get_ids_by_name(const string& proc_name) {
    vector<int> ids;
    DIR* procs_dir = opendir("/proc");
    struct dirent* dir;
    // going through all /proc entries
    while ((dir = readdir(procs_dir)) != nullptr) {
        int id = atoi(dir->d_name);
        // considering only numeric entries
        if (id > 0) {
            // reading contents of virtual /proc/{pid}/cmdline file
            string proc_path = string("/proc/") + dir->d_name + "/cmdline";
            ifstream file(proc_path);
            string id_info_line;
            getline(file, id_info_line);
            if (!id_info_line.empty()) {
                string id_name = extract_proc_name(id_info_line);
                if (proc_name == id_name) {
                    ids.push_back(id);
                }
            }
        }
    }
    closedir(procs_dir);
    return ids;
}

#endif //KILLER_PROCESS_FUNC_H
