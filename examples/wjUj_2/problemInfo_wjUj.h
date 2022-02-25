//
// Created by henri on 01/07/21.
//

#ifndef K_ADAPTABILITY_PROBLEMINFO_WJUJ_H
#define K_ADAPTABILITY_PROBLEMINFO_WJUJ_H

#include <problemInfo.hpp>
#include "wjUj_Instance.h"

class KAdaptableInfo_wjUj : public KAdaptableInfo {
public:
    explicit KAdaptableInfo_wjUj(const wjUj_Instance& t_instance);
    void build();
    KAdaptableInfo *create() const override;
    KAdaptableInfo *clone() const override;
protected:
    KAdaptableInfo_wjUj() = default;

    const wjUj_Instance* m_instance = nullptr;
    void makeVars() override;
    void makeUncSet() override;
    void makeConsX() override;
    void makeConsY(unsigned int k) override;
};


#endif //K_ADAPTABILITY_PROBLEMINFO_WJUJ_H
