#include <iostream>
#include <random>
#include <fstream>
#include <sstream>
#include <string>
#include "log.h"
#include "settings.h"
#include "stochastic.h"
using namespace std;
void StochasticModule::goDeepIntoTree(InsuranceModel *insurer, Result *res, Inventory *inv_Data, float cpt, unsigned iTimeSpan)
{
    if (iTimeSpan<timeSpan)
    {
        float          curProbablity;
        float          probability;
        unsigned       location_index;
        unsigned       damage_index;
        unsigned _IDTableIndex_End;
        float reinsuranceCoverageIncome;
        float treatyReinsurancePremium=insurer->getTreatyReinsurancePremium();
        for(unsigned hazard_index=0;hazard_index<inv_Data->getNumberOFHazards();hazard_index++)
        {
            //InsuranceModel temp(insurer, inv_Data);
            InsuranceModel temp=*insurer;
            probability=inv_Data->getProbabilityOfHazard(hazard_index);
            curProbablity=cpt*probability;
            if(!temp.isBankrupt)
            {
                reinsuranceCoverageIncome=0;
                _IDTableIndex_End=inv_Data->getEndingIDTableIndexOfHazard(hazard_index);
                for(unsigned j=inv_Data->getBeginingIDTableIndexOfHazard(hazard_index);j<=_IDTableIndex_End;j++)
                {
                    vector<float>  damage;
                    vector<float>  _insuranceUsage;
                    vector<float>  value;
                    location_index=inv_Data->getLocationIndexByIDTableIndex(j);
                    insurer->getInsuranceUsage(_insuranceUsage, location_index);
                    inv_Data->getValueOfProperty(value, location_index);
                    damage_index=inv_Data->getDamageIndexByIDtableIndex(j);
                    inv_Data->getDamagesOfTypes(damage, damage_index);
                    for(unsigned m=0;m<inv_Data->getNumberOfTypes();m++)
                    {
                        double currentPremium=temp.getPremiumByLocationAndTypeIndex(location_index,m);
                        reinsuranceCoverageIncome+=damage[m]*_insuranceUsage[m];
                        temp.updateReserveFund(damage[m]*_insuranceUsage[m], currentPremium);
                    }
                }
                reinsuranceCoverageIncome=temp.calculateDamageAfterWithThresholds(reinsuranceCoverageIncome,inv_Data->getSumOfPropetiesValueByInsuranceCoverageAndPolicyConditions(&temp),temp.getTreatyReinsuranceAttachmentPoint(),temp.getTreatyReinsuranceMaximumPoint());
                temp.updateReserveFund(treatyReinsurancePremium, reinsuranceCoverageIncome);
            }
            if(Settings::st_doesLogScenarioDetails)
                res->updateHistory(hazard_index,temp.getLatestFundValue(),curProbablity,iTimeSpan+1);
            res->updateSamallestSolvencyRatio(temp.getSolvencyRatio(),temp.getTimeSpanIndex());
            //cout<<temp.getSolvencyRatio()<<endl;
            temp.nextTimeSpan(hazard_index);
            goDeepIntoTree(&temp, res, inv_Data, curProbablity, iTimeSpan+1);
        }
    }
    else
    {
        res->branchCounter++;
        float latest=insurer->getLatestFundValueMinusOne();
        if(latest<0)
            latest=0;
        res->updateFinalValuesOfScenarios(cpt,latest,insurer->isBankrupt,insurer->getBankruptcyValue());
    }
}


StochasticModule::StochasticModule()
{
    timeSpan=Settings::st_timeSpan;
}
