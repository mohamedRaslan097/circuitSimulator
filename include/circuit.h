#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <map>
#include <unordered_map>
#include <vector>
#include "Include.h"

class Circuit : public I_Printable {
protected:
    std::map<std::string, Node*> nodes;
    std::map<std::string, Component*> components;
    std::map<std::pair<std::string,std::string>, double> mna_matrix;
    std::map<std::string, double> mna_vector;
    std::string circuit_name;
    int extra_variables = 0;
    
    void add_node(std::string& nodeId);
    void add_resistor(std::string& resistorId, std::string& node1, std::string& node2, double resistance);
    void add_voltage_source(std::string& voltageSourceId, std::string& node1, std::string& node2, double voltage);
    void add_current_source(std::string& currentSourceId, std::string& node1, std::string& node2, double current);
public:
    Circuit(std::string name="Circuit");
    void parse_netlist(const std::string& filename);
    virtual void print(std::ostream& os = std::cout) const override;
    void assemble_MNA_system();
    void display_MNA_system(std::ostream& os = std::cout) const;
    ~Circuit();
};

#endif