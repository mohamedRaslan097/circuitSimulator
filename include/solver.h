/**
 * @file solver.h
 * @brief High-level solver wrapper for the circuit simulator.
 * 
 * Provides a unified interface for solving MNA linear systems,
 * abstracting the underlying iterative method and providing
 * timing information.
 */

#ifndef SOLVER_H
#define SOLVER_H

#include <complex>
#include "I_Printable.h"
#include "component.h"
#include "gauss_seidel.h"
#include "ac_analyzer.h"

/**
 * @class Solver
 * @brief High-level interface for solving MNA linear systems.
 * 
 * The Solver class wraps the underlying iterative solver (Gauss-Seidel)
 * and provides additional functionality such as timing measurements
 * and a simplified interface for the Simulator class.
 * 
 * **Usage:**
 * ```cpp
 * Solver solver(1000, 1e-9, 0.5);  // max_iter, tolerance, damping
 * std::vector<double> solution;
 * solver.solve_MNA_system(mna_matrix, mna_vector, solution);
 * ```
 * 
 * @note The solver automatically resizes the solution vector to match
 *       the system size.
 * 
 * @see Gauss_seidel, Simulator
 */
class Solver : public I_Printable {
private:
    Gauss_seidel<double> gauss_seidel;      // DC solver (real-valued)
    Gauss_seidel<std::complex<double>> gauss_seidel_ac;  // AC solver (complex-valued)
    Ac_analyzer ac_analyzer;                // AC analysis handler
    std::chrono::microseconds duration;     // Time taken for DC solve operation
    std::chrono::microseconds ac_duration;  // Time taken for AC solve operation
    int avg_ac_duration;                    // Average time taken per AC frequency point

    /**
     * @brief Computes AC response at a single frequency point.
     * @param ac_components Map of AC-capable components.
     * @param frequency Analysis frequency in Hertz.
     * 
     * Internal helper that assembles AC MNA system, solves it,
     * and logs the solution for one frequency point.
     */
    void get_ac_response(const std::unordered_map<std::string, Component*>& ac_components, double frequency);
    
public:
    /**
     * @brief Constructs a Solver with specified parameters.
     * @param max_iter Maximum number of iterations (default: 1000).
     * @param tolerance Convergence tolerance (default: 1e-9).
     * @param damping_factor Under-relaxation factor (default: 0.5).
     * @param ac_output_file Path for AC analysis results (default: "ac_analysis_results.csv").
     */
    Solver(const std::string& ac_output_file = "ac_analysis_results.csv", int max_iter = 1000, double tolerance = 1e-9, double damping_factor = 0.5);
    
    /**
     * @brief Sets the AC output file path.
     * @param path Path for AC analysis results CSV.
     */
    void set_ac_output_file(const std::string& path);
    
    /**
     * @brief Solves the MNA linear system Ax = b.
     * @param mna_matrix Sparse system matrix A (row -> col -> value).
     * @param mna_vector Right-hand side vector b (row -> value).
     * @param solution Output solution vector x (resized automatically).
     * 
     * The solution vector contains:
     * - Index 0: Ground (always 0)
     * - Indices 1 to N: Node voltages
     * - Indices N+1 to M: Extra variables (source/inductor currents)
     * 
     * @par Time Complexity
     * O(I × N × K) where:
     * - I = iterations to convergence
     * - N = system dimension
     * - K = average non-zeros per row
     * 
     * @par Space Complexity
     * O(N) for solution vector and internal solver state
     */
    void solve_MNA_system(const std::unordered_map<int,std::unordered_map<int,double>>& mna_matrix,
                          const std::unordered_map<int, double>& mna_vector,
                          std::vector<double>& solution);
    
    /**
     * @brief Initializes the AC analysis system from DC MNA matrix.
     * @param mna_matrix Sparse DC system matrix A (real-valued).
     * @param extra_vars Map of extra variable IDs (voltage source/inductor currents).
     * @param initial_solution Initial DC solution vector for AC analysis.
     * 
     * Converts the DC MNA matrix to complex representation for AC analysis,
     * filtering out extra variables that are handled differently in AC mode.
     * 
     * @note Must be called before solve_ac_system().
     * 
     * @par Time Complexity
     * O(NNZ) for matrix conversion
     */
    void assemble_ac_system(const std::unordered_map<int,std::unordered_map<int,double>>& mna_matrix,
                            const std::map<int, std::string>& extra_vars,
                            const std::vector<double>& initial_solution);
    
    /**
     * @brief Performs AC frequency sweep analysis.
     * @param ac_components Map of AC-capable components (C, L, V with AC).
     * @param freq1 Start frequency in Hertz (default: 1000 Hz).
     * @param freq2 End frequency in Hertz (default: 100,000 Hz).
     * @param step Frequency step size in Hertz (default: 100 Hz).
     * @param log_scale If true, uses logarithmic stepping (default: false).
     * 
     * Iterates from freq1 to freq2 in steps of 'step', at each frequency:
     * 1. Assembles AC component contributions (admittances)
     * 2. Solves the complex MNA system using Gauss-Seidel
     * 3. Logs results to file
     * 
     * @par Time Complexity
     * O(F × I × R × K) where F = frequency points, I = iterations,
     * R = matrix rows, K = non-zeros per row
     * 
     * @par Space Complexity
     * O(NNZ + M) for complex matrix and solution vector
     */
    void solve_ac_system(const std::unordered_map<std::string, Component*>& ac_components,
                         double freq1, double freq2, double step, bool log_scale = false);
    
    /**
     * @brief Prints solver configuration and timing information.
     * @param os Output stream (default: std::cout).
     * 
     * Includes Gauss-Seidel parameters, convergence status,
     * iterations taken, and execution time.
     */
    virtual void print(std::ostream& os = std::cout) const override;
};

#endif