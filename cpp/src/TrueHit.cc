#include "TrueHit.h"

TrueHit::TrueHit(double t, int id)
{
    fTime = t;
    fParentId = id;
    fNuIntIdx = -1;
    fEnergy = 0.;
    fStartTime = 0.;
    fStartEnergy = 0.;
    fCreatorProcess = 0;
    for(int i=0; i<3; i++)
    {
        fPosition[i] = 0.;
        fDirection[i] = 0.;
        fStartPosition[i] = 0.;
        fStartDirection[i] = 0.;
        fBin[i] = 0;
    }
};

TrueHit::TrueHit() : TrueHit(0.)
{
};

TrueHit::~TrueHit()
{
};
