//
// Created by henri on 21/06/21.
//

#include "mmkp_Instance.h"
#include <fstream>

mmkp_Instance::mmkp_Instance(const std::string &t_filename, unsigned int t_n_points) {
    std::ifstream file(t_filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open instance file, " + t_filename);
    }

    file >> m_n_items >> m_n_knapsack;
    file >> m_capacity >> m_gamma;

    m_low_weights.reserve(m_n_items);
    m_high_weights.reserve(m_n_items);
    m_profits.reserve(m_n_items);

    double p_i, w_i_lo, w_i_hi;
    for (unsigned int i = 0 ; i < m_n_items ; ++i) {
        file >> p_i >> w_i_lo >> w_i_hi;
        m_low_weights.emplace_back(w_i_lo);
        m_high_weights.emplace_back(w_i_hi);
        m_profits.emplace_back(p_i);
    }

    file.close();
}
