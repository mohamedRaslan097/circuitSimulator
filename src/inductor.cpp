#include "inductor.h"
#include "voltage_source.h"

Inductor::Inductor(const std::string& id, Node* ni, Node* nj, double l): Ac_component(id, ni, nj),vc_id(Node::node_count++), inductance(l), admittance(0), current(0.0) {}

double Inductor::get_voltage_drop(){
    return 0.0;
}

double Inductor::get_current(){
    return current;
}

Component_contribution<double> Inductor::get_contribution(){
    Component_contribution<double> contribution;
    if(ni->id != 0){
        contribution.stampMatrix(ni->id, vc_id, 1.0);
        contribution.stampMatrix(vc_id, ni->id, 1.0);
    }
    if(nj->id != 0){
        contribution.stampMatrix(nj->id, vc_id, -1.0);
        contribution.stampMatrix(vc_id, nj->id, -1.0);
    }
    return contribution;
}

Component_contribution<std::complex<double>> Inductor::get_ac_contribution(double frequency){
    Component_contribution<std::complex<double>> contribution;

    if (frequency == 0.0)
        return contribution;

    admittance = std::complex<double>(0, -1.0 / (2.0 * PI * frequency * inductance)) - admittance; // 1/jωL
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

void Inductor::print(std::ostream& os) const {
    double displayValue = inductance * 1e6;  // Convert H to uH
    os << std::left << std::setw(10) << "L(" + componentId + ")"
       << std::setw(6) << ni->name 
       << std::setw(6) << nj->name 
       << std::right << std::fixed << std::setprecision(4) << std::setw(12) << displayValue << " uH" << std::endl;
}