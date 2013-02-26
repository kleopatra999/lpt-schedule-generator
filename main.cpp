
#include <inttypes.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <time.h>
#include "lpt.h"

std::vector<std::string> split (std::string s, char delim) {
    if (s.empty()) return std::vector<std::string>();
     // This is really inefficient, but it doesn't matter.
    std::vector<std::string> r;
    bool delimmed = true;
    for (uint i = 0; i < s.size(); i++) {
        if (s[i] == delim) {
            delimmed = true;
        }
        else {
            if (delimmed) {
                r.push_back("");
                delimmed = false;
            }
            r.back() += s[i];
        }
    }
    return r;
}


void usage (int argc, char** argv, std::string mess = "");

int run (uint n_machines, const std::vector<uint>& tasks, bool optimal, bool verbose) {
    using namespace std;
    if (optimal) {
        if (verbose) {
            cerr << "Cannot use both -v (verbose) and -o (optimal)." << endl;
            return 127;
        }
        cout << optimal_time(n_machines, tasks) << endl;
        return 0;
    }
    const Schedule& s = lpt(n_machines, tasks);
    if (verbose) {
        cout << "===";
        for (uint i = 0; i < tasks.size(); i++) {
            cout << tasks[i];
            if (i != tasks.size() - 1) cout << " ";
        }
        cout << endl;
        cout << " # 0";
        for (uint i = 5; i < s.total_time; i += 5) {
            cout << std::setw(5) << i;
        }
        cout << endl << "   =";
        for (uint i = 0; i < s.total_time; i++) cout << "=";
        cout << endl;
        for (uint m = 0; m < n_machines; m++) {
            cout << std::setw(2) << m << " |";
            for (uint i = 0; i < s.allocations[m].size(); i++) {
                if (s.allocations[m][i] > 0) {
                    cout << std::setw(s.allocations[m][i]) << "|";
                }
            }
            cout << endl << "   =";
            for (uint i = 0; i < s.total_time; i++) cout << "=";
            cout << endl;
        }
        cout << "Total time: " << s.total_time << endl;
        return 0;
    }
    else {
        cout << s.total_time << endl;
        return 0;
    }
}

int main (int argc, char** argv) {
    using namespace std;
    if (argc <= 1) {
        usage(argc, argv, "Not enough arguments.");
        return 127;
    }
     // Parse command-line arguments
    uint n_machines = 0;
    vector<uint> tasks;
    vector<string> filenames;
    bool verbose = false;
    bool optimal = false;
    bool from_file = false;
    for (int i = 1; i < argc; i++) {
        string arg = (const char*)argv[i];
        if (arg == "-v") {
            verbose = true;
        }
        else if (arg == "-o") {
            optimal = true;
        }
        else if (arg == "-f") {
            if (!tasks.empty()) {
                usage(argc, argv, "No tasks may be given on the command line with -f.");
                return 127;
            }
            if (n_machines == 0) {
                usage(argc, argv, "The number of machines must be given before -f.");
            }
            from_file = true;
        }
        else if (n_machines == 0) {
            stringstream(arg) >> n_machines;
            if (n_machines == 0) {
                usage(argc, argv, "The number of machines must not be 0.");
                return 127;
            }
        }
        else if (from_file) {
            filenames.push_back(arg);
        }
        else {
            uint task;
            stringstream(arg) >> task;
            tasks.push_back(task);
        }
    }
     // Read tasks from files if necessary.
    if (from_file) {
        vector<vector<uint> > taskses;
         // C++ IS VERY BLETCHFUL
        for (vector<string>::iterator fsi = filenames.begin(); fsi != filenames.end(); fsi++) {
            ifstream f (fsi->c_str());
            f.exceptions(ifstream::failbit);
            string file = string(istreambuf_iterator<char>(f), istreambuf_iterator<char>());
            f.close();
            const vector<string>& lines = split(file, '\n');
            taskses.resize(lines.size());
            for (uint li = 0; li < lines.size(); li++) {
                const vector<string>& parts = split(lines[li], '\t');
                taskses[li].resize(parts.size());
                for (uint pi = 0; pi < parts.size(); pi++) {
                    stringstream(parts[pi]) >> taskses[li][pi];
                }
            }
        }
        if (verbose) cout << "===NUMBER OF INSTANCES: " << taskses.size() << endl;
        for (vector<vector<uint> >::iterator tli = taskses.begin(); tli != taskses.end(); tli++) {
            int ret = run(n_machines, *tli, optimal, verbose);
            if (ret != 0)
                return ret;
        }
        return 0;
    }
    else {
        return run(n_machines, tasks, optimal, verbose);
    }

}

void usage (int argc, char** argv, std::string mess) {
    using namespace std;
    if (!mess.empty()) cerr << mess << endl;
    cerr << "Usage: " << string((const char*)argv[0]) << " [options] [number of machines] [tasks]..." << endl;
    cerr << "       " << string((const char*)argv[0]) << " [options] [number of machines] -f [filenames]..." << endl;
    cerr << "    number of machines must be greater than 0" << endl;
    cerr << "Options:" << endl;
    cerr << "    -f: Read lists of tasks from the given file(s) with one space-separated list of numbers per line." << endl;
    cerr << "    -v: Print out a chart of the selected schedule as well as the total schedule length." << endl;
    cerr << "    -o: Print the optimal schedule length (incompatible with -v).  Takes exponential CPU time." << endl;
}

