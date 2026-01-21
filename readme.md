# Circuit Simulator

A high-performance circuit simulation tool.

## 📦 Current Versions

**Release: v2.5**  
*DC Solver v2*

---

## 📁 Project Structure

```
circuitSimulator/
├── include/              # Header files
├── src/                  # Implementation files
├── main/                 # Main application
├── tests/                # Test suites
└── build/                # Compiled executables
```

---

## ✨ Release Notes

### V2.5: DC Analysis ✅ COMPLETE
- ✅ Enhanced Modified Gauss-Seidel Method solver (PIONEERED Again)
- ✅ Inductor/Capacitor DC behavior support

### V2.0: DC Analysis ✅ COMPLETE
- ✅ Modified Gauss-Seidel Method solver (PIONEERED)
- ✅ DC analysis validation
- ✅ Solution verification
- ✅ Resistor DC behavior

### V1.0: MNA Assembly ✅ COMPLETE
- ✅ Project structure and build system
- ✅ Core component classes
- ✅ Netlist parsing
- ✅ MNA system assembly

### Core Components
- ✅ **Resistors** - Linear resistive elements
- ✅ **Voltage Sources** - Independent DC voltage sources
- ✅ **Current Sources** - Independent DC current sources
- ✅ **Inductors** - Short circuit at DC
- ✅ **Capacitors** - Open circuit at DC

### Netlist Parsing
- ✅ **SPICE-like Format** - Industry-standard syntax

### Circuit Analysis
- ✅ **Modified Nodal Analysis (MNA)** - Efficient matrix assembly
- ✅ **DC Analysis Solver (OP)** - DC Operating Point Solver
  - ✅ **Modified Gauss-Seidel Solver (OP)** - Pioneered iterative solver for DC analysis

### User Interface
- ✅ **Command-Line Interface** - Flexible argument parsing

---

## 🚀 Quick Start

### Prerequisites
- C++17 compatible compiler (g++ recommended)
- Windows with MSYS2/MinGW (or equivalent on Linux/Mac)

### Building the Project

**Compile the main program:**
```bash
# Build main program
g++ -std=c++17 -Wall -g -I./include src/*.cpp main/main.cpp -o build/debug/main.exe

# Build tests
g++ -std=c++17 -Wall -g -I./include src/*.cpp tests/test_components.cpp -o build/debug/test_components.exe
g++ -std=c++17 -Wall -g -I./include src/*.cpp tests/test_netlist_parsing.cpp -o build/debug/test_netlist_parsing.exe
g++ -std=c++17 -Wall -g -I./include src/*.cpp tests/test_mna_assembly.cpp -o build/debug/test_mna_assembly.exe
g++ -std=c++17 -Wall -g -I./include src/*.cpp tests/test_dc_analysis.cpp -o build/debug/test_dc_analysis.exe
g++ -std=c++17 -Wall -g -I./include src/*.cpp tests/test_dc_analysis_lc.cpp -o build/debug/test_dc_analysis_lc.exe
```

### Running the Simulator

```bash
# Basic usage
./build/debug/main.exe -i netlist.txt

# With output file
./build/debug/main.exe -i circuit.net -o results.txt

# Verbose mode
./build/debug/main.exe -i circuit.net -v

# Show help
./build/debug/main.exe -h
```

#### Command-Line Options

| Flag | Description |
|------|-------------|
| `-i <file>` | Input netlist file (required) |
| `-o <file>` | Output results file (default: output.log) |
| `-v` | Verbose mode (display results to console) |
| `-h` | Show help message |

### Running Tests

**Run tests:**
```bash
./build/debug/test_components.exe
./build/debug/test_netlist_parsing.exe
./build/debug/test_mna_assembly.exe
./build/debug/test_dc_analysis.exe
./build/debug/test_dc_analysis_lc.exe
```

---

## 🔌 Supported Components

### Resistor (R)
```
Syntax:  R<name> <node+> <node-> <value>
Example: R1 1 0 1000
Units:   Ohms (Ω)
Range:   R > 0
```

**MNA Stamping:**
- Adds conductance `G = 1/R` to diagonal elements `[i,i]` and `[j,j]`
- Adds `-G` to off-diagonal elements `[i,j]` and `[j,i]`

### Voltage Source (V)
```
Syntax:  V<name> <node+> <node-> <value>
Example: V1 1 0 10
Units:   Volts (V)
Range:   Any real number
```

**Convention:** `V<name> n+ n-` means voltage at `n+` is `value` volts higher than `n-`

**MNA Stamping:**
- Introduces extra variable for branch current
- Adds KCL entries linking nodes to branch current
- Adds voltage constraint equation to RHS

### Current Source (I)
```
Syntax:  I<name> <node+> <node-> <value>
Example: I1 0 1 0.01
Units:   Amperes (A)
Range:   Any real number
```

**Convention:** `I<name> n+ n-` means current flows FROM `n+` TO `n-`

**MNA Stamping:**
- Stamps `-I` to RHS at `node+` (current leaving)
- Stamps `+I` to RHS at `node-` (current entering)

### Inductor (L)
```
Syntax:  L<name> <node+> <node-> <value>
Example: L1 1 2 0.001
Units:   Henries (H)
Range:   L > 0
```

**DC Behavior:** Acts as **SHORT CIRCUIT** (wire) - introduces extra variable for current with zero voltage drop constraint.

### Capacitor (C)
```
Syntax:  C<name> <node+> <node-> <value>
Example: C1 1 2 0.0001
Units:   Farads (F)
Range:   C > 0
```

**DC Behavior:** Acts as **OPEN CIRCUIT** - no MNA contribution at DC steady state.

### Components Quick Reference
```
╔══════════════════════════════════════════════════════╗
║           COMPONENTS QUICK REFERENCE                 ║
╠══════════════════════════════════════════════════════╣
║  RESISTOR                                            ║
║  ────────                                            ║
║  Syntax:  R<name> <n+> <n-> <value>                  ║
║  Example: R1 1 0 1000                                ║
║  Units:     (ohms)                                   ║
║  Range:   R > 0                                      ║
║                                                      ║
║  VOLTAGE SOURCE                                      ║
║  ──────────────                                      ║
║  Syntax:  V<name> <n+> <n-> <DC_value>               ║
║  Example: VCC 1 0 5                                  ║
║  Units:   V (volts)                                  ║
║  Range:   Any real number                            ║
║                                                      ║
║  CURRENT SOURCE                                      ║
║  ──────────────                                      ║
║  Syntax:  I<name> <n+> <n-> <DC_value>               ║
║  Example: I1 0 1 1                                   ║
║  Units:   A (amperes)                                ║
║  Range:   Any real number                            ║
║                                                      ║
║  INDUCTOR                                            ║
║  ────────                                            ║
║  Syntax:  L<name> <n+> <n-> <value>                  ║
║  Example: L1 1 2 0.001                               ║
║  Units:   H (henries)                                ║
║  DC:      Short circuit                              ║
║                                                      ║
║  CAPACITOR                                           ║
║  ─────────                                           ║
║  Syntax:  C<name> <n+> <n-> <value>                  ║
║  Example: C1 1 2 0.0001                              ║
║  Units:   F (farads)                                 ║
║  DC:      Open circuit                               ║
╚══════════════════════════════════════════════════════╝
```

---

## 📝 Netlist Format

### Basic Structure
```
* Comment line (starts with *)
<component> <node1> <node2> <value>
...
```

### Features
- First line starting with `*` sets circuit name
- Comments can appear anywhere (line must start with `*`)
- Node IDs can be strings or numbers
- Node "0" is always ground reference
- Case-insensitive component prefixes

### Example Netlists

**Voltage Divider:**
```
* Voltage Divider
V1 1 0 10
R1 1 2 1000
R2 2 0 1000
```

**Current Source with Resistor:**
```
* Current Source with Resistor
I1 0 1 0.01
R1 1 0 1000
```

**Wheatstone Bridge:**
```
* Wheatstone Bridge
V1 1 0 10.0
R1 1 2 1000
R2 2 0 1000
R3 1 3 1000
R4 3 0 1000
R5 2 3 100
```

---

## 🏗️ Architecture

### Core Classes

| Class | File | Description |
|-------|------|-------------|
| `Circuit` | circuit.h/cpp | Main circuit container - holds nodes, components, MNA system |
| `Simulator` | simulator.h/cpp | Orchestrates simulation runs (DC analysis) |
| `Solver` | solver.h/cpp | Wrapper for linear system solving |
| `Gauss_seidel` | gauss_seidel.h/cpp | Modified Gauss-Seidel iterative solver |
| `Component` | component.h/cpp | Abstract base class for all circuit elements |
| `Node` | node.h/cpp | Represents circuit nodes with voltage |
| `Component_contribution` | component_contribution.h/cpp | Stores MNA matrix/vector stamps |

### Component Classes

| Component | Class | Prefix | MNA Behavior |
|-----------|-------|--------|--------------|
| Resistor | `Resistor` | R | Conductance stamping (G matrix) |
| Voltage Source | `Voltage_source` | V | Extra variable for branch current |
| Current Source | `Current_source` | I | RHS vector stamping only |
| Inductor | `Inductor` | L | Short circuit at DC (extra variable) |
| Capacitor | `Capacitor` | C | Open circuit at DC (no contribution) |

---

## 🧮 Modified Nodal Analysis (MNA)

### System Structure
The MNA system solves: **[A][x] = [b]**

Where:
- **[A]** = MNA matrix (conductances + voltage source constraints)
- **[x]** = Solution vector (node voltages + branch currents)
- **[b]** = RHS vector (current sources + voltage source values)

### Matrix Assembly
1. Ground node (node "0") is reference (V=0), not included in matrix
2. Each component provides `Component_contribution` with:
   - `matrixStamps` - entries for [A] matrix
   - `vectorStamps` - entries for [b] vector
3. Contributions are accumulated into sparse matrix structure

---

## 🔧 Solver: Modified Gauss-Seidel

### Algorithm Features
- **Iterative Method:** Gauss-Seidel with configurable damping
- **Zero Diagonal Handling:** Dynamic pivoting (target swapping)
- **Convergence Check:** Every 5 iterations, compares LHS vs RHS

### Configuration

| Parameter | Default | Description |
|-----------|---------|-------------|
| `max_iter` | 1000 | Maximum iterations |
| `tolerance` | 1e-9 | Convergence tolerance |
| `damping_factor` | 0.5 | Relaxation factor (0 < λ ≤ 1) |

### Update Formula
```
x_new[i] = (b[i] - Σ A[i,j]*x[j]) / A[i,i]   (j ≠ i)
x[i] = λ * x_new[i] + (1-λ) * x_old[i]
```

---

## 📊 Output Format

### Circuit Information
- Node list with voltages
- Component list with values
- MNA matrix visualization
- Extra variables (branch currents)

### DC Solution
```
╔════════════════════════════════════╗
║   DC ANALYSIS RESULTS              ║
╚════════════════════════════════════╝

Circuit Nodes:
Node(ID)        Voltage
Node(0)       0.000000 V
Node(1)      10.000000 V
...

Circuit VS Currents:
I_VS(ID)        Current
I_VS(V1)     0.010000 A
```

---

## 🗺️ Development Roadmap

## 📋 Phase 1: Foundation & Core Architecture ✅
### Objectives:

- Set up project structure with proper build system
- Design core data structures:
  - Component base class (resistor, voltage source, current source)
  - Node class (connection points)
  - Circuit/Netlist class
- Implement basic I/O:
  - Parse simple netlist format (SPICE-like or custom)
  - Output circuit topology

### Deliverables:

- ✅ Project compiles
- ✅ Can read a simple circuit file
- ✅ Circuit Topology represented
- ✅ Unit tests for basic components

## 🔧 Phase 2: DC Analysis Engine ✅

### Objectives:

- Implement Modified Nodal Analysis (MNA):
  - Build conductance matrix (G matrix)
  - Build source vector (I vector)
  - Solve linear system: Gv = I
- Add linear solver:
  - Start with Gauss-Seidel
  - Later: LU decomposition 🔧
- Support basic components:
  - Resistors
  - Independent voltage/current sources
  - Ground node handling

### Deliverables:

- ✅ DC operating point calculation
- ✅ Node voltage outputs
- ✅ Branch current calculations
- ✅ Validation against known circuits (voltage divider, Wheatstone bridge)


## 📊 Phase 3: AC Analysis

### Objectives:


- Component base class (capacitor, inductor, AC source)
- Frequency domain analysis:
  - Small-signal linearization
  - Complex arithmetic for impedances
  - Frequency sweep
- Calculate frequency response:
  - Magnitude and phase
  - Bode plots data generation

### Deliverables:

- ⬜ AC sweep from fmin to fmax
- ⬜ Filter response (low-pass, high-pass, band-pass)
- ⬜ Gain and phase output
- ⬜ Input/output impedance calculation

## ⚡ Phase 4: Transient Analysis

### Objectives:

- Implement time-domain simulation:
  - Numerical integration (Backward Euler or Trapezoidal)
  - Timestep control
- Add dynamic components:
  - Capacitors (companion model)
  - Inductors (companion model)
- Time-varying sources:
  - Pulse sources
  - Sinusoidal sources
  - PWL (Piecewise Linear) sources

### Deliverables:

- ⬜ RC/RL/RLC circuit transient response
- ⬜ Output waveform data (CSV format)
- ⬜ Configurable simulation time and timestep
- ⬜ Energy conservation verification

## 🔬 Phase 5: Nonlinear Components

### Objectives:

- Implement Newton-Raphson solver:
  - Jacobian matrix calculation
  - Iterative convergence
- Add nonlinear devices:
  - Diode (exponential I-V characteristic)
  - BJT transistor (Ebers-Moll model)
  - MOSFET (simple Level 1 model)
- Convergence handling:
  - Damping strategies
  - Source stepping
  - Continuation methods

### Deliverables:

- ⬜ Diode rectifier circuit simulation
- ⬜ BJT amplifier bias point
- ⬜ MOSFET inverter analysis
- ⬜ Convergence diagnostics

## 🚀 Phase 6: Optimization & Advanced Features

### Objectives:

- Performance optimization:
  - Sparse matrix techniques
  - Parallel processing (OpenMP)
  - Memory profiling
  - Create a new data structure 
    - Optimized storage for MNA matrix
    - Faster access patterns
      - Unordered map with open addressing
- Advanced analyses:
  - Fourier analysis (FFT)
  - Noise analysis
  - Sensitivity analysis
- Model library:
  - Op-amp models
  - Transformer models
  - Transmission lines

### Deliverables:

- ⬜ 10x performance improvement
- ⬜ Component library with 20+ models
- ⬜ Benchmark suite
- ⬜ Documentation and examples
