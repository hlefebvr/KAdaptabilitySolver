//
// Created by henri on 21/06/21.
//

#ifndef K_ADAPTABILITY_PROBLEMINFO_MMKP_H
#define K_ADAPTABILITY_PROBLEMINFO_MMKP_H

#include "problemInfo.hpp"

class mmkp_Instance;

class KAdaptableInfo_MMKP : public KAdaptableInfo {
public:
    explicit KAdaptableInfo_MMKP(const mmkp_Instance& t_instance);
    void build();
    KAdaptableInfo *create() const override { return new KAdaptableInfo_MMKP(); }
    KAdaptableInfo *clone() const override { return new KAdaptableInfo_MMKP(*this); }
protected:
    KAdaptableInfo_MMKP() = default;
    void makeVars() override;
    void makeUncSet() override;
    void makeConsX() override;
    void makeConsY(unsigned int k) override;
private:
    const mmkp_Instance* m_instance = nullptr;
};

#endif //K_ADAPTABILITY_PROBLEMINFO_MMKP_H
