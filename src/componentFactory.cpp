#include "ComponentFactory.h"
#include "component_descriptor.h"
#include "resistor.h"
#include "current_source.h"
#include "voltage_source.h"
#include "capacitor.h"
#include "inductor.h"

Component* ComponentFactory::create_voltage_source(std::string voltageSourceId, Node* node1, Node* node2, double dc_voltage, double ac_voltage) {
    Component* voltageSource = new Voltage_source(voltageSourceId, node1, node2, dc_voltage, ac_voltage);
    return voltageSource;

}

Component* ComponentFactory::create_current_source(std::string currentSourceId, Node* node1, Node* node2, double current) {
    Component* currentSource = new Current_source(currentSourceId, node1, node2, current);
    return currentSource;
}

Component* ComponentFactory::create_resistor(std::string resistorId, Node* node1, Node* node2, double resistance) {
    if (resistance <= 0)
        throw std::runtime_error("Resistor with ID " + resistorId + " has non-positive resistance.");

    Component* resistor = new Resistor(resistorId, node1, node2, resistance);
    return resistor;
}

Component* ComponentFactory::create_inductor(std::string inductorId, Node* node1, Node* node2, double inductance) {
    if(inductance <= 0)
        throw std::runtime_error("Inductor with ID " + inductorId + " has non-positive inductance.");
    
    Component* inductor = new Inductor(inductorId, node1, node2, inductance);
    return inductor;
}

Component* ComponentFactory::create_capacitor(std::string capacitorId, Node* node1, Node* node2, double capacitance) {
    if(capacitance <= 0)
        throw std::runtime_error("Capacitor with ID " + capacitorId + " has non-positive capacitance.");
    
    Component* capacitor = new Capacitor(capacitorId, node1, node2, capacitance);
    return capacitor;
}
Component* ComponentFactory::create_component(const ComponentDescriptor& descriptor) {
    if(descriptor.is_directive) {
        throw std::runtime_error("Directives are not components and cannot be created by ComponentFactory.");
    }

    switch(descriptor.type) {
        case 'V': {
            double dc_voltage = 0;
            double ac_voltage = 0;
            if (descriptor.keyed.find("DC") == descriptor.keyed.end())
                dc_voltage = descriptor.positional.empty() ? 0 : descriptor.positional[0];
            else
                dc_voltage = descriptor.keyed.at("DC");
            
            if (descriptor.keyed.find("AC") != descriptor.keyed.end())
                ac_voltage = descriptor.keyed.at("AC");

            return create_voltage_source(descriptor.id, descriptor.ni, descriptor.nj, dc_voltage, ac_voltage);
        }

        case 'I':
            if (descriptor.positional.size() < 1)
                throw std::runtime_error("Current source " + descriptor.id + " is missing current value.");
            return create_current_source(descriptor.id, descriptor.ni, descriptor.nj, descriptor.positional[0]);
            
        case 'R':
            if (descriptor.positional.size() < 1)
                throw std::runtime_error("Resistor " + descriptor.id + " is missing resistance value.");
            return create_resistor(descriptor.id, descriptor.ni, descriptor.nj, descriptor.positional[0]);
        
        case 'L':
            if (descriptor.positional.size() < 1)
                throw std::runtime_error("Inductor " + descriptor.id + " is missing inductance value.");
            return create_inductor(descriptor.id, descriptor.ni, descriptor.nj, descriptor.positional[0]);
        
        case 'C':
            if (descriptor.positional.size() < 1)
                throw std::runtime_error("Capacitor " + descriptor.id + " is missing capacitance value.");
            return create_capacitor(descriptor.id, descriptor.ni, descriptor.nj, descriptor.positional[0]);
        
        default:
            throw std::runtime_error("Unknown component type '" + std::string(1, descriptor.type) + "' for component ID " + descriptor.id);
    }
}