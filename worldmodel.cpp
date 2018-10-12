#include "func.h"
#include "worldmodel.h"
#include "damage.h"
#include "earthquake.h"

/*  CALCULATION FUNCTIONS  */
double WorldModel::attenuation(float m,float d,attenuationType _at,float dev){
    float std_dev=0;
    double logPGA=0;
    switch(_at)
    {
    case AMBBom91:
    {
        float h0=6;
        float r=sqrt(d*d+h0*h0);
        logPGA=-1.09+0.238*m-1.0*log10(r)-0.0005*r;
        std_dev=0.28;
        break;
    }
    case RMZ:
    {
        float _H=pow(fabs(16*m-d),0.63);
        double _PGA_=4000/981*pow(20+d+_H,-2.11)*exp(0.79*m);
        logPGA=log10(_PGA_);
        std_dev=0.27;
        break;
    }
    case SRV:
    {
        float  h0=3;
        float r=sqrt(d*d+h0*h0);
        logPGA=-1.507+0.24*m-0.5421*log10(r)-0.00397*r;
        std_dev=0.26;
        break;
    }
    default:
        cout<<"error in attenuation.Exiting!"<<endl;
        exit(1);
    }
    logPGA+=std_dev*dev;
    return pow(10,logPGA);
}

float WorldModel::getSurfaceDistance(float x1,float y1,float x2,float y2){
    const float earthRadius=6371;
    float deltaX=x2-x1;
    float deltaY=y2-y1;
    float a=sin(deltaY/2/180*PI)*sin(deltaY/2/180*PI) + cos(y1*PI/180)*cos(y2*PI/180)*sin(deltaX/2/180*PI)*sin(deltaX/2/180*PI);
    float c=2*atan2(sqrt(a),sqrt(1-a));
    return earthRadius*c;
}

void WorldModel::getTheNearestPointOnTheLineToPoint(vector<float> &out_point , float _X1, float _Y1, unsigned iEQ)
{
    float _b1;
    float _a1;
    float _x;
    float _y;
    bool isAccepted=false;
    for(unsigned i=0;i<eq.lineSlope[iEQ].size();i++)
    {
        if(eq.lineSlope[iEQ][i]!=0)
        {
            _a1=1/(-1*eq.lineSlope[iEQ][i]);
            _b1=_Y1-_a1*_X1;
            _x=(eq.distanceFromOrigin[iEQ][i]-_b1)/(_a1-eq.lineSlope[iEQ][i]);
            _y=_a1*_x+_b1;
        }
        else
        {
            _x=_X1;
            _y=eq.distanceFromOrigin[iEQ][i];
        }
        float distance_min=999999999;
        float distance_temp;
        float avance=0.01;
        float ymin, ymax, xmin, xmax;
        if(eq.coordination_line_X[iEQ][i]>eq.coordination_line_X[iEQ][i+1])
        {
            xmax=eq.coordination_line_X[iEQ][i];
            xmin=eq.coordination_line_X[iEQ][i+1];
        }
        else
        {
            xmax=eq.coordination_line_X[iEQ][i+1];
            xmin=eq.coordination_line_X[iEQ][i];
        }
        if(eq.coordination_line_Y[iEQ][i]>eq.coordination_line_Y[iEQ][i+1])
        {
            ymax=eq.coordination_line_Y[iEQ][i];
            ymin=eq.coordination_line_Y[iEQ][i+1];
        }
        else
        {
            ymax=eq.coordination_line_Y[iEQ][i+1];
            ymin=eq.coordination_line_Y[iEQ][i];
        }
        //cout<<"sina="<<eq.isInAdditionalPointGap(iEQ,i)<<"   "<<(( xmin-avance<=_x && _x<=xmax+avance) && (ymin-avance<=_y && _y<=ymax+avance))<<endl;
        if(!eq.isInAdditionalPointGap(iEQ,i)&& ( xmin-avance<=_x && _x<=xmax+avance) && (ymin-avance<=_y && _y<=ymax+avance))
        {
            // x and y are accepted
            distance_temp=getSurfaceDistance(_X1,_Y1,_x,_y);
            //cout<<distance_temp<<endl;
            if(distance_temp<distance_min)
            {
                distance_min=distance_temp;
                out_point.resize(2,0);
                out_point[0]=_x;
                out_point[1]=_y;
            }
            isAccepted=true;
        }
    }
    if(!isAccepted)
    {
        cout<<"no closest line was found. Finding another point"<<endl;
        float distance_min=9999999;
        float distance;
        unsigned i_min=0;
        for(unsigned i=0;i<eq.coordination_line_X[iEQ].size();i++)
        {
            distance=getSurfaceDistance(eq.coordination_line_X[iEQ][i],eq.coordination_line_Y[iEQ][i],_X1,_Y1);
            if(distance<distance_min)
            {
                distance_min=distance;
                i_min=i;
            }
        }
        out_point.push_back(eq.coordination_line_X[iEQ][i_min]);
        out_point.push_back(eq.coordination_line_Y[iEQ][i_min]);
    }
}
vector<double> WorldModel::calculatePGAByEarthquakeAndPointNumber(unsigned int iEQ,unsigned int iPoint,vector<float> &_weight){
    float r;
    if(eq.isPointSource(iEQ))
    {
        r=getSurfaceDistance(property.mapCoordination[iPoint][0],property.mapCoordination[iPoint][1],eq.coordination[iEQ][0],eq.coordination[iEQ][1]);
    }
    else if(eq.isLineSource(iEQ))
    {
        vector<float> nearest_point;
        getTheNearestPointOnTheLineToPoint(nearest_point,property.mapCoordination[iPoint][0],property.mapCoordination[iPoint][1],iEQ);
        r=getSurfaceDistance(property.mapCoordination[iPoint][0],property.mapCoordination[iPoint][1],nearest_point[0],nearest_point[1]);
    }
    else
    {
        cout<<"error in earthquake source definition"<<endl;
        exit(1);
    }
    vector<double> _PGA;
    for(int iDev=-3;iDev<=3;iDev++)
    {
        double temp_PGA=0;
        float dev=float(iDev)/2;
        temp_PGA+=0.4*attenuation(eq.magnitude[iEQ],r,WorldModel::RMZ,dev);
        temp_PGA+=0.35*attenuation(eq.magnitude[iEQ],r,WorldModel::AMBBom91,dev);
        temp_PGA+=0.25*attenuation(eq.magnitude[iEQ],r,WorldModel::SRV,dev);
        _PGA.push_back(temp_PGA);
        _weight.push_back(assignWeight(dev));
    }
    return _PGA;
}

Earthquake WorldModel::getEarthquakeObject()
{
    return eq;
}

Earthquake *WorldModel::getEarthquakePointer()
{
    return &eq;
}


Property   WorldModel::getPropertyObject()
{
    return property;
}

Property  *WorldModel::getPropertyPointer()
{
    return &property;
}

WorldModel::WorldModel(const char *earthquakeFileName, const char *mapCoordinationFileName, const char *inventoryFileName, const char *valueRatioFile, VulnerabilityModel *vm)
{
    vector<vector<float> > data;
    ifstream file1(earthquakeFileName,ifstream::in);
    string   line;
    while(getline(file1, line)){
        vector<float>   lineData;
        stringstream  linestream(line);
        float value;

        while(linestream >> value){
            lineData.push_back(value);
        }
        data.push_back(lineData);
    }
    vector<float> temp_coord;
    unsigned c=0;
    bool isFirst=true;
    for (unsigned i=0;i<data.size();i++){
        if(data[i][0]>0){
            isFirst=true;
            eq.numberOfDefinitionPoints.push_back(unsigned(data[i][0]));
            eq.magnitude.push_back(data[i][1]);
            if(data[i][0]==1){
                temp_coord.push_back(data[i][2]);
                temp_coord.push_back(data[i][3]);
                eq.coordination.push_back(temp_coord);
                temp_coord.clear();
                vector<float>temp_x;
                temp_x.push_back(0);
                vector<float>temp_y;
                temp_y.push_back(0);
                eq.coordination_line_X.push_back(temp_x);
                eq.coordination_line_Y.push_back(temp_y);
            }
            else if(data[i][0]>1)
            {
                vector<float> temp_x;
                vector<float> temp_y;
                for(unsigned j=0;j<data[i][0];j++){
                    temp_x.push_back(data[i][2+2*j]);
                    temp_y.push_back(data[i][3+2*j]);
                }
                eq.coordination_line_X.push_back(temp_x);
                eq.coordination_line_Y.push_back(temp_y);
                vector<float> temp;
                temp.push_back(0);
                temp.push_back(0);
                eq.coordination.push_back(temp);
            }
            eq.probability.push_back(data[i][4+2*(data[i][0]-1)]);
        }
        else if(data[i][0]<0){
            c++;
            vector<unsigned> temp;
            unsigned lastIndex=eq.numberOfDefinitionPoints.size();
            isFirst=true;
            temp.push_back(lastIndex-1);
            temp.push_back(eq.coordination_line_X[lastIndex-1].size()-1);
            eq.additionalDefinitionData.push_back(temp);//atten
            for(unsigned j=0;j<unsigned(abs(data[i][0]));j++){
                eq.coordination_line_X[lastIndex-1].push_back(data[i][1+2*j]);
                eq.coordination_line_Y[lastIndex-1].push_back(data[i][2+2*j]);
            }
            eq.numberOfDefinitionPoints[lastIndex-1]+=abs(data[i][0]);
        }
        eq.calculateLineEquationParameters(i-c);
    }
    eq.earthquakeQuantity=eq.magnitude.size();

    ifstream file2(mapCoordinationFileName,ifstream::in);
    if(file2.good()==false)
    {
        cout<<"Unable to open file "<<mapCoordinationFileName<<". Exiting!"<<endl;
        getch();
        exit(1);
    }
    while(getline(file2, line))
    {
        vector<float> lineData;
        stringstream  linestream(line);
        float value;
        while(linestream >> value)
            lineData.push_back(value);
        if (lineData.size()>0)
            property.mapCoordination.push_back(lineData);
    }

    ifstream file3(inventoryFileName,ifstream::in);
    if(file3.good()==false)
    {
        cout<<"Unable to open file "<<inventoryFileName<<". Exiting!"<<endl;
        getch();
        exit(1);
    }
    while(getline(file3, line))
    {
        vector<float> lineData;
        stringstream  linestream(line);
        float value;
        while(linestream >> value)
            lineData.push_back(value);
        if (lineData.size()>0)
            property.area.push_back(lineData);
    }
    ifstream file4(valueRatioFile,ifstream::in);
    if(file4.good()==false)
    {
        cout<<"Unable to open file "<<valueRatioFile<<". Exiting!"<<endl;
        getch();
        exit(1);
    }
    while(getline(file4, line)){
        <float> lineData;
        stringstream  linestream(line);
        float value;
        bool is_first_time=true;
        while(linestream >> value){
            if(is_first_time){
                property.baseValue.push_back(value);
                is_first_time=false;
            }
            else{
                lineData.push_back(value);
            }
        }
        if (lineData.size()>0)
            property.valueRatio.push_back(lineData);
        else{
            cout<<"one of the lines in Property Value file seems is not configured properly."<<endl;
            getch();
            exit(0);
        }
    }
    property.prepareVariables(vm,eq,7,7);
    delete [] earthquakeFileName;
    delete [] mapCoordinationFileName;
    delete [] inventoryFileName;
    delete [] valueRatioFile;
}
