#ifndef NODE_H
#define NODE_H

#include <string>

#include "I_printable.h"

class Node : public I_Printable {
public:
    std::string id;
    double voltage;
    static bool valid;

    Node(std::string nodeId);
    bool operator==(const Node& other) const;
    bool operator<(const Node& other) const;
    virtual void print(std::ostream& os) const override;
};

#endif