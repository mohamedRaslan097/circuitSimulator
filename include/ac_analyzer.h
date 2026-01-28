/**
 * @file ac_analyzer.h
 * @brief AC frequency-domain analysis handler for the circuit simulator.
 * 
 * Manages complex-valued MNA system assembly and frequency sweep analysis.
 * Converts DC MNA matrices to complex representation and applies AC component
 * contributions (capacitor/inductor admittances, AC voltage sources).
 */

#ifndef AC_ANALYZER_H
#define AC_ANALYZER_H

#include <unordered_map>
#include <map>
#include <complex>
#include <fstream>
#include "I_Printable.h"
#include "component.h"
#include "inductor.h"
#include <chrono>

/**
 * @class Ac_analyzer
 * @brief Handles AC frequency-domain analysis for the circuit simulator.
 * 
 * The Ac_analyzer class manages:
 * - Conversion of real-valued DC MNA system to complex-valued AC system
 * - Assembly of frequency-dependent component contributions (C, L)
 * - Logging of AC solutions for frequency sweep analysis
 * 
 * **AC Analysis Workflow:**
 * ```cpp
 * Ac_analyzer analyzer("ac_results.txt");
 * analyzer.initialize(dc_mna_matrix, extra_vars);  // Convert to complex
 * 
 * for (double freq = f1; freq <= f2; freq += step) {
 *     analyzer.assemble_ac_mna_system(ac_components, freq);
 *     solver.solve(analyzer.mna_matrix, analyzer.mna_vector, analyzer.solution);
 *     analyzer.log_ac_inst_solution(freq, duration);
 * }
 * ```
 * 
 * **Component AC Contributions:**
 * - Capacitor: Admittance Y = jωC
 * - Inductor: Admittance Y = 1/(jωL)
 * - Voltage Source: Phasor voltage stamping
 * 
 * @note Resistors contribute the same in AC as DC (purely real conductance).
 * 
 * @see Solver, Gauss_seidel, Ac_component
 */
class Ac_analyzer : public I_Printable {
    friend class Solver;
private:
    std::string output_file;    // Path to output results file
    
    // Complex MNA system matrix A (sparse representation using nested maps)
    std::unordered_map<int, std::unordered_map<int, std::complex<double>>> mna_matrix;
    
    // MNA excitation vector b (right-hand side)
    std::unordered_map<int, std::complex<double>> mna_vector;

    // Solution vector x (complex voltages and currents)
    std::vector<std::complex<double>> solution;
    
    void log_header() const;
public:
    /**
     * @brief Constructs an AC analyzer with specified output file.
     * @param output_file Path to write AC analysis results (default: "ac_analysis_results.csv").
     * 
     * Results are logged in CSV format: frequency, Re(V1), Im(V1), Re(V2), Im(V2), ..., duration_us
     */
    Ac_analyzer(const std::string& output_file = "ac_analysis_results.csv");

    /**
     * @brief Initializes the complex MNA system from DC analysis base.
     * @param mna_matrix Sparse DC system matrix A (real-valued).
     * @param extra_vars Map of extra variable IDs to names (voltage source/inductor currents).
     * 
     * Converts the real-valued DC MNA matrix to complex representation,
     * filtering out rows/columns corresponding to extra variables (which are
     * handled differently in AC analysis - inductors become admittances).
     * 
     * @par Time Complexity
     * O(NNZ) where NNZ = non-zero entries in DC MNA matrix
     * 
     * @par Space Complexity
     * O(NNZ) for complex matrix storage (2× real storage)
     */
    void initialize(const std::unordered_map<int,std::unordered_map<int,double>>& mna_matrix,
                    const std::map<int, std::string>& extra_vars,
                    const std::vector<double>& initial_solution);
    
    /**
     * @brief Assembles AC component contributions at a specific frequency.
     * @param ac_components Map of AC-capable components (capacitors, inductors, AC sources).
     * @param frequency Analysis frequency in Hertz (Hz).
     * 
     * Iterates through all AC components and stamps their frequency-dependent
     * contributions to the complex MNA matrix:
     * - Capacitors: Admittance jωC where ω = 2πf
     * - Inductors: Admittance 1/(jωL)
     * - Voltage Sources: Phasor voltage if AC amplitude specified
     * 
     * @note This method should be called for each frequency point in a sweep.
     * 
     * @par Time Complexity
     * O(A × S) where A = AC components, S = stamps per component (≤4)
     */
    void assemble_ac_mna_system(const std::unordered_map<std::string, Component*>& ac_components, double frequency);
    
    /**
     * @brief Logs the AC solution for a single frequency point to file.
     * @param frequency The analysis frequency in Hertz.
     * @param duration Time taken to solve at this frequency (default: 0).
     * @param converge_iters Number of iterations taken to converge (default: 0).
     * 
     * Writes a CSV line containing: frequency, complex solution values, solve time.
     * Format: freq, Re(x[0]), Im(x[0]), Re(x[1]), Im(x[1]), ..., duration_us
     * 
     * @throws std::runtime_error if output file cannot be opened.
     */
    void log_ac_inst_solution(double frequency, std::chrono::microseconds duration = std::chrono::microseconds(0), int converge_iters = 0);

    /**
     * @brief Prints AC analyzer information.
     * @param os Output stream (default: std::cout).
     */
    void print(std::ostream& os = std::cout) const override;
};

#endif