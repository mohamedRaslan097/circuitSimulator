#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "solver.h"
#include "circuit.h"

class Simulator : public I_Printable {
private:
    Solver solver;
    std::vector<double> solution;
public:
    void run_dc_analysis(Circuit& circuit);
    virtual void print(std::ostream& os = std::cout) const override;
};

#endif