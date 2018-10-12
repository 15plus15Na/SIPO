#ifndef INVENTORY_H_INCLUDED
#define INVENTORY_H_INCLUDED
#include <vector>
#include "..\worldmodel.h"
#include "..\damage.h"
#include "insurancemodel.h"
#include "log.h"

#define IDTable_Hazard 0
#define IDTable_Location 1
#define IDTable_Damage 2
#define HazardTable_Probability 0
//using namespace std;
class Log;
class InsuranceModel;
class Inventory
{
protected:
    unsigned number_HazardCases;
    unsigned number_Locations;
    unsigned number_Types;
    unsigned number_IDTable;
    double   totalMeanLoss;
    double   totalGrossMeanLoss;
    double   sumOfPeropertiesValues;
    std::vector<std::vector<unsigned> > hazard_IDTAble_IDRealtionship;
    std::vector<std::vector<unsigned> > _IDTable;
    std::vector<std::vector<float> >    _DamageTable;
    std::vector<std::vector<float> >    _GrossDamageTable;
    std::vector<std::vector<float> >    _LocationTable;
    std::vector<std::vector<float> >    _HazardTable;
    std::vector<std::vector<Damage> >   damage;
    std::vector<std::vector<float> >    _value;
    //std::vector<std::vector<float> >
    unsigned searchForIndexOfFirstMatch(unsigned);

public:
    //std::vector <std::vector<float> > damage;
    //***** Functions ******
    unsigned getNumberOFHazards() const;
    unsigned getNumberOfLocations() const;
    unsigned getNumberOfTypes() const;
    unsigned getNumberOFIDTable() const;
    unsigned getHazardIndexByIDTableIndex(unsigned) const;
    unsigned getLocationIndexByIDTableIndex(unsigned) const;
    unsigned getDamageIndexByIDtableIndex(unsigned) const;
    unsigned getEndingIDTableIndexOfHazard(unsigned) const;
    unsigned getBeginingIDTableIndexOfHazard(unsigned) const;
    void     getValueOfProperty(std::vector<float> &, unsigned)  const;
    void     getDamagesOfTypes(std::vector<float> &, unsigned)  const;
    void     getGrossDamagesOfTypes(std::vector<float> &, unsigned)  const;
    void     showData() const;
    void     addToLog();
    void     showSelective() const;
    float    getProbabilityOfHazard(unsigned) const;
    double   getSumOfPropetiesValueByInsuranceCoverageAndPolicyConditions(InsuranceModel *);
    void     prepareDamageType(InsuranceModel *);
    //void     prepareTreatyReinsuredDamageType(InsuranceModel *);
    Inventory();
    Inventory(WorldModel *);
};

inline float Inventory::getProbabilityOfHazard(unsigned hazard_index) const
{
    return _HazardTable[hazard_index][HazardTable_Probability];
}

inline unsigned Inventory::getEndingIDTableIndexOfHazard(unsigned hazard_ID)  const
{
    return hazard_IDTAble_IDRealtionship[hazard_ID][1];
}

inline unsigned Inventory::getBeginingIDTableIndexOfHazard(unsigned hazard_ID)  const
{
    return hazard_IDTAble_IDRealtionship[hazard_ID][0];
}

inline unsigned Inventory::getLocationIndexByIDTableIndex(unsigned i)  const
{
    return _IDTable[i][IDTable_Location];
}

inline void Inventory::getValueOfProperty(vector<float> &_value, unsigned location_index)  const
{
    _value.resize(_LocationTable[location_index].size()-2,0);
    for(unsigned i=2;i<_LocationTable[location_index].size();i++)
    {
        _value[i-2]=_LocationTable[location_index][i];
    }
}

inline unsigned Inventory::getDamageIndexByIDtableIndex(unsigned i)  const
{
    return _IDTable[i][IDTable_Damage];
}

inline void Inventory::getDamagesOfTypes(vector<float> &_damage, unsigned _damage_index)  const
{
    //_damage.resize(_DamageTable[_damage_index].size(),0);
    //for(unsigned i=0;i<_DamageTable[_damage_index].size();i++)
    //{
        //_damage[i]=_DamageTable[_damage_index][i];
    //}
    _damage=_DamageTable[_damage_index];
}

#endif // INVENTORY_H_INCLUDED
