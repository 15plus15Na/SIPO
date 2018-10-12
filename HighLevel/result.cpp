#include<iostream>
#include<math.h>
#include<string>
#include<sstream>
#include<conio.h>
#include "log.h"
#include "settings.h"

using namespace std;


void Result::updateHistory(unsigned i_value, float fund_value, float probability_value, unsigned timeSpan_value)
{
    if(timeSpan_value<=last_time_span_index){
        idH++;
        for(unsigned j=0;j<timeSpan;j++)
        {
            scenarioHistory[idH][j]=scenarioHistory[idH-1][j];
            fundHistory[idH][j]=fundHistory[idH-1][j];
            probabilityHistory[idH][j]=probabilityHistory[idH-1][j];
        }
    }
    scenarioHistory[idH][timeSpan_value-1]=i_value;
    fundHistory[idH][timeSpan_value-1]=fund_value;
    probabilityHistory[idH][timeSpan_value-1]=probability_value;
    last_time_span_index=timeSpan_value;
}

void Result::setHistoryIdentifiersToZeros()
{
    cout<<"idH="<<idH<<endl;
    cout<<"last_time_span_index="<<last_time_span_index<<endl;
    getch();
    idH=0;
    last_time_span_index=0;
}

void Result::scenarioHistory_NextLine()
{
    idH++;
}

void Result::showHistory()
{
    for(unsigned i=0;i<maxIndex;i++)
    {
        for(unsigned j=0;j<timeSpan;j++)
        {
            if(j!=0)
                cout<<" --> ";
            cout<<scenarioHistory[i][j];
        }
        cout<<endl;
    }
}

void Result::showFund()
{
    for(unsigned i=0;i<maxIndex;i++)
    {
        for(unsigned j=0;j<timeSpan;j++)
        {
            if(j!=0)
                cout<<" --> ";
            cout<<fundHistory[i][j];
        }
        cout<<endl;
    }
}


void Result::showProbability()
{
    for(unsigned i=0;i<maxIndex;i++)
    {
        for(unsigned j=0;j<timeSpan;j++)
        {
            if(j!=0)
                cout<<" --> ";
            cout<<probabilityHistory[i][j];
        }
        cout<<endl;
    }
}

void Result::showFinalResults()
{
    cout<<"* * * * * * * * * *"<<endl;
    for(unsigned i=0;i<idF;i++)
    {
        cout<<"Scenario   "<<i<<endl;
        cout<<"Fund= "<<scenarioFinalFund[i]<<endl;
        cout<<"Probability= "<<scenarioProbablity[i]<<endl;
        cout<<"is Bankrupt=";
        bankrupcy[i]?cout<<"Yes"<<endl:cout<<"No"<<endl;
        cout<<endl;
    }
}

void Result::updateFinalValuesOfScenarios(double cpt,float latest_fund,bool is_bankrupt,float _bv)
{
    bankruptcyValue[idF]=_bv;
    scenarioFinalFund[idF]=latest_fund;
    scenarioProbablity[idF]=cpt;
    bankrupcy[idF]=is_bankrupt;
    double smallNumberTemp=-9999999999;
    for(unsigned i=0;i<solvencyRatio_Smallest_temp.size();i++)
    {
        if(fabs(solvencyRatio_Smallest_temp[i])>smallNumberTemp)
        {
            smallNumberTemp=fabs(solvencyRatio_Smallest_temp[i]);
        }
    }
    solvencyRatio_Smallest+=smallNumberTemp*cpt;
    solvencyRatio_Biggest+=solvencyRatio_Biggest_temp*cpt;
    idF++;
}

void Result::setLatestFund(float value)
{
    scenarioFinalFund[idF]=value;
}

void Result::setLatestBankrupcy(bool ivalue)
{
    bankrupcy[idF]=ivalue;
}

void Result::setLatestProbablity(double p)
{
    scenarioProbablity[idF]=p;
}

void Result::finalValueWithPenalty(InsuranceModel *insurer)
{
    culmativeBankrupcy=0;
    double noneMinusMean=0;
    unsigned bankrupcy_penalty=false;
    double scenarioFinalFund_NoneMinus=0;
    /////////////////////  Objective function Without Zero  /////////////////////
    for(unsigned i=0;i<maxIndex;i++)
    {
        if(scenarioFinalFund[i]>0)
            scenarioFinalFund_NoneMinus=scenarioFinalFund[i];
        else
            scenarioFinalFund_NoneMinus=0;
        noneMinusMean+=scenarioFinalFund_NoneMinus*scenarioProbablity[i];
        if(bankrupcy[i]==true)
            culmativeBankrupcy+=scenarioProbablity[i];
    }
    /////////////////////  Bankruptcy Deviance  /////////////////////
    culmativeBankrupcyDeviance=0;
    for(unsigned i=0;i<maxIndex;i++)
    {
        if(bankrupcy[i]==true)
            culmativeBankrupcyDeviance+=scenarioProbablity[i]*(culmativeBankrupcy-1)*(culmativeBankrupcy-1);
        else
            culmativeBankrupcyDeviance+=scenarioProbablity[i]*(culmativeBankrupcy)*(culmativeBankrupcy);
    }
    culmativeBankrupcyDeviance=sqrt(culmativeBankrupcyDeviance);
    /////////////////////  Objective function Deviance  /////////////////////
    double noneMinusVar=0;
    for(unsigned i=0;i<maxIndex;i++)
    {
        if(bankrupcy[i]==false)
            scenarioFinalFund_NoneMinus=scenarioFinalFund[i];
        else

        noneMinusVar+=scenarioProbablity[i]*(noneMinusMean-scenarioFinalFund_NoneMinus)*(noneMinusMean-scenarioFinalFund_NoneMinus);
    }
    /////////////////////  Objective function With Zeros  /////////////////////
    double grossMean=0;
    for(unsigned i=0;i<maxIndex;i++)
    {
        if(bankrupcy[i]==false)
        {
            grossMean+=scenarioFinalFund[i]*scenarioProbablity[i];

        }
        else
        {
            grossMean+=bankruptcyValue[i]*scenarioProbablity[i];
        }
    }
    /////////////////////  Objective function With Zeros Deviance  /////////////////////
    double grossVar=0;
    for(unsigned i=0;i<maxIndex;i++)
    {
        if(bankrupcy[i]==false)
            grossVar+=scenarioProbablity[i]*(grossMean-scenarioFinalFund[i])*(grossMean-scenarioFinalFund[i]);
        else
            grossVar+=scenarioProbablity[i]*(grossMean-bankruptcyValue[i])*(grossMean-bankruptcyValue[i]);
    }

    /////////////////////  Show  /////////////////////
    //cout<<"Bankruptcy Probability="<<culmativeBankrupcy<<endl;
    if(culmativeBankrupcy>insurer->getBankrupcyProbability())
    {
        bankrupcy_penalty=true;
        //cout<<"bankruptcy rate exceeded: "<<culmativeBankrupcy<<endl;;
    }
    /////////////////////  Penalty Value  /////////////////////
    //cout<<"sum of funds="<<noneMinusMean<<"   "<<"gross mean="<<grossMean<<endl;
    float penalty=0;
    if(bankrupcy_penalty)
    {
        penalty=(insurer->getBankrupcyProbability()-culmativeBankrupcy)*-100;//*(insurer->getBankrupcyProbability()-culmativeBankrupcy)*10000;
    }
    vector<float> coefficients=Settings::st_ObjectiveFunctionCoefficient;
    float smallestRatio=0;
    if(solvencyRatio_Smallest<1)
    {
        smallestRatio=fabs(1-solvencyRatio_Smallest);
    }
    //cout<<"Penalty="<<coefficients[1]*penalty<<" & " <<coefficients[2]*smallestRatio<<endl;
    objectiveFunctionValue=coefficients[0]*noneMinusMean-coefficients[1]*penalty-coefficients[2]*smallestRatio;
    //cout<<"objective Function="<<objectiveFunctionValue<<endl;
    grossObjectiveFunctionValue=noneMinusMean;
    grossObjectiveFunctionValueWithZero=grossMean;
    grossObjectiveFunctionDeviance=sqrt(noneMinusVar);
    grossObjectiveFunctionDevianceWithZero=sqrt(grossVar);
}

void Result::addToLog()
{
     Log::objectFunctionPushBack(objectiveFunctionValue,grossObjectiveFunctionValue,grossObjectiveFunctionDeviance,grossObjectiveFunctionValueWithZero,grossObjectiveFunctionDevianceWithZero);
     Log::bankrupcyPushBack(culmativeBankrupcy,culmativeBankrupcyDeviance);
     Log::smallestAndBiggestSolvencyRatio(solvencyRatio_Smallest,solvencyRatio_Biggest);
}

double Result::getObjectiveValue()
{
    return objectiveFunctionValue;
}

Result::Result(const std::string isFull,const unsigned option,const unsigned timeSpan_value)
{
    solvencyRatio_Biggest=0;
    solvencyRatio_Smallest=0;
    solvencyRatio_Smallest_temp.resize(timeSpan_value,0);
    solvencyRatio_Biggest_temp=-9999999999999;
    culmativeBankrupcy=0;
    objectiveFunctionValue=0;
    grossObjectiveFunctionValue=0;
    penaltyFunctionValue=0;
    branchCounter=0;
    last_time_span_index=0;
    timeSpan=timeSpan_value;
    idH=0;
    idF=0;
    if(isFull=="FULL")
    {
        maxIndex=pow(double(option),double(option));
    }
    else if(isFull=="PARTIAL")
    {
        maxIndex=pow(double(option),double(timeSpan_value));
    }
    else
    {
        std::cout<<'"'<<"isFULL"<<'"'<<" does not have a legitimate value: "<<isFull<<std::endl;
    }
    if(Settings::st_doesLogScenarioDetails)
    {
        scenarioHistory.resize(maxIndex);
        fundHistory.resize(maxIndex);
        probabilityHistory.resize(maxIndex);
        for(unsigned i=0;i<maxIndex;i++)
        {
            fundHistory[i].resize(timeSpan);
            probabilityHistory[i].resize(timeSpan);
            scenarioHistory[i].resize(timeSpan);
        }
    }
    bankruptcyValue.resize(maxIndex,0);
    scenarioProbablity.resize(maxIndex,0);
    scenarioFinalFund.resize(maxIndex,0);
    bankrupcy.resize(maxIndex,false);
}

Result::~Result()
{
    //for(unsigned i=0;i<)
}
