#include "capacitor.h"

Capacitor::Capacitor(const std::string& id, Node* ni, Node* nj, double c): Component(id, ni, nj), capacitance(c) {}

double Capacitor::get_voltage_drop(){
    if(!Node::valid)
        throw std::runtime_error("Node voltages are not valid.");
    return ni->voltage - nj->voltage;
}

double Capacitor::get_current(){
    return 0.0;
}

Component_contribution Capacitor::get_contribution(){
    return Component_contribution();
}

void Capacitor::print(std::ostream& os) const {
    double displayValue = capacitance * 1e9;  // Convert F to nF
    os << std::left << std::setw(10) << "C(" + componentId + ")"
       << std::setw(6) << ni->name 
       << std::setw(6) << nj->name 
       << std::right << std::fixed << std::setprecision(4) << std::setw(12) << displayValue << " nF" << std::endl;
}