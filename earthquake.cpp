#include<iostream>
#include "earthquake.h"
inline bool     Earthquake::isLineSource(unsigned iEQ)
{
    if(numberOfDefinitionPoints[iEQ]>1)
        return 1;
    else
        return 0;
}

inline bool     Earthquake::isPointSource(unsigned iEQ)
{
    if(numberOfDefinitionPoints[iEQ]==1)
        return 1;
    else
        return 0;
}

inline void     Earthquake::calculateLineEquationParameters(unsigned iEQ)
{
    vector<float> temp_a;
    vector<float>  temp_b;
    if(isLineSource(iEQ))
    {
        if(lineSlope.size()-1==iEQ)
        {
            lineSlope[iEQ].clear();
            distanceFromOrigin[iEQ].clear();
            for(unsigned i=0;i<numberOfDefinitionPoints[iEQ]-1;i++)
            {
                float _a;
                if(coordination_line_X[iEQ][i+1]!=coordination_line_X[iEQ][i])
                    _a=(coordination_line_Y[iEQ][i+1]-coordination_line_Y[iEQ][i])/(coordination_line_X[iEQ][i+1]-coordination_line_X[iEQ][i]);
                else
                    _a=99999999;
                lineSlope[iEQ].push_back(_a);
                float _b=coordination_line_Y[iEQ][i]-_a*coordination_line_X[iEQ][i];
                distanceFromOrigin[iEQ].push_back(_b);
            }
        }
        else
        {
            for(unsigned i=0;i<numberOfDefinitionPoints[iEQ]-1;i++)
            {
                float _a;
                //cout<<coordination_line_X[iEQ][i+1]<<"   "<<coordination_line_X[iEQ][i]<<"   "<<coordination_line_Y[iEQ][i+1]<<"   "<<coordination_line_Y[iEQ][i]<<endl;
                if(coordination_line_X[iEQ][i+1]!=coordination_line_X[iEQ][i])
                    _a=(coordination_line_Y[iEQ][i+1]-coordination_line_Y[iEQ][i])/(coordination_line_X[iEQ][i+1]-coordination_line_X[iEQ][i]);
                else
                    _a=99999999;
                temp_a.push_back(_a);
                float _b=coordination_line_Y[iEQ][i]-_a*coordination_line_X[iEQ][i];
                temp_b.push_back(_b);
            }
            lineSlope.push_back(temp_a);
            distanceFromOrigin.push_back(temp_b);
        }
    }
    else if(isPointSource(iEQ))
    {
        temp_a.push_back(float(0));
        temp_b.push_back(float(0));
        lineSlope.push_back(temp_a);
        distanceFromOrigin.push_back(temp_b);
    }
    else
    {
        cout<<"error in calucluate LineEquationParameters"<<endl;
    }
}

bool Earthquake::isInAdditionalPointGap(unsigned iEQ, unsigned slopeNumber)
{
    for(unsigned i=0;i<additionalDefinitionData.size();i++)
    {
        if(additionalDefinitionData[i][0]==iEQ)
        {
            for(unsigned j=1;j<additionalDefinitionData[i].size();j++)
            {
                if(slopeNumber==additionalDefinitionData[i][j])
                    return true;
            }
        }
    }
    return false;
}



float Earthquake::getProbability(unsigned i)
{
    return probability[i];
}

float Earthquake::getMaginitude(unsigned i)
{
    return magnitude[i];
}

void Earthquake::getCoordination(unsigned i, vector<float> &out_coord)
{
    out_coord.push_back(coordination[i][0]);
    out_coord.push_back(coordination[i][1]);
}
