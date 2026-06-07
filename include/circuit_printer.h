#ifndef CIRCUIT_PRINTER_H
#define CIRCUIT_PRINTER_H

#include <iostream>

class Circuit;

/**
 * @class CircuitPrinter
 * @brief Stateless utility class responsible for printing and formatting Circuit data.
 */
class CircuitPrinter {
public:
    /**
     * @brief Prints all node voltages of the circuit.
     * @param circuit The Circuit instance to print.
     * @param os Output stream.
     */
    static void print_nodes(const Circuit& circuit, std::ostream& os);

    /**
     * @brief Prints all components in the circuit.
     * @param circuit The Circuit instance to print.
     * @param os Output stream.
     */
    static void print_components(const Circuit& circuit, std::ostream& os);

    /**
     * @brief Prints the MNA system equations form of the circuit.
     * @param circuit The Circuit instance to print.
     * @param os Output stream.
     */
    static void print_MNA_system(const Circuit& circuit, std::ostream& os);

    /**
     * @brief Prints computed extra var branch currents.
     * @param circuit The Circuit instance to print.
     * @param os Output stream.
     */
    static void print_extraVars(const Circuit& circuit, std::ostream& os);

    /**
     * @brief Prints node and extra var final results.
     * @param circuit The Circuit instance to print.
     * @param os Output stream.
     */
    static void print_solution(const Circuit& circuit, std::ostream& os);

    /**
     * @brief Prints full summary of the circuit including components, matrices, results.
     * @param circuit The Circuit instance to print.
     * @param os Output stream.
     */
    static void print(const Circuit& circuit, std::ostream& os);
};

#endif // CIRCUIT_PRINTER_H
