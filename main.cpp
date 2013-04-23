
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

int run (uint n_machines, const std::vector<std::vector<uint> >& taskses, bool optimal, bool verbose, uint reps, bool just_time) {
    using namespace std;
    clock_t pre = clock();
    if (optimal) {
        if (verbose) {
            cerr << "Cannot use both -v (verbose) and -o (optimal)." << endl;
            return 127;
        }
        for (uint i = 0; i < taskses.size(); i++) {
            for (uint r = reps - 1; r; r--) {
                optimal_time(n_machines, taskses[i]);
            }
            if (!just_time)
                cout << optimal_time(n_machines, taskses[i]) << endl;
        }
    }
    else {
        for (uint i = 0; i < taskses.size(); i++) {
            for (uint r = reps - 1; r; r--) {
                lpt(n_machines, taskses[i]);
            }
            const Schedule& s = lpt(n_machines, taskses[i]);
            if (verbose) {
                cout << "===";
                for (uint j = 0; j < taskses[i].size(); j++) {
                    cout << taskses[i][j];
                    if (j != taskses[i].size() - 1) cout << " ";
                }
                cout << endl;
                cout << " # 0";
                for (uint j = 5; j < s.total_time; j += 5) {
                    cout << std::setw(5) << j;
                }
                cout << endl << "   =";
                for (uint j = 0; j < s.total_time; j++) cout << "=";
                cout << endl;
                for (uint m = 0; m < n_machines; m++) {
                    cout << std::setw(2) << m << " |";
                    for (uint j = 0; j < s.allocations[m].size(); j++) {
                        if (s.allocations[m][j] > 0) {
                            cout << std::setw(s.allocations[m][j]) << "|";
                        }
                    }
                    cout << endl << "   =";
                    for (uint j = 0; j < s.total_time; j++) cout << "=";
                    cout << endl;
                }
                cout << "Total time: " << s.total_time << endl;
            }
            else if (!just_time) {
                cout << s.total_time << endl;
            }
        }
    }
    clock_t post = clock();
    double spent = (post - pre) * 1.0 / CLOCKS_PER_SEC;
    if (just_time)
        cout << spent << endl;
    else
        cout << "Approximate calculation time: " << spent << "s" << endl;
    return 0;
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
    bool want_reps = false;
    bool just_time = false;
    uint reps = 1;
    for (int i = 1; i < argc; i++) {
        string arg = (const char*)argv[i];
        if (want_reps) {
            stringstream(arg) >> reps;
            want_reps = false;
        }
        else if (arg == "-v") {
            verbose = true;
        }
        else if (arg == "-o") {
            optimal = true;
        }
        else if (arg == "-n") {
            want_reps = true;
        }
        else if (arg == "-t") {
            just_time = true;
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
         // C++ IS VERY BLETCHFUL
        for (vector<string>::iterator fsi = filenames.begin(); fsi != filenames.end(); fsi++) {
            ifstream f (fsi->c_str());
            f.exceptions(ifstream::failbit);
            string file = string(istreambuf_iterator<char>(f), istreambuf_iterator<char>());
            f.close();
            const vector<string>& lines = split(file, '\n');
            vector<vector<uint> > taskses (lines.size());
            for (uint li = 0; li < lines.size(); li++) {
                const vector<string>& parts = split(lines[li], '\t');
                taskses[li].resize(parts.size());
                for (uint pi = 0; pi < parts.size(); pi++) {
                    stringstream(parts[pi]) >> taskses[li][pi];
                }
            }
            if (verbose) cout << "===NUMBER OF INSTANCES: " << taskses.size() << endl;
            int ret = run(n_machines, taskses, optimal, verbose, reps, just_time);
            if (ret != 0) {
                cerr << "Calculation aborted due to error" << endl;
                return ret;
            }
        }
        return 0;
    }
    else {
        return run(n_machines, std::vector<std::vector<uint> >(1, tasks), optimal, verbose, reps, just_time);
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
    cerr << "    -n [reps]: Repeat the calculations this many times, for benchmarking." << endl;
    cerr << "    -t: Print nothing but the time the calculation took, in seconds." << endl;
}

