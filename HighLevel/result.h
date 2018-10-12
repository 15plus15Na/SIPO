#ifndef RESULT_H_INCLUDED
#define RESULT_H_INCLUDED
#include<string>
#include "insurancemodel.h"


using namespace std;

class InsuranceModel;

class Result
{
    friend class Log;
protected:
    vector<vector<unsigned> > scenarioHistory;
    vector<float> scenarioFinalFund;
    vector<vector<float> > probabilityHistory;
    vector<double> scenarioProbablity;
    vector<float>  bankruptcyValue;
    vector<bool> bankrupcy;
    float  culmativeBankrupcy;
    float  culmativeBankrupcyDeviance;
    float  st_minimumPremiumRatio;
    double noneMinusMean;
    double objectiveFunctionValue;
    double grossObjectiveFunctionValue;
    double grossObjectiveFunctionValueWithZero;
    double grossObjectiveFunctionDeviance;
    double grossObjectiveFunctionDevianceWithZero;
    double penaltyFunctionValue;
    double solvencyRatio_Smallest;
    double solvencyRatio_Biggest;
    vector<double> solvencyRatio_Smallest_temp;
    double solvencyRatio_Biggest_temp;
    unsigned idH;
    unsigned idF;
    unsigned maxIndex;
    unsigned timeSpan;
    unsigned last_time_span_index;
public:
    //float    **fundHistory;
    vector<vector<float> > fundHistory;
    unsigned branchCounter;
    void setLatestProbablity(double);
    void pushBackTimeHistoryFromPointer(unsigned **);
    void updateHistory(unsigned, float, float, unsigned);
    void scenarioHistory_NextLine();
    void showHistory();
    void showFund();
    void showProbability();
    void showFinalResults();
    void updateSamallestSolvencyRatio(double ,unsigned);
    void updateBiggestSolvencyRatio(double);
    void updateFinalValuesOfScenarios(double ,float ,bool,float);
    void setLatestFund(float);
    void setLatestBankrupcy(bool);
    void finalValueWithPenalty(InsuranceModel *);
    void setHistoryIdentifiersToZeros();
    void addToLog();
    double getObjectiveValue();
    Result(const string, const unsigned, const unsigned);
    ~Result();
};

inline void Result::updateSamallestSolvencyRatio(double _ratio_, unsigned tpi)
{
        solvencyRatio_Smallest_temp[tpi]=_ratio_;
}

inline void Result::updateBiggestSolvencyRatio(double _ratio_)
{
    if(fabs(_ratio_>solvencyRatio_Biggest_temp))
        solvencyRatio_Biggest_temp=_ratio_;
}

#endif // RESULT_H_INCLUDED
