#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <future>
#include <syncstream>
#include <unordered_map>
#include <vector>

using namespace std;


void compute(const string& name, int seconds) {
    this_thread::sleep_for(chrono::seconds(seconds));
    osyncstream(cout) << name << "\n";
}


struct Task {
    string name;
    int duration;
    vector<string> deps;
};

void work() {
    auto start = chrono::steady_clock::now();

    unordered_map<string, Task> tasks = {
        {"A1", {"A1", 1, {}}},
        {"A2", {"A2", 7, {}}},
        {"A3", {"A3", 7, {}}},
        {"A4", {"A4", 1, {}}},
        {"B",  {"B", 1, {"A1", "A2"}}},
        {"C",  {"C", 1, {"A3", "A4"}}},
        {"D",  {"D", 1, {"B"}}}
    };


    unordered_map<string, future<void>> fut;


    auto runTask = [&](const string& name) -> future<void> {
        return async(launch::async, [&, name]() {

            for (auto& dep : tasks[name].deps) {
                fut[dep].get();
            }
            compute(name, tasks[name].duration);
            });
        };

    fut["A1"] = runTask("A1");
    fut["A2"] = runTask("A2");
    fut["A3"] = runTask("A3");
    fut["A4"] = runTask("A4");

    fut["B"] = runTask("B");
    fut["D"] = runTask("D");
    fut["C"] = runTask("C");

    fut["D"].get();
    fut["C"].get();

    auto end = chrono::steady_clock::now();
    double time_sec =
        chrono::duration<double>(end - start).count();

    osyncstream(cout) << "Time: " << time_sec << " seconds\n";
    osyncstream(cout) << "Work is done!\n";
}

int main() {
    work();
    return 0;
}


