#ifndef STATIC_H_INCLUDED
#define STATIC_H_INCLUDED

////////////Static members defination///////////
vector<float>   Log::_log_reinsuranceParticipation;
vector<float>   Log::_log_reinsuranceAttachmentPoint;
vector<float>   Log::_log_reinsuranceMaximumPoint;
vector<double>  Log::objectFunctionValue;
vector<double>  Log::grossObjectiveFunctionValue;
vector<double>  Log::grossObjectiveFunctionValueWithZero;
vector<double>  Log::grossObjectiveFunctionDeviance;
vector<double>  Log::grossObjectiveFunctionDevianceWithZero;
vector<float>   Log::_log_bankrupcy;
vector<float>   Log::_log_bankrupcy_Deviance;
vector<double>  Log::aggregatedPremium;
vector<double>  Log::initialFund;
vector<double>  Log::totalLoss;
vector<double>  Log::smallestSolvencyRatio;
vector<double>  Log::biggestSolvencyRatio;
vector<float>   Settings::st_ObjectiveFunctionCoefficient;
vector<vector<float> > Settings::st_beginingMinimum_vec;
vector<vector<float> > Settings::st_beginingMaximum_vec;
vector<vector<float> > Settings::settingsData;
vector<vector<float> > Log::briefDV;
vector<vector<vector<float> > > Log::minimumCoverage;
vector<vector<vector<float> > > Log::maximumCoverage;
vector<vector<vector<float> > > Log::insuranceCoverage;
vector<vector<vector<vector<float> > > > Log::dv;
unsigned Log::itr_count1=0;
unsigned Settings::number_decisionVariable;
unsigned Settings::st_timeSpan;
unsigned Settings::st_iteration_step_max;
float    Settings::st_maximumAcceptableProbability;
float    Settings::st_managmentCostsRatio;
float    Settings::st_reinsuranceOverhead;
float    Settings::st_treatyReinsurancePremiumAddingDeviance;
float    Settings::st_premiumOverheadratio;
float    Settings::st_minimumPremiumRatio;
float    Settings::st_beginingMinimum;
float    Settings::st_beginingMaximum;
float    Settings::st_turnOverRatio;
float    Settings::st_minimumCapital;
double   Settings::st_beginingFund;
float    Settings::multiplier;
float    Settings::maximum_fixed;
double   Log::totalGrossLoss;
string   Settings::st_outTextFileName;
string   Settings::st_outPath;
const bool     Settings::st_doesLogScenarioDetails;
float    Settings::ghabli;
////////////////////////////////////////////////

#endif // STATIC_H_INCLUDED
