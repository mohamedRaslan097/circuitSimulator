/**
 * @file ui.h
 * @brief User interface class for command-line argument parsing and output.
 * 
 * Handles command-line argument parsing, help display, and result output
 * for the circuit simulator application.
 */

#ifndef UI_H
#define UI_H

#include <iostream>
#include <fstream>
#include <string>

/**
 * @class UI
 * @brief Manages user interface operations for the circuit simulator.
 * 
 * The UI class handles:
 * - Command-line argument parsing
 * - Help and usage information display
 * - Result output to files and console
 * - Application banner display
 * 
 * **Supported Command-Line Options:**
 * - `-i <file>`: Input netlist file (required)
 * - `-o <file>`: Output results file (default: output.log)
 * - `-v`: Verbose mode (print results to console)
 * - `-p`: Pause before exit (wait for Enter key)
 * - `-h`: Show help message
 * 
 * **Example Usage:**
 * ```
 * circuit_simulator -i circuit.net -o results.txt -v
 * ```
 * 
 * @example
 * @code
 * int main(int argc, char* argv[]) {
 *     UI ui;
 *     if (!ui.parse_arguments(argc, argv)) {
 *         return 1;  // Invalid arguments or help requested
 *     }
 *     
 *     Circuit circuit;
 *     circuit.parse_netlist(ui.get_input_file());
 *     // ... run analysis ...
 *     
 *     std::ostringstream oss;
 *     circuit.print_solution(oss);
 *     ui.output_results(oss.str());
 *     
 *     return 0;
 * }
 * @endcode
 */
class UI {
private:
    std::string input_file;     // Path to input netlist file
    std::string output_file;    // Path to output results file
    bool verbose;               // Whether to print results to console
    bool pause;                 // Whether to pause before exit
    std::string program_name;   // Name of the executable (from argv[0])
    
    /**
     * @brief Prints usage information to stdout.
     * 
     * Displays all available command-line options and their descriptions.
     */
    void print_usage() const;
    
    /**
     * @brief Prints the application banner/header.
     * @param os Output stream (default: std::cout).
     * 
     * Displays program name, version, and decorative border.
     */
    void print_banner(std::ostream& os = std::cout) const;
    
public:
    /**
     * @brief Constructs UI with default settings.
     * 
     * Default values:
     * - output_file: "output.log"
     * - verbose: false
     * - pause: false
     */
    UI();
    
    /**
     * @brief Parses command-line arguments.
     * @param argc Argument count from main().
     * @param argv Argument vector from main().
     * @return true if parsing successful and program should continue.
     * @return false if help was requested or error occurred.
     * 
     * On error, prints error message and usage information.
     */
    bool parse_arguments(int argc, char* argv[]);
    
    /**
     * @brief Writes analysis results to output file.
     * @param circuit_output String containing the formatted results.
     * 
     * Writes banner and results to the output file.
     * If verbose mode is enabled, also prints to console.
     */
    void output_results(const std::string& circuit_output);
    
    /**
     * @brief Gets the input file path.
     * @return Const reference to the input file path string.
     */
    const std::string& get_input_file() const { return input_file; }
    
    /**
     * @brief Destructor - handles pause-before-exit if enabled.
     * 
     * If pause mode is enabled, waits for user to press Enter.
     */
    ~UI();
};

#endif // UI_H
