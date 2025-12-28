#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <unordered_map>
#include <vector>
#include "Include.h"

class Circuit : public I_Printable {
protected:
    std::unordered_map<std::string, Node*> nodes;
    std::unordered_map<std::string, Component*> components;
    std::string circuit_name;
    
    void add_node(std::string& nodeId);
    void add_resistor(std::string& resistorId, std::string& node1, std::string& node2, double resistance);
    void add_voltage_source(std::string& voltageSourceId, std::string& node1, std::string& node2, double voltage);
    void add_current_source(std::string& currentSourceId, std::string& node1, std::string& node2, double current);
    public:
    Circuit(std::string name="Circuit");
    void parse_netlist(const std::string& filename);
    virtual void print(std::ostream& os) const override;
    ~Circuit();
};

#endif