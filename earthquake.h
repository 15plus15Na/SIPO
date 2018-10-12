#ifndef EARTHQUAKE_H_INCLUDED
#define EARTHQUAKE_H_INCLUDED
#include<vector>
using namespace std;
class Earthquake
{
protected:
    int earthquakeQuantity;
    vector<unsigned>       numberOfDefinitionPoints;
    vector<float>          magnitude;
    vector<float>          depth;
    vector<float>          probability;
    vector<vector<float> > coordination;
    vector<vector<float> > coordination_line_X;
    vector<vector<float> > coordination_line_Y;
    vector<vector<float> > distanceFromOrigin;
    vector<vector<unsigned> > additionalDefinitionData;
    bool                   isPointSource(unsigned);
    bool                   isLineSource(unsigned);
    void                   calculateLineEquationParameters(unsigned);
public:
    vector<vector<float> > lineSlope;
    bool     isInAdditionalPointGap(unsigned, unsigned);
    unsigned getNumberOfEarthquakes();
    float    getProbability(unsigned);
    float    getMaginitude(unsigned);
    void     getCoordination(unsigned, vector<float> &);
    friend class WorldModel;
    friend class Property;
};

inline unsigned Earthquake::getNumberOfEarthquakes()
{
    return magnitude.size();
}

#endif // EARTHQUAKE_H_INCLUDED
