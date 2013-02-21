#ifndef HAVE_LPT_H
#define HAVE_LPT_H

#include <vector>

typedef uint32_t uint;

struct Schedule {
     // Indexed by [machine][task number on this machine]
    std::vector<std::vector<uint> > allocations;
    uint total_time;
};

Schedule lpt (uint n_machines, const std::vector<uint>& tasks);

uint optimal_time (uint n_machines, const std::vector<uint>& tasks);

#endif
