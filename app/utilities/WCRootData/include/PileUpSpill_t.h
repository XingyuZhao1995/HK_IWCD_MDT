#pragma once

#include <vector>

const int kMaxNuInt = 200;
// Limit on the total number of tracks of all interactions in one spill.
// Single NEUT events in WCSim files can have >10k saved tracks.
const int kMaxTracks = 1000000;

struct PileUpSpill_t
{
    unsigned int NNuIntTot;
    unsigned int NIDNuInt;
    unsigned int NBeamBkg;
    unsigned int NuIntType[kMaxNuInt];
    float NuIntTime[kMaxNuInt];  
    unsigned int NuIntBunch[kMaxNuInt];
    // Truth of each interaction, from the NEUT RooTracker tree of its input file
    int NeutCode[kMaxNuInt];
    int NuPDG[kMaxNuInt];      // Incoming neutrino
    float NuEne[kMaxNuInt];    // MeV
    float NuPos[kMaxNuInt][3]; // Vertex, cm
    float NuDir[kMaxNuInt][3]; // Unit vector
    float EdepoOD[kMaxNuInt];
    unsigned int FileIdNum[kMaxNuInt];
    unsigned int EventIdNum[kMaxNuInt];
    
    unsigned int NTracks;
    unsigned int TrackNuIntIdx[kMaxTracks];

    // Interaction index of each true hit, -1 for dark noise. Same order as
    // GetCherenkovHitTimes() of trigger 0 in wcsimrootevent and wcsimrootevent_OD.
    std::vector<int> PENuIntIdx;
    std::vector<int> PENuIntIdx_OD;
    // ROOT reads a vector branch through a pointer to it
    std::vector<int> *PENuIntIdxPtr = &PENuIntIdx;
    std::vector<int> *PENuIntIdx_ODPtr = &PENuIntIdx_OD;

    void Clear()
    {
        NNuIntTot = 0;
        NIDNuInt = 0;
        NBeamBkg = 0;
        for(int i=0; i<kMaxNuInt; i++)
        {
            NuIntType[i] = 99999;
            NuIntTime[i] = -99999.;
            NuIntBunch[i] = 99999;
            EdepoOD[i] = -99999.;
            NeutCode[i] = -99999;
            NuPDG[i] = -99999;
            NuEne[i] = -99999.; 
            for(int j=0; j<3; j++)
            {
                NuPos[i][j] = -99999.;
                NuDir[i][j] = -99999.;
            }
            FileIdNum[i] = 99999;
            EventIdNum[i] = 99999;
        } 
        NTracks = 0;
        for(int i=0; i<kMaxTracks; i++)
        {
            TrackNuIntIdx[i] = 99999;
        }
        PENuIntIdx.clear();
        PENuIntIdx_OD.clear();
    }

    void CreateBranch(TTree *t)
    {
        t->Branch("NNuIntTot",      &NNuIntTot,     "NNuIntTot/i");
        t->Branch("NIDNuInt",       &NIDNuInt,      "NIDNuInt/i");
        t->Branch("NBeamBkg",       &NBeamBkg,      "NBeamBkg/i");
        t->Branch("NuIntType",      NuIntType,      "NuIntType[NNuIntTot]/i");
        t->Branch("NuIntTime",      NuIntTime,      "NuIntTime[NNuIntTot]/F");
        t->Branch("NuIntBunch",     NuIntBunch,     "NuIntBunch[NNuIntTot]/i");
        t->Branch("NeutCode",  NeutCode,  "NeutCode[NNuIntTot]/I");
        t->Branch("NuPDG",     NuPDG,     "NuPDG[NNuIntTot]/I");
        t->Branch("NuEne",     NuEne,     "NuEne[NNuIntTot]/F");
        t->Branch("NuPos",     NuPos,     "NuPos[NNuIntTot][3]/F");
        t->Branch("NuDir",     NuDir,     "NuDir[NNuIntTot][3]/F");
        t->Branch("EdepoOD",        EdepoOD,        "EdepoOD[NNuIntTot]/F");
        t->Branch("FileIdNum",      FileIdNum,      "FileIdNum[NNuIntTot]/i");
        t->Branch("EventIdNum",     EventIdNum,     "EventIdNum[NNuIntTot]/i");
        t->Branch("NTracks",        &NTracks,       "NTracks/i");
        t->Branch("TrackNuIntIdx",  TrackNuIntIdx,  "TrackNuIntIdx[NTracks]/i");
        t->Branch("PENuIntIdx",     &PENuIntIdx);
        t->Branch("PENuIntIdx_OD",  &PENuIntIdx_OD);
    }

    void SetBranch(TTree *t)
    {
        t->SetBranchAddress("NNuIntTot",        &NNuIntTot);
        t->SetBranchAddress("NIDNuInt",         &NIDNuInt);
        t->SetBranchAddress("NBeamBkg",         &NBeamBkg);
        t->SetBranchAddress("NuIntType",        NuIntType);
        t->SetBranchAddress("NuIntTime",        NuIntTime);
        t->SetBranchAddress("NuIntBunch",       NuIntBunch);
        t->SetBranchAddress("NeutCode",         NeutCode);
        t->SetBranchAddress("NuPDG",            NuPDG);
        t->SetBranchAddress("NuEne",            NuEne);
        t->SetBranchAddress("NuPos",            NuPos);
        t->SetBranchAddress("NuDir",            NuDir);
        t->SetBranchAddress("EdepoOD",          EdepoOD);
        t->SetBranchAddress("FileIdNum",        FileIdNum);
        t->SetBranchAddress("EventIdNum",       EventIdNum);
        t->SetBranchAddress("NTracks",          &NTracks);
        t->SetBranchAddress("TrackNuIntIdx",    TrackNuIntIdx);
        t->SetBranchAddress("PENuIntIdx",       &PENuIntIdxPtr);
        t->SetBranchAddress("PENuIntIdx_OD",    &PENuIntIdx_ODPtr);
    }
};
