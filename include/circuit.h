#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <unordered_map>
#include <map>
#include "Resistor.h"
#include "voltage_source.h"
#include "current_source.h"
#include "inductor.h"
#include "capacitor.h"

class Circuit : public I_Printable {
private:
    static constexpr const char* default_name = "Circuit";
protected:
    std::unordered_map<std::string, Node*> nodes;
    std::map<int,std::string> nodeId_map;

    std::unordered_map<std::string, Component*> components;
    std::map<int,std::string> extraVarId_map;

    std::unordered_map<int, std::unordered_map<int, double>> mna_matrix;
    std::unordered_map<int, double> mna_vector;
    
    std::string circuit_name;
    
    void add_node(std::string& nodeId);
    void add_resistor(std::string& resistorId, std::string& node1, std::string& node2, double resistance);
    void add_voltage_source(std::string& voltageSourceId, std::string& node1, std::string& node2, double voltage);
    void add_current_source(std::string& currentSourceId, std::string& node1, std::string& node2, double current);
    void add_inductor(std::string& inductorId, std::string& node1, std::string& node2, double inductance);
    void add_capacitor(std::string& capacitorId, std::string& node1, std::string& node2, double capacitance);
public:
    Circuit(std::string name=default_name);
    
    void parse_netlist(const std::string& filename);
    void assemble_MNA_system();
    void deploy_dc_solution(const std::vector<double>& solution);
    
    const std::unordered_map<int, std::unordered_map<int, double>>& get_MNA_matrix() const { return mna_matrix; }
    const std::unordered_map<int, double>& get_MNA_vector() const { return mna_vector; }
    const std::unordered_map<std::string, Node*>& get_nodes() const { return nodes; }
    
    void print_nodes(std::ostream& os = std::cout) const;
    void print_components(std::ostream& os = std::cout) const;
    void print_MNA_system(std::ostream& os = std::cout) const;
    void print_extraVars(std::ostream& os = std::cout) const;
    void print_solution(std::ostream& os = std::cout) const;
    virtual void print(std::ostream& os = std::cout) const override;

    ~Circuit();
};

#endif