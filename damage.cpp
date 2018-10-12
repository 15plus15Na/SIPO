#include "Damage.h"
inline double Damage::getDamageRatio(unsigned iEQ,unsigned iPoint,unsigned iStructul,unsigned iDamageState)
{
    return damageRatio[iEQ][iPoint][iStructul][iDamageState];
}

inline double Damage::getDamageStateShare(unsigned iEQ,unsigned iPoint,unsigned iStructul,unsigned iDamageState)
{
    return damageStateShare[iEQ][iPoint][iStructul][iDamageState];
}


inline double Damage::getDamagedArea(unsigned iEQ,unsigned iPoint,unsigned iStructul,unsigned iDamageState)
{
    return damagedArea[iEQ][iPoint][iStructul][iDamageState];
}

double        Damage::getMonetaryLoss(unsigned iEQ,unsigned iPoint,unsigned iStructul,unsigned iDamageState)
{
    return monetaryLoss[iEQ][iPoint][iStructul][iDamageState];
}

vector<vector<vector<vector<double> > > > *Damage::getMonetaryLossVectorPointer()
{
    return &monetaryLoss;
}


/*  SET FUNCTIONS */
inline void Damage::setDamageRatio(double Damage_ratio,unsigned iEQ,unsigned iPoint,unsigned iStructul,unsigned iDamageState){
    damageRatio[iEQ][iPoint][iStructul][iDamageState]=Damage_ratio;
}

inline void Damage::setDamageStateShare(double Damage_state_share,unsigned iEQ,unsigned iPoint,unsigned iStructul,unsigned iDamageState)
{
    damageStateShare[iEQ][iPoint][iStructul][iDamageState]=Damage_state_share;
}

inline void Damage::setDamagedArea(double Damaged_area,unsigned iEQ,unsigned iPoint,unsigned iStructul,unsigned iDamageState)
{
    damagedArea[iEQ][iPoint][iStructul][iDamageState]=Damaged_area;
}

inline void Damage::setMonetaryLoss(double monetary_loss,unsigned iEQ,unsigned iPoint,unsigned iStructul,unsigned iDamageState)
{
    monetaryLoss[iEQ][iPoint][iStructul][iDamageState]=monetary_loss;
}


void Damage::setMonetaryLossByVectorPointer(vector<vector<vector<vector<double> > > > *ml_dmg)
{
    monetaryLoss=*ml_dmg;
}

void Damage::showMonetaryLoss()
{
    for(unsigned i=0;i<monetaryLoss.size();i++)
    {
        for(unsigned j=0;j<monetaryLoss[i].size();j++)
        {
            for(unsigned k=0;k<monetaryLoss[i][j].size();k++)
            {
                for(unsigned l=0;l<monetaryLoss[i][j][k].size();l++)
                {
                    cout<<monetaryLoss[i][j][k][l]<<"   ";
                }
            }
            cout<<endl;
        }
        getch();
    }
}

Damage::Damage()
{
}

Damage::Damage(const Damage &dmg)
{
    if(&dmg!=this)
    {
        monetaryLoss=dmg.monetaryLoss;
        weight=dmg.weight;
    }
    else
    {
        cout<<"error in copy constructor for Inventory Level in damage.cpp file. Two objects are the same. exiting!"<<endl;
        exit(0);
    }
}
