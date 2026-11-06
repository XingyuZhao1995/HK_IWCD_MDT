// This class manages informaion about both true and digitized hits 
// This class assumed to be instanceated for each PMT
// The following information is stored:
//
//  - true hits (just photoelectrons)
//      - number of true hits
//      - true hit time 
//      - parent charged particle ID 
//          - negative value for noise 
//
//  - digitized hits
//      - number of digitized hits
//      - digitized hit time
//      - digitized charge (p.e)
//      - composition of true hits

#pragma once

#include <vector>
#include <algorithm>

#include "TrueHit.h"
using std::vector;

#include "TH1F.h"

class HitTube
{
    public: 
        HitTube(int);
        virtual ~HitTube();
        // Owns its true hits, so must not be copied
        HitTube(const HitTube&) = delete;
        HitTube& operator=(const HitTube&) = delete;
        void SetTubeID(const int i){ fTubeID = i; }
        int GetTubeID() const { return fTubeID; }
        void SetmPMTID(const int i){ fmPMTID = i; }
        int GetmPMTID() const { return fmPMTID; } 
        void SetmPMT_PMTID(const int i){ fmPMT_PMTID = i; }
        int GetmPMT_PMTID() const { return fmPMT_PMTID; } 
		float GetPosition(int i) const { return fPosition[i]; }
		float GetOrientation(int i) const { return fOrientation[i]; }
		void SetPosition(int i, float f) { fPosition[i] = f; }
		void SetOrientation(int i, float f) { fOrientation[i] = f; }


        // For a true hit that does not come from a photon, i.e. dark noise.
        // Photon information is the same as in WCSimWCAddDarkNoise.
        void AddRawPE(const float t, const int id=9999999)
            { 
                TrueHit *th = new TrueHit(t, id);
                th->SetStartTime(t);
                for(int i=0; i<3; i++)
                {
                    th->SetPosition(i, fPosition[i]);
                    th->SetStartPosition(i, fPosition[i]);
                    th->SetDirection(i, -fOrientation[i]);
                    th->SetStartDirection(i, -fOrientation[i]);
                }
                this->AddRawPE(th);
            }
        void AddRawPE(TrueHit* th)
            { 
                fPhotoElectrons.push_back(th);
                fNRawPE+=1; 
            }
        int GetNRawPE() const { return fNRawPE; }
        float GetTimeRaw(const int i) const { return fPhotoElectrons[i]->GetTime(); }
        void SortTrueHits()
        {
            std::sort(fPhotoElectrons.begin(),
              fPhotoElectrons.end(), 
              [](const TrueHit* left, const TrueHit* right)
              { return left->GetTime() < right->GetTime(); }
             );
        }
        const vector<TrueHit*>& GetPhotoElectrons() const { return fPhotoElectrons; }

        // id: parent track IDs of the true hits making up this digit
        // peIdx: positions of these true hits in GetPhotoElectrons()
        void AddDigiHit(const float t, const float q, const vector<int> &id=vector<int>(), const vector<int> &peIdx=vector<int>())
            {
                fTimeDigi.push_back( t );
                fChargeDigi.push_back( q );
                fParentCompDigi.push_back( id );
                fPEIdxDigi.push_back( peIdx );
                fNDigiHits+=1;
            }

        int GetNDigiHits() const { return fNDigiHits; }
        float GetTimeDigi(const int i) const { return fTimeDigi[i]; }
        float GetChargeDigi(const int i) const { return fChargeDigi[i]; }
        const vector<int>& GetParentCompositionDigi(const int i) const { return fParentCompDigi[i]; }
        const vector<int>& GetPEIndicesDigi(const int i) const { return fPEIdxDigi[i]; }

        void SetDigiWF(const TH1F& hist)
        {
            if(hDigiWF != nullptr) delete hDigiWF;
            hDigiWF = new TH1F(hist);
            hDigiWF->SetDirectory(0);
        }
        TH1F* GetDigiWF()
        {
            return hDigiWF;
        }
        void SetDigiPulls(float pullT, float pullQ) { fPullT = pullT; fPullQ = pullQ;}
        void SetTrueTQ(float trueT, float trueQ) { fTrueT = trueT; fTrueQ = trueQ;}
        float GetPullT() {return fPullT;}
        float GetPullQ() {return fPullQ;}
        float GetTrueT() {return fTrueT;}
        float GetTrueQ() {return fTrueQ;}
        
    private:
        int fNRawPE;
        vector<TrueHit*> fPhotoElectrons;

        int fTubeID;
        int fmPMTID;
        int fmPMT_PMTID;
        int fNDigiHits;
		float fPosition[3];
		float fOrientation[3];
        vector<float> fTimeDigi;
        vector<float> fChargeDigi;
        vector<vector<int>> fParentCompDigi;
        vector<vector<int>> fPEIdxDigi;

        TH1F* hDigiWF;
        float fPullT; 
        float fPullQ;
        float fTrueT; 
        float fTrueQ;
};
