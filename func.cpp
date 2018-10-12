#include <fstream>
#include <thread>
#include <cmath>
#include <limits>
#include "damage.h"
#include "func.h"
#include "worldmodel.h"
#include "HighLevel/settings.h"
void calculateDamageRatioAndDamageStateShare(WorldModel *wm,VulnerabilityModel *vm){
    for(unsigned i=0;i<wm->getPropertyPointer()->getNumberOfPoints();i++){
        vector<float> temp;
        vector<float> coord;
        wm->getPropertyPointer()->getCoordination(i,coord);
        temp.push_back(coord[0]);
        temp.push_back(coord[1]);
    }
    Earthquake *eq=wm->getEarthquakePointer();
    double damageRatio;
    unsigned structuralNo=wm->getPropertyPointer()->getNumberOfStructuralType();
    float  dmg_std_dev;
    double LastDamageState;
    double currentDamageState;
    double diffrence;
    unsigned earthquakeNo=eq->getNumberOfEarthquakes();
    unsigned pointNo=wm->getPropertyPointer()->getNumberOfPoints();
    for (unsigned int iEQ=0;iEQ < earthquakeNo;iEQ++)// counter for Earthquakes
{
        for (unsigned iPoint=0;iPoint < pointNo;iPoint++)// counter for points(locations)
            {
            vector<double> _PGA;
            vector<float>  _PGA_weight;
            _PGA=wm->calculatePGAByEarthquakeAndPointNumber(iEQ,iPoint,_PGA_weight);
            for(unsigned iPGA=0;iPGA<_PGA_weight.size();iPGA++){
                for(unsigned iStructural=0;iStructural<structuralNo;iStructural++){// counter for structural type
                    unsigned damageStateNo=vm->getNumberOfDamageStatesForStructuralType(iStructural);
                    for(unsigned iDamageState=0;iDamageState<damageStateNo;iDamageState++)// counter for damage states
                        {
                        damageRatio=vm->getDamageStateCurveValue(_PGA[iPGA],iStructural,iDamageState);
                        dmg_std_dev=vm->getCurveStandardDeviation(iStructural);
                        unsigned c=0;
                        // Logic tree on PGA uncertainity
                        for(int i=-3;i<=3;i++){
                            float dev=float(i)/2;
                            float _weight_=assignWeight(dev);
                            float conditionedDamageRatio=damageRatio*exp(dmg_std_dev*dev);
                            if(conditionedDamageRatio>1)
                                conditionedDamageRatio=1;
                            else if(conditionedDamageRatio<-0.000000001){
                                cout<<"Logical Error: conditionedDamageRatio<0. Exiting!"<<endl;
                                exit(1);
                            }
                            //c is counter for damage distributionb
                            wm->property.damage[iPGA][c].setDamageRatio(conditionedDamageRatio,iEQ,iPoint,iStructural,iDamageState);
                            wm->property.damage[iPGA][c].setWeight(_weight_*_PGA_weight[iPGA]); // the final logic tree weight
                            c++;
                        }
                    }
                    //Calculating deferrential damages
                    for(unsigned iDev=0;iDev<7;iDev++){
                        LastDamageState=0;
                        diffrence=0;
                        for(int iDamageState=damageStateNo-1;iDamageState>=0;iDamageState--){
                            currentDamageState=wm->property.damage[iPGA][iDev].getDamageRatio(iEQ,iPoint,iStructural,iDamageState);
                            diffrence=currentDamageState-LastDamageState;
                            LastDamageState=currentDamageState;
                            wm->property.damage[iPGA][iDev].setDamageStateShare(diffrence,iEQ,iPoint,iStructural,iDamageState);
                        }
                    }
                }
            }
        }
    }
}

void calculateDamagedAreaAndmonetaryLoss(WorldModel *wm,VulnerabilityModel *vm)
{
    unsigned structuralNo=wm->getPropertyObject().getNumberOfStructuralType();
    unsigned earthquakeNo=wm->getEarthquakeObject().getNumberOfEearthquakes();
    unsigned pointNo=wm->getPropertyObject().getNumberOfPoints();
    for (unsigned iEQ=0;iEQ < earthquakeNo;iEQ++) // counter for Earthquakes
    {
        for (unsigned iPoint=0;iPoint < pointNo;iPoint++) // counter for points
        {
            for (unsigned iStructural=0;iStructural<structuralNo;iStructural++) // counter for structural type
            {
                unsigned damageStateNo=vm->getNumberOfDamageStatesForStructuralType(iStructural);
                double _area=wm->property.getArea(iPoint,iStructural);
                for(unsigned iDamageState=0;iDamageState<damageStateNo;iDamageState++)
                {
                    double damage_state_share;
                    for(unsigned iPGA=0;iPGA<7;iPGA++)
                    {
                        for(unsigned iDev=0;iDev<7;iDev++)
                        {
                            damage_state_share=wm->property.damage[iPGA][iDev].getDamageStateShare(iEQ,iPoint,iStructural,iDamageState);
                            wm->property.damage[iPGA][iDev].setDamagedArea(_area*damage_state_share,iEQ,iPoint,iStructural,iDamageState);
                        }
                    }
                }
                double monetary_value=wm->property.getMonetaryValue(iPoint,iStructural);
                float  damagestate_to_monetaryloss_ratio;
                double damaged_area;
                double _product=0;
                for(unsigned iDamageState=0;iDamageState<damageStateNo;iDamageState++)
                {
                    damagestate_to_monetaryloss_ratio=vm->getDamageStateToMonetaryLossRatio(iStructural,iDamageState);
                    for(unsigned iPGA=0;iPGA<7;iPGA++)
                    {
                        for(unsigned iDev=0;iDev<7;iDev++)
                        {
                            damaged_area=wm->property.damage[iPGA][iDev].getDamagedArea(iEQ,iPoint,iStructural,iDamageState);
                            _product=damagestate_to_monetaryloss_ratio*damaged_area*monetary_value;
                            wm->property.damage[iPGA][iDev].setMonetaryLoss(_product,iEQ,iPoint,iStructural,iDamageState);
                        }
                    }
                }
            }
        }
    }
}

float assignWeight(float dev)
{
    if(fabs(fabs(dev)>1.25))
        return 0.1056;
    else if(fabs(fabs(dev)-1)<0.00001)
        return 0.1210;
    else if(fabs(fabs(dev)-0.5)<0.00001)
        return 0.1747;
    else if(fabs(fabs(dev)-0)<0.00001)
        return 0.1974;
    return 0;
}
