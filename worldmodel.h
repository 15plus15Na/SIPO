#ifndef WORLDMODEL_H_INCLUDED
#define WORLDMODEL_H_INCLUDED
#include<iostream>
#include<fstream>
#include<vector>
#include<sstream>
#include<math.h>
#include<conio.h>
#include "damage.h"
#include "earthquake.h"
#include "property.h"
//#include "earthquake.cpp"
//#include "property.cpp"
#define PI 3.14159265
using namespace std;

class WorldModel{
private:
    enum   attenuationType{AMBBom91,RMZ,SRV,AMB96};
    float  getSurfaceDistance(float ,float ,float ,float );
    void   getTheNearestPointOnTheLineToPoint(vector<float> & , float, float, unsigned);
    double attenuation(float ,float,attenuationType,float);

public:
    Earthquake eq;
    Property   property;
    vector<double>     calculatePGAByEarthquakeAndPointNumber(unsigned int ,unsigned int ,vector<float> &);
    Earthquake  getEarthquakeObject();
    Earthquake *getEarthquakePointer();
    Property    getPropertyObject();
    Property   *getPropertyPointer();
    WorldModel(const char *,const char *, const char *, const char *, VulnerabilityModel *);
};

#endif // WORLDMODEL_H_INCLUDED
