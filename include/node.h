/**
 * @file node.h
 * @brief Node class representing electrical nodes in the circuit.
 * 
 * Nodes are connection points in the circuit where two or more
 * components meet. Each node has a unique identifier and stores
 * the computed voltage after circuit analysis.
 */

#ifndef NODE_H
#define NODE_H

#include <string>

#include "I_printable.h"

/**
 * @class Node
 * @brief Represents an electrical node (junction) in the circuit.
 * 
 * A node is a point in the circuit where components connect. The circuit
 * simulator uses Modified Nodal Analysis (MNA), where node voltages are
 * the primary unknowns. Node "0" is conventionally the ground reference.
 * 
 * @note The ground node (name="0") always has id=0 and voltage=0V.
 * @note Node IDs are automatically assigned incrementally via node_count.
 * 
 * @example
 * @code
 * Node ground("0");      // Ground node, id=0
 * Node vcc("VCC");       // Power supply node, id=1
 * Node output("OUT");    // Output node, id=2
 * @endcode
 */
class Node : public I_Printable {
public:
    std::string name;       // Human-readable node identifier (e.g., "0", "N1", "VCC")
    int id;                 // Unique numeric ID used in MNA matrix indexing
    double voltage;         // Computed node voltage in Volts (valid after DC analysis)
    
    static int node_count;  // Global counter for automatic ID assignment
    static bool valid;      // Flag indicating if node voltages have been computed

    /**
     * @brief Constructs a new Node with the given identifier.
     * @param nodeId String identifier for the node (e.g., "0", "N1", "VCC").
     * 
     * Automatically assigns a unique numeric ID based on node_count.
     * Initial voltage is set to 0.0V.
     */
    Node(std::string nodeId);
    
    /**
     * @brief Equality comparison based on node name.
     * @param other The node to compare with.
     * @return true if both nodes have the same name.
     */
    bool operator==(const Node& other) const;
    
    /**
     * @brief Less-than comparison for ordering nodes.
     * @param other The node to compare with.
     * @return true if this node's name is lexicographically less than other's.
     */
    bool operator<(const Node& other) const;
    
    /**
     * @brief Prints node information in formatted output.
     * @param os Output stream (default: std::cout).
     * 
     * Output format: "Node(name)    voltage V"
     */
    virtual void print(std::ostream& os = std::cout) const override;
};

#endif