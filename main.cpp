
#include <inttypes.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "lpt.h"

using std::string;
using std::stringstream;
using std::cout;
using std::cerr;
using std::endl;

void usage (int argc, char** argv);

int main (int argc, char** argv) {
    if (argc <= 1) {
        usage(argc, argv);
        return 127;
    }
     // Parse command-line arguments
    uint n_machines = 0;
    std::vector<uint> tasks;
    bool verbose = false;
    bool optimal = false;
    for (int i = 1; i < argc; i++) {
        string arg = (const char*)argv[i];
        if (arg == "-v") {
            verbose = true;
        }
        else if (arg == "-o") {
            optimal = true;
        }
        else if (n_machines == 0) {
            stringstream(arg) >> n_machines;
            if (n_machines == 0) {
                usage(argc, argv);
                return 127;
            }
        }
        else {
            uint task;
            stringstream(arg) >> task;
            tasks.push_back(task);
        }
    }
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
    }
    else {
        cout << s.total_time << endl;
    }

}

void usage (int argc, char** argv) {
    cout << "Usage: " << string((const char*)argv[0]) << " [options] [number of machines] [tasks]..." << endl;
    cout << "    number of machines must be greater than 0" << endl;
    cout << "Options:" << endl;
    cout << "    -v: Print out a chart of the selected schedule as well as the total schedule length." << endl;
    cout << "    -o: Print the optimal schedule length (incompatible with -v).  Takes exponential CPU time." << endl;
}

