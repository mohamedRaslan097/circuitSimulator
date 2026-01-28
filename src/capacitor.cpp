#include "capacitor.h"
#include <math.h>

Capacitor::Capacitor(const std::string& id, Node* ni, Node* nj, double c): Ac_component(id, ni, nj), capacitance(c), admittance(0) {}

double Capacitor::get_voltage_drop(){
    if(!Node::valid)
        throw std::runtime_error("Node voltages are not valid.");
    return ni->voltage - nj->voltage;
}

double Capacitor::get_current(){
    return 0.0;
}

Component_contribution<double> Capacitor::get_contribution(){
    return Component_contribution<double>();
}

Component_contribution<std::complex<double>> Capacitor::get_ac_contribution(double frequency){
    Component_contribution<std::complex<double>> contribution;

    if (frequency == 0.0)
        return contribution;

    admittance = std::complex<double>(0, 2.0 * PI * frequency * capacitance) - admittance; // jωC
    if(ni->id != 0)
        contribution.stampMatrix(ni->id, ni->id, admittance);
    if(nj->id != 0)
        contribution.stampMatrix(nj->id, nj->id, admittance);
    if (ni->id != 0 && nj->id != 0)
    {
        contribution.stampMatrix(ni->id, nj->id, -admittance);
        contribution.stampMatrix(nj->id, ni->id, -admittance);
    }
    return contribution;
}

void Capacitor::print(std::ostream& os) const {
    double displayValue = capacitance * 1e9;  // Convert F to nF
    os << std::left << std::setw(10) << "C(" + componentId + ")"
       << std::setw(6) << ni->name 
       << std::setw(6) << nj->name 
       << std::right << std::fixed << std::setprecision(4) << std::setw(12) << displayValue << " nF" << std::endl;
}