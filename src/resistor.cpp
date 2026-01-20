#include "resistor.h"

Resistor::Resistor( const std::string& id, Node* ni, Node* nj, double r) : Component(id, ni, nj), resistance(r) {}

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
    double displayValue = resistance / 1000.0;  // Convert Ohms to kOhms
    os << std::left << std::setw(10) << "R(" + componentId + ")"
       << std::setw(6) << ni->name 
       << std::setw(6) << nj->name 
       << std::right << std::fixed << std::setprecision(4) << std::setw(12) << displayValue << " kΩ" << std::endl;
}

Component_contribution Resistor::get_contribution(){
    Component_contribution contribution;
    double conductance = 1.0 / resistance;
    if(ni->id != 0)
        contribution.stampMatrix(ni->id, ni->id, conductance);
    if(nj->id != 0)
        contribution.stampMatrix(nj->id, nj->id, conductance);
    if (ni->id != 0 && nj->id != 0)
    {
        contribution.stampMatrix(ni->id, nj->id, -conductance);
        contribution.stampMatrix(nj->id, ni->id, -conductance);
    }
    return contribution;
}