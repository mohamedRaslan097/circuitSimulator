#include "node.h"

bool Node::valid = false;

Node::Node(std::string nodeId) : id(nodeId), voltage(0.0){}

bool Node::operator==(const Node& other) const {return id == other.id;}

bool Node::operator<(const Node& other) const {return id < other.id;}

void Node::print(std::ostream& os) const {
    os << std::left << std::setw(10) << "Node(" + id + ")"
       << std::right << std::fixed << std::setprecision(6) << std::setw(14) << voltage << " V";
}
