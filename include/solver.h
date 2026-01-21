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

#include <unordered_map>
#include <string>
#include <vector>
#include <chrono>
#include "I_Printable.h"
#include "gauss_seidel.h"

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
    Gauss_seidel gauss_seidel;           // Underlying Gauss-Seidel iterative solver
    std::chrono::microseconds duration;   // Time taken for last solve operation
    
public:
    /**
     * @brief Constructs a Solver with specified parameters.
     * @param max_iter Maximum number of iterations (default: 1000).
     * @param tolerance Convergence tolerance (default: 1e-9).
     * @param damping_factor Under-relaxation factor (default: 0.5).
     */
    Solver(int max_iter = 1000, double tolerance = 1e-9, double damping_factor = 0.5);
    
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
     * @brief Prints solver configuration and timing information.
     * @param os Output stream (default: std::cout).
     * 
     * Includes Gauss-Seidel parameters, convergence status,
     * iterations taken, and execution time.
     */
    virtual void print(std::ostream& os = std::cout) const override;
};

#endif