#ifndef STOCHASTIC_H_INCLUDED
#define STOCHASTIC_H_INCLUDED
//#include "inventory.h"

class StochasticModule
{
protected:
unsigned timeSpan;
//unsigned c;
public:
    void goDeepIntoTree(InsuranceModel*, Result *, Inventory *,float cpt, unsigned iTimeSpan);
    StochasticModule();
};
#endif // STOCHASTIC_H_INCLUDED
