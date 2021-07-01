//
// Created by henri on 21/06/21.
//

#ifndef K_ADAPTABILITY_MMKP_INSTANCE_H
#define K_ADAPTABILITY_MMKP_INSTANCE_H

#include <vector>
#include <random>
#include <iostream>

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
        // 5-2
        m_n_items = 5;
        m_n_knapsack = 2;
        m_gamma = 1; //(unsigned int) (.4 * (double) m_n_items);

        std::random_device random;
        std::mt19937 engine(random());

        double sum_capacity = 0.0;
        for (unsigned int i = 0 ; i < m_n_items ; ++i) {
            m_low_weights.emplace_back(std::uniform_real_distribution<double>(0, 100)(engine));
            m_high_weights.emplace_back(m_low_weights.back() * 1.3);
            sum_capacity += m_high_weights.back();

            m_profits.emplace_back(m_low_weights.back() + 10);
        }

        m_capacity = sum_capacity / m_n_knapsack * .7;
    }

    mmkp_Instance(const std::string& t_filename, unsigned int t_n_points);
};

static std::ostream& operator<<(std::ostream& t_os, const mmkp_Instance& t_instance) {

    t_os << t_instance.n_items() << '\t' << t_instance.n_knapsacks() << '\n';
    t_os << t_instance.capacity() << '\t' << t_instance.gamma() << '\n';
    for (unsigned int i = 0, I = t_instance.n_items() ; i < I ; ++i) {
        t_os << t_instance.profit(i) << '\t' << t_instance.low_weight(i) << '\t' << t_instance.high_weight(i) << '\n';
    }

    return t_os;
}


#endif //K_ADAPTABILITY_MMKP_INSTANCE_H
