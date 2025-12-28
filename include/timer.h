#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <chrono>

using namespace std;

class Timer {
    chrono::high_resolution_clock::time_point start;
public:
    Timer();
    ~Timer();
};

#endif