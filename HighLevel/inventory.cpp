#include<iostream>
#include<fstream>
#include<vector>
#include<sstream>
#include<conio.h>
#include "inventory.h"

#include "insurancemodel.h"

using namespace std;

void Inventory::showSelective() const
{
    cout<<"This is hazID"<<endl;
    cout<<hazard_IDTAble_IDRealtionship[0][0]<<endl;
    cout<<hazard_IDTAble_IDRealtionship[0][1]<<endl;
    cout<<"----------"<<endl;
}

unsigned Inventory::getNumberOFHazards() const
{
    return number_HazardCases;
}

unsigned Inventory::getNumberOfLocations() const
{
    return number_Locations;
}

unsigned Inventory::getNumberOfTypes() const
{
    return number_Types;
}

unsigned Inventory::getNumberOFIDTable() const
{
    return number_IDTable;
}

unsigned Inventory::getHazardIndexByIDTableIndex(unsigned i)  const
{
    return _IDTable[i][IDTable_Hazard];
}

unsigned Inventory::searchForIndexOfFirstMatch(unsigned hazard_ID)
{
    for(unsigned i=0;i<getNumberOFIDTable();i++)
    {
        if(_IDTable[i][IDTable_Hazard]==hazard_ID)
            return i;
    }
    return 0;
    cout<<"serious error in "<<'"'<<"unsigned Inventory::searchForIndexOfFirstMatch(unsigned hazard_ID)"<<'"'<<'.'<<endl;
}

void Inventory::getGrossDamagesOfTypes(vector<float> &_damage, unsigned _damage_index)  const
{
    _damage.resize(_GrossDamageTable[_damage_index].size(),0);
    for(unsigned i=0;i<_DamageTable[_damage_index].size();i++)
    {
        _damage[i]=_GrossDamageTable[_damage_index][i];
    }
}

void Inventory::showData() const
{
    cout<<"number of IDTable="<<number_IDTable<<endl;
    for (unsigned i=0;i<number_IDTable;i++)
    {
        for (unsigned j=0;j<_IDTable[i].size();j++)
            cout<<_IDTable[i][j]<<"   ";
        cout<<endl;
    }
    cout<<"number of Hazards= "<<number_HazardCases<<endl;
    cout<<"Hazard cases consist of: "<<endl;
    for (unsigned i=0;i<number_HazardCases;i++)
    {
        for (unsigned j=0;j<_HazardTable[i].size();j++)
            cout<<_HazardTable[i][j]<<"   ";
        cout<<endl;
    }
    cout<<"number of Locations= "<<number_Locations<<endl;
    cout<<"Locations consist of: "<<endl;
    for (unsigned i=0;i<number_Locations;i++)
    {
        for (unsigned j=0;j<_LocationTable[i].size();j++)
            cout<<_LocationTable[i][j]<<"   ";
        cout<<endl;
    }
    cout<<"number of Types= "<<number_Types<<endl;
    cout<<"Damages of Types consist of: "<<endl;
    for (unsigned i=0;i<_DamageTable.size();i++)
    {
        for (unsigned j=0;j<_DamageTable[i].size();j++)
            cout<<_DamageTable[i][j]<<"   ";
        cout<<endl;
    }
}


void Inventory::addToLog()
{
    Log::totalLoss.push_back(totalMeanLoss);
}

Inventory::Inventory()
{
    const char *idFile="id.txt";
    const char *locationFile="loc.txt";
    const char *damageFile="damage.txt";
    const char *hazardFile="hazard.txt";
    string   line;

    //********* opening id File ***********
    ifstream idFileRef(idFile,ifstream::in);
    unsigned unsigned_temp;
    float    float_temp;
    while(getline(idFileRef,line))
    {
        vector<unsigned>   lineData;
        stringstream  linestream(line);
        while(linestream>>unsigned_temp)
        {
            lineData.push_back(unsigned_temp);
        }
        _IDTable.push_back(lineData);
    }

    //********* opening damage File ***********
    ifstream damageFileRef(damageFile,ifstream::in);
    while(getline(damageFileRef,line))
    {
        vector<float>  lineData;
        stringstream   linestream(line);
        while(linestream>>float_temp)
        {
            lineData.push_back(float_temp);
        }
        _DamageTable.push_back(lineData);
    }
    //********* opening Location File ***********
    ifstream locationFileRef(locationFile,ifstream::in);
    while(getline(locationFileRef,line))
    {
        vector<float>   lineData;
        stringstream  linestream(line);
        while(linestream>>float_temp)
        {
            lineData.push_back(float_temp);
        }
        _LocationTable.push_back(lineData);
    }
    //********* opening Hazard File ***********
    ifstream hazardFileRef(hazardFile,ifstream::in);
    while(getline(hazardFileRef,line))
    {
        vector<float>   lineData;
        stringstream    linestream(line);
        while(linestream>>float_temp)
        {
            lineData.push_back(float_temp);
        }
        _HazardTable.push_back(lineData);
    }
    //********* set parameters ***********
    number_HazardCases=_HazardTable.size();
    number_Locations=_LocationTable.size();
    if(_DamageTable.size()>0)
        number_Types=_DamageTable[0].size();
    number_IDTable=_IDTable.size();
    hazard_IDTAble_IDRealtionship.resize(getNumberOFHazards());
    for(unsigned i=0;i<getNumberOFHazards();i++)
    {
        hazard_IDTAble_IDRealtionship[i].resize(2);
        hazard_IDTAble_IDRealtionship[i][0]=searchForIndexOfFirstMatch(i);
        if(i!=getNumberOFHazards()-1)
            hazard_IDTAble_IDRealtionship[i][1]=searchForIndexOfFirstMatch(i+1)-1;
        else
            hazard_IDTAble_IDRealtionship[i][1]=getNumberOFIDTable()-1;
    }
    //********* converting vector to array to be done **********
    delete [] idFile;
    delete [] locationFile;
    delete [] damageFile;
    delete [] hazardFile;
}

Inventory::Inventory(WorldModel *wm)
{
    totalMeanLoss=0;
    totalGrossMeanLoss=0;
    Earthquake *eq=wm->getEarthquakePointer();
    Property   *property=wm->getPropertyPointer();
    /////  hazardTable /////
    float aggregatedProbability=0;
    for(unsigned i=0;i<eq->getNumberOfEearthquakes();i++)
    {
        aggregatedProbability+=eq->getProbability(i);
        vector<float> line_data;
        vector<float> coord;
        eq->getCoordination(i,coord);
        line_data.push_back(eq->getProbability(i));
        line_data.push_back(coord[0]);
        line_data.push_back(coord[1]);
        line_data.push_back(eq->getMaginitude(i));
        _HazardTable.push_back(line_data);
    }
    vector<float> line_data_temp;
    line_data_temp.push_back(1-aggregatedProbability);
    line_data_temp.push_back(0);
    line_data_temp.push_back(0);
    line_data_temp.push_back(0);
    _HazardTable.push_back(line_data_temp);
    /*for(unsigned i=0;i<_HazardTable.size();i++)
    {
        for(unsigned j=0;j<_HazardTable[i].size();j++)
        {
            cout<<_HazardTable[i][j]<<"   ";
        }
        cout<<endl;
    }
    getch();*/
    /////  Location table  /////
    for(unsigned i=0;i<property->getNumberOfPoints();i++)
    {
        vector<float> line_data;
        vector<float> coord;
        property->getCoordination(i,coord);
        line_data.push_back(coord[0]);
        line_data.push_back(coord[1]);
        for(unsigned j=0;j<property->getNumberOfStructuralType();j++)
        {
            line_data.push_back(property->getMonetaryValue(i,j)*property->getArea(i,j));
        }
        _LocationTable.push_back(line_data);
    }
    /////  Damage  /////
    cout<<"Creating Damage Table."<<endl;
    damage.resize(property->damage.size());
    for(unsigned i=0;i<property->damage.size();i++)
    {
        damage[i].resize(property->damage[i].size());
        for(unsigned j=0;j<property->damage[i].size();j++)
        {
            damage[i][j].setMonetaryLossByVectorPointer(property->damage[i][j].getMonetaryLossVectorPointer());
            damage[i][j].setWeight(property->damage[i][j].getWeight());
        }
    }

    /////  ID TABLE  /////
    cout<<"creating ID TAble."<<endl;
    unsigned _counter=0;
    for (unsigned int iEQ=0;iEQ < eq->getNumberOfEearthquakes();iEQ++)// counter for Earthquakes
    {
        for (unsigned int iPoint=0;iPoint < property->getNumberOfPoints();iPoint++)// counter for points
        {
            vector<unsigned> line_data;
            line_data.push_back(iEQ);
            line_data.push_back(iPoint);
            line_data.push_back(_counter);
            _IDTable.push_back(line_data);
            _counter++;
        }
    }

    for (unsigned int iPoint=0;iPoint < property->getNumberOfPoints();iPoint++)// counter for points
    {
        vector<unsigned> line_data_unsigned_temp;
        line_data_unsigned_temp.push_back(eq->getNumberOfEearthquakes());
        line_data_unsigned_temp.push_back(iPoint);
        line_data_unsigned_temp.push_back(_counter);
        _IDTable.push_back(line_data_unsigned_temp);
    }

    /////  Set Values  /////
    cout<<"Setting The Values"<<endl;
    _value.resize(property->getNumberOfPoints());
    for(unsigned iPoint=0;iPoint<property->getNumberOfPoints();iPoint++)
    {
        _value[iPoint].resize(property->getNumberOfStructuralType(),0);
        for(unsigned iType=0;iType<property->getNumberOfStructuralType();iType++)
        {
            _value[iPoint][iType]=property->getMonetaryValue(iPoint,iType)*property->getArea(iPoint,iType);
        }
    }
    /////  Set Parameters  /////
    number_HazardCases=_HazardTable.size();
    number_Locations=_LocationTable.size();
    number_Types=property->getNumberOfStructuralType();
    number_IDTable=_IDTable.size();
    hazard_IDTAble_IDRealtionship.resize(getNumberOFHazards());
    for(unsigned i=0;i<getNumberOFHazards();i++)
    {
        hazard_IDTAble_IDRealtionship[i].resize(2);
        hazard_IDTAble_IDRealtionship[i][0]=searchForIndexOfFirstMatch(i);
        if(i!=getNumberOFHazards()-1)
            hazard_IDTAble_IDRealtionship[i][1]=searchForIndexOfFirstMatch(i+1)-1;
        else
            hazard_IDTAble_IDRealtionship[i][1]=getNumberOFIDTable()-1;
    }
    sumOfPeropertiesValues=0;
    for(unsigned i=0;i<getNumberOfLocations();i++)
    {
        for(unsigned j=0;j<getNumberOfTypes();j++)
        {
            sumOfPeropertiesValues+=property->getArea(i,j)*property->getMonetaryValue(i,j);
        }
    }

    /////  Gross Damage  /////
    float weight=0;
    double temp=0;
    totalGrossMeanLoss=0;
    _GrossDamageTable.resize((getNumberOFHazards()-1)*getNumberOfLocations());
    unsigned _DGTCounter=0;
    for(unsigned iEQ=0;iEQ<getNumberOFHazards()-1;iEQ++)
    {
        for(unsigned iPoint=0;iPoint<getNumberOfLocations();iPoint++)
        {
            _GrossDamageTable[_DGTCounter].resize(getNumberOfTypes(),0);
            for(unsigned iType=0;iType<getNumberOfTypes();iType++)
            {
                for(unsigned i=0;i<damage.size();i++)
                {
                    for(unsigned j=0;j<damage[i].size();j++)
                    {
                        temp=0;
                        weight=damage[i][j].getWeight();
                        for(unsigned iDamageState=0;iDamageState<damage[i][j].monetaryLoss[iEQ][iPoint][iType].size();iDamageState++)
                        {
                            temp+=damage[i][j].monetaryLoss[iEQ][iPoint][iType][iDamageState];
                        }
                        temp*=weight;
                        totalGrossMeanLoss+=temp;
                        _GrossDamageTable[_DGTCounter][iType]+=temp;
                    }
                }
            }
            _DGTCounter++;
        }
    }
    vector<float> anzalichi;
    for(unsigned iType=0;iType<getNumberOfTypes();iType++)
    {
        anzalichi.push_back(0);
    }
    _GrossDamageTable.push_back(anzalichi);
    Log::totalGrossLoss=totalGrossMeanLoss;
}

double Inventory::getSumOfPropetiesValueByInsuranceCoverageAndPolicyConditions(InsuranceModel *ins)
{
    double out=0;
    for(unsigned location_index=0;location_index<getNumberOfLocations();location_index++)
    {
        for(unsigned type_index=0;type_index<getNumberOfTypes();type_index++)
        {
            out+=ins->insuranceUsage[location_index][type_index]*_value[location_index][2+type_index]*(fabs(ins->policyMaximum[location_index][type_index]-ins->policyDeductible[location_index][type_index]));
        }
    }
    //cout<<"sum of insured value="<<sumOfPeropertiesValues<<endl;
    return sumOfPeropertiesValues;
}

void Inventory::prepareDamageType(InsuranceModel *ins)
{
    float  weight=0;
    double temp=0;
    totalMeanLoss=0;
    _DamageTable.clear();
    _DamageTable.resize((getNumberOFHazards()-1)*getNumberOfLocations());
    unsigned _DTCounter=0;
    for(unsigned iEQ=0;iEQ<getNumberOFHazards()-1;iEQ++)
    {
        for(unsigned iPoint=0;iPoint<getNumberOfLocations();iPoint++)
        {
            _DamageTable[_DTCounter].resize(getNumberOfTypes(),0);
            for(unsigned iType=0;iType<getNumberOfTypes();iType++)
            {
                for(unsigned i=0;i<damage.size();i++)
                {
                    for(unsigned j=0;j<damage[i].size();j++)
                    {
                        temp=0;
                        weight=damage[i][j].getWeight();
                        for(unsigned iDamageState=0;iDamageState<damage[i][j].monetaryLoss[iEQ][iPoint][iType].size();iDamageState++)
                        {
                            temp+=ins->calculateDamageAfterPolicyCondition(damage[i][j].monetaryLoss[iEQ][iPoint][iType][iDamageState], _value[iPoint][iType], iPoint, iType);
                        }
                        temp*=weight;
                        totalMeanLoss+=temp;
                        _DamageTable[_DTCounter][iType]+=temp;
                    }
                }
            }
            _DTCounter++;
        }
    }
    vector<float> anzalichi;
    for(unsigned iType=0;iType<getNumberOfTypes();iType++)
    {
        anzalichi.push_back(0);
    }
    _DamageTable.push_back(anzalichi);
}

/*void Inventory::prepareTreatyReinsuredDamageType(InsuranceModel *)
{
    float  weight=0;
    double temp=0;
    totalMeanLoss=0;
    _TreatyReinsuredDamageTable.clear();
    _TreatyReinsuredDamageTable.resize((getNumberOFHazards()-1)*getNumberOfLocations());
    unsigned _DTCounter=0;
    for(unsigned iEQ=0;iEQ<getNumberOFHazards()-1;iEQ++)
    {
        for(unsigned iPoint=0;iPoint<getNumberOfLocations();iPoint++)
        {
            _TreatyReinsuredDamageTable[_DTCounter].resize(getNumberOfTypes(),0);
            for(unsigned iType=0;iType<getNumberOfTypes();iType++)
            {
                for(unsigned i=0;i<damage.size();i++)
                {
                    for(unsigned j=0;j<damage[i].size();j++)
                    {
                        temp=0;
                        weight=damage[i][j].getWeight();
                        for(unsigned iDamageState=0;iDamageState<damage[i][j].monetaryLoss[iEQ][iPoint][iType].size();iDamageState++)
                        {
                            temp+=ins->calculateDamageAfterPolicyCondition(damage[i][j].monetaryLoss[iEQ][iPoint][iType][iDamageState], _value[iPoint][iType], iPoint, iType);
                        }
                        temp*=weight;
                        totalMeanLoss+=temp;
                        _DamageTable[_DTCounter][iType]+=temp;
                    }
                }
            }
            _DTCounter++;
        }
    }
    vector<float> anzalichi;
    for(unsigned iType=0;iType<getNumberOfTypes();iType++)
    {
        anzalichi.push_back(0);
    }
    _DamageTable.push_back(anzalichi);
}*/
