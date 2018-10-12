#ifndef PROPERTY_H_INCLUDED
#define PROPERTY_H_INCLUDED
#include <vector>
#include "damage.h"
#include "vulnerabilitymodel.h"
#include "earthquake.h"

using namespace std;

class Property
{
protected:
    unsigned                                  finalNumberOfTypes;
    vector<float>                             baseValue;
    vector<vector<float> >                    mapCoordination;
    vector<vector<float> >                    area;
    vector<vector<float> >                    valueRatio;
    bool     checkDataIntegrity(VulnerabilityModel *);
    void     prepareVariables(VulnerabilityModel *, Earthquake,unsigned, unsigned);
public:
    friend class WorldModel;
    vector<vector<Damage> >  damage;
    unsigned getSizeOfBuildingTypes();
    unsigned getNumberOfPoints();
    unsigned getNumberOfStructuralType();
    float    getArea(unsigned, unsigned);
    double   getMonetaryValue(unsigned , unsigned);
    void     getCoordination(unsigned , vector<float> &);
};

inline unsigned Property::getNumberOfPoints()
{
    return mapCoordination.size();
}

inline unsigned  Property::getNumberOfStructuralType()
{
    return finalNumberOfTypes;
}

inline float Property::getArea(unsigned iPoint,unsigned iStructul)
{
    return area[iPoint][iStructul];
}


inline double Property::getMonetaryValue(unsigned iPoint,unsigned iStructul)
{
    return baseValue[iPoint]*valueRatio[iPoint][iStructul];
}


#endif // PROPERTY_H_INCLUDED
