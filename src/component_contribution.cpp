#include "component_contribution.h"

MatrixContribution::MatrixContribution(std::string r, std::string c, double v) : row(r), col(c), value(v) {}

VectorContribution::VectorContribution(std::string r, double v) : row(r), value(v) {}

void Component_contribution::stampMatrix(std::string row, std::string col, double value) {
    matrixStamps.emplace_back(row, col, value);
}

void Component_contribution::stampVector(std::string row, double value) {
    vectorStamps.emplace_back(row, value);
}

void Component_contribution::print(std::ostream &os) const {
    os << "Matrix Contributions:\n";
    for (const auto& mc : matrixStamps) {
        os << "[" << mc.row << "][" << mc.col << "] = " << mc.value << "\n";
    }
    os << "Vector Contributions:\n";
    for (const auto& vc : vectorStamps) {
        os << "[" << vc.row << "] = " << vc.value << "\n";
    }
}