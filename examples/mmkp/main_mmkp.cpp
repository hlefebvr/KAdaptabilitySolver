//
// Created by henri on 21/06/21.
//

#include <iostream>
#include "robustSolver.hpp"
#include "problemInfo_mmkp.h"
#include "mmkp_Instance.h"

int main(int argc, const char** argv) {

    const bool heuristic_mode = false;
    const unsigned int Kmax = 4;

    mmkp_Instance instance;

    KAdaptableInfo_MMKP info(instance);
    info.build();

    KAdaptableSolver solver(info);
    std::vector<double> x;

    // solving in a loop the 1-,2-,...,K-adaptability problems
    // will automatically warm-start from the previous solve
    for (unsigned int K = 1; K <= Kmax; K++) {
        solver.solve_KAdaptability(K, heuristic_mode, x);
    }

    return 0;
}
