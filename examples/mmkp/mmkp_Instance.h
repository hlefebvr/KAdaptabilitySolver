//
// Created by henri on 21/06/21.
//

#ifndef K_ADAPTABILITY_MMKP_INSTANCE_H
#define K_ADAPTABILITY_MMKP_INSTANCE_H

#include <vector>

class mmkp_Instance {
    unsigned int m_n_items = 0;
    unsigned int m_n_knapsack = 0;
    unsigned int m_gamma = 0;
    std::vector<double> m_low_weights;
    std::vector<double> m_high_weights;
    std::vector<double> m_profits;
    double m_capacity = 0;
public:
    unsigned int n_items() const { return m_n_items; }
    unsigned int n_knapsacks() const { return m_n_knapsack; }
    unsigned int gamma() const { return m_gamma; }
    double profit(unsigned int t_index) const { return m_profits[t_index]; }
    double low_weight(unsigned int t_index) const { return m_low_weights[t_index]; }
    double high_weight(unsigned int t_index) const { return m_high_weights[t_index]; }
    double capacity() const { return m_capacity; }

    mmkp_Instance() {
        m_n_items = 3;
        m_n_knapsack = 2;
        m_gamma = 1;
        m_low_weights = { 1, 2, 3 };
        m_high_weights = { 2, 3, 4 };
        m_profits = { 1, 2, 1.5 };
        m_capacity = 7;
    }
};


#endif //K_ADAPTABILITY_MMKP_INSTANCE_H
