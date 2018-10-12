#include <iostream>
#include "log.h"
#include <algorithm>
#include <conio.h>
#include "settings.h"
#include "insurancemodel.h"

/** Protected Functions
 */

void InsuranceModel::updatePremium(const Inventory *inv_Data, const bool ifConsiderPolicyConditions)
{
    totalDamage=0;
    vector<float> damage1;
    float    insuranceLossAfterPolicyConditions;
    float    probability;
    unsigned location_index;
    unsigned damage_index;
    unsigned hazard_index;
    premium.resize(inv_Data->getNumberOfLocations());
    for(unsigned i=0;i<inv_Data->getNumberOfLocations();i++)
        premium[i].resize(inv_Data->getNumberOfTypes(),0);
    vector <float> _insuranceUsage;
    vector <float> value;
    for (unsigned i=0; i<inv_Data->getNumberOFIDTable(); i++)
    {
        location_index=inv_Data->getLocationIndexByIDTableIndex(i);
        getInsuranceUsage(_insuranceUsage, location_index);
        inv_Data->getValueOfProperty(value, location_index);
        damage_index=inv_Data->getDamageIndexByIDtableIndex(i);
        if(ifConsiderPolicyConditions)
            inv_Data->getDamagesOfTypes(damage1, damage_index);
        else
            inv_Data->getGrossDamagesOfTypes(damage1, damage_index);
        hazard_index=inv_Data->getHazardIndexByIDTableIndex(i);
        probability=inv_Data->getProbabilityOfHazard(hazard_index);
        for (unsigned j=0;j<inv_Data->getNumberOfTypes();j++)
        {
            premium[location_index][j]+=damage1[j]*probability*_insuranceUsage[j]*(1+managementChargeRatio)*overhead;
            totalDamage+=damage1[j]*probability*_insuranceUsage[j];
        }
    }
}

void InsuranceModel::updateTreatyReinsurancePremium(Inventory *inv_Data)
{
    //inv_Data->prepareTreatyReinsuredDamageType(this);
    vector<float> damage1;
    float    probability;
    unsigned location_index;
    unsigned damage_index;
    unsigned _IDTableIndex_End;
    treatyReinsurancePremium=0;
    float expectedReinsuredLoss=0;
    float variance=0;
    double sumOfDamage_PerHazard=0;

    for(unsigned hazard_index=0;hazard_index<inv_Data->getNumberOFHazards();hazard_index++)
    {
        sumOfDamage_PerHazard=0;
        probability=inv_Data->getProbabilityOfHazard(hazard_index);
        _IDTableIndex_End=inv_Data->getEndingIDTableIndexOfHazard(hazard_index);
        for(unsigned j=inv_Data->getBeginingIDTableIndexOfHazard(hazard_index);j<=_IDTableIndex_End;j++)
        {
            location_index=inv_Data->getLocationIndexByIDTableIndex(j);
            damage_index=inv_Data->getDamageIndexByIDtableIndex(j);
            inv_Data->getDamagesOfTypes(damage1, damage_index);
            for (unsigned m=0;m<inv_Data->getNumberOfTypes();m++)
            {
                sumOfDamage_PerHazard+=damage1[m]*insuranceUsage[location_index][m];
            }
        }
        //cout<<"Sum per haz1="<<sumOfDamage_PerHazard<<endl;
        sumOfDamage_PerHazard=calculateDamageAfterWithThresholds(sumOfDamage_PerHazard,inv_Data->getSumOfPropetiesValueByInsuranceCoverageAndPolicyConditions(this),treatyReinsuranceAttachPoint,treatyReinsuranceMaximumPoint);
        //cout<<"Sum per haz2="<<sumOfDamage_PerHazard<<endl;
        expectedReinsuredLoss+=probability*sumOfDamage_PerHazard;
        treatyReinsurancePremium+=(1+Settings::st_reinsuranceOverhead)*probability*sumOfDamage_PerHazard;
        //getch();
    }
    //cout<<"getSumOfPropetiesValue="<<inv_Data->getSumOfPropetiesValueByInsuranceCoverageAndPolicyConditions(this)<<endl;

    for(unsigned hazard_index=0;hazard_index<inv_Data->getNumberOFHazards();hazard_index++)
    {
        sumOfDamage_PerHazard=0;
        probability=inv_Data->getProbabilityOfHazard(hazard_index);
        _IDTableIndex_End=inv_Data->getEndingIDTableIndexOfHazard(hazard_index);
        for(unsigned j=inv_Data->getBeginingIDTableIndexOfHazard(hazard_index);j<=_IDTableIndex_End;j++)
        {
            location_index=inv_Data->getLocationIndexByIDTableIndex(j);
            damage_index=inv_Data->getDamageIndexByIDtableIndex(j);
            inv_Data->getDamagesOfTypes(damage1, damage_index);
            for (unsigned m=0;m<inv_Data->getNumberOfTypes();m++)
            {
                sumOfDamage_PerHazard+=damage1[m]*insuranceUsage[location_index][m];
            }
        }
        sumOfDamage_PerHazard=calculateDamageAfterWithThresholds(sumOfDamage_PerHazard,inv_Data->getSumOfPropetiesValueByInsuranceCoverageAndPolicyConditions(this),treatyReinsuranceAttachPoint,treatyReinsuranceMaximumPoint);

        variance+=probability*(sumOfDamage_PerHazard/**treatyReinsuranceParticipation*/-expectedReinsuredLoss)*(sumOfDamage_PerHazard/**treatyReinsuranceParticipation*/-expectedReinsuredLoss);
    }
    float deviance=sqrt(variance);
    //cout<<"treatyReinsurancePremium="<<treatyReinsurancePremium<<endl;
    treatyReinsurancePremium=treatyReinsurancePremium;//+Settings::st_treatyReinsurancePremiumAddingDeviance*deviance;
    //cout<<"Treaty Reinsurance Premium1="<<treatyReinsurancePremium<<endl;
}

void InsuranceModel::setTreatyReinsuranceAttachmentPointByMaximumPremiumAsInsuredLoss(Inventory *inv_Data)
{
    treatyReinsuranceAttachPoint=getTotalPremiums()/inv_Data->getSumOfPropetiesValueByInsuranceCoverageAndPolicyConditions(this);
    //cout<<treatyReinsuranceAttachPoint<<endl;
}

void InsuranceModel::setTreatyReinsuranceMaximumPointTo(float rmp)
{
    treatyReinsuranceMaximumPoint=rmp;
}

float InsuranceModel::getTreatyReinsuranceAttachmentPoint()
{
    return treatyReinsuranceAttachPoint;
}

float InsuranceModel::getTreatyReinsuranceMaximumPoint()
{
    return treatyReinsuranceMaximumPoint;
}

float InsuranceModel::getTreatyReinsurancePremium()
{
    return treatyReinsurancePremium;
}

void InsuranceModel::updateReserveFund(double damage_value,double _premium)
{
    fund[iF]+=_premium-damage_value;
}

void InsuranceModel::nextTimeSpan(unsigned hazard_Index)
{
    if(fund[iF]>=0&&isBankrupt==false)
    {
        if(iF!=timeSpan-1)
            fund[iF+1]=fund[iF];
        iF++;
    }
    else
    {
        if(isBankrupt==false)
            goBankrupt(hazard_Index,iF,float(getLatestFundValue()));
        if(iF!=timeSpan-1)
            fund[iF+1]=0;
        iF++;
    }
}

void InsuranceModel::updateTheLatestReserveZero()
{
    fund[iF-1]=0;
}

bool InsuranceModel::ifReservebiggerThanZero()
{
    if (fund[iF-1]>0)
        return 1;
    else
        return 0;
}

void InsuranceModel::updateReserveFundWithZero()
{
    fund[iF++]=0;
}

void InsuranceModel::goBankrupt(unsigned i_value, unsigned iTimeSpan_value,float _bv)
{
    isBankrupt=true;
    bankrupcy_time[0]=i_value;
    bankrupcy_time[1]=iTimeSpan_value;
    bankruptcyValue=_bv;
}

double InsuranceModel::getLatestFundValue()
{
    return fund[iF];
}

float InsuranceModel::getLatestFundValueMinusOne()
{
    return fund[iF-1];
}

double InsuranceModel::getBankruptcyValue()
{
    return bankruptcyValue;
}

void InsuranceModel::setPolicyConditionThresholds(const float d_input, const float m_input, const Inventory *inv_Data)
{
    for(unsigned i=0;i<inv_Data->getNumberOfLocations();i++)
    {
        for(unsigned j=0;j<inv_Data->getNumberOfTypes();j++)
        {
            float d,m;
            if (d_input>=0 && d_input<1)
                d=d_input;
            else
            {
                std::cout<<"Warning: input deductible is out of range. It has been replaced by zero."<<std::endl;
                d=0;
            }
            if (m_input>0 && m_input<=1)
                m=m_input;
            else
            {
                std::cout<<"Warning: input maximum is out of range. It has been replaced by one."<<std::endl;
                m=1;
            }
            if(d>m)
            {
                std::cout<<"Warning: deductible value is bigger than maximum. Values are swapped."<<std::endl;
                float temp;
                temp=d;
                d=m;
                m=temp;
            }
            else if(d==m)
            {
                std::cout<<"Warning: deductible and maximum are the same. Nothing has been done."<<std::endl;
            }
            policyDeductible[i][j]=d;
            policyMaximum[i][j]=m;
        }
    }
}

void InsuranceModel::initalGuessOfTurnOver(Inventory *inv_Data, const std::string s_ch, float _value)
{
    if(s_ch=="uniform")
    {
        for(unsigned i=0;i<inv_Data->getNumberOfLocations();i++)
        {
            for(unsigned j=0;j<inv_Data->getNumberOfTypes();j++)
            {
                insuranceUsage[i][j]=_value;
            }
        }
    }
    else
        std::cout<<"error 1"<<std::endl;
}

void InsuranceModel::initalGuessOfTurnOver(vector<vector<float> > turnover)
{
    insuranceUsage=turnover;
}


void InsuranceModel::getInsuranceUsage(std::vector<float> &_UseRatio, unsigned location_index) const
{
    _UseRatio.resize(insuranceUsage[location_index].size(),0);
    for(unsigned i=0;i<insuranceUsage[location_index].size();i++)
    {
        _UseRatio[i]=insuranceUsage[location_index][i];
    }
}

double InsuranceModel::getPremiumByLocationAndTypeIndex(unsigned location_index, unsigned type_index)
{
    return premium[location_index][type_index];
}

double InsuranceModel::getFund(unsigned i)
{
    return fund[i];
}

float InsuranceModel::getPolicyConditionDeductible(unsigned loc, unsigned typ) const
{
    return policyDeductible[loc][typ];
}

float InsuranceModel::getPolicyConditionMaximum(unsigned loc, unsigned typ) const
{
    return policyMaximum[loc][typ];
}

void InsuranceModel::setDecesionVariableIntoModel(float **_DV, const Inventory *inv_Data)
{
    for(unsigned i=0;i<inv_Data->getNumberOfLocations();i++)
    {
        for(unsigned j=0;j<inv_Data->getNumberOfTypes();j++)
        {
            policyDeductible[i][j]=_DV[i][2*j];
            policyMaximum[i][j]=_DV[i][2*j+1];
        }
    }
}

void  InsuranceModel::setDecesionVariableIntoModel(vector<vector<vector<float> > > _DV_Vector, vector<float> _UDV)
{
    for(unsigned i=0;i<_DV_Vector.size();i++)
    {
        for(unsigned j=0;j<_DV_Vector[i].size();j++)
        {
            policyDeductible[i][j]=_DV_Vector[i][j][0];
            policyMaximum[i][j]=_DV_Vector[i][j][1];
            insuranceUsage[i][j]=_DV_Vector[i][j][2];
        }
    }
    //treatyReinsuranceParticipation=_UDV[0];
    treatyReinsuranceAttachPoint=_UDV[0];
    treatyReinsuranceMaximumPoint=_UDV[1];
}

void  InsuranceModel::setDecesionVariableIntoModel(vector<vector<float> > _DV_Vector_min, vector<vector<float> > _DV_Vector_max)
{
    policyDeductible=_DV_Vector_min;
    policyMaximum=_DV_Vector_max;
}

void InsuranceModel::setDeductibleForAllLocationUniformly(float deduc,unsigned begIndex,unsigned endIndex)
{
    for(unsigned i=0;i<policyDeductible.size();i++)
    {
        for(unsigned j=begIndex;j<=endIndex;j++)
        {
            policyDeductible[i][j]=deduc;
        }
    }
}

void InsuranceModel::setMaximumForAllLocationUniformly(float maxim,unsigned begIndex,unsigned endIndex)
{
    for(unsigned i=0;i<policyMaximum.size();i++)
    {
        for(unsigned j=begIndex;j<=endIndex;j++)
        {
            policyMaximum[i][j]=maxim;
        }
    }
}

float InsuranceModel::getBankrupcyProbability()
{
    return bankrupcyProbability;
}

/*float InsuranceModel::getTreatyReinsuranceParticipation()
{
     return treatyReinsuranceParticipation;
}*/

double InsuranceModel::calculateDamageAfterPolicyCondition(float damage, double value, unsigned location_index, unsigned type)
{
    double insuranceLossAfterPolicyConditions=0;
    float minimum_ratio=getPolicyConditionDeductible(location_index, type);
    float maximum_ratio=getPolicyConditionMaximum(location_index, type);
    if(maximum_ratio>1 || minimum_ratio<0)
    {
        cout<<"Inside calculateDamageAfterPolicyCondition maximumRatio or minimumRatio is not good"<<endl;
        getch();
    }
    if(damage<=minimum_ratio*value)
        insuranceLossAfterPolicyConditions=0;
    else if(damage>=maximum_ratio*value)
        insuranceLossAfterPolicyConditions=(maximum_ratio-minimum_ratio)*value;
    else
        insuranceLossAfterPolicyConditions=damage-minimum_ratio*value;
    return insuranceLossAfterPolicyConditions;
}

double InsuranceModel::calculateDamageAfterWithThresholds(float damage, double value,float _down,float _up)
{
    double out=0;
    if(damage<value*_down)
        out=0;
    else if(damage>value*_up)
            out=value*(_up-_down);
    else
        out=damage-value*_down;
    return out;
}

float InsuranceModel::makeSureOfTheMinimumPremium(float insuranceLossAfterPolicyConditions,float value)
{
    if(insuranceLossAfterPolicyConditions>=minimumPremiumRatio*value)
        return insuranceLossAfterPolicyConditions;
    else
        return minimumPremiumRatio*value;
}

void InsuranceModel::addToLog()
{
    Log::decisionVariablePushBack(policyDeductible, policyMaximum,insuranceUsage);
    Log::one_DecisionVariablePushBack(treatyReinsuranceAttachPoint,treatyReinsuranceMaximumPoint);
    Log::intialFundPushBack(beginingFund);
    Log::aggregatedPremiumPushBack(totalPremium);
}

void  InsuranceModel::addToLogDEP(float _min,float _max,float _cov)
{
    vector<float> temp;
    temp.push_back(_min);
    temp.push_back(_max);
    temp.push_back(_cov);
    Log::briefDV.push_back(temp);
}

void  InsuranceModel::cleanUpForNextRound()
{
    isBankrupt=false;
    fund.resize(timeSpan,0);
    for(unsigned i=0;i<premium.size();i++)
    {
        for(unsigned j=0;j<premium[i].size();j++)
        {
            premium[i][j]=0;
        }

    }
    //for(unsigned i=0;i<premium.size();i++)
        //premium[i].clear();
    //premium.clear();
    iF=0;
    fund[0]=beginingFund;
    isBankrupt=false;
}

void InsuranceModel::showPremium()
{
    for(unsigned i=0;i<premium.size();i++)
    {
        for(unsigned j=0;j<premium[i].size();j++)
        {
            cout<<premium[i][j]<<"   ";
        }
        cout<<endl;
    }
}

void InsuranceModel::showMinimum()
{
    for(unsigned i=0;i<policyDeductible.size();i++)
    {
        for(unsigned j=0;j<policyDeductible[i].size();j++)
        {
            cout<<policyDeductible[i][j]<<"   ";
        }
        cout<<endl;
    }
}

void InsuranceModel::showMaximum()
{
    for(unsigned i=0;i<policyMaximum.size();i++)
    {
        for(unsigned j=0;j<policyMaximum[i].size();j++)
        {
            cout<<policyMaximum[i][j]<<"   ";
        }
        cout<<endl;
    }
}


double InsuranceModel::getTotalPremiums()
{
    double out=0;
    for(unsigned i=0;i<premium.size();i++)
    {
        for(unsigned j=0;j<premium[i].size();j++)
        {
            out+=premium[i][j];
        }
    }
    totalPremium=out;
    return out;
}

double InsuranceModel::getTotalDamage()
{
    return totalDamage;
}

void  InsuranceModel::setBeginingFundMultipliedOfPremium(unsigned mlt)
{
    beginingFund=mlt*getTotalPremiums();
    fund[0]=beginingFund;
    //cout<<beginingFund<<"   "<<minimumCapital<<endl;
}

void InsuranceModel::setBeginingFund(double bf)
{
    getTotalPremiums();
    beginingFund=bf;
    fund[0]=bf;
}

InsuranceModel::InsuranceModel(const Inventory *inv_Data, unsigned timeSpan_value)
{
//    treatyReinsuranceParticipation=0;
    treatyReinsuranceAttachPoint=0;
    treatyReinsuranceMaximumPoint=0.01;
    treatyReinsurancePremium=0;
    minimumCapital=Settings::st_minimumCapital;
    bankruptcyValue=0;
    totalPremium=0;
    beginingFund=0;
    minimumPremiumRatio=Settings::st_minimumPremiumRatio;
    policyDeductible.resize(inv_Data->getNumberOfLocations());
    policyMaximum.resize(inv_Data->getNumberOfLocations());
    insuranceUsage.resize(inv_Data->getNumberOfLocations());
    premium.resize(inv_Data->getNumberOfLocations());
    for(unsigned i=0;i<inv_Data->getNumberOfLocations();i++)
    {
        policyDeductible[i].resize(inv_Data->getNumberOfTypes(),0);
        policyMaximum[i].resize(inv_Data->getNumberOfTypes(),0);
        insuranceUsage[i].resize(inv_Data->getNumberOfTypes(),0);
        premium[i].resize(inv_Data->getNumberOfTypes(),0);
        /*for(unsigned j=0;j<inv_Data->getNumberOfTypes();j++)
        {
            policyDeductible[i][j]=0;
            policyMaximum[i][j]=0;
        }*/
    }
    isBankrupt=false;
    timeSpan=timeSpan_value;
    fund.resize(timeSpan,0);
    iF=0;
    fund[0]=Settings::st_beginingFund;
    managementChargeRatio=Settings::st_managmentCostsRatio;
    overhead=Settings::st_premiumOverheadratio;
    bankrupcyProbability=Settings::st_maximumAcceptableProbability;
}

InsuranceModel::InsuranceModel(const InsuranceModel *temp, const Inventory *inv_Data)
{
    if(this!=temp)
    {
//        treatyReinsuranceParticipation=temp->treatyReinsuranceParticipation;
        treatyReinsuranceAttachPoint=temp->treatyReinsuranceAttachPoint;
        treatyReinsuranceMaximumPoint=temp->treatyReinsuranceMaximumPoint;
        treatyReinsurancePremium=temp->treatyReinsurancePremium;
        overhead=temp->overhead;
        managementChargeRatio=temp->managementChargeRatio;
        bankruptcyValue=temp->bankruptcyValue;
        totalDamage=temp->totalDamage;
        minimumCapital=temp->minimumCapital;
        totalPremium=temp->totalPremium;
        beginingFund=temp->beginingFund;
        minimumPremiumRatio=temp->minimumPremiumRatio;
        policyDeductible.resize(temp->policyDeductible.size());
        policyMaximum.resize(temp->policyMaximum.size());
        if(temp->policyDeductible.size()!=temp->policyMaximum.size())
        {
            cout<<"Serious error InsuranceModel copy construct 1"<<endl;
            exit(1);
        }
        for(unsigned i=0;i<inv_Data->getNumberOfLocations();i++)
        {
            policyDeductible[i].resize(temp->policyDeductible[i].size());
            policyMaximum[i].resize(temp->policyMaximum[i].size());
            if(temp->policyDeductible[i].size()!=temp->policyMaximum[i].size())
            {
                cout<<"Serious error InusranceModel copy construct 2"<<endl;
                exit(1);
            }
            for(unsigned j=0;j<inv_Data->getNumberOfTypes();j++)
            {
                policyDeductible[i][j]=temp->policyDeductible[i][j];
                policyMaximum[i][j]=temp->policyMaximum[i][j];
            }
        }
        bankrupcyProbability=temp->bankrupcyProbability;
        premium.resize(temp->premium.size());
        for(unsigned i=0;i<temp->premium.size();i++)
        {
            premium[i].resize(temp->premium[i].size());
            for(unsigned j=0;j<temp->premium[i].size();j++)
            {
                premium[i][j]=temp->premium[i][j];
            }
        }
        insuranceUsage.resize(temp->insuranceUsage.size());
        for (unsigned i=0;i<temp->insuranceUsage.size();i++)
        {
            insuranceUsage[i].resize(temp->insuranceUsage[i].size());
            for (unsigned j=0;j<temp->insuranceUsage[i].size();j++)
            {
                insuranceUsage[i][j]=temp->insuranceUsage[i][j];
            }
        }
        bankrupcy_time[0]=temp->bankrupcy_time[0];
        bankrupcy_time[1]=temp->bankrupcy_time[1];
        isBankrupt=temp->isBankrupt;
        fund.resize(temp->timeSpan);
        iF=temp->iF;
        timeSpan=temp->timeSpan;
        for(unsigned i=0; i<=temp->iF;i++)
        {
            fund[i]=temp->fund[i];
        }
    }
    else
    {
        cout<<" -------------------------------------- "<<endl;
        cout<<"|error: InsuranceModel is equal to this|"<<endl;
        cout<<" -------------------------------------- "<<endl;
    }
}

InsuranceModel::~InsuranceModel()
{
}
