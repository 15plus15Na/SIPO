#ifndef APPROXIMATION_H_INCLUDED
#define APPROXIMATION_H_INCLUDED
#include<vector>
#include<random>
#include "insurancemodel.h"
#include "result.h"

using namespace std;
class DEP
{
protected:
    vector<vector<float> > x;
    vector<float> low_bound;
    vector<float> up_bound;
    vector<float> part;
    unsigned place;
    unsigned type;
    vector<vector<float> > candidate;
    vector<vector<float> >single_candidate;
    vector<vector<vector<float> > > final_candidate;
    void setTripleSize(vector<vector<vector<float> > > ,unsigned ,unsigned ,unsigned );
    double soot;
public:
    void setLimits(unsigned, float,float);
    void setParts(unsigned,float);
    void descrtisizeEqualParts();
    void goDeep(unsigned,vector<vector<float> >);
    void goDeepForSingle(unsigned,vector<float>,vector<vector<float> > &);
    void calculateDiffrentParts(InsuranceModel,Inventory */*,vector<Result> &*/);
    unsigned getNumberOfInstances();
    DEP(unsigned, unsigned, unsigned);
};

class InsuranceDEP
{
public:
    InsuranceDEP(unsigned, unsigned, unsigned);
};

////////////////////////////////////////////////////////////////////////////////////

class SPSA
{
    vector<vector<vector<float> > > decisionVariable;
    int model_counter;
    vector<float> one_DecisionVariable;
    float model_c;
    float model_a;
    float model_a_k;
    float model_c_k;
    float model_A;
    float model_alpha;
    float model_gamma;
    float lastValue;
    float currentValue;
    float minValue;
    default_random_engine generator;
    void nextModelVariable();
    void nextCandidate(InsuranceModel , Inventory *);
    void copyInitialValueOfDecisionVariables(InsuranceModel , const Inventory *);
    void configFromFile(const char*);
public:
    int iteration_max;
    void beginIteration(InsuranceModel,Inventory *);
    void boundCheck(unsigned ,float &,float , float &);
    void boundCheck(unsigned ,float &);
    bool decideIfToContinue();
    void showDV();
    void addToLog();
    void minorAndMajorCheck(float &,float &,float,float,float &,float&);
    void minorAndMajorCheck(float &_minor_data,float &_major_data);
    SPSA();
    //~SPSA();
};

#endif // APPROXIMATION_H_INCLUDED
