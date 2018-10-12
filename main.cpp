#include <iostream>
#include <vector>
#include <conio.h>
#include <stdlib.h>

#include "damage.h"
#include "property.h"
#include "func.h"
#include "vulnerabilitymodel.h"
#include "earthquake.cpp"
#include "worldmodel.h"
#include "damage.cpp"
#include "property.cpp"
#include "func.cpp"
#include "worldmodel.cpp"
//--------------------------------------------
#include "insurancemodel.h"
#include "inventory.h"
#include "result.h"
#include "stochastic.h"
#include "settings.h"
#include "approximation.h"
#include "static.h"
#include "log.h"

using namespace std;

int main(int argc, char *argv[])
{
    vector<string> file_name;
    ifstream file("config.txt",ifstream::in);
    if(file.good()!=true)
    {
        cout<<"config not opened. Exiting!"<<endl;
        file.close();
        exit(1);
    }
    string   line;
    WorldModel *wm;
    while(getline(file, line))
        file_name.push_back(line);
    if(file_name.size()>4)
    {
        VulnerabilityModel vm(file_name , 4);
        wm=new WorldModel(file_name[0].c_str(),file_name[1].c_str(),file_name[2].c_str(),file_name[3].c_str(), &vm);
        cout<<"calculating damaspline1dinterpolantge ratio and damage state share"<<endl;
        calculateDamageRatioAndDamageStateShare(wm,&vm);
        cout<<"-->calculating damaged area based in damage state and its corresponding monetary damage"<<endl;
        calculateDamagedAreaAndmonetaryLoss(wm,&vm);
    }
    else
    {
        cout<<"error in config file: the required number of files don't match. There should be at least 4 lines of file names. exiting!"<<endl;
        exit(1);
    }
    // -----------------------------------------------------
    Log _log(Log::SPSA);
    Settings settings(argc,argv);
    SPSA sa;
    Inventory  inventory(wm);
    delete wm;
    InsuranceModel insurer(&inventory,Settings::st_timeSpan);
    insurer.setPolicyConditionThresholds(0.075, 0.875,&inventory);
    insurer.initalGuessOfTurnOver(&inventory, "uniform", 0.25);
    cout<<"begining"<<endl;
    sa.beginIteration(insurer,&inventory);
    //getch();
    return 0;
}

