#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H

#include "component_descriptor.h"
#include "component.h"

/**
 * @class ComponentFactory
 * @brief Static factory class for creating Component instances from ComponentDescriptor.
 * This class encapsulates the logic of mapping parsed netlist descriptors to actual
 * Component subclasses (Resistor, Voltage_source, etc.) and handles error checking.
 */
class ComponentFactory {
    /**
     * @brief creates a resistor to the circuit.
     * @param resistorId Unique resistor identifier.
     * @param node1 Positive terminal node name.
     * @param node2 Negative terminal node name.
     * @param resistance Resistance value in Ohms.
     * @throws std::runtime_error if resistance is non-positive or ID already exists.
     */
    Component* create_resistor(std::string resistorId, Node* node1, Node* node2, double resistance);
    
    /**
     * @brief creates a voltage source to the circuit.
     * @param voltageSourceId Unique voltage source identifier.
     * @param node1 Positive terminal node name.
     * @param node2 Negative terminal node name.
     * @param dc_voltage DC source voltage in Volts.
     * @param ac_voltage AC source voltage in Volts (for AC analysis).
     * @throws std::runtime_error if ID already exists.
     */
    Component* create_voltage_source(std::string voltageSourceId, Node* node1, Node* node2, double dc_voltage, double ac_voltage = 0);
    
    /**
     * @brief creates a current source to the circuit.
     * @param currentSourceId Unique current source identifier.
     * @param node1 Positive terminal node name.
     * @param node2 Negative terminal node name.
     * @param current Source current in Amperes.
     * @throws std::runtime_error if ID already exists.
     */
    Component* create_current_source(std::string currentSourceId, Node* node1, Node* node2, double current);
    
    /**
     * @brief creates an inductor to the circuit.
     * @param inductorId Unique inductor identifier.
     * @param node1 Positive terminal node name.
     * @param node2 Negative terminal node name.
     * @param inductance Inductance value in Henries.
     * @throws std::runtime_error if ID already exists.
     */
    Component* create_inductor(std::string inductorId, Node* node1, Node* node2, double inductance);
    
    /**
     * @brief creates a capacitor to the circuit.
     * @param capacitorId Unique capacitor identifier.
     * @param node1 Positive terminal node name.
     * @param node2 Negative terminal node name.
     * @param capacitance Capacitance value in Farads.
     * @throws std::runtime_error if ID already exists.
     */
    Component* create_capacitor(std::string capacitorId, Node* node1, Node* node2, double capacitance);
    
public:
    /**
     * @brief Creates a Component instance based on the provided descriptor.
     * @param descriptor The parsed component descriptor containing type, nodes, and values.
     * @return A pointer to the created Component instance.
     * @throws std::runtime_error if the descriptor is invalid or contains unknown types.
     */
    Component* create_component(const ComponentDescriptor& descriptor);
};



#endif // COMPONENT_FACTORY_H