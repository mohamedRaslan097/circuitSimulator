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
 * - AC Analysis: Frequency-domain phasor analysis
 * 
 * **Typical Workflow:**
 * ```cpp
 * Circuit circuit;
 * circuit.parse_netlist("circuit.net");
 * circuit.assemble_MNA_system();
 * 
 * // DC Analysis
 * Simulator sim("ac_results.csv");  // Specify AC output file
 * sim.run_dc_analysis(circuit);
 * 
 * // AC Analysis (frequency sweep)
 * sim.run_ac_analysis(circuit, 1.0, 10000.0, 100.0);  // 1Hz to 10kHz, 100Hz steps
 * 
 * // Or single frequency
 * sim.run_ac_analysis(circuit, 1000.0);  // 1kHz only
 * 
 * circuit.print_solution();
 * std::cout << sim;  // Print solver info
 * ```
 * 
 * @see Circuit, Solver
 */
class Simulator : public I_Printable {
private:
    Solver solver;                  // Linear system solver
    std::vector<double> solution;   // Last computed solution vector
    
public:
    /**
     * @brief Constructs a Simulator with optional AC output file path.
     * @param ac_output_file Path for AC analysis results (default: "ac_analysis_results.csv").
     */
    Simulator(const std::string& ac_output_file = "ac_analysis_results.csv");
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
     * @brief Performs AC frequency sweep analysis.
     * @param circuit The circuit to analyze (must have MNA system assembled).
     * @param freq1 Start frequency in Hertz (default: 1000 Hz = 1 kHz).
     * @param freq2 End frequency in Hertz (default: 100,000 Hz = 100 kHz).
     * @param step Frequency step size in Hertz (default: 100 Hz).
     * @param log_scale If true, uses logarithmic stepping (default: false).
     * 
     * This method:
     * 1. Retrieves the DC MNA matrix as base for AC system
     * 2. Initializes the complex-valued AC MNA system
     * 3. Performs frequency sweep from freq1 to freq2
     * 4. At each frequency, computes phasor voltages/currents
     * 5. Logs results to file (ac_analysis_results.csv)
     * 
     * **AC Component Behavior:**
     * - Resistors: Same as DC (conductance stamping)
     * - Capacitors: Admittance Y = jωC
     * - Inductors: Admittance Y = 1/(jωL)
     * - Voltage Sources: Phasor voltage from AC amplitude
     * 
     * @note DC analysis should typically be run first to establish operating point.
     * 
     * @par Time Complexity
     * O(F × I × N × K) where F = (freq2-freq1)/step frequency points
     * 
     * @par Space Complexity
     * O(N) for complex solution vector (2× real storage)
     * 
     * @see run_dc_analysis()
     */
    void run_ac_analysis(Circuit& circuit, double freq1, double freq2, double step, bool log_scale = false);

    /**
     * @brief Performs AC analysis at a single frequency.
     * @brief Overload of run_ac_analysis for single frequency.
     * @param circuit The circuit to analyze (must have MNA system assembled).
     * @param frequency Frequency in Hertz.
     * 
     * This method is similar to the full frequency sweep but only
     * analyzes the circuit at a single specified frequency.
     * 
     * @par Time Complexity
     * O(I × N × K) dominated by the iterative solver
     * 
     * @par Space Complexity
     * O(N) for complex solution vector
     * 
     * @see run_dc_analysis(), run_ac_analysis()
     */
    void run_ac_analysis(Circuit& circuit, double frequency);
    
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