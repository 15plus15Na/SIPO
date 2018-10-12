#ifndef SETTINGS_H
#define SETTINGS_H
#include "insurancemodel.h"

using namespace std;
class Settings
{
    public:
        static float multiplier;
        static float maximum_fixed;
        static unsigned number_decisionVariable;
        static vector<vector<float> >  st_beginingMinimum_vec;
        static vector<vector<float> >  st_beginingMaximum_vec;
        static vector<vector<float> > settingsData;
        static vector<float>          st_ObjectiveFunctionCoefficient;
        static bool isThereAConstraintsOnDecisionVariable(unsigned,bool);
        static float getDecisionVariableConstraint(unsigned,bool);
        static unsigned st_timeSpan;
        static unsigned st_numberOfHazard;
        static unsigned st_iteration_step_max;
        static float    st_maximumAcceptableProbability;
        static float    st_managmentCostsRatio;
        static float    st_reinsuranceOverhead;
        static float    st_treatyReinsurancePremiumAddingDeviance;
        static float    st_premiumOverheadratio;
        static float    st_minimumPremiumRatio;
        static float    st_beginingMinimum;
        static float    st_beginingMaximum;
        static float    st_turnOverRatio;
        static float    st_minimumCapital;
        static double   st_beginingFund;
        static string   st_outTextFileName;
        static string   st_outPath;
        static float    ghabli;

        bool   isDVDefined;
        bool   st_use_single;
        const static bool   st_doesLogScenarioDetails=false;
        void   getDVIntoSettings(InsuranceModel);
        Settings(int, char**);
        //virtual ~Settings();
    protected:
        void readPredefinedDecisionvariables(string);
        void showUsage();
        void showGreetings();
    private:
};

#endif // SETTINGS_H
