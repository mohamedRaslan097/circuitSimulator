/**
 * @file circuit.h
 * @brief Main circuit container class for the circuit simulator.
 * 
 * The Circuit class manages all nodes and components, parses netlists,
 * assembles the Modified Nodal Analysis (MNA) system, and applies
 * solutions to compute node voltages and branch currents.
 */

#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <unordered_map>
#include <map>
#include <sstream>
#include "component.h"
#include "component_descriptor.h"

/**
 * @class Circuit
 * @brief Container class that manages the complete circuit topology and analysis.
 * 
 * The Circuit class is the central data structure of the simulator. It:
 * - Stores all nodes and components
 * - Parses SPICE-like netlist files
 * - Assembles the Modified Nodal Analysis (MNA) system of equations
 * - Deploys solutions back to nodes and components
 * 
 * **Modified Nodal Analysis (MNA):**
 * The MNA method formulates the circuit as a linear system Ax = b, where:
 * - A is the system matrix (conductances and source coefficients)
 * - x is the solution vector (node voltages and source currents)
 * - b is the excitation vector (source values)
 * 
 * **Netlist Format:**
 * The parser supports SPICE-like netlist format:
 * ```
 * * Circuit name (optional comment on first line)
 * R1 N1 N2 1000      ; Resistor: ID node+ node- value(Ω)
 * V1 N1 0 5          ; Voltage source: ID node+ node- value(V)
 * I1 N2 0 0.001      ; Current source: ID node+ node- value(A)
 * L1 N1 N2 0.001     ; Inductor: ID node+ node- value(H)
 * C1 N1 N2 1e-6      ; Capacitor: ID node+ node- value(F)
 * ```
 * 
 * @note Node "0" is always the ground reference with voltage = 0V.
 * 
 * @see Simulator, Solver, Component
 * 
 * @example
 * @code
 * Circuit circuit("My Circuit");
 * circuit.parse_netlist("circuit.net");
 * circuit.assemble_MNA_system();
 * // Use Simulator to solve and deploy solution
 * circuit.print_solution();
 * @endcode
 */
class Circuit : public I_Printable {
    // friend void Ac_analyzer::log_ac_inst_solution(double frequency, std::chrono::microseconds duration, int converge_iters);
private:
    static constexpr const char* default_name = "Circuit";  // Default circuit name
    
protected:
    // Map of node names to Node pointers
    std::unordered_map<std::string, Node*> nodes;
    
    // Ordered map of node IDs to names (for iteration in order)
    std::map<int,std::string> nodeId_map;

    // Map of component IDs to Component pointers
    std::unordered_map<std::string, Component*> components;
    
    // Map of ac component IDs to Component pointers
    std::unordered_map<std::string, Component*> ac_components;

    // Map of extra variable IDs to names (for voltage source/inductor currents)
    std::map<int,std::string> extraVarId_map;

    // MNA system matrix A (sparse representation using nested maps)
    std::unordered_map<int, std::unordered_map<int, double>> mna_matrix;
    
    // MNA excitation vector b (right-hand side)
    std::unordered_map<int, double> mna_vector;
    
    // Human-readable name for the circuit
    std::string circuit_name;
    
    /**
     * @brief Adds a node to the circuit if it doesn't exist.
     * @param nodeId Node identifier string.
     */
    void add_node(std::string& nodeId);

    /**
     * @brief Adds a component to the circuit based on the descriptor.
     * @param descriptor Parsed component descriptor containing type, nodes, and values.
     * @throws std::runtime_error if the descriptor is invalid or contains unknown types.
     */
    void add_component(const ComponentDescriptor& descriptor);
    
public:
    /**
     * @brief Constructs an empty circuit with optional name.
     * @param name Circuit name (default: "Circuit").
     * 
     * @note Automatically creates ground node "0" with id=0.
     */
    Circuit(std::string name=default_name);
    
    /**
     * @brief Assembles the MNA system matrix and vector.
     * 
     * Iterates through all components and collects their contributions
     * to build the complete system of equations.
     * 
     * @par Time Complexity
     * O(C × S) where:
     * - C = number of components
     * - S = stamps per component (constant, typically 4 for resistors, 5 for voltage sources)
     * 
     * Overall: O(C) since S is bounded by a small constant
     * 
     * @par Space Complexity
     * O(NNZ) where NNZ = total non-zero entries in MNA matrix
     * Typically NNZ = O(N + C) for circuit matrices
     */
    void assemble_MNA_system();
    
    /**
     * @brief Applies the solution vector to nodes and components.
     * @param solution Vector of solved values (voltages and currents).
     * 
     * Updates node voltages and source/inductor currents from the
     * solution vector. Sets Node::valid = true after deployment.
     */
    void deploy_dc_solution(const std::vector<double>& solution);
    
    /**
     * @brief Gets the MNA system matrix.
     * @return Const reference to the sparse matrix representation.
     */
    const std::unordered_map<int, std::unordered_map<int, double>>& get_MNA_matrix() const { return mna_matrix; }
    
    /**
     * @brief Gets the MNA excitation vector.
     * @return Const reference to the RHS vector.
     */
    const std::unordered_map<int, double>& get_MNA_vector() const { return mna_vector; }
    
    /**
     * @brief Gets all circuit nodes.
     * @return Const reference to the nodes map.
     */
    const std::unordered_map<std::string, Node*>& get_nodes() const { return nodes; }

    /**
     * @brief Gets the node ID to name map.
     * @return Const reference to the node ID to name map.
     */
    const std::map<int, std::string>& get_nodeId_map() const { return nodeId_map; }
    
    /**
     * @brief Gets the extra variable ID to name map.
     * @return Const reference to the extra variable map.
     */
    const std::map<int, std::string>& get_extraVarId_map() const { return extraVarId_map; }

    /**
     * @brief Gets ac components.
     * @return Const reference to the components map.
     */
    const std::unordered_map<std::string, Component*>& get_ac_components() const { return ac_components; }
    
    /**
     * @brief Prints all node voltages.
     * @param os Output stream (default: std::cout).
     */
    void print_nodes(std::ostream& os = std::cout) const;
    
    /**
     * @brief Prints all component information.
     * @param os Output stream (default: std::cout).
     */
    void print_components(std::ostream& os = std::cout) const;
    
    /**
     * @brief Prints the MNA system matrix and vector.
     * @param os Output stream (default: std::cout).
     */
    void print_MNA_system(std::ostream& os = std::cout) const;
    
    /**
     * @brief Prints extra variables (source/inductor currents).
     * @param os Output stream (default: std::cout).
     */
    void print_extraVars(std::ostream& os = std::cout) const;
    
    /**
     * @brief Prints the complete DC solution (nodes and components).
     * @param os Output stream (default: std::cout).
     */
    void print_solution(std::ostream& os = std::cout) const;
    
    /**
     * @brief Prints circuit summary including name, nodes, and components.
     * @param os Output stream (default: std::cout).
     */
    virtual void print(std::ostream& os = std::cout) const override;

    /**
     * @brief Destructor - frees all allocated nodes and components.
     */
    ~Circuit();

    friend class CircuitBuilder;
    friend class CircuitPrinter;
};

#endif