#include<iostream>
#include<math.h>
#include "approximation.h"
#include "stochastic.h"
#include "log.h"
#include "settings.h"
#include "inventory.h"
using namespace std;

DEP::DEP(unsigned p, unsigned t,unsigned dvp)
{
    place=p;
    type=t;
    x.resize(dvp);
    low_bound.resize(dvp,0);
    up_bound.resize(dvp,0);
    part.resize(dvp,0);
    candidate.resize(place*type);
    soot=0;
}

void DEP::setLimits(unsigned i,float l, float u)
{
    low_bound[i-1]=l;
    up_bound[i-1]=u;
}

void DEP::setParts(unsigned i,float p)
{
    part[i-1]=fabs(p);
}

void DEP::descrtisizeEqualParts()
{
    for(unsigned i=0;i<x.size();i++)
    {
        unsigned numberOfParts=floor(fabs(up_bound[i]-low_bound[i])/part[i]+0.001)+1;
        x[i].resize(numberOfParts,0);
        for(unsigned j=0;j<numberOfParts;j++)
        {
            x[i][j]=low_bound[i]+j*part[i];
        }
    }
}

void DEP::calculateDiffrentParts(InsuranceModel insurer,Inventory *inventoryData/*,vector<Result> &res*/)
{
    vector<float> temp_candidate;
    temp_candidate.resize(x.size(),0);
    vector<vector<float> > single_candidate;
    goDeepForSingle(0,temp_candidate,single_candidate);
    goDeep(0,single_candidate);
    if(final_candidate.size()!=getNumberOfInstances())
    {
        cout<<"number of instances and candidate sizes are not the same. No Action will take place"<<endl;
    }
    else
    {
        for(unsigned i=0;i<final_candidate.size();i++)
        {
            cout<<i<<endl;
            InsuranceModel temp_insurer=insurer;
            Result  res("PARTIAL",inventoryData->getNumberOFHazards(),Settings::st_timeSpan);
            temp_insurer.cleanUpForNextRound();
            temp_insurer.setDeductibleForAllLocationUniformly(final_candidate[i][0][0],0,8);
            temp_insurer.setMaximumForAllLocationUniformly(final_candidate[i][0][1],0,8);
            temp_insurer.initalGuessOfTurnOver(inventoryData, "uniform", final_candidate[i][0][2]);
            cout<<final_candidate[i][0][0]<<"   "<<final_candidate[i][0][1]<<"   "<<final_candidate[i][0][2]<<endl;
            inventoryData->prepareDamageType(&temp_insurer);
            temp_insurer.updatePremium(inventoryData,true);
            temp_insurer.setBeginingFundMultipliedOfPremium(30);
            StochasticModule stm;
            cout<<"go deep"<<endl;
            stm.goDeepIntoTree(&temp_insurer,&res,inventoryData,1,0);
            cout<<"done"<<endl;
            res.finalValueWithPenalty(&temp_insurer);
            res.addToLog();
            temp_insurer.addToLog();
            temp_insurer.addToLogDEP(final_candidate[i][0][0],final_candidate[i][0][1],final_candidate[i][0][2]);
            inventoryData->addToLog();
            if(Settings::st_doesLogScenarioDetails)
                Log::writeResults(i,"ap",temp_insurer,res);
        }
    }
}

void DEP::goDeep(unsigned c,vector<vector<float> >single_candidate)
{
    if(c<place*type)
    {
        for(unsigned i=0;i<single_candidate.size();i++)
        {
            candidate[c]=single_candidate[i];
            goDeep(c+1,single_candidate);
        }
    }
    else
    {
        final_candidate.push_back(candidate);
    }
}

void DEP::goDeepForSingle(unsigned c,vector<float> temp_candidate,vector<vector<float> > &single_candidate)
{
    if(c<x.size())
    {
        for(unsigned i=0;i<x[c].size();i++)
        {
            temp_candidate[c]=x[c][i];
            goDeepForSingle(c+1,temp_candidate,single_candidate);
        }
    }
    else
    {
        single_candidate.push_back(temp_candidate);
    }
}

inline void DEP::setTripleSize(vector<vector<vector<float> > > vec,unsigned first,unsigned second,unsigned third)
{
    vec.resize(first);
    for(unsigned i=0;i<first;i++)
    {
        vec[i].resize(second);
        for(unsigned j=0;j<second;j++)
        {
            vec[i][j].resize(third,0);
        }
    }
}

unsigned DEP::getNumberOfInstances()
{
    unsigned out=1;
    for(unsigned i=0;i<x.size();i++)
    {
        out*=x[i].size();
    }
    out=pow(out,place*type);
    return out;
}

///////////////////////////////////////////////////////////////////////////////

void SPSA::beginIteration(InsuranceModel insurer, Inventory *inv_Data)
{
    InsuranceModel temp_insurer=insurer;
    copyInitialValueOfDecisionVariables(temp_insurer, inv_Data);
    bool icontinue=true;
    while(icontinue)
    {
        Result  res("PARTIAL",inv_Data->getNumberOFHazards(),Settings::st_timeSpan);
        temp_insurer.cleanUpForNextRound();
        cout<<"----------   "<<model_counter<<"   ----------"<<endl;
        nextModelVariable(); ///calculates ak and ck of SPSA
        nextCandidate(temp_insurer, inv_Data); ///calculates theta
        temp_insurer.setDecesionVariableIntoModel(decisionVariable,one_DecisionVariable); ///assign new theta to insurance model
        StochasticModule stm;
        inv_Data->prepareDamageType(&temp_insurer);
        temp_insurer.updatePremium(inv_Data,true);
        temp_insurer.updateTreatyReinsurancePremium(inv_Data);
        temp_insurer.setBeginingFundMultipliedOfPremium(5);
        stm.goDeepIntoTree(&temp_insurer, &res, inv_Data,1.0,0); ///calculates all scenarios and the probability
        if(Settings::st_doesLogScenarioDetails)
            Log::writeResults(model_counter,"after",temp_insurer,res);
        res.finalValueWithPenalty(&temp_insurer); /// get The final result (including probability mean and penalty for constraints)
        icontinue=decideIfToContinue(); ///decide if to continue
        res.addToLog();
        temp_insurer.addToLog();
        inv_Data->addToLog();
        model_counter++;
    }
}

void SPSA::copyInitialValueOfDecisionVariables(InsuranceModel insurer,const Inventory *inv_Data_const)
{
    decisionVariable.clear();
    unsigned number_Location=inv_Data_const->getNumberOfLocations();
    unsigned number_Type=inv_Data_const->getNumberOfTypes();
    unsigned number_decisionVariable=Settings::number_decisionVariable;
    vector<float> _usage;
    decisionVariable.resize(number_Location);
    for(unsigned i=0;i<number_Location;i++)
    {
        insurer.getInsuranceUsage(_usage,i);
        decisionVariable[i].resize(number_Type);
        for(unsigned j=0;j<number_Type;j++)
        {
            decisionVariable[i][j].resize(number_decisionVariable);
            decisionVariable[i][j][0]=insurer.getPolicyConditionDeductible(i,j);
            decisionVariable[i][j][1]=insurer.getPolicyConditionMaximum(i,j);
            decisionVariable[i][j][2]=_usage[j];
        }
    }
    //vector<float> null_vec;
    //null_vec.swap(one_DecisionVariable);
    one_DecisionVariable.clear();
    //one_DecisionVariable.push_back(insurer.getTreatyReinsuranceParticipation());
    one_DecisionVariable.push_back(insurer.getTreatyReinsuranceAttachmentPoint());
    one_DecisionVariable.push_back(insurer.getTreatyReinsuranceMaximumPoint());
}


void SPSA::nextModelVariable()
{
    model_a_k=model_a/pow(model_A+model_counter+1,model_alpha);
    model_c_k=model_c/pow(model_counter+1,model_gamma);
}

void SPSA::nextCandidate( InsuranceModel insurer, Inventory *inv_Data)
{
    //insurer.cleanUpForNextRound();
    //////////////////// DELTA ////////////////////
    const unsigned number_Location=inv_Data->getNumberOfLocations();
    const unsigned number_Type=inv_Data->getNumberOfTypes();
    const unsigned number_decisionVariable=Settings::number_decisionVariable;
    bernoulli_distribution distribution(0.5);
    uniform_real_distribution<float> dist(-0.25,0.25);
    vector<vector <vector<float> > > delta;
    delta.resize(number_Location);
    for(unsigned i=0;i<number_Location;i++)
    {
        delta[i].resize(number_Type);
        for(unsigned j=0;j<number_Type;j++)
        {
            delta[i][j].resize(number_decisionVariable,0);
            for(unsigned k=0;k<number_decisionVariable;k++)
            {
                delta[i][j][k]=(2*distribution(generator)-1);
            }
        }
    }
    vector<float> oneDecisionVariable_delta;
    oneDecisionVariable_delta.clear();
    oneDecisionVariable_delta.push_back(2*distribution(generator)-1);
    oneDecisionVariable_delta.push_back(2*distribution(generator)-1);
    //////////////////// Plus and Minus Part ////////////////////
    vector<vector<vector<float> > > plusPart, minusPart;
    plusPart.resize(number_Location);
    minusPart.resize(number_Location);
    for(unsigned i=0;i<number_Location;i++)
    {
        plusPart[i].resize(number_Type);
        minusPart[i].resize(number_Type);
        for(unsigned j=0;j<number_Type;j++)
        {
            plusPart[i][j].resize(number_decisionVariable,0);
            minusPart[i][j].resize(number_decisionVariable,0);
            for(unsigned k=0;k<number_decisionVariable;k++)
            {
                float theta=decisionVariable[i][j][k];
                plusPart[i][j][k]=theta+model_c_k*delta[i][j][k];
                minusPart[i][j][k]=theta-model_c_k*delta[i][j][k];
                boundCheck(k, plusPart[i][j][k],theta,delta[i][j][k]);
                boundCheck(k, minusPart[i][j][k],theta,delta[i][j][k]);
            }
        }
    }
    unsigned bound_counter=0;
    /*for(unsigned i=0;i<oneDecisionVariable_delta.size();i++)
    {
        plusPart_one_decisionVariable.push_back(one_DecisionVariable[bound_counter]+model_c_k*oneDecisionVariable_delta[bound_counter]);
        minusPart_one_decisionVariable.push_back(one_DecisionVariable[bound_counter]-model_c_k*oneDecisionVariable_delta[bound_counter]);
        boundCheck(3+bound_counter, plusPart_one_decisionVariable[bound_counter],one_DecisionVariable[bound_counter],oneDecisionVariable_delta[bound_counter]);
        boundCheck(3+bound_counter, minusPart_one_decisionVariable[bound_counter],one_DecisionVariable[bound_counter],oneDecisionVariable_delta[bound_counter]);
        bound_counter++;
    }
    //cout<<oneDecisionVariable_delta[0]<<"   "<<oneDecisionVariable_delta[1]<<endl;
    minorAndMajorCheck(plusPart_one_decisionVariable[0],plusPart_one_decisionVariable[1],one_DecisionVariable[0],one_DecisionVariable[1],oneDecisionVariable_delta[0],oneDecisionVariable_delta[1]);
    minorAndMajorCheck(minusPart_one_decisionVariable[0],minusPart_one_decisionVariable[1],one_DecisionVariable[0],one_DecisionVariable[1],oneDecisionVariable_delta[0],oneDecisionVariable_delta[1]);
*/

    //////////////////// Calculations ////////////////////
    //InsuranceModel temp1(&insurer,inv_Data);
    for(unsigned gi=0;gi<100;gi++)
    {
        cout<<"-----  "<<gi<<"  -----"<<endl;
        vector<float> plusPart_one_decisionVariable, minusPart_one_decisionVariable;
        plusPart_one_decisionVariable.resize(2,0);
        minusPart_one_decisionVariable.resize(2,0);
        bound_counter=0;
        for(unsigned i=0;i<oneDecisionVariable_delta.size();i++)
        {
            plusPart_one_decisionVariable[i]=(one_DecisionVariable[bound_counter]+dist(generator)/sqrt(sqrt(i)));
            minusPart_one_decisionVariable[i]=(one_DecisionVariable[bound_counter]-dist(generator));
            boundCheck(3+bound_counter, plusPart_one_decisionVariable[bound_counter]);//,one_DecisionVariable[bound_counter],oneDecisionVariable_delta[bound_counter]);
            boundCheck(3+bound_counter, minusPart_one_decisionVariable[bound_counter]);//,one_DecisionVariable[bound_counter],oneDecisionVariable_delta[bound_counter]);
            bound_counter++;
        }
        cout<<"      "<<plusPart_one_decisionVariable[0]<<"   "<<plusPart_one_decisionVariable[1]<<endl;
        cout<<"      "<<minusPart_one_decisionVariable[0]<<"   "<<minusPart_one_decisionVariable[1]<<endl;
        //minorAndMajorCheck(plusPart_one_decisionVariable[0],plusPart_one_decisionVariable[1],one_DecisionVariable[0],one_DecisionVariable[1],oneDecisionVariable_delta[0],oneDecisionVariable_delta[1]);
        //minorAndMajorCheck(minusPart_one_decisionVariable[0],minusPart_one_decisionVariable[1],one_DecisionVariable[0],one_DecisionVariable[1],oneDecisionVariable_delta[0],oneDecisionVariable_delta[1]);
        InsuranceModel temp1=insurer;
        StochasticModule stm1;
        Result  res1("PARTIAL",inv_Data->getNumberOFHazards(),Settings::st_timeSpan);
        temp1.setDecesionVariableIntoModel(plusPart,plusPart_one_decisionVariable);
        inv_Data->prepareDamageType(&temp1);
        temp1.updatePremium(inv_Data,true);
        temp1.updateTreatyReinsurancePremium(inv_Data);
        temp1.setBeginingFundMultipliedOfPremium(5);
        //cout<<"Go Deep 1"<<endl;
        stm1.goDeepIntoTree(&temp1, &res1,inv_Data,1,0);
        res1.finalValueWithPenalty(&temp1);
        double plusValue=res1.getObjectiveValue();
        //Log::writeResults(1,"next1",temp1,res1);

        //InsuranceModel temp2(&insurer,inv_Data);
        InsuranceModel temp2=insurer;
        StochasticModule stm2;
        Result  res2("PARTIAL",inv_Data->getNumberOFHazards(),Settings::st_timeSpan);
        temp2.setDecesionVariableIntoModel(minusPart,minusPart_one_decisionVariable);
        inv_Data->prepareDamageType(&temp2);
        temp2.updatePremium(inv_Data,true);
        temp2.updateTreatyReinsurancePremium(inv_Data);
        temp2.setBeginingFundMultipliedOfPremium(5);
        //cout<<"Go Deep 2"<<endl;
        stm2.goDeepIntoTree(&temp2, &res2, inv_Data,1,0);
        res2.finalValueWithPenalty(&temp2);
        double minusValue=res2.getObjectiveValue();
        //Log::writeResults(1,"next2",temp2,res2);

        //////////////////// New DV ////////////////////
        /*for(unsigned i=0;i<number_Location;i++)
        {
            for(unsigned j=0;j<number_Type;j++)
            {
                for(unsigned k=0;k<number_decisionVariable;k++)
            {
                decisionVariable[i][j][k]=decisionVariable[i][j][k]+model_a_k*(plusValue-minusValue)/(2*model_c_k*delta[i][j][k]);
                boundCheck(k, decisionVariable[i][j][k]);
            }
        }*/
        cout<<"ghabli= "<<Settings::ghabli<<"   "<<"plusvalue= "<<plusValue<<"   "<<"minusValue= "<<minusValue<<endl;
        if(plusValue>Settings::ghabli)
        {
            Settings::ghabli=plusValue;
            one_DecisionVariable[0]=plusPart_one_decisionVariable[0];
            one_DecisionVariable[1]=plusPart_one_decisionVariable[1];
        }
        if(minusValue>Settings::ghabli &&minusValue>plusValue)
        {
            Settings::ghabli=minusValue;
            one_DecisionVariable[0]=minusPart_one_decisionVariable[0];
            one_DecisionVariable[1]=minusPart_one_decisionVariable[1];
        }
        //minorAndMajorCheck(one_DecisionVariable[0],one_DecisionVariable[1]);
        cout<<"Final   "<<one_DecisionVariable[0]<<"   "<<one_DecisionVariable[1]<<endl;
        getch();
    }
    exit(0);
}

void SPSA::addToLog()
{
}

void SPSA::boundCheck(unsigned iForDV, float &plusPart,float firstTheta, float &delta)
{
    bool iupperbound=true;/*Settings::isThereAConstraintsOnDecisionVariable(operatingDV,1);*/
    bool ilowerbound=true;/*Settings::isThereAConstraintsOnDecisionVariable(operatingDV,0);*/
    float upperbound=Settings::settingsData[iForDV][1];
    float lowerbound=Settings::settingsData[iForDV][0];
    float plusPartTemp=plusPart;
    bool  isChanged=false;
    if(ilowerbound && plusPart<lowerbound)
    {
        plusPart=lowerbound;
        isChanged=true;
    }
    else if(iupperbound && upperbound<plusPart)
    {
        plusPart=upperbound;
        isChanged=true;
    }

    if(isChanged)
    {
        float difference=fabs(plusPartTemp-plusPart)/fabs(plusPartTemp-firstTheta);
        if(difference<0.000000001f)
            difference=0.000000001f;
        delta=delta*difference;
    }
}

void SPSA::boundCheck(unsigned iForDV, float &plusPart)
{
    bool iupperbound=true;/*Settings::isThereAConstraintsOnDecisionVariable(operatingDV,1);*/
    bool ilowerbound=true;/*Settings::isThereAConstraintsOnDecisionVariable(operatingDV,0);*/
    float upperbound=Settings::settingsData[iForDV][1];
    float lowerbound=Settings::settingsData[iForDV][0];
    if(ilowerbound && plusPart<lowerbound)
    {
        plusPart=lowerbound;
    }
    else if(iupperbound && upperbound<plusPart)
    {
        plusPart=upperbound;
    }
}

bool SPSA::decideIfToContinue()
{
    if (/*abs(lastValue-currentValue)<0.0001 ||*/ model_counter+1>=iteration_max/*Settings::st_iteration_step_max*/)
        return false;
    else
        return true;
}


void SPSA::configFromFile(const char * fileName)
{
    ifstream SA_conf(fileName,ifstream::in);
    if(SA_conf.good()==false)
    {
        cout<<"Unable to open file "<<fileName<<". Exiting!"<<endl;
        getch();
        exit(1);
    }
    float    float_tmp;
    string   line;
    vector<vector<float> > temp_data;
    while(getline(SA_conf,line))
    {
        vector<float> lineData;
        stringstream  linestream(line);
        while(linestream>>float_tmp)
        {
            lineData.push_back(float_tmp);
        }
        temp_data.push_back(lineData);
    }
    model_alpha=temp_data[0][0];
    model_gamma=temp_data[1][0];
    model_c=temp_data[2][0];
    model_a=temp_data[3][0];
    model_A=temp_data[4][0];
    iteration_max=temp_data[5][0];

}

void SPSA::minorAndMajorCheck(float &_minor_data,float &_major_data,float _minor_first_Theta, float _major_first_Theta, float &minor_delta,float &major_delta)
{
    if(_major_data<_minor_data)
    {
        cout<<"major data is bigger than minor one. Data changed automatically."<<endl;
        float major_temp=_major_data;
        float minor_temp=_minor_data;
        float middle_data_point=(minor_temp+major_temp)/2;
        _major_data=_major_first_Theta-0.8*fabs(_major_first_Theta-middle_data_point);
        _minor_data=_minor_first_Theta+0.8*fabs(middle_data_point-_minor_first_Theta);
        float difference=0;
        difference=fabs(minor_temp-_minor_data)/fabs(minor_temp-_minor_first_Theta);
        //if(difference<0.000000001f)
            //difference=0.000000001f;
        _minor_first_Theta=_minor_first_Theta*difference;

        difference=fabs(major_temp-_major_data)/fabs(major_temp-_major_first_Theta);
        //if(difference<0.000000001f)
            //difference=0.000000001f;
        _major_first_Theta=_major_first_Theta*difference;
    }
}

void SPSA::minorAndMajorCheck(float &_minor_data,float &_major_data)
{
    if(_major_data<_minor_data)
    {
        cout<<"major data is bigger than minor one. Data changed automatically."<<endl;
        float major_temp=_major_data;
        float minor_temp=_minor_data;
        float middle_data_point=(minor_temp+major_temp)/2;
        _major_data=middle_data_point;
        _minor_data=middle_data_point;
    }
}

SPSA::SPSA()
{
    cout<<"constructing SPSA class instance"<<endl;
    model_counter=0;
    const char *SAFileName="SPSA_Conf.txt";
    cout<<"Reading SPSA_Conf.txt"<<endl;
    configFromFile(SAFileName);
    delete [] SAFileName;
    cout<<"model_alpha="<<model_alpha<<endl;
    cout<<"model_gamma="<<model_gamma<<endl;
    cout<<"model_c="<<model_c<<endl;
    cout<<"model_a="<<model_a<<endl;
    cout<<"model_A="<<model_A<<endl;
    cout<<"iteration_max="<<iteration_max<<endl;
    ///Initializing the InsuranceModel and Inventory Objects
}



