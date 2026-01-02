#include "resistor.h"

int Resistor::id_counter = 0;

Resistor::Resistor( const std::string& id, Node* ni, Node* nj, double r) : Component(id, ni, nj), resistance(r) {}

Resistor::Resistor(Node* ni, Node* nj, double r) : Resistor(default_id+std::to_string(id_counter++), ni, nj, r) {}

double Resistor::get_voltage_drop(){
    if(!Node::valid)
        throw std::runtime_error("Node voltages are not valid.");
    return ni->voltage - nj->voltage;
}

double Resistor::get_current(){
    double voltage_drop = get_voltage_drop();
    return voltage_drop / resistance;
}

void Resistor::print(std::ostream& os) const {
    os << std::left << std::setw(10) << "R(" + componentId + ")"
       << std::setw(6) << ni->id 
       << std::setw(6) << nj->id 
       << std::right << std::fixed << std::setprecision(4) << std::setw(12) << resistance << " Ohms" << std::endl;
}

Component_contribution Resistor::get_contribution(){
    Component_contribution contribution;
    double conductance = 1.0 / resistance;
    
    contribution.stampMatrix(ni->id, ni->id, conductance);
    if (ni->id != "0" && nj->id != "0")
    {
        contribution.stampMatrix(nj->id, nj->id, conductance);
        contribution.stampMatrix(ni->id, nj->id, -conductance);
        contribution.stampMatrix(nj->id, ni->id, -conductance);
    }
    return contribution;
}