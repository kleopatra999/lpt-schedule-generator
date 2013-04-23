
#include <inttypes.h>
#include <iostream>
#include "lpt.h"
#include "prique.h"
using namespace std;

 // This represents an available slot for a task.
struct Slot {
    uint machine;
    uint time;
    Slot (uint m, uint t) : machine(m), time(t) { }
};
 // This selects the preferred slot for a task.
bool earlier_slot (const Slot& a, const Slot& b) {
     // prefer earlier times or, if they're the same, lower-number machines.
    return a.time == b.time
        ? a.machine < b.machine
        : a.time < b.time;
}

Schedule lpt (uint n_machines, const std::vector<uint>& task_list) {
    Schedule r;
    r.allocations.resize(n_machines);
    r.total_time = 0;
     // Build queue of tasks;
    Prique<uint, default_gt> tasks;
    for (uint i = 0; i < task_list.size(); i++) {
        tasks.push(task_list[i]);
    }
     // Start slot queue with one 0-time slot for each machine.
    Prique<Slot, earlier_slot> slots;
    for (uint i = 0; i < n_machines; i++) {
        slots.push(Slot(i, 0));
    }
     // Now do the actual processing.
    while (!tasks.empty()) {
        uint task = tasks.pop();
        Slot slot = slots.pop();
        r.allocations[slot.machine].push_back(task);
        uint finish = slot.time + task;
        if (finish > r.total_time) r.total_time = finish;
        slots.push(Slot(slot.machine, finish));
    }
     // We should be done now.
    return r;
}

uint optimal_part (bool& done, uint lower, uint n_machines, const std::vector<uint>& task_list, std::vector<uint>& mtimes, const uint task_i) {
    if (task_i == task_list.size()) {
        uint max = 0;
        for (uint m = 0; m < n_machines; m++) {
            if (mtimes[m] > max) max = mtimes[m];
        }
        if (max == lower)
            done = true;
        return max;
    }
    else {
        uint min = (uint)-1;  // Who doesn't use two's compliment?
        for (uint m = 0; m < n_machines; m++) {
            uint omt = mtimes[m];
            mtimes[m] += task_list[task_i];
            uint thisres = optimal_part(done, lower, n_machines, task_list, mtimes, task_i + 1);
            if (done) return thisres;
            mtimes[m] = omt;
            if (thisres < min) min = thisres;
        }
        return min;
    }
}

uint optimal_time (uint n_machines, const std::vector<uint>& task_list) {
    std::vector<uint> mtimes (n_machines, 0);
    bool done = false;
    uint max = 0;
    for (uint i = 0; i < task_list.size(); i++) {
        max += task_list[i];
    }
    return optimal_part(done, (max / n_machines + (max % n_machines != 0)), n_machines, task_list, mtimes, 0);
}

