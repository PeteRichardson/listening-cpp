#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <vector>

using std::cout, std::endl;

struct Listener {
    int port{};
    std::string command{};
    int pid;
    std::string user;
    std::string node;
    std::string name{};
    std::string action{"LISTEN"};

    Listener(std::string);
};

Listener::Listener(std::string lsof_line) {
    // node                        10166 pete   31u  IPv4 0xe4ad34249b227fc5      0t0  TCP 127.0.0.1:45623 (LISTEN)
    std::stringstream ss{lsof_line};
    std::string dummy;

    ss >> this->command >> this->pid >> this->user;
    ss >> dummy >> dummy >> dummy >> dummy >> this->node;
    ss >> this->name;

    std::string escaped_space = "\\x20";
    while(this->command.find(escaped_space) != std::string::npos) {
        this->command.replace(this->command.find(escaped_space), escaped_space.size(), " ");
    };

    size_t pos = name.find(":");
    auto namestr = name.substr(pos+1);
    this->port = stoi(namestr);
}

std::ostream& operator<<(std::ostream& out, Listener l) {
    // out << std::setw(7) <<  << " " <<  << " " <<  << " " <<  << " " << e << " " <<  << " " << << endl;
    cout << std::setw(7) << l.port
         << std::setw(25) << l.command
         << std::setw(7) << l.pid
         << std::setw(7) << l.user
         << std::setw(5) << l.node
         << std::setw(17) << l.name
         << std::setw(8) << l.action;
    return out;
}

int main(int argc, char*argv[]) {
    char cmd[]{"lsof -nP +c 0 -i4"};

    FILE *fp = popen(cmd, "r");
    if (fp == NULL) {
        std::cerr << "Failed to run command '" << cmd << "'" << endl;
        std::exit(EXIT_FAILURE);
    }

    std::vector<Listener> listeners{};

    const size_t kBUFSIZE = 512;
    char line_buffer[kBUFSIZE];
    while (!feof(fp)) {
        if (fgets(line_buffer, kBUFSIZE, fp) != NULL) {
            std::string line{line_buffer};
            if (line.ends_with("(LISTEN)\n"))
                listeners.push_back(Listener(line));
        }
    }
    pclose(fp);

    sort(listeners.begin(), listeners.end(), [] (Listener& lhs, Listener& rhs) { return lhs.port < rhs.port; });

    cout << std::setw(7) << "PORT"
         << std::setw(25) << "COMMAND"
         << std::setw(7) << "PID"
         << std::setw(7) << "USER"
         << std::setw(5) << "NODE"
         << std::setw(17) << "NAME"
         << std::setw(8) << "ACTION"
         << endl;
    cout << std::setw(7) << "----"
         << std::setw(25) << "-------"
         << std::setw(7) << "---"
         << std::setw(7) << "----"
         << std::setw(5) << "----"
         << std::setw(17) << "----"
         << std::setw(8) << "------"
         << endl;
    for (auto & l : listeners) {
        cout << l << endl;
    }
}