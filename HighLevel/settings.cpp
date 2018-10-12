#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <cmath>
#include "settings.h"

void Settings::getDVIntoSettings(InsuranceModel insurer)
{
    st_beginingMinimum_vec=insurer.policyDeductible;
    st_beginingMaximum_vec=insurer.policyMaximum;
}

Settings::Settings(int argc, char **argv)
{
    unsigned c=0;
    for(unsigned i=0;i<unsigned(argc);i++)
    {
        string key(argv[i]);
        //if(key=="--dv" || key=="--DV" || key=="-decisionVariable")
        if(false)
        {
            c++;
            string option("dv.txt"/*argv[i+c]*/);
            isDVDefined=true;
            readPredefinedDecisionvariables(option);
        }
        else
            isDVDefined=false;

        //if(key=="--s" || key=="--S" || key=="single")
        if(false)
        {
            st_use_single=true;
        }
        else
            st_use_single=false;

        if(key=="--?" || key=="--h" || key=="--H" || key=="-help")
        {
            showGreetings();
            showUsage();
            exit(0);
        }
    }
    /*showGreetings();
    bool igo=true;
    char in;
    cout<<"Are you wiling to use the wizard which help you through configuration,"<<endl;
    cout<<"or you you would like to use configuration file named "<<'"'<<"config"<<'"'<<'?'<<endl;
    cout<<"Enter"<<endl;
    while(igo)
    {
        cout<<"      1  -  for the wizard"<<endl;
        cout<<"      2  -  for the config file"<<endl;
        if(in=='1' || in=='2')
        {
            igo=false;
        }
        if(igo)
            cout<<"Enter either 1 or 2 please"<<endl;
    }*/
    multiplier=10;
    maximum_fixed=0.0001;
    st_maximumAcceptableProbability=0.01;
    st_managmentCostsRatio=0.50;
    st_reinsuranceOverhead=0.50;
    st_treatyReinsurancePremiumAddingDeviance=1;
    st_premiumOverheadratio=1;
    st_minimumPremiumRatio=0.0001;
    st_beginingMinimum=0.0;
    st_beginingMaximum=0.85;
    st_turnOverRatio=0.1;
    number_decisionVariable=3;
    st_timeSpan=1;
    st_beginingFund=20000000000;
    st_outTextFileName="out";
    st_outPath="out//";
    st_minimumCapital=100000000000;
    ghabli=-10000000;
    //st_doesLogScenarioDetails=false;
    st_iteration_step_max=2;
    string settings_file="Decision_Variable_Constraints.txt";
    ifstream file(settings_file.c_str(), ifstream::in);
    if(file.good()==false)
    {
        cout<<"Unable to open file Decision_Variable_Constraints.txt. Exiting!"<<endl;
        getch();
        exit(1);
    }
    string line;
    float    float_temp;
    while(getline(file,line))
    {
        vector<float>   lineData;
        stringstream  linestream(line);
        while(linestream>>float_temp)
        {
            lineData.push_back(float_temp);
        }
        settingsData.push_back(lineData);
    }
    file.close();
    file.clear();
    string objectiveFunction_file="objectiveFunctionVariable.txt";
    file.open(objectiveFunction_file.c_str(), ifstream::in);
    if(file.good()==false)
    {
        cout<<"Unable to open file "<<objectiveFunction_file<<". Exiting!"<<endl;
        getch();
        exit(1);
    }
    st_ObjectiveFunctionCoefficient.clear();
    getline(file,line);
    stringstream  linestream(line);
    while(linestream>>float_temp)
    {
        st_ObjectiveFunctionCoefficient.push_back(float_temp);
    }
    unsigned minimumOfOFC=3;
    if(st_ObjectiveFunctionCoefficient.size()!=minimumOfOFC)
    {
        cout<<"Objective Function Coefficient should be at least "<<minimumOfOFC<<". Exiting!"<<endl;
        getch();
        exit(1);
    }
    file.close();
}

void Settings::showGreetings()
{
    cout<<"******************************************************"<<endl;
    cout<<"*                                                    *"<<endl;
    cout<<"*                                                    *"<<endl;
    cout<<"*               welcome to KSA                       *"<<endl;
    cout<<"*                                                    *"<<endl;
    cout<<"*                                                    *"<<endl;
    cout<<"*                                                    *"<<endl;
    cout<<"******************************************************"<<endl;
}

void Settings::showUsage()
{
    cout<<"use command like:"<<endl<<'"'<<"KSA.exe [--o] [option-Argument]"<<'"'<<" or "<<'"'<<" KSA.exe [-option] [option-arguemnt"<<'"'<<endl;
    cout<<"arguemnt list:"<<endl;
    cout<<"               request for console usage help-->   [-?] [-h] [help]"<<endl;
    cout<<"               pre defined Decision Variable-->    [--dv]    [-deciionVariable] textfilename"<<endl;
}

void Settings::readPredefinedDecisionvariables(string option)
{
    ifstream file(option.c_str(),ifstream::in);
    vector<vector<float> > total;
    if(file.good())
    {
        string temp_str;
        while(getline(file,temp_str))
        {
            vector<float> line_temp;
            float float_temp;
            stringstream temp_sstream(temp_str);
            while(temp_sstream>>float_temp)
                line_temp.push_back(float_temp);
            total.push_back(line_temp);
        }
        if(fmod(total.size(),2)==0)
        {
            for(unsigned i=0;i<total.size()/2;i++)
            {
                st_beginingMinimum_vec.push_back(total[i]);
            }
            for(unsigned i=total.size()/2;i<total.size();i++)
            {
                st_beginingMaximum_vec.push_back(total[i]);
            }
            for(unsigned i=0;i<st_beginingMinimum_vec.size();i++)
            {
                for(unsigned j=0;j<st_beginingMinimum_vec[i].size();j++)
                    cout<<st_beginingMinimum_vec[i][j]<<"   ";
                cout<<endl;
            }
            for(unsigned i=0;i<st_beginingMaximum_vec.size();i++)
            {
                for(unsigned j=0;j<st_beginingMaximum_vec[i].size();j++)
                    cout<<st_beginingMaximum_vec[i][j]<<"   ";
                cout<<endl;
            }
        }
        else
        {
            cout<<"number of lines of decision variable file should be even. Exiting."<<endl;
            exit(1);
        }
    }
    else
    {
        cout<<"unable to open file named "<<'"'<<option.c_str()<<'"'<<". Exiting."<<endl;
        exit(1);
    }
    file.close();
}

bool Settings::isThereAConstraintsOnDecisionVariable(unsigned numberOFDV, bool iupper)
{
    if(numberOFDV>number_decisionVariable)
    {
        cout<<"number of decesion variable exceeds. Exiting."<<endl;
        exit(1);
    }
    if(settingsData[numberOFDV][iupper]<0)
        return false;
    else
        return true;
}

float Settings::getDecisionVariableConstraint(unsigned numberOfDV, bool upper)
{
    return settingsData[numberOfDV][upper];
}

/*Settings::~Settings()
{
    //dtor
}*/
