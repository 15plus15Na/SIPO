#include "property.h"
#include <stdlib.h>

bool Property::checkDataIntegrity(VulnerabilityModel *vm)
{
    unsigned vul_type_number=vm->getNumberOfStructuralType();
    if(vul_type_number==area[0].size())
    {
        finalNumberOfTypes=vul_type_number;
    }
    else
    {
        cout<<"Warning: Property type numbers and vulnerability types numbers does not match"<<endl;
        cout<<"The one with smaller values is being considered"<<endl;
        if(vul_type_number<area[0].size())
            finalNumberOfTypes=vul_type_number;
        else
            finalNumberOfTypes=area[0].size();
    }
    if(vm->damageStateToMonetaryLossRatio.size()!=finalNumberOfTypes)
    {
        cout<<"error: number of types in Damage State To Monetary Loss Ratio input file is not consistat with number of types"<<endl;
        exit(1);
    }
    cout<<"Number of types="<<finalNumberOfTypes<<endl;
    return true;
}

void Property::prepareVariables(VulnerabilityModel *vm, Earthquake eq_temp, unsigned numberOfPGAs, unsigned numberOfDamages)
{
    if(!checkDataIntegrity(vm))
    {
        cout<<"Program Failed to continue"<<endl;
        exit(1);
    }
    damage.resize(numberOfPGAs);
    for(unsigned j=0;j<numberOfPGAs;j++)
    {
        damage[j].resize(numberOfDamages);
        for(unsigned i=0;i<damage.size();i++)
        {
            unsigned number_earthquakes=eq_temp.getNumberOfEearthquakes();
            damage[j][i].damagedArea.resize(number_earthquakes);
            damage[j][i].damageRatio.resize(number_earthquakes);
            damage[j][i].damageStateShare.resize(number_earthquakes);
            damage[j][i].monetaryLoss.resize(number_earthquakes);
            for (unsigned iEQ=0; iEQ < number_earthquakes ;iEQ++){// counter for Earthquakes
                unsigned number_points=getNumberOfPoints();
                damage[j][i].damagedArea[iEQ].resize(number_points);
                damage[j][i].damageRatio[iEQ].resize(number_points);
                damage[j][i].damageStateShare[iEQ].resize(number_points);
                damage[j][i].monetaryLoss[iEQ].resize(number_points);
                for (unsigned iPoint=0;iPoint < getNumberOfPoints();iPoint++){ // counter for points
                    unsigned structuralNo=getNumberOfStructuralType();
                    damage[j][i].damagedArea[iEQ][iPoint].resize(structuralNo);
                    damage[j][i].damageRatio[iEQ][iPoint].resize(structuralNo);
                    damage[j][i].damageStateShare[iEQ][iPoint].resize(structuralNo);
                    damage[j][i].monetaryLoss[iEQ][iPoint].resize(structuralNo);
                    for (unsigned iStructural=0;iStructural<structuralNo;iStructural++){// counter for structural type
                        unsigned int damageStateNo=vm->getNumberOfDamageStatesForStructuralType(iStructural);
                        damage[j][i].damagedArea[iEQ][iPoint][iStructural].resize(damageStateNo,0);
                        damage[j][i].damageRatio[iEQ][iPoint][iStructural].resize(damageStateNo,0);
                        damage[j][i].damageStateShare[iEQ][iPoint][iStructural].resize(damageStateNo,0);
                        damage[j][i].monetaryLoss[iEQ][iPoint][iStructural].resize(damageStateNo,0);
                    }
                }
            }
        }
    }
}


//Get Fuctions

void Property::getCoordination(unsigned iPoint, vector<float> & out_coord)
{
    out_coord.push_back(mapCoordination[iPoint][0]);
    out_coord.push_back(mapCoordination[iPoint][1]);
}
