#ifndef SOLVER_H
#define SOLVER_H

#include <map>
#include <string>
#include <vector>
#include <chrono>
#include "I_Printable.h"

class Solver : public I_Printable {
private:
    int max_iter;
    double tolerance;
    int converge_iters;
    bool converged;
    std::chrono::microseconds duration;
    void gauss_seidel_solver(const std::map<int,std::map<int,double>>& mna_matrix,
                             const std::map<int, double>& mna_vector,
                             std::vector<double>& solution);
public:
    Solver(int max_iter = 1000, double tolerance = 1e-9);
    void solve_MNA_system(const std::map<int,std::map<int,double>>& mna_matrix,
                          const std::map<int, double>& mna_vector,
                          std::vector<double>& solution);
    virtual void print(std::ostream& os = std::cout) const override;
};

#endif