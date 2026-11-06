#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "TRandom3.h"

#include "MDTManager.h"
#include "WCRootDataIDNuInt.h"
#include "WCRootDataBeamBkg.h"
#include "WCRootDataPileUpSpill.h"
#include "BeamTiming.h"

// PMT type used for 3-inch PMTs of IWCD/WCTE
#include "PMTResponse3inchR12199_02.h"

std::string fInFileTextIDNuInt = "";
std::string fInFileTextBeamBkg = "";
std::string fOutFileNamePrefix = "";
std::string fMDTParFileName = "";
int fSeed = 99999;
int fNumOfSpillsPerFile = 1000;
float fIDNuIntRate = 3.5;
float fBeamBkgRate = 7.5;
int fTotalNumofSpills = 10000;
int fUseOD = 0;

void ReadConfiguration(const char*);

int main(int argc, char **argv)
{
    if (argc==1)
    {
        std::cout<<" ** Please specify input config file ** "<<std::endl;
        std::cout<<" ** Usage: appGenPileUpSpill config_file ** "<<std::endl;
        return 0;
    }

    std::string ConfigFileName = argv[1];
    ReadConfiguration(argv[1]);

    Configuration *Conf = Configuration::GetInstance();
    Conf->ReadParameter(fMDTParFileName);
    Conf->PrintParameters();
    Conf = 0;

    TRandom3 *rndm = new TRandom3(fSeed);
    const int seed_mdt = rndm->Integer(1000000);
    const int seed_idnuint = rndm->Integer(1000000);
    const int seed_beambkg = rndm->Integer(1000000);
    const int seed_beamtiming = rndm->Integer(1000000);
    delete rndm; rndm = 0;

    // IWCD PMT type for ID and OD
    int NPMTType = !fUseOD ? 1 : 2 ;
    vector<string> fPMTType(NPMTType);
    fPMTType[0] = "PMT3inchR12199_02";
    if (fUseOD>0) fPMTType[1] = "PMT3inchR12199_02_OD";

    // Will be manaing marging true hits and digitizing merged true hits
    MDTManager *MDT = new MDTManager(seed_mdt);
    MDT->RegisterPMTType(fPMTType[0], new PMTResponse3inchR12199_02());
    if (fUseOD) MDT->RegisterPMTType(fPMTType[1], new PMTResponse3inchR12199_02());

    vector<string> listWCRootEvt(NPMTType);
    listWCRootEvt[0] = "wcsimrootevent";
    if (fUseOD) listWCRootEvt[1] = "wcsimrootevent_OD";

    // Manage input files for ID interactions 
    WCRootDataIDNuInt *daIDNuInt = new WCRootDataIDNuInt();

    // Manage input files for Beam background interactions 
    WCRootDataBeamBkg *daBeamBkg = new WCRootDataBeamBkg();

    // Manage pile-up spills (i.e. results of this program)
    WCRootDataPileUpSpill *daPileUp = new WCRootDataPileUpSpill();

    // Initialize random number generator
    daIDNuInt->SetSeed(seed_idnuint);
    daBeamBkg->SetSeed(seed_beambkg);

    // Will be used to extract file ID number
    // KMTsui: I don't have it has real usage
    // daIDNuInt->SetNDigitsFileIDNumber(5);
    // daBeamBkg->SetNDigitsFileIDNumber(6);

    // Load input files from text file
    daIDNuInt->LoadFiles(fInFileTextIDNuInt.c_str(),listWCRootEvt);
    daBeamBkg->LoadFiles(fInFileTextBeamBkg.c_str(),listWCRootEvt);

    // Set number of ID interactions per spill
    daIDNuInt->SetInteractionRate(fIDNuIntRate);
    daBeamBkg->SetInteractionRate(fBeamBkgRate);

    // Mange neutrino interaction timing
    // Currently, T2K's 8 bunched beam structure is assumed
    BeamTiming *bt = new BeamTiming(seed_beamtiming);

    // Set number of spills that will be saved per output file
    daPileUp->SetNumOfSpillesSavedPerFile(fNumOfSpillsPerFile);

    // Some TTree objects contained in input files will be copied into output files. 
    // It is assumed that all these objects are identical among input files.
    // So. will copy these objects from one of input files
    daPileUp->SetFileNameForCopyTree(daIDNuInt->GetCurrentInputFileName());

    // Create output file. Its name will be something like "OutFileNamePrefix".00000.root
    daPileUp->CreateTree(fOutFileNamePrefix.c_str(),listWCRootEvt);

    float nuIntTime; // interaction time
    int nuIntBunch;  // bunch id number 
    const int PCTLVerbose = std::max(fTotalNumofSpills/10, 1);
    int nSavedSpills = 0;
    std::cout<<" Will be generating up to " << fTotalNumofSpills <<" pile-up spills " <<std::endl;
    for(int i=0; i<fTotalNumofSpills ; i++)
    {
        // Number of ID/Beam bkg interactions of this spill
        // Let there be at least one interaction 
        int nThisIDNuInt = 0;
        int nThisBeamBkg = 0;
        while((nThisIDNuInt + nThisBeamBkg) < 1){
            nThisIDNuInt = daIDNuInt->DrawNumOfInteraction();
            nThisBeamBkg = daBeamBkg->DrawNumOfInteraction();
        }

        if( i%PCTLVerbose==0 )
        {
            cout<<" i:" << i
                 <<" nIDNuInt:" << nThisIDNuInt 
                 <<" nBeamBkg:" << nThisBeamBkg
                 <<endl;
        }

        // All PMTs are needed for dark noise, even when this spill has no interactions
        for(int k=0; k<NPMTType; k++)
            daIDNuInt->SetTubes(MDT->GetHitTubeCollection(fPMTType[k]), k);

        // Each input event is used once, so the input can run out before all the spills are made
        bool hasEnoughInput = true;

        // ID interactions
        for(int j=0; j<nThisIDNuInt; j++)
        {
            if( !daIDNuInt->Next() ){ hasEnoughInput = false; break; }

            // Drawing interaction time and bunch at which this interaction is taking place 
            bt->DrawInteractionTime(nuIntTime, nuIntBunch);
            
            // Add true hits of this interaction to MDT
            const int nuIntIdx = daPileUp->GetPileUpSpill()->NNuIntTot;
            for(int k=0; k<NPMTType; k++)
                daIDNuInt->AddTrueHitsToMDT(MDT->GetHitTubeCollection(fPMTType[k]), MDT->GetPMTResponse(fPMTType[k]), nuIntTime, k, nuIntIdx);

            // Add information about this interaction
            daPileUp->AddInteraction(daIDNuInt, nuIntTime, nuIntBunch);
        }

        // Repeating the above, but for beam bkg interactions
        for(int j=0; hasEnoughInput && j<nThisBeamBkg; j++)
        {
            if( !daBeamBkg->Next() ){ hasEnoughInput = false; break; }

            bt->DrawInteractionTime(nuIntTime, nuIntBunch);
            const int nuIntIdx = daPileUp->GetPileUpSpill()->NNuIntTot;
            for(int k=0; k<NPMTType; k++)
                daBeamBkg->AddTrueHitsToMDT(MDT->GetHitTubeCollection(fPMTType[k]), MDT->GetPMTResponse(fPMTType[k]), nuIntTime, k, nuIntIdx);
            daPileUp->AddInteraction(daBeamBkg, nuIntTime, nuIntBunch);
        }

        // This spill did not get all the interactions drawn for it, so drop it.
        // Only FillTree below puts a spill in the output, so nothing of it is saved.
        if( !hasEnoughInput )
        {
            std::cout<<" No input events left for spill " << i <<": stopping here" <<std::endl;
            break;
        }

        // Now all the true hits have been merged into one spill
        // Add dark noise hits, and then make digitized hits
        for(int j=0; j<NPMTType; j++)
        {
            // A spill without true hits gets dark noise around the beam bunches
            MDT->DoAddDark(fPMTType[j], 0., kBunchInterval*(kNBunches-1));
            MDT->DoDigitize(fPMTType[j]);
            MDT->DoTrigger(fPMTType[j]);

            // Add the resultant digitized hits to the output
            TriggerInfo *ti = MDT->GetTriggerInfo(fPMTType[j]);
            daPileUp->AddDigiHits(MDT->GetHitTubeCollection(fPMTType[j]), ti, i, j);
        }
 
        daPileUp->FillTree();
        nSavedSpills += 1;

        // Clear all the true and digitized hits of this spill for the next spill
        MDT->DoInitialize();
    }

    daIDNuInt->CloseFile();
    daBeamBkg->CloseFile();
    daPileUp->WriteTree();

    std::cout<<" Saved " << nSavedSpills <<" of " << fTotalNumofSpills <<" requested pile-up spills " <<std::endl;

    Conf = Configuration::GetInstance();
    Conf->Finalize();

    std::cout<<" ** FINISHED ** " <<std::endl;
}


void ReadConfiguration(const char *filename)
{    
    Configuration *Conf = Configuration::GetInstance();
    Conf->ReadParameter(std::string(filename));
    Conf->PrintParameters();
    Conf->GetValue<string>("ListIDNuIntFiles", fInFileTextIDNuInt);
    Conf->GetValue<string>("ListBeamBkgFiles", fInFileTextBeamBkg);
    Conf->GetValue<string>("OutFileNamePrefix", fOutFileNamePrefix);
    Conf->GetValue<string>("MDTParFile", fMDTParFileName);
    Conf->GetValue<int>("InitialSeed", fSeed);
    Conf->GetValue<int>("NumOfSpillsSavedPerFile", fNumOfSpillsPerFile);
    Conf->GetValue<float>("IDNuIntRate", fIDNuIntRate);
    Conf->GetValue<float>("BeamBkgRate", fBeamBkgRate);
    Conf->GetValue<int>("TotalNumOfSpills", fTotalNumofSpills);
    Conf->GetValue<int>("UseOD", fUseOD);
    Conf->Finalize();
}
