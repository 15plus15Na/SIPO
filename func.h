//#pragma once
#ifndef FUNC_H_INCLUDED
#define FUNC_H_INCLUDED
#include "vulnerabilitymodel.h"
#include "worldmodel.h"
// Calculates The DamagesRatio
float assignWeight(float dev);
void calculateDamageRatioAndDamageStateShare(WorldModel *wm,VulnerabilityModel *vm);

/*

*/
/*void exportResultsIntoTextFiles(WorldModel *wm, VulnerabilityModel *vm)
{
    std::ofstream hazard_file("hazard.txt", std::ofstream::out);
    Earthquake eq=wm->getEarthquakeObject();
    if(hazard_file.good())
    {
        for(unsigned i=0;i<eq.getNumberOfEearthquakes();i++)
        {
            std::vector<float> _coord;
            eq.getCoordination(i,_coord);
            hazard_file<<eq.getProbability(i)<<"   "<<_coord[0]<<"   "<<_coord[1]<<"    "<<eq.getMaginitude(i)<<std::endl;
        }
    }
    float probabilityAggregation=0;
    for(unsigned i=0;i<eq.getNumberOfEearthquakes();i++)
    {
        probabilityAggregation+=eq.getProbability(i);
    }
    hazard_file<<1-probabilityAggregation<<"   "<<(float) 0.0<<"   "<<(float) 0.0<<"   "<<(float) 0.0<<std::endl;

    hazard_file.close();

    Property property=wm->getPropertyObject();
    std::ofstream damage_file("damage.txt",std::ofstream::out);
    for (unsigned int iEQ=0;iEQ < wm->getEarthquakeObject().getNumberOfEearthquakes();iEQ++)// counter for Earthquakes
    {
        for (unsigned int iPoint=0;iPoint < wm->getPropertyObject().getNumberOfPoints();iPoint++)// counter for points
        {
            unsigned structuralNo=wm->getPropertyObject().getNumberOfStructuralType();
            for (unsigned int iStructural=0;iStructural<structuralNo;iStructural++)// counter for structural type
            {
                double aggregateLoss=0;
                unsigned damageStateNo=vm->getNumberOfDamageStatesForStructuralType(iStructural);
                for (unsigned int iDamageState=0;iDamageState<damageStateNo;iDamageState++)// counter for damage states
                {
                    aggregateLoss+=property.getMonetaryLoss(iEQ, iPoint, iStructural, iDamageState);
                }
                damage_file<<aggregateLoss<<"   ";
            }
            damage_file<<std::endl;
        }
    }
    for (unsigned int iStructural=0;iStructural<wm->getPropertyObject().getNumberOfStructuralType();iStructural++)// counter for structural type
        damage_file<<0<<"   ";
    damage_file<<std::endl;
    damage_file.close();

    std::ofstream location_file("loc.txt",std::ofstream::out);
    for (unsigned int iPoint=0;iPoint < wm->getPropertyObject().getNumberOfPoints();iPoint++)// counter for points
    {
        std::vector<float> _coord;
        property.getCoordination(iPoint, _coord);
        location_file<<_coord[0]<<"   "<<_coord[1]<<"   ";
        unsigned structuralNo=wm->getPropertyObject().getNumberOfStructuralType();
        for (unsigned int iStructural=0;iStructural<structuralNo;iStructural++)// counter for structural type
        {
            location_file<<property.getMonetaryValue(iPoint,iStructural)*property.getArea(iPoint,iStructural)<<"   ";
        }
        location_file<<std::endl;
    }
    location_file.close();

    std::ofstream _ID_Table_file("id.txt",std::ofstream::out);
    unsigned _counter=0;
    for (unsigned int iEQ=0;iEQ < wm->getEarthquakeObject().getNumberOfEearthquakes();iEQ++)// counter for Earthquakes
    {
        for (unsigned int iPoint=0;iPoint < wm->getPropertyObject().getNumberOfPoints();iPoint++)// counter for points
        {
            _ID_Table_file<<iEQ<<"   "<<iPoint<<"   "<<_counter<<std::endl;
            _counter++;
        }
    }

    for (unsigned int iPoint=0;iPoint < wm->getPropertyObject().getNumberOfPoints();iPoint++)// counter for points
    {
        _ID_Table_file<<eq.getNumberOfEearthquakes()<<"   "<<iPoint<<"   "<<_counter<<std::endl;
    }
    _ID_Table_file.close();
}*/

#endif // FUNC_H_INCLUDED
