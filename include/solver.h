#ifndef SOLVER_H
#define SOLVER_H

#include <unordered_map>
#include <string>
#include <vector>
#include <chrono>
#include "I_Printable.h"
#include "gauss_seidel.h"

class Solver : public I_Printable {
private:
    Gauss_seidel gauss_seidel;
    std::chrono::microseconds duration;
public:
    Solver(int max_iter = 1000, double tolerance = 1e-9, double damping_factor = 0.5);
    void solve_MNA_system(const std::unordered_map<int,std::unordered_map<int,double>>& mna_matrix,
                          const std::unordered_map<int, double>& mna_vector,
                          std::vector<double>& solution);
    
    virtual void print(std::ostream& os = std::cout) const override;
};

#endif