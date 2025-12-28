#include "Include.h"

using namespace std;

int main(int argc, char* argv[]) {
    Timer timer;
    Circuit circuit;
    circuit.parse_netlist("netlist.txt");
    cout << circuit << endl;
    return 0;
}