#include "inductor.h"
#include "voltage_source.h"

Inductor::Inductor(const std::string& id, Node* ni, Node* nj, double l): Component(id, ni, nj),vc_id(Node::node_count++), inductance(l), current(0.0) {}

double Inductor::get_voltage_drop(){
    return 0.0;
}

double Inductor::get_current(){
    return current;
}

Component_contribution Inductor::get_contribution(){
    Component_contribution contribution;
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

void Inductor::print(std::ostream& os) const {
    double displayValue = inductance * 1e6;  // Convert H to uH
    os << std::left << std::setw(10) << "L(" + componentId + ")"
       << std::setw(6) << ni->name 
       << std::setw(6) << nj->name 
       << std::right << std::fixed << std::setprecision(4) << std::setw(12) << displayValue << " uH" << std::endl;
}