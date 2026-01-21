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
#include "Resistor.h"
#include "voltage_source.h"
#include "current_source.h"
#include "inductor.h"
#include "capacitor.h"

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
private:
    static constexpr const char* default_name = "Circuit";  // Default circuit name
    
protected:
    /// Map of node names to Node pointers
    std::unordered_map<std::string, Node*> nodes;
    
    /// Ordered map of node IDs to names (for iteration in order)
    std::map<int,std::string> nodeId_map;

    /// Map of component IDs to Component pointers
    std::unordered_map<std::string, Component*> components;
    
    /// Map of extra variable IDs to names (for voltage source/inductor currents)
    std::map<int,std::string> extraVarId_map;

    /// MNA system matrix A (sparse representation using nested maps)
    std::unordered_map<int, std::unordered_map<int, double>> mna_matrix;
    
    /// MNA excitation vector b (right-hand side)
    std::unordered_map<int, double> mna_vector;
    
    /// Human-readable name for the circuit
    std::string circuit_name;
    
    /**
     * @brief Adds a node to the circuit if it doesn't exist.
     * @param nodeId Node identifier string.
     */
    void add_node(std::string& nodeId);
    
    /**
     * @brief Adds a resistor to the circuit.
     * @param resistorId Unique resistor identifier.
     * @param node1 Positive terminal node name.
     * @param node2 Negative terminal node name.
     * @param resistance Resistance value in Ohms.
     * @throws std::runtime_error if resistance is non-positive or ID already exists.
     */
    void add_resistor(std::string& resistorId, std::string& node1, std::string& node2, double resistance);
    
    /**
     * @brief Adds a voltage source to the circuit.
     * @param voltageSourceId Unique voltage source identifier.
     * @param node1 Positive terminal node name.
     * @param node2 Negative terminal node name.
     * @param voltage Source voltage in Volts.
     * @throws std::runtime_error if ID already exists.
     */
    void add_voltage_source(std::string& voltageSourceId, std::string& node1, std::string& node2, double voltage);
    
    /**
     * @brief Adds a current source to the circuit.
     * @param currentSourceId Unique current source identifier.
     * @param node1 Positive terminal node name.
     * @param node2 Negative terminal node name.
     * @param current Source current in Amperes.
     * @throws std::runtime_error if ID already exists.
     */
    void add_current_source(std::string& currentSourceId, std::string& node1, std::string& node2, double current);
    
    /**
     * @brief Adds an inductor to the circuit.
     * @param inductorId Unique inductor identifier.
     * @param node1 Positive terminal node name.
     * @param node2 Negative terminal node name.
     * @param inductance Inductance value in Henries.
     * @throws std::runtime_error if ID already exists.
     */
    void add_inductor(std::string& inductorId, std::string& node1, std::string& node2, double inductance);
    
    /**
     * @brief Adds a capacitor to the circuit.
     * @param capacitorId Unique capacitor identifier.
     * @param node1 Positive terminal node name.
     * @param node2 Negative terminal node name.
     * @param capacitance Capacitance value in Farads.
     * @throws std::runtime_error if ID already exists.
     */
    void add_capacitor(std::string& capacitorId, std::string& node1, std::string& node2, double capacitance);
    
public:
    /**
     * @brief Constructs an empty circuit with optional name.
     * @param name Circuit name (default: "Circuit").
     * 
     * @note Automatically creates ground node "0" with id=0.
     */
    Circuit(std::string name=default_name);
    
    /**
     * @brief Parses a SPICE-like netlist file and populates the circuit.
     * @param filename Path to the netlist file.
     * @throws std::runtime_error if file cannot be opened or parsing fails.
     * 
     * Supported component types: R, V, I, L, C (case insensitive).
     * First line starting with '*' is treated as circuit name comment.
     * Comments start with '*' and are ignored.
     * 
     * @par Time Complexity
     * O(L × S) where L = number of lines, S = average line length
     * Each component/node addition is O(1) amortized (hash map insertion)
     * 
     * @par Space Complexity
     * O(N + C) where N = nodes created, C = components created
     */
    void parse_netlist(const std::string& filename);
    
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
};

#endif