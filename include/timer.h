/**
 * @file timer.h
 * @brief RAII-based scope timer for performance measurement.
 * 
 * Provides a simple way to measure execution time of code blocks
 * using the RAII pattern - timing starts on construction and
 * elapsed time is printed on destruction.
 */

#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <chrono>

using namespace std;

/**
 * @class Timer
 * @brief RAII timer that measures and reports execution time.
 * 
 * Creates a scope-based timer that automatically measures the time
 * between construction and destruction. Useful for profiling code
 * sections without explicit start/stop calls.
 * 
 * **Usage:**
 * ```cpp
 * {
 *     Timer t;  // Timer starts
 *     // ... code to measure ...
 * }  // Timer stops and prints elapsed time
 * ```
 * 
 * @note Uses high_resolution_clock for maximum precision.
 * @note Output is printed to stdout in the destructor.
 * 
 * @example
 * @code
 * void expensive_operation() {
 *     Timer timer;  // Starts timing
 *     // ... perform operations ...
 * }  // Destructor prints: "Elapsed time: X ms"
 * @endcode
 */
class Timer {
    chrono::high_resolution_clock::time_point start;  // Timestamp when timer was created
    
public:
    /**
     * @brief Constructs the timer and records the start time.
     * 
     * Captures the current high-resolution clock time point.
     */
    Timer();
    
    /**
     * @brief Destructor that calculates and prints elapsed time.
     * 
     * Computes the duration since construction and outputs
     * the elapsed time to standard output.
     */
    ~Timer();
};

#endif