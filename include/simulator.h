/**
 * @file simulator.h
 * @brief Main simulator class that orchestrates circuit analysis.
 * 
 * The Simulator class provides high-level analysis functions that
 * coordinate the circuit, solver, and solution deployment.
 */

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "solver.h"
#include "circuit.h"

/**
 * @class Simulator
 * @brief Orchestrates circuit analysis operations.
 * 
 * The Simulator class is the main entry point for performing circuit
 * analysis. It coordinates between the Circuit (which holds the topology
 * and MNA system) and the Solver (which computes solutions).
 * 
 * **Supported Analysis Types:**
 * - DC Analysis: Computes steady-state operating point
 * 
 * **Typical Workflow:**
 * ```cpp
 * Circuit circuit;
 * circuit.parse_netlist("circuit.net");
 * circuit.assemble_MNA_system();
 * 
 * Simulator sim;
 * sim.run_dc_analysis(circuit);
 * 
 * circuit.print_solution();
 * std::cout << sim;  // Print solver info
 * ```
 * 
 * @note Future extensions could add AC analysis, transient analysis, etc.
 * 
 * @see Circuit, Solver
 */
class Simulator : public I_Printable {
private:
    Solver solver;                  // Linear system solver
    std::vector<double> solution;   // Last computed solution vector
    
public:
    /**
     * @brief Performs DC operating point analysis.
     * @param circuit The circuit to analyze (must have MNA system assembled).
     * 
     * This method:
     * 1. Retrieves the MNA matrix and vector from the circuit
     * 2. Solves the linear system Ax = b
     * 3. Deploys the solution back to the circuit's nodes and components
     * 
     * After this call, node voltages and source currents are available
     * through the circuit's accessor methods.
     * 
     * @par Time Complexity
     * O(I × N × K) dominated by the iterative solver, where:
     * - I = iterations to convergence (≤ max_iter)
     * - N = nodes + extra variables
     * - K = average non-zeros per row (≤ 5 typical)
     * 
     * @par Space Complexity
     * O(N) for solution vector and solver internal state
     */
    void run_dc_analysis(Circuit& circuit);
    
    /**
     * @brief Prints simulation results and solver statistics.
     * @param os Output stream (default: std::cout).
     * 
     * Includes solver configuration, convergence info, timing,
     * and the raw solution vector.
     */
    virtual void print(std::ostream& os = std::cout) const override;
};

#endif