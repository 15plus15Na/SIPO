#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED
#include<string>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include "result.h"

using namespace std;

class Log
{
protected:
    static vector<float>  _log_reinsuranceParticipation;
    static vector<float>  _log_reinsuranceAttachmentPoint;
    static vector<float>  _log_reinsuranceMaximumPoint;
    static vector<float>  _log_bankrupcy;
    static vector<float>  _log_bankrupcy_Deviance;
    static vector<double> objectFunctionValue;
    static vector<double> grossObjectiveFunctionValue;
    static vector<double> grossObjectiveFunctionValueWithZero;
    static vector<double> grossObjectiveFunctionDeviance;
    static vector<double> grossObjectiveFunctionDevianceWithZero;
    static vector<double> smallestSolvencyRatio;
    static vector<double> biggestSolvencyRatio;
    static vector<vector<vector<float> > >  maximumCoverage;
    static vector<vector<vector<float> > >  minimumCoverage;
    static vector<vector<vector<float> > >  insuranceCoverage;
    static vector<double>  aggregatedPremium;
    static vector<double>  initialFund;
    void   writeDEPIntoCSV(string name);

public:
    static vector<vector<vector<vector<float> > > > dv;
    static double totalGrossLoss;
    static unsigned itr_count1;
    static vector<double> totalLoss;
    static vector<vector<float> > briefDV;
    enum analysisType{SPSA,DEP};
    void addSPSAIterationResults();
    void addNewObjectFunctuionValue(double);
    static void nextIteration();
    static void objectFunctionPushBack(double, double,double, double,double);
    static void decisionVariablePushBack(vector<vector<float> >, vector<vector<float> >);
    static void decisionVariablePushBack(vector<vector<float> >, vector<vector<float> >,vector<vector<float> >);
    static void one_DecisionVariablePushBack(float,float);
    static void bankrupcyPushBack(float ,float);
    static void writeResults(unsigned, string , InsuranceModel ,Result );
    static void writeSPSA(string name);
    static void intialFundPushBack(double);
    static void aggregatedPremiumPushBack(double);
    static void smallestAndBiggestSolvencyRatio(double,double);
    Log(analysisType);
    ~Log();
protected:
    enum analysisType _type;
};
#endif // LOG_H_INCLUDED
