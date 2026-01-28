# Circuit Simulator - Time and Space Complexity Analysis

This document provides a comprehensive analysis of time and space complexity for all major classes and functions in the circuit simulator.

## Table of Contents

- [Notation](#notation)
- [Data Structures](#data-structures)
- [Core Classes](#core-classes)
  - [Circuit](#circuit)
  - [Simulator](#simulator)
  - [Solver](#solver)
  - [Gauss_seidel](#gauss_seidel)
  - [Ac_analyzer](#ac_analyzer)
- [Component Classes](#component-classes)
- [Helper Classes](#helper-classes)
- [Overall Analysis Pipeline](#overall-analysis-pipeline)

---

## Notation

| Symbol | Description |
|--------|-------------|
| **N** | Number of nodes in the circuit |
| **C** | Number of components (R, V, I, L, C) |
| **V** | Number of voltage sources + inductors (extra MNA variables) |
| **M** | System dimension = N + V (total MNA variables) |
| **NNZ** | Number of non-zero entries in MNA matrix |
| **K** | Average non-zeros per row (typically ≤ 5 for circuits) |
| **I** | Number of iterations until convergence |
| **F** | Number of frequency points in AC sweep |
| **L** | Number of lines in netlist file |
| **S** | Average string/line length |

---

## Data Structures

### Circuit Storage

| Structure | Type | Space Complexity | Description |
|-----------|------|------------------|-------------|
| `nodes` | `unordered_map<string, Node*>` | O(N) | Node name → Node pointer |
| `nodeId_map` | `map<int, string>` | O(N) | Node ID → Node name (ordered) |
| `components` | `unordered_map<string, Component*>` | O(C) | Component ID → Component pointer |
| `extraVarId_map` | `map<int, string>` | O(V) | Extra variable ID → name |
| `mna_matrix` | `unordered_map<int, unordered_map<int, double>>` | O(NNZ) | Sparse MNA matrix |
| `mna_vector` | `unordered_map<int, double>` | O(M) | RHS excitation vector |

### Solver Internal State

| Structure | Type | Space Complexity | Description |
|-----------|------|------------------|-------------|
| `lhs_values` | `vector<double>` | O(M) | Stores computed LHS values for convergence check |
| `targets` | `vector<int>` | O(M) | Maps row → which variable to solve for |
| `var_to_target` | `vector<int>` | O(M) | Inverse mapping: variable → which row solves it |
| `independent_targets` | `set<int>` | O(V) | Tracks variables uniquely determined by one row |
| `solution` | `vector<double>` | O(M) | Solution vector |

### AC Analysis Storage (Ac_analyzer)

| Structure | Type | Space Complexity | Description |
|-----------|------|------------------|-------------|
| `mna_matrix` | `unordered_map<int, unordered_map<int, complex<double>>>` | O(NNZ) | Complex sparse MNA matrix |
| `mna_vector` | `unordered_map<int, complex<double>>` | O(M) | Complex RHS vector |
| `solution` | `vector<complex<double>>` | O(M) | Complex solution vector |

---

## Core Classes

### Circuit

The central container managing circuit topology and MNA system.

| Method | Time Complexity | Space Complexity | Notes |
|--------|-----------------|------------------|-------|
| `Circuit()` | O(1) | O(1) | Creates ground node "0" |
| `~Circuit()` | O(N + C) | O(1) | Iterates and deletes all nodes/components |
| `add_node()` | O(1) amortized | O(1) | `unordered_map::find` + `insert` |
| `add_resistor()` | O(1) amortized | O(1) | `find` + `new Resistor` + `insert` |
| `add_voltage_source()` | O(1) amortized | O(1) | Also inserts into `extraVarId_map` |
| `add_current_source()` | O(1) amortized | O(1) | No extra variable needed |
| `add_inductor()` | O(1) amortized | O(1) | Also inserts into `extraVarId_map` |
| `add_capacitor()` | O(1) amortized | O(1) | No contribution in DC |
| **`parse_netlist()`** | **O(L)** | **O(N + C)** | File I/O; each line parsed in O(1) |
| **`assemble_MNA_system()`** | **O(C × S)** | **O(NNZ)** | S = stamps/component (≤4 matrix + ≤2 vector) |
| **`deploy_dc_solution()`** | **O(M)** | **O(1)** | Single loop over solution indices |
| `get_MNA_matrix()` | O(1) | O(1) | Returns const reference |
| `get_MNA_vector()` | O(1) | O(1) | Returns const reference |
| `get_nodes()` | O(1) | O(1) | Returns const reference |
| `print_nodes()` | O(N) | O(1) | Iterates `nodeId_map` |
| `print_components()` | O(C) | O(1) | Iterates `components` |
| `print_MNA_system()` | O(M² + NNZ) | O(1) | Nested loops over all var combinations |
| `print_solution()` | O(N + V) | O(1) | Calls `print_nodes` + `print_extraVars` |

#### parse_netlist() Implementation Details

```cpp
// From circuit.cpp:
while (file >> component_id) {           // O(L) iterations
    if (component_id[0] == '*') {        // Skip comments
        getline(file, comment_line);
        continue;
    }
    file >> node1_id >> node2_id >> value;  // O(1) per component
    add_node(node1_id);                     // O(1) amortized
    add_node(node2_id);                     // O(1) amortized
    add_<component>(...);                   // O(1) amortized
}
```

#### assemble_MNA_system() Implementation Details

```cpp
// From circuit.cpp:
mna_matrix.clear();                                    // O(NNZ) to clear existing
for (const auto& component : components) {             // O(C) components
    Component_contribution contrib = component.second->get_contribution();  // O(1)
    for (const auto& mc : contrib.matrixStamps)        // ≤4 stamps for most components
        mna_matrix[mc.row][mc.col] += mc.value;        // O(1) amortized hash insertion
    for (const auto& vc : contrib.vectorStamps)        // ≤2 stamps
        mna_vector[vc.row] += vc.value;                // O(1) amortized
}
```
**Total: O(C × S) where S ≤ 6 stamps/component → effectively O(C)**

### Simulator

Orchestrates the complete analysis pipeline.

| Method | Time Complexity | Space Complexity | Notes |
|--------|-----------------|------------------|-------|
| `Simulator()` | O(1) | O(1) | Default constructs Solver |
| **`run_dc_analysis()`** | **O(I × R × K)** | **O(M)** | Dominated by `solve()` |
| `print()` | O(M) | O(1) | Iterates solution vector |

#### run_dc_analysis() Implementation

```cpp
// From simulator.cpp:
void Simulator::run_dc_analysis(Circuit& circuit) {
    const auto& mna_matrix = circuit.get_MNA_matrix();  // O(1) - reference
    const auto& mna_vector = circuit.get_MNA_vector();  // O(1) - reference
    solver.solve_MNA_system(mna_matrix, mna_vector, solution);  // O(I × R × K)
    circuit.deploy_dc_solution(solution);              // O(M)
}
```

### Solver

High-level wrapper for the linear system solver.

| Method | Time Complexity | Space Complexity | Notes |
|--------|-----------------|------------------|-------|
| `Solver()` | O(1) | O(1) | Initializes Gauss-Seidel parameters |
| **`solve_MNA_system()`** | **O(M) + O(I × R × K)** | **O(M)** | Resize + solve |
| `print()` | O(1) | O(1) | Prints timing info |

#### solve_MNA_system() Implementation

```cpp
// From solver.cpp:
void Solver::solve_MNA_system(...) {
    solution.resize(mna_matrix.size() + 1, 0.0);  // O(M) - allocates & zero-initializes
    auto start = high_resolution_clock::now();
    gauss_seidel.solve(mna_matrix, mna_vector, solution);  // O(I × R × K)
    auto end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start);
}
```

### Gauss_seidel

The core iterative solver implementation with dynamic pivoting for zero-diagonal handling.

| Method | Time Complexity | Space Complexity | Notes |
|--------|-----------------|------------------|-------|
| `Gauss_seidel()` | O(1) | O(1) | Sets parameters only |
| `initialize()` | O(M) | O(M) | Allocates `lhs_values`, `targets`, `var_to_target`; initializes identity mapping |
| `handle_zero_diagonal()` | O(K) | O(1) | Iterates row's non-zeros; O(log V) for `independent_targets.find/insert` |
| `compute_row_update()` | O(K) | O(1) | Iterates row's non-zeros + one RHS lookup |
| `check_convergence()` | O(M) | O(1) | Compares all `lhs_values[i]` vs `mna_vector[i]` |
| **`solve()`** | **O(I × R × K)** | **O(M)** | R = MNA matrix rows (may be < M due to sparse storage) |
| `print()` | O(1) | O(1) | Prints convergence info |

#### solve() Detailed Analysis (Based on Implementation)

```cpp
// From gauss_seidel.cpp:
for (converge_iters = 1; converge_iters < max_iter; converge_iters++) {
    for (const auto& [row, col_map] : mna_matrix) {  // Only iterates actual rows in matrix
        handle_zero_diagonal(row, col_map);          // O(K) where K = col_map.size()
        // Skip if diagonal is zero after pivoting
        lhs_values[row] = compute_row_update(...);   // O(K)
    }
    if (converge_iters % 5 != 0) continue;           // Convergence check every 5 iterations
    converged = check_convergence(...);              // O(M)
    if (converged) return;
}
```

**Per iteration cost:**
- Row processing: O(R × K) where R = number of rows in mna_matrix
- Convergence check: O(M) but only every 5 iterations → amortized O(M/5)

**Total: O(I × (R × K + M/5)) ≈ O(I × R × K)** for sparse matrices where K << M

**Note:** Convergence is checked every 5 iterations (optimization), reducing overhead.

### Ac_analyzer

Handles AC frequency sweep analysis with complex-valued MNA systems.

| Method | Time Complexity | Space Complexity | Notes |
|--------|-----------------|------------------|-------|
| `Ac_analyzer()` | O(1) | O(1) | Sets output file path |
| `initialize()` | O(NNZ) | O(NNZ) | Converts real MNA to complex, filters extra vars |
| `assemble_ac_mna_system()` | O(A × S) | O(1) | A = AC components, S = stamps per component |
| `log_ac_inst_solution()` | O(M) | O(1) | Writes solution to file |
| `print()` | O(1) | O(1) | Prints configuration |

#### AC Analysis Workflow

```cpp
// Per frequency point:
for (freq = freq1; freq <= freq2; freq += step) {
    ac_analyzer.assemble_ac_mna_system(ac_components, freq);  // O(A × S)
    gauss_seidel_ac.solve(mna_matrix, mna_vector, solution);  // O(I × R × K)
    ac_analyzer.log_ac_inst_solution(freq, duration);         // O(M)
}
```

**Total AC Analysis: O(F × (A × S + I × R × K + M))** where F = frequency points

---

## Component Classes

All component classes inherit from `Component` and implement `get_contribution()`.

| Class | `get_contribution()` Time | `get_contribution()` Space | Matrix Stamps | Vector Stamps |
|-------|---------------------------|----------------------------|---------------|---------------|
| `Resistor` | O(1) | O(1) | 2-4 (depends on ground) | 0 |
| `Voltage_source` | O(1) | O(1) | 2-4 (depends on ground) | 1 |
| `Current_source` | O(1) | O(1) | 0 | 0-2 (depends on ground) |
| `Inductor` | O(1) | O(1) | 2-4 (depends on ground) | 0 |
| `Capacitor` | O(1) | O(1) | 0 (open circuit in DC) | 0 |

### AC Component Contributions

| Class | `get_ac_contribution()` Time | Matrix Stamps | Notes |
|-------|------------------------------|---------------|-------|
| `Capacitor` | O(1) | 2-4 | Admittance jωC stamping |
| `Inductor` | O(1) | 2-4 | Admittance 1/(jωL) stamping |
| `Voltage_source` | O(1) | 2-4 + 1 vector | Phasor voltage stamping |

#### Resistor::get_contribution() Implementation

```cpp
// From resistor.cpp - stamps depend on whether nodes are ground:
Component_contribution<double> Resistor::get_contribution() {
    Component_contribution<double> contribution;
    double conductance = 1.0 / resistance;
    if (ni->id != 0)                                    // Skip if positive node is ground
        contribution.stampMatrix(ni->id, ni->id, conductance);
    if (nj->id != 0)                                    // Skip if negative node is ground
        contribution.stampMatrix(nj->id, nj->id, conductance);
    if (ni->id != 0 && nj->id != 0) {                   // Only stamp off-diagonals if both non-ground
        contribution.stampMatrix(ni->id, nj->id, -conductance);
        contribution.stampMatrix(nj->id, ni->id, -conductance);
    }
    return contribution;  // 2-4 stamps depending on ground connections
}
```

### Component_contribution

| Method | Time Complexity | Space Complexity | Notes |
|--------|-----------------|------------------|-------|
| `MatrixContribution()` | O(1) | O(1) | Constructor |
| `VectorContribution()` | O(1) | O(1) | Constructor |
| `stampMatrix()` | O(1) amortized | O(1) | `emplace_back` into vector |
| `stampVector()` | O(1) amortized | O(1) | `emplace_back` into vector |
| `print()` | O(S) | O(1) | S = total stamps |

---

## Helper Classes

### Node

| Method | Time Complexity | Space Complexity |
|--------|-----------------|------------------|
| `Node()` | O(1) | O(S) where S = name length |
| `operator==` | O(S) | O(1) |
| `operator<` | O(S) | O(1) |
| `print()` | O(1) | O(1) |

### Timer

| Method | Time Complexity | Space Complexity |
|--------|-----------------|------------------|
| `Timer()` | O(1) | O(1) |
| `~Timer()` | O(1) | O(1) |

### UI

| Method | Time Complexity | Space Complexity |
|--------|-----------------|------------------|
| `UI()` | O(1) | O(1) |
| `parse_arguments()` | O(argc × S) | O(S) |
| `output_results()` | O(output length) | O(1) |

---

## Overall Analysis Pipeline

### Complete DC Analysis Workflow

```
1. Circuit::parse_netlist()       → O(L)           Space: O(N + C)
2. Circuit::assemble_MNA_system() → O(C)           Space: O(NNZ)
3. Simulator::run_dc_analysis()   → O(I × R × K)   Space: O(M)
   ├─ Solver::solve_MNA_system()
   │  ├─ solution.resize()        → O(M)
   │  └─ Gauss_seidel::solve()
   │     ├─ initialize()          → O(M)
   │     └─ iterate until convergence:
   │        ├─ process each row   → O(R × K) per iteration
   │        └─ check_convergence  → O(M) every 5 iterations
   └─ Circuit::deploy_dc_solution() → O(M)
```

### Complete AC Analysis Workflow

```
4. Simulator::run_ac_analysis()   → O(F × I × R × K)  Space: O(NNZ + M)
   ├─ Solver::assemble_ac_system()
   │  └─ Ac_analyzer::initialize() → O(NNZ) - convert real to complex
   └─ Solver::solve_ac_system()   → O(F × (A × S + I × R × K))
      └─ for each frequency:
         ├─ assemble_ac_mna_system() → O(A × S)
         ├─ gauss_seidel_ac.solve()  → O(I × R × K)
         └─ log_ac_inst_solution()   → O(M)
```

### Total Complexity

| Analysis | Time Complexity | Dominant Factor |
|----------|-----------------|------------------|
| **DC Only** | O(L + C + I×R×K + M) | **O(I × R × K)** (solver iterations) |
| **AC Only** | O(F × (A×S + I×R×K + M)) | **O(F × I × R × K)** (F frequency points) |
| **DC + AC** | O(L + C + (1+F) × I×R×K) | **O(F × I × R × K)** |

| Metric | Space Complexity | Notes |
|--------|-----------------|-------|
| **DC** | O(N + C + NNZ + M) | Real-valued |
| **AC** | O(N + C + 2×NNZ + 2×M) | Complex values = 2× storage |

Where:
- **R** = rows in MNA matrix (typically R ≈ N + V, but stored sparsely)
- **K** = average non-zeros per row ≈ 2-5 for circuit matrices
- **I** = iterations to convergence (checked every 5 iterations)

### Practical Performance

For typical circuits:
- **K ≈ 2-5** (sparse connectivity, depends on ground connections)
- **NNZ = O(C)** (each component contributes constant stamps)
- **I ≈ 10-200** (depends on damping factor and circuit conditioning)

| Circuit Size | Parse | Assemble | Solve (typical) | Total Space |
|--------------|-------|----------|-----------------|-------------|
| Small (N < 100) | < 0.1 ms | < 0.1 ms | < 1 ms | < 10 KB |
| Medium (N < 1000) | < 1 ms | < 1 ms | < 10 ms | < 100 KB |
| Large (N < 10000) | < 10 ms | < 10 ms | < 1 sec | < 10 MB |
| Very Large (N > 10000) | ~100 ms | ~100 ms | seconds-minutes | > 100 MB |

---

## Implementation-Specific Optimizations

### 1. Convergence Check Frequency
```cpp
if (converge_iters % 5 != 0) continue;  // Only check every 5 iterations
```
Reduces O(M) convergence checks by 80%, making per-iteration cost closer to O(R × K).

### 2. Dynamic Pivoting for Zero Diagonals
```cpp
// handle_zero_diagonal() swaps targets to handle voltage source/inductor rows
targets[row] = max_idx;  // Redirect which variable this row solves
var_to_target[max_idx] = row;  // Update inverse mapping
```
Avoids division by zero without matrix row swaps. O(K) per row with zero diagonal.

### 3. Sparse Matrix Iteration
```cpp
for (const auto& [row, col_map] : mna_matrix)  // Only iterates actual rows
```
Skips rows not in the sparse matrix, making iteration proportional to stored rows (R) not system size (M).

### 4. Ground Node Optimization
Components skip stamping for ground node (id=0), reducing stamps by up to 50% for components connected to ground.

---

## Notes

1. **Sparse Matrix Advantage**: The use of `unordered_map` for the MNA matrix provides O(1) average access and only stores non-zero entries, making it efficient for the typically sparse circuit matrices.

2. **Iterative Solver Trade-offs**: 
   - Modified Gauss-Seidel is O(I × NNZ) per solve
   - Direct methods (LU) would be O(M³) but only O(M²) per additional solve
   - For single DC analysis, iterative methods are often faster for sparse systems

3. **Memory Layout**: Hash maps have overhead (~40 bytes per entry) compared to dense arrays. For very large circuits, consider CSR/CSC sparse matrix formats.

4. **Convergence Factors**: 
   - Damping factor (default 0.1) trades convergence speed for stability
   - Tolerance (default 1e-9) determines precision vs. iteration count
   - Circuit topology affects condition number and convergence rate

---