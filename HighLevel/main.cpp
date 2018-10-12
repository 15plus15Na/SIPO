/*******************************************************************
* This File Has been Created By Sina Naeimi (sinanaeimi(@)Yahoo.com
* This is main file, responsible of making the main objects and
* assigning the values.
*********************************************************************/
#include <iostream>
#include <vector>
#include <conio.h>
#include <math.h>
#include "log.h"
#include "insurancemodel.h"
#include "insurancemodel.cpp"
#include "result.h"
#include "result.cpp"
#include "Inventory.h"
#include "Inventory.cpp"
#include "stochastic.h"
#include "stochastic.cpp"
#include "settings.h"
#include "settings.cpp"
#include "approximation.h"
#include "approximation.cpp"

using namespace std;

////////////Static members defination///////////
vector<double>  Log::objectFunctionValue;
vector<float>   Log::_log_bankrupcy;
vector<double>  Log::aggregatedPremium;
vector<double>  Log::initialFund;
vector<vector<float> > Settings::st_beginingMinimum_vec;
vector<vector<float> > Settings::st_beginingMaximum_vec;
vector<vector<float> > Settings::settingsData;
vector<vector<vector<float> > > Log::minimumCoverage;
vector<vector<vector<float> > > Log::maximumCoverage;
unsigned Log::itr_count1=0;
unsigned Settings::number_decisionVariable;
unsigned Settings::st_timeSpan;
unsigned Settings::st_iteration_step_max;
float    Settings::st_maximumAcceptableProbability;
float    Settings::st_managmentCostsRatio;
float    Settings::st_premiumOverheadratio;
float    Settings::st_minimumPremiumRatio;
float    Settings::st_beginingMinimum;
float    Settings::st_beginingMaximum;
float    Settings::st_turnOverRatio;
double   Settings::st_beginingFund;
string   Settings::st_outTextFileName;
string   Settings::st_outPath;
////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    Log _log(Log::DEP);
    Settings settings(argc,argv);
    //vector<InsuranceModel> insurer;
    //vector<Inventory> inventoryData;
    //vector<Result> res;
    DEP ap(1,1,3);
    ap.setLimits(1,0,0.15);
    ap.setLimits(2,0.85,1);
    ap.setLimits(3,.1,.3);
    ap.setParts(1,0.04);
    ap.setParts(2,0.04);
    ap.setParts(3,0.15);
    ap.descrtisizeEqualParts();
    Inventory  inventory;
    InsuranceModel insurer(&inventory,Settings::st_timeSpan);
    vector<Result> res;
    //insurer.setPolicyConditionThresholds(Settings::st_beginingMinimum, Settings::st_beginingMaximum,&inventory);
    //insurer.initalGuessOfTurnOver(&inventory, "uniform", Settings::st_turnOverRatio);
    cout<<insurer.getTotalPremiums()<<endl;
    res.resize(ap.getNumberOfInstances(),Result("PARTIAL",inventory.getNumberOFHazards(),Settings::st_timeSpan));
    cout<<"number of instances="<<ap.getNumberOfInstances()<<endl;
    ap.calculateDiffrentParts(insurer,&inventory,res);

    return 0;
}
