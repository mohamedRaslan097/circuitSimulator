#include "component.h"

Component::Component(const std::string& id, Node* node_i, Node* node_j) : ni(node_i), nj(node_j), componentId(id) {}

Component::Component(const Component& src) : ni(src.ni), nj(src.nj), componentId(src.componentId) {}

Component::Component(Component&& src) : ni(src.ni), nj(src.nj), componentId(src.componentId) {src.ni = src.nj = nullptr;}

Component::~Component(){ni = nj = nullptr;}
