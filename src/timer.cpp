#include "timer.h"

Timer::Timer() : start(chrono::high_resolution_clock::now()) {}

Timer::~Timer() {
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "\nExecution time: " << duration.count() << " microseconds (" 
         << duration.count() / 1000.0 << " ms)" << endl;
}
