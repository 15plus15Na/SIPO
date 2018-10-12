#ifndef INSURANCEMODEL_H_INCLUDED
#define INSURANCEMODEL_H_INCLUDED
#include<vector>
#include<string>
#include "inventory.h"
//#include "settings.h"
using namespace std;
class Inventory;
class InsuranceModel
{
    friend class Settings;
    friend class Log;
    friend class Inventory;
protected:
    //float    treatyReinsuranceParticipation;
    float    treatyReinsuranceAttachPoint;
    float    treatyReinsuranceMaximumPoint;
    //float    treatyReinsurancePremium;
    float    managementChargeRatio;
    float    overhead;
    float    minimumPremiumRatio;
    float    minimumCapital;
    float    bankruptcyValue;
    unsigned timeSpan;
    unsigned bankrupcy_time[2];
    vector<vector<float> > policyDeductible;
    vector<vector<float> > policyMaximum;
    vector<vector<double> > premium;
    //double   totalPremium;
    float bankrupcyProbability;
    double   beginingFund;
public:
    float    treatyReinsurancePremium;
    double   totalPremium;
    vector<double> fund;
    unsigned iF;
    vector <vector<float> > insuranceUsage;
    double totalDamage;
    bool   isBankrupt;
    void   updatePremium(const Inventory *, const bool);
    void   updateTreatyReinsurancePremium(Inventory *);
    void   setTreatyReinsuranceAttachmentPointByMaximumPremiumAsInsuredLoss(Inventory *);
    void   setTreatyReinsuranceMaximumPointTo(float);
    float  getTreatyReinsuranceAttachmentPoint();
    float  getTreatyReinsuranceMaximumPoint();
    float  getTreatyReinsurancePremium();
    void   updateReserveFund(double, double);
    void   nextTimeSpan(unsigned);
    void   updateTheLatestReserveZero();
    bool   ifReservebiggerThanZero();
    void   updateReserveFundWithZero();
    void   goBankrupt(unsigned, unsigned,float);
    double  getLatestFundValue();
    double  getBankruptcyValue();
    float  getLatestFundValueMinusOne();
    void   getInsuranceUsage(vector<float> &, unsigned) const;
    void   setPolicyConditionThresholds(const float ,const float, const Inventory *);
    void   initalGuessOfTurnOver(Inventory *, const string, float);
    void   initalGuessOfTurnOver(vector<vector<float> > );
    double getPremiumByLocationAndTypeIndex(unsigned, unsigned);
    float  getPolicyDeductibleRatio(unsigned, unsigned);
    float  getPolicyMaximumRatio(unsigned ,unsigned);
    double getFund(unsigned);
    float  getPolicyConditionDeductible(unsigned, unsigned ) const;
    float  getPolicyConditionMaximum(unsigned, unsigned ) const;
    void   setDecesionVariableIntoModel(float **, const Inventory *);
    void   setDecesionVariableIntoModel(vector<vector<vector<float> > > ,vector<float>);
    void   setDecesionVariableIntoModel(vector<vector<float> > , vector<vector<float> > );
    void   setDeductibleForAllLocationUniformly(float, unsigned, unsigned);
    void   setMaximumForAllLocationUniformly(float, unsigned, unsigned);
    float  getBankrupcyProbability();
    float  getTreatyReinsuranceParticipation();
    double calculateDamageAfterPolicyCondition(float, double, unsigned, unsigned);
    static double calculateDamageAfterWithThresholds(float, double,float,float);
    float  makeSureOfTheMinimumPremium(float,float);
    void   addToLog();
    void   addToLogDEP(float,float,float);
    void   cleanUpForNextRound();
    void   showPremium();
    void   showMinimum();
    void   showMaximum();
    double getTotalPremiums();
    double getTotalDamage();
    void   setBeginingFundMultipliedOfPremium(unsigned);
    void   setBeginingFund(double);
    double getSolvencyRatio();
    unsigned getTimeSpanIndex();
    InsuranceModel(const Inventory *, unsigned);
    InsuranceModel(const InsuranceModel *, const Inventory *);
    ~InsuranceModel();
};

inline unsigned InsuranceModel::getTimeSpanIndex()
{
    return iF;
}
inline double InsuranceModel::getSolvencyRatio()
{
    double _ratio=0;
    if(getLatestFundValue()>0)
    {
        //double temp=getLatestFundValue()-getTotalDamage();
        //if(temp>0)
            //
        _ratio=(getLatestFundValue())/minimumCapital;
    }
    return _ratio;
}

#endif // INSURANCEMODEL_H_INCLUDED
