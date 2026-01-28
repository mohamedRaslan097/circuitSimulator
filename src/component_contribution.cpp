#include "component_contribution.h"
#include <complex>

template<typename T>
MatrixContribution<T>::MatrixContribution(int r, int c, T v) : row(r), col(c), value(v) {}

template<typename T>
VectorContribution<T>::VectorContribution(int r, T v) : row(r), value(v) {}

template<typename T>
void Component_contribution<T>::stampMatrix(int row, int col, T value) {
    matrixStamps.emplace_back(row, col, value);
}

template<typename T>
void Component_contribution<T>::stampVector(int row, T value) {
    vectorStamps.emplace_back(row, value);
}

template<typename T>
void Component_contribution<T>::print(std::ostream &os) const {
    os << "Matrix Contributions:\n";
    for (const auto& mc : matrixStamps) {
        os << "[" << mc.row << "][" << mc.col << "] = " << mc.value << "\n";
    }
    os << "Vector Contributions:\n";
    for (const auto& vc : vectorStamps) {
        os << "[" << vc.row << "] = " << vc.value << "\n";
    }
}

// Explicit template instantiations
template struct MatrixContribution<double>;
template struct MatrixContribution<std::complex<double>>;
template struct VectorContribution<double>;
template struct VectorContribution<std::complex<double>>;
template class Component_contribution<double>;
template class Component_contribution<std::complex<double>>;