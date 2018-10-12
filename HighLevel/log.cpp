#include <iostream>
#include <sstream>
#include <conio.h>
#include <cmath>
#include "settings.h"
#include "log.h"
void Log::nextIteration()
{
    itr_count1++;
}

void Log::addNewObjectFunctuionValue(double temp)
{
    if(objectFunctionValue.size()+1==itr_count1)
    {
        objectFunctionValue.push_back(temp);
    }
    else if(objectFunctionValue.size()==itr_count1)
    {
        objectFunctionValue[itr_count1-1]=temp;
    }
    else
    {
        cout<<"unexpected thing happend in addnewpbjectfunctionvalue"<<endl;
        exit(1);
    }

}

void Log::objectFunctionPushBack(double _OFV,double _GOFV,double _GOFD,double _GOFVWZ,double _GOFDWZ)
{
    objectFunctionValue.push_back(_OFV);
    grossObjectiveFunctionValue.push_back(_GOFV);
    grossObjectiveFunctionDeviance.push_back(_GOFD);
    grossObjectiveFunctionValueWithZero.push_back(_GOFVWZ);
    grossObjectiveFunctionDevianceWithZero.push_back(_GOFDWZ);
}

void Log::smallestAndBiggestSolvencyRatio(double SR_S,double SR_B)
{
    smallestSolvencyRatio.push_back(SR_S);
    biggestSolvencyRatio.push_back(SR_B);
}

void Log::decisionVariablePushBack(vector<vector<float> > _minc, vector<vector<float> > _maxc)
{
    minimumCoverage.push_back(_minc);
    maximumCoverage.push_back(_maxc);
}

void Log::decisionVariablePushBack(vector<vector<float> > _minc, vector<vector<float> > _maxc, vector<vector<float> > _ic)
{
    minimumCoverage.push_back(_minc);
    maximumCoverage.push_back(_maxc);
    insuranceCoverage.push_back(_ic);
}

void Log::one_DecisionVariablePushBack(float temp_1,float temp_2)
{
    //reinsuranceParticipation.push_back(temp_U);
    _log_reinsuranceAttachmentPoint.push_back(temp_1);
    _log_reinsuranceMaximumPoint.push_back(temp_2);
}

void Log::bankrupcyPushBack(float cb,float cbd)
{
    _log_bankrupcy.push_back(cb);
    _log_bankrupcy_Deviance.push_back(cbd);
}

void Log::writeResults(unsigned c, string s, InsuranceModel insurer, Result result)
{
    string name("seq//");
    string txt(".txt");
    stringstream number;
    number<<c;
    name=name+number.str()+"_"+s+txt;
    ofstream write(name.c_str(), ofstream::out);
    if(write.good())
    {
        write<<s<<endl;
        write<<"fund"<<endl;
        for(unsigned i=0;i<result.maxIndex;i++)
        {
            for(unsigned j=0;j<result.timeSpan;j++)
            {
                if(j!=0)
                    write<<" --> ";
                write<<result.fundHistory[i][j];
            }
            write<<endl;
        }
        write<<"premium"<<endl;
        for(unsigned i=0;i<insurer.premium.size();i++)
        {
            for(unsigned j=0;j<insurer.premium[i].size();j++)
            {
                write<<insurer.premium[i][j]<<"   ";
            }
            write<<endl;
        }
        write<<"Results"<<endl;
        for(unsigned i=0;i<result.idF;i++)
        {
            write<<"Scenario   "<<i<<endl;
            write<<"Fund= "<<result.scenarioFinalFund[i]<<endl;
            write<<"Probability= "<<result.scenarioProbablity[i]<<endl;
            write<<"is Bankrupt=";
            result.bankrupcy[i]?write<<"Yes"<<endl:write<<"No"<<endl;
            write<<endl;
        }
        write<<"final results"<<endl;
        double returnValue=0;
        unsigned bankrupcy_penalty=false;
        for(unsigned i=0;i<result.maxIndex;i++)
        {
            returnValue+=result.scenarioFinalFund[i]*result.scenarioProbablity[i];
            if(result.bankrupcy[i]==true)
                result.culmativeBankrupcy+=result.scenarioProbablity[i];
        }
        write<<"bankrupcyProbability="<<result.culmativeBankrupcy<<endl;
        if(result.culmativeBankrupcy>insurer.getBankrupcyProbability())
        {
            bankrupcy_penalty=true;
            write<<"bankrupcy rate exceeded: "<<result.culmativeBankrupcy<<endl;;
        }
        write<<"sum of funds="<<returnValue<<endl;
        /*if(bankrupcy_penalty)
        {
            returnValue+=sqrt(sin(1-float(_c)/float(maxIndex)));
            cout<<"Penalty "<<sqrt(sin(1-float(_c)/float(maxIndex)))<<endl;
        }*/
        write<<"return value="<<returnValue<<endl;

        write<<"minimum"<<endl;
        for(unsigned i=0;i<insurer.policyDeductible.size();i++)
        {
            for(unsigned j=0;j<insurer.policyDeductible[i].size();j++)
            {
                write<<insurer.policyDeductible[i][j]<<"   ";
            }
            write<<endl;
        }
        write<<"maximum"<<endl;
        for(unsigned i=0;i<insurer.policyMaximum.size();i++)
        {
            for(unsigned j=0;j<insurer.policyMaximum[i].size();j++)
            {
                write<<insurer.policyMaximum[i][j]<<"   ";
            }
            write<<endl;
        }
    }
    else
    {
        cout<<"unable to write the sequentially results."<<endl;
        getch();
    }
    write.close();
}

void Log::intialFundPushBack(double bf)
{
    initialFund.push_back(bf);
}
void Log::aggregatedPremiumPushBack(double tp)
{
    aggregatedPremium.push_back(tp);
}

Log::Log(analysisType whatKind)
{
    _type=whatKind;
    /*if(whatKind==SPSA)
        ilk=0;
    else if(whatKind=DEP)
        ilk=1;
    else
    {
        cout<<"ERROR: Log Kind Has An Error"<<endl;
        exit(1);
    }*/
    itr_count1=0;
}

void Log::writeDEPIntoCSV(string name)
{
    ofstream out_file(name, ofstream::out);
    out_file<<"min   ,   max   ,   coverage   , Value   , Deviance ,  Bankruptcy Probability, Deviance , Value With Zero, Deviance , Smallest Solvency Ratio"<<endl;
    unsigned loopCounter=briefDV.size();
    if(briefDV.size()!=objectFunctionValue.size())
    {
        cout<<briefDV.size()<<"   "<<objectFunctionValue.size()<<endl;
        cout<<"Size of briefDV and objectFunctionValue is not the same. The one with lower value is selected in order to prevent the program from crashig."<<endl;
        getch();
        if(briefDV.size()>grossObjectiveFunctionDeviance.size())
            loopCounter=grossObjectiveFunctionDeviance.size();
    }
    if(out_file.good())
    {
        for(unsigned i=0;i<loopCounter;i++)
        {
            for(unsigned j=0;j<briefDV[i].size();j++)
            {
                out_file<<briefDV[i][j]<<" , ";
            }
            out_file<<grossObjectiveFunctionValue[i]<<" , "<<grossObjectiveFunctionDeviance[i]<<" , "<<_log_bankrupcy[i]<<" , "<<_log_bankrupcy_Deviance[i]<<" , "<<grossObjectiveFunctionValueWithZero[i]<<" , "<<grossObjectiveFunctionDevianceWithZero[i]<<" , "<<smallestSolvencyRatio[i]<<endl;
        }
    }
    else
    {
        cout<<"could not write on file "<<name<<". Press any key."<<endl;
        getch();
    }
    out_file.close();
}

void Log::writeSPSA(string name)
{

    ofstream out_file(name+string(".csv"), ofstream::out);
    out_file<<"Value   , Deviance ,  Bankruptcy Probability, Deviance , Value With Zero, Deviance , Smallest Solvency Ratio , Objective Function"<<endl;
    unsigned loopCounter=grossObjectiveFunctionDeviance.size();
    if(out_file.good())
    {
        for(unsigned i=0;i<loopCounter;i++)
        {
            out_file<<grossObjectiveFunctionValue[i]<<" , "<<grossObjectiveFunctionDeviance[i]<<" , "<<_log_bankrupcy[i]<<" , "<<_log_bankrupcy_Deviance[i]<<" , "<<grossObjectiveFunctionValueWithZero[i]<<" , "<<grossObjectiveFunctionDevianceWithZero[i]<<" , "<<smallestSolvencyRatio[i]<<" , "<<objectFunctionValue[i]<<endl;
        }
    }
    else
    {
        cout<<"could not write on file "<<name<<". Press any key."<<endl;
        getch();
    }
    out_file.close();
    out_file.clear();

    for(unsigned i=0;i<dv.size();i++)
    {
        stringstream s_temp;
        s_temp<<i;
        //cout<<name+string("_")+s_temp.str()+string(".csv")<<endl;
        //getch();
        out_file.open(name+s_temp.str()+string(".csv"), ofstream::out);
        if(out_file.good())
        {
            for(unsigned iDV=0;iDV<Settings::number_decisionVariable;iDV++)
            {
                for(unsigned iLocation=0;iLocation<dv[i].size();iLocation++)
                {
                    for(unsigned iType=0;iType<dv[i][iLocation].size();iType++)
                    {
                        out_file<<dv[i][iLocation][iType][iDV]<<" , ";
                    }
                    out_file<<endl;
                }
                for(unsigned iType=0;iType<dv[0].size();iType++)
                {
                    out_file<<" , ";
                }
                out_file<<endl;
            }
        }
        else
        {
            cout<<"Could not write in SPSA details. Press any key to continue"<<endl;
            getch();
        }
        out_file.close();
    }
}

Log::~Log()
{
    if(_type==Log::DEP)
        writeDEPIntoCSV(Settings::st_outPath+string("_excel_DEP.csv"));
    else if(_type==Log::SPSA)
    {
        writeSPSA(Settings::st_outPath+string("SPSA"));
    }
    else
    {
        cout<<"I don't know what kind of Analysis it is in Log"<<endl;
        getch();
    }
    string outFileName_1=Settings::st_outPath+Settings::st_outTextFileName+string("_decisionVariable1")+string(".csv");
    ofstream out_file(outFileName_1, ofstream::out);
    if(out_file.good())
    {
        for(unsigned i=0;i<minimumCoverage.size();i++)
        {
            for(unsigned j=0;j<minimumCoverage[i].size();j++)
            {
                for(unsigned k=0;k<minimumCoverage[i][j].size();k++)
                {
                    out_file<<minimumCoverage[i][j][k]<<"   ,   ";
                }
                out_file<<endl;
            }
            for(unsigned k=0;k<minimumCoverage[0][0].size();k++)
            {
                out_file<<"   "<<",";
            }
            out_file<<endl;
        }
    }
    else
    {
        cout<<"There is a problem in saving into "<<outFileName_1.c_str()<<" file."<<endl;
    }
    out_file.close();
    out_file.clear();

    outFileName_1=Settings::st_outPath+Settings::st_outTextFileName+string("_decisionVariable2")+string(".csv");
    out_file.open(outFileName_1, ofstream::out);
    if(out_file.good())
    {
        for(unsigned i=0;i<maximumCoverage.size();i++)
        {
            for(unsigned j=0;j<maximumCoverage[i].size();j++)
            {
                for(unsigned k=0;k<maximumCoverage[i][j].size();k++)
                {
                    out_file<<maximumCoverage[i][j][k]<<" , ";
                }
                out_file<<endl;
            }
            for(unsigned k=0;k<minimumCoverage[0][0].size();k++)
            {
                out_file<<"   "<<",";
            }
            out_file<<endl;
        }
    }
    else
    {
        cout<<"There is a problem in saving into "<<outFileName_1.c_str()<<" file."<<endl;
    }
    out_file.close();
    out_file.clear();

    outFileName_1=Settings::st_outPath+Settings::st_outTextFileName+string("_decisionVariable3")+string(".csv");
    out_file.open(outFileName_1, ofstream::out);
    if(out_file.good())
    {
        for(unsigned i=0;i<insuranceCoverage.size();i++)
        {
            for(unsigned j=0;j<insuranceCoverage[i].size();j++)
            {
                for(unsigned k=0;k<insuranceCoverage[i][j].size();k++)
                {
                    out_file<<insuranceCoverage[i][j][k]<<"  , ";
                }
                out_file<<endl;
            }
            for(unsigned k=0;k<insuranceCoverage[0][0].size();k++)
            {
                    out_file<<"   "<<",";
            }
            out_file<<endl;
        }
    }
    else
    {
        cout<<"There is a problem in saving into "<<outFileName_1.c_str()<<" file."<<endl;
    }
    out_file.close();
    out_file.clear();

    outFileName_1=Settings::st_outPath+Settings::st_outTextFileName+string("_decisionVariable4")+string(".csv");
    out_file.open(outFileName_1, ofstream::out);
    if(out_file.good())
    {
        for(unsigned i=0;i<_log_reinsuranceAttachmentPoint.size();i++)
        {
            out_file<<_log_reinsuranceAttachmentPoint[i]<<" , "<<_log_reinsuranceMaximumPoint[i]<<endl;
        }
    }
    else
    {
        cout<<"There is a problem in saving into "<<outFileName_1.c_str()<<" file."<<endl;
    }
    out_file.close();
    out_file.clear();

    /*outFileName_1=Settings::st_outPath+Settings::st_outTextFileName+string("_decisionVariable5")+string(".csv");
    out_file.open(outFileName_1, ofstream::out);
    if(out_file.good())
    {
        for(unsigned i=0;i<_log_reinsuranceMaximumPoint.size();i++)
        {
            out_file<<_log_reinsuranceMaximumPoint[i]<<endl;
        }
    }
    else
    {
        cout<<"There is a problem in saving into "<<outFileName_1.c_str()<<" file."<<endl;
    }
    out_file.close();
    out_file.clear();*/

    string outFileName_FV=Settings::st_outPath+Settings::st_outTextFileName+string("_FinalValues")+string(".txt");
    ofstream out_file_FV(outFileName_FV, ofstream::out);
    out_file_FV<<"Total Gross Loss="<<totalGrossLoss<<endl;
    if(out_file_FV.good())
    {

        for(unsigned i=0;i<objectFunctionValue.size();i++)
        {
            out_file_FV<<endl<<endl<<"------------------------------"<<endl<<endl;
            out_file_FV<<i+1<<":   "<<"Object Function Value="<<objectFunctionValue[i]<<"   "<<"Bankrupcy rate="<<_log_bankrupcy[i]*100<<'%'<<endl;
            double temp=initialFund[i]/aggregatedPremium[i];
            out_file_FV<<"     TotalPremium="<<aggregatedPremium[i]<<"   "<<"initialFund="<<initialFund[i]<<"   "<<"the ratio="<<floor(floor(temp)+(temp-floor(temp))*100)<<endl;
            out_file_FV<<"Total Loss="<<totalLoss[i]<<"   "<<endl;;
        }
    }
    else
    {
        cout<<"There is a problem in saving into "<<outFileName_FV.c_str()<<" file."<<endl;
    }
    out_file_FV.close();
}
