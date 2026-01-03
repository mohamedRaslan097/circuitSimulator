#ifndef COMPONENT_CONTRIBUTION_H
#define COMPONENT_CONTRIBUTION_H

#include <I_printable.h>
#include <vector>

struct MatrixContribution {
    int row;
    int col;
    double value;
    MatrixContribution(int r, int c, double v);
};

struct VectorContribution {
    int row;
    double value;
    VectorContribution(int r, double v);
};

class Component_contribution : public I_Printable {
public:
    std::vector<MatrixContribution> matrixStamps;
    std::vector<VectorContribution> vectorStamps;
    
    void stampMatrix(int row, int col, double value);
    void stampVector(int row, double value);
    virtual void print(std::ostream& os = std::cout) const override;
};


#endif