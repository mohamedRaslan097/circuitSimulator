/**
 * @file include.h
 * @brief Convenience header that includes all circuit simulator components.
 * 
 * This header provides a single include point for applications using
 * the circuit simulator library. It includes all necessary headers
 * in the correct dependency order.
 * 
 * **Included Components:**
 * - Timer: RAII-based performance measurement
 * - I_Printable: Base interface for printable objects
 * - Node: Circuit node representation
 * - Component_contribution: MNA stamping data structures
 * - Component: Base class for circuit elements
 * - Resistor, Voltage_source, Current_source: Circuit components
 * - Circuit: Main circuit container
 * - Solver: Linear system solver
 * - Simulator: Analysis orchestration
 * 
 * @note For more granular control, include individual headers instead.
 * 
 * @example
 * @code
 * #include "include.h"
 * 
 * int main() {
 *     Circuit circuit;
 *     circuit.parse_netlist("circuit.net");
 *     circuit.assemble_MNA_system();
 *     
 *     Simulator sim;
 *     sim.run_dc_analysis(circuit);
 *     
 *     circuit.print_solution();
 *     return 0;
 * }
 * @endcode
 */

#ifndef Include_H
#define Include_H

#include <iostream>
#include "timer.h"
#include "I_Printable.h"
#include "node.h"
#include "component_contribution.h"
#include "component.h"
#include "Resistor.h"
#include "voltage_source.h"
#include "current_source.h"
#include "circuit.h"
#include "solver.h"
#include "simulator.h"

#endif