#include "WCRootDataNuInt.h"

//WCRootDataNuInt::WCRootDataNuInt(int seed)
WCRootDataNuInt::WCRootDataNuInt()
{
    fRnd = new TRandom3(7698);
//    fRnd = new TRandom3(seed);
    fFriendC = 0;
    fFileList.clear();
    fFriendList.clear();

    fHasFriend = false;
    fNuIntType = eSingle;

    fItrEntry = 0;
    fItrFile = 0;
    fNumEntries = 0;
    fNumFiles = 0;
    fFileIdNum = -9999;
    fIntRate = 0.;
    fNDigitsFileIdNum = 5;
    fRooTrackerT = 0;
    fRooVtx = 0;
}

WCRootDataNuInt::~WCRootDataNuInt()
{
    delete fRnd; fRnd = 0;
    if( fFriendC ){ delete fFriendC; fFriendC = 0; }
    fFileList.clear();
    fFriendList.clear();
    std::vector<pair<TString, int>>().swap(fFileList);
    std::vector<pair<TString, int>>().swap(fFriendList);
}

void WCRootDataNuInt::LoadFiles(const char *filename, const vector<string> &list)
{
    ifstream fin(filename);
    string aLine;
    string sname;
    string svalue;
    int  num = -1;
    while( std::getline(fin, aLine) )
    {
        stringstream ss(aLine);
        string stmp;
        ss>>stmp;
        //num = this->GetFileIdNumber(stmp.c_str());
        num++;
        fFileList.push_back(pair<TString, int>(stmp, num));
        std::cout<<" Adding " << stmp <<" " << num <<std::endl;
        //while( ss>>stmp )
        //{
        //    std::cout<<" stmp " << stmp <<std::endl;
        //    if( stmp.find('wcsim')!=string::npos )
        //    {
        //        fFileList.push_back(stmp);
        //    }
        //}
    }
    fin.close();

    fNumFiles = fFileList.size();
    std::cout<<" Found " << fNumFiles <<" files " <<std::endl;

    fWCRootEvtList = list;
    
    fItrFile = fRnd->Integer(fNumFiles);
    this->ReadFile(fFileList[fItrFile].first,fWCRootEvtList);
    fFileIdNum = fFileList[fItrFile].second;
    fNumEntries = this->GetEntries();
    this->ReadRooTracker();
    fItrEntry = fRnd->Integer(fNumEntries);

    std::cout<<" Starting from: " << fFileList[fItrFile].first
             <<" " << fFileList[fItrFile].second
             <<" " << fItrEntry
             <<"/" << fNumEntries
             <<std::endl;
}


bool WCRootDataNuInt::Next()
{
    const int kMaxSkipped = 1000;
    int nSkipped = 0;

    this->ReadNextEntry();
    while( !this->HasSimulatedParticles() )
    {
        std::cout<<" Skipping entry " << this->GetEventIdNumber()
                 <<" of " << fFileList[fItrFile].first
                 <<": no particles were simulated" <<std::endl;
        nSkipped += 1;
        if( nSkipped==kMaxSkipped )
        {
            std::cout<<" [ERROR] WCRootDataNuInt::Next " <<std::endl;
            std::cout<<"  - Skipped " << kMaxSkipped <<" entries in a row. Are these neutrino interaction files?" <<std::endl;
            std::cout<<" -> EXIT "<< std::endl;
            exit(-1);
        }
        this->ReadNextEntry();
    }
    return true;
}

// WCSim runs out of vertices at the end of its vector file, but still writes one more event.
// Nothing is simulated in it: it has only the flag -1 and -2 (neutrino and target) tracks, no
// hits, and truth left over from other vertices. Hits alone cannot tell this apart, as real
// interactions can have none.
bool WCRootDataNuInt::HasSimulatedParticles() const
{
    const TClonesArray *tracks = this->GetTrigger(0)->GetTracks();
    for(int i=0; i<tracks->GetEntries(); i++)
    {
        if( ((WCSimRootTrack*)tracks->At(i))->GetFlag()==0 ){ return true; }
    }
    for(unsigned int i=0; i<fSpEvt.size(); i++)
    {
        if( this->GetTrigger(0, i)->GetNcherenkovhittimes()>0 ){ return true; }
    }
    return false;
}

void WCRootDataNuInt::ReadNextEntry()
{
    if( fItrEntry>=fNumEntries )
    {
        this->CloseFile();
        if( fHasFriend ){ this->CloseFriend(); };
        fItrFile += 1;
        if( fItrFile==fNumFiles  )
        {
            fItrFile = fRnd->Integer(fNumFiles);
            std::cout<<" Rewiding file from " << fFileList[fItrFile].first <<std::endl;
        }
        this->ReadFile(fFileList[fItrFile].first,fWCRootEvtList);
        if( fHasFriend ){ this->ReadFriend(fFriendList[fItrFile].first); };

        fFileIdNum = fFileList[fItrFile].second;
        fNumEntries = this->GetEntries();
        this->ReadRooTracker();
        fItrEntry = fRnd->Integer(fNumEntries);

        std::cout<<" Reading " << fFileList[fItrFile].first 
                 <<" " << fFileList[fItrFile].second
                 <<" " << fItrEntry 
                 <<"/" << fNumEntries 
                 <<std::endl;
    }
    this->GetEntry(fItrEntry);
    if( fRooTrackerT ){ fRooTrackerT->GetEntry(fItrEntry); }
    if( fHasFriend ){ fFriendC->GetEntry(fItrEntry); }
    fItrEntry++;
}

void WCRootDataNuInt::ReadRooTracker()
{
    // WCSim fills this tree once per wcsimT entry
    fRooVtx = 0;
    fRooTrackerT = (TTree*)fWCSimC->GetFile()->Get("fRooTrackerOutputTree");
    if( fRooTrackerT && fRooTrackerT->GetEntries()!=fNumEntries )
    {
        std::cout<<" [WARNING] WCRootDataNuInt::ReadRooTracker " <<std::endl;
        std::cout<<"  - fRooTrackerOutputTree has " << fRooTrackerT->GetEntries()
                 <<" entries, but wcsimT has " << fNumEntries <<std::endl;
        fRooTrackerT = 0;
    }
    if( !fRooTrackerT )
    {
        std::cout<<" [WARNING] No neutrino truth in pupspillT for: " << fWCSimC->GetFile()->GetName() <<std::endl;
        return;
    }
    fRooTrackerT->SetBranchAddress("NRooTrackerVtx", &fRooVtx);
}

void WCRootDataNuInt::SetInteractionInformation(PileUpSpill_t *p) const
{
    if( !fRooTrackerT ){ return; }

    const unsigned int n = p->NNuIntTot;
    p->NeutCode[n] = fRooVtx->EvtCode->GetString().Atoi();
    for(int i=0; i<3; i++){ p->NuPos[n][i] = fRooVtx->EvtVtx[i]*100.; } // m -> cm

    // The incoming neutrino. Do not use the flag -1 track in wcsimT for this,
    // as WCSim overwrites it with the outgoing neutrino of NC events.
    for(int i=0; i<fRooVtx->StdHepN; i++)
    {
        const int absPDG = abs(fRooVtx->StdHepPdg[i]);
        if( fRooVtx->StdHepStatus[i]!=0 || (absPDG!=12 && absPDG!=14 && absPDG!=16) ){ continue; }

        p->NuPDG[n] = fRooVtx->StdHepPdg[i];
        p->NuEne[n] = fRooVtx->StdHepP4[i][3]*1000.; // GeV -> MeV
        const double *P = fRooVtx->StdHepP4[i];
        const double absP = sqrt(P[0]*P[0] + P[1]*P[1] + P[2]*P[2]);
        for(int k=0; k<3; k++){ p->NuDir[n][k] = P[k]/absP; }
        break;
    }
}

int WCRootDataNuInt::GetFileIdNumber(const char *filename, const char *prefix)
{
	const int size_of_num = fNDigitsFileIdNum;
	TString InFileName(filename);
	int total_size = InFileName.Sizeof() - 1;
	TString sPrefix(prefix);
	TString sROOT(".root");
	int index_Prefix = InFileName.Index( sPrefix );
	int len_Prefix = sPrefix.Sizeof() - 1;
	int len_ROOT = sROOT.Sizeof() - 1;
	int reco_total_size = index_Prefix + len_Prefix + size_of_num + len_ROOT;

	// Check if the file name is acceptable
	if( reco_total_size!=total_size )
	{
		std::cout<<" [ERROR] WCRootDataNuInt::GetFileIdNumber " <<std::endl;
		std::cout<<"  - Input file name is not formatted in: -----" << sPrefix <<"XXXXX.root " <<std::endl;
		std::cout<<"  , where XXXX assumed to be 00000 - 99999 " <<std::endl;
		std::cout<<" Cannot obtain file number for: " << InFileName <<std::endl;
		std::cout<<" -> EXIT "<< std::endl;
		exit(-1);
	}
	int index_start = index_Prefix + sPrefix.Sizeof();
	TString sNum = InFileName(index_start, size_of_num);
	return atoi( sNum );
}
