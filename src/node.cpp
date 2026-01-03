#include "node.h"

bool Node::valid = false;
int Node::node_count = 0;

Node::Node(std::string nodeName) : name(nodeName),id(node_count++), voltage(0.0){}

bool Node::operator==(const Node& other) const {return name == other.name;}

bool Node::operator<(const Node& other) const {return name < other.name;}

void Node::print(std::ostream& os) const {
    os << std::left << std::setw(10) << "Node(" + name + ")"
       << std::right << std::fixed << std::setprecision(6) << std::setw(14) << voltage << " V";
}
