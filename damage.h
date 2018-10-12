#ifndef DAMAGE_H_INCLUDED
#define DAMAGE_H_INCLUDED
#include<vector>

class Damage
{
    friend class Property;
    friend class Inventory;
    //friend class WorldModel;
protected:
    std::vector<std::vector<std::vector<std::vector<double> > > > damagedArea;
    std::vector<std::vector<std::vector<std::vector<double> > > > damageRatio;
    std::vector<std::vector<std::vector<std::vector<double> > > > damageStateShare;
    std::vector<std::vector<std::vector<std::vector<double> > > > monetaryLoss;
    float weight;
public:
    double   getDamageRatio(unsigned, unsigned ,unsigned ,unsigned);
    double   getDamageStateShare(unsigned, unsigned ,unsigned ,unsigned);
    double   getDamagedArea(unsigned , unsigned , unsigned , unsigned);
    double   getMonetaryLoss(unsigned , unsigned , unsigned , unsigned);
    float    getWeight();
    std::vector<std::vector<std::vector<std::vector<double> > > > *getMonetaryLossVectorPointer();
    void     setDamageRatio(double ,unsigned, unsigned ,unsigned ,unsigned);
    void     setDamageStateShare(double ,unsigned, unsigned ,unsigned ,unsigned);
    void     setDamagedArea(double, unsigned, unsigned ,unsigned ,unsigned);
    void     setMonetaryLoss(double, unsigned, unsigned ,unsigned ,unsigned);
    void     setWeight(float);
    void     setMonetaryLossByVectorPointer(std::vector<std::vector<std::vector<std::vector<double> > > > *);
    void     showMonetaryLoss();
             Damage();
             Damage(const Damage &);
};

inline float  Damage::getWeight()
{
    return weight;
}

inline void Damage::setWeight(float input_weight)
{
    weight=input_weight;
}

#endif // DAMAGE_H_INCLUDED
