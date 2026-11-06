#include "HitTube.h"

HitTube::HitTube(int id)
{
    fNRawPE=0;
    fPhotoElectrons.clear();

    fNDigiHits=0;
    fTimeDigi.clear();
    fChargeDigi.clear();
    fParentCompDigi.clear();
    fPEIdxDigi.clear();

    fTubeID = id;
    for(int i=0; i<3; i++)
    {
        fPosition[i] = 0.;
        fOrientation[i] = 0.;
    }

    hDigiWF = nullptr;
    fPullQ = -99;
    fPullT = -99;
    fTrueT = -99;
    fTrueQ = -99;
}


HitTube::~HitTube()
{
    for(TrueHit *th : fPhotoElectrons){ delete th; }
    vector<TrueHit*>().swap(fPhotoElectrons );
 //    vector<pair<float,int>>().swap( fPhotoElectrons );
 //   vector<TrueHit*>().swap( fTrueHitInfo );

    vector<float>().swap( fTimeDigi );
    vector<float>().swap( fChargeDigi );
    vector<vector<int>>().swap( fParentCompDigi );
    vector<vector<int>>().swap( fPEIdxDigi );

    if(hDigiWF != nullptr) delete hDigiWF;
}
