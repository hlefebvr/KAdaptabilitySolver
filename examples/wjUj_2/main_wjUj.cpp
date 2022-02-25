//
// Created by henri on 01/07/21.
//

#include <iostream>
#include <fstream>
#include "robustSolver.hpp"
#include "problemInfo_wjUj.h"
#include "wjUj_Instance.h"

int main(int argc, const char** argv) {

    const bool heuristic_mode = false;

    if (argc != 2) {
        throw std::runtime_error("Usage: ./executable <PATH_TO_INSTANCE>");
    }

    const std::string path_to_instance = argv[1];
    const unsigned int maximum_K = 7; // maximum value for K

    wjUj_Instance source_instance(path_to_instance);

    unsigned int maximum_gamma = 0; // maximum value for gamma
    switch (source_instance.jobs().size()) {
        case 5: maximum_gamma = 3;
        case 10: maximum_gamma = 7;
        default: maximum_gamma = 10;
    }

    for (unsigned int gamma = 1 ; gamma < maximum_gamma ; ++gamma) {

        source_instance.set_gamma(gamma);

        for (unsigned int k = 1; k <= maximum_K; ++k) {
            KAdaptableInfo_wjUj info(source_instance);
            info.build();

            KAdaptableSolver solver(info);
            std::vector<double> x;

            std::cout << source_instance.instance_filename_stripped() << ","
                      << "2,"
                      << "subra,"
                      << source_instance.jobs().size() << ","
                      << gamma << ',';
            auto solution_status = solver.solve_KAdaptability(k, heuristic_mode, x);
            if (solution_status != 0) {
                break;
            }
        }
    }

    return 0;
}
