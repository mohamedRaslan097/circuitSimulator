#ifndef COMPONENT_CONTRIBUTION_H
#define COMPONENT_CONTRIBUTION_H

#include <I_printable.h>
#include <vector>

struct MatrixContribution {
    std::string row;
    std::string col;
    double value;
    MatrixContribution(std::string r, std::string c, double v);
};

struct VectorContribution {
    std::string row;
    double value;
    VectorContribution(std::string r, double v);
};

class Component_contribution : public I_Printable {
public:
    std::vector<MatrixContribution> matrixStamps;
    std::vector<VectorContribution> vectorStamps;
    
    void stampMatrix(std::string row, std::string col, double value);
    void stampVector(std::string row, double value);
    virtual void print(std::ostream& os = std::cout) const override;
};


#endif