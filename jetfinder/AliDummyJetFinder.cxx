// simple jet finder on generic input vector
//
// Redmer Alexander Bertens, 2017 (UTK, CERN)

#include "fastjet/Pseudojet.h"

#include <iostream>
#include <vector>





AliDummyJetFinder::ParseEvent() {
    // called for each event
    std::vector <fastjet:PseudoJet> fjInputVector;

    for(/* track loop */) {
        fastjet::PseudoJet fjInputProtoJet(px, py, pz, 4vec);
        fjInputProtoJet.set_user_index(index);
        fjInputVector.push_back(fjInputProtoJet);
    }

// todo the actual jet finding

   fastjet::GhostAreaSPec       ghostSpec(max_eta_track, 1, 0.001);
   fastjet::Strategy            strategy = fastjet::Best;
   fastjet::RecombinationScheme recombScheme = fastjet::BIpt_scheme;
   fastjet::AreaType            areaType = fastjet::active_area;
   fastjet::AreaDefinition      areaDef = fastjet::AreaDefinition(areaType, ghostSpec);

   // loop for various jet radii
   for (int iR =0; iR < nR; iR++) {
       float jetR = 0.2+0.1*iR;

       pt_lead_jet = -1;
       float phi_lead_jet= 0;
       pt_all_track=0;
       // end modification 12/05/2014 ... 
       fastjet::RangeDefinition range(-max_eta_jet, max_eta_jet, 0, 2.*fastjet::pi);

       fastjet::JetDefinition jetDefCh(fastjet::antikt_algorithm, jetR,recombScheme, strategy);
       fastjet::ClusterSequenceArea clustSeqCh(fjInputs, jetDefCh,areaDef);

       // here we get the jets
       vector <fastjet::PseudoJet> inclusiveJetsCh = clustSeqCh.inclusive_jets();

       fastjet::JetMedianBackgroundEstimator bge;  //.......... Background Sutraction event by event   
       fastjet::ClusterSequenceArea *clustSeqBG = 0;

       if (do_bkg) {
       fastjet::JetDefinition jetDefBG(fastjet::kt_algorithm, jetR, recombScheme, strategy);
       fastjet::Selector  BGSelector = fastjet::SelectorStrip(2*jetR);  //.......... Background Sutraction event by event
           BGSelector.set_reference(inclusiveJetsCh[iJet]);
           fastjet::ClusterSequenceArea clustSeqBG(fjInputs, jetDefBG,areaDef);

       BGSelector.set_jets(inclusiveJetsCh);//...............
       clustSeqBG = new fastjet::ClusterSequenceArea(fjInputs, jetDefBG,areaDef);//............
       vector <fastjet::PseudoJet> BGJets = clustSeqBG->inclusive_jets();

       bge.set_selector(BGSelector);
       bge.set_jets(BGJets);
       }
       eta_lead_jet=0; 
       float jet_eta= 0;
       float jet_phi= 0;
       float jet_pt= 0;
       float jet_pt_bgsub= 0;

       for (unsigned int iJet = 0; iJet < inclusiveJetsCh.size(); iJet++)
       {
           if (!range.is_in_range(inclusiveJetsCh[iJet]))   
               continue;

           // get the jet constituents
           std::vector<fastjet::PseudoJet> constituents = clustSeqCh.constituents(inclusiveJetsCh[iJet]);
           double maxpt(0);
           for(int i(0); i < constituents.size(); i++) {
               if(constituents[i].perp() > maxpt) maxpt = constituents[i].perp();
           }
           if(maxpt < LH_CUT) continue;


           jet_pt = inclusiveJetsCh[iJet].perp();
           jet_eta = inclusiveJetsCh[iJet].eta();
           float dphi_jh = dphi(inclusiveJetsCh[iJet].phi(),phi_lead);

           float rho = 0;

           if (do_bkg) {
               rho= bge.rho(inclusiveJetsCh[iJet]);
               float jet_area = clustSeqCh.area(inclusiveJetsCh[iJet]);
               float bgsub = rho*jet_area;
               jet_pt_bgsub = jet_pt - bgsub;     //.......... Background Sutraction event by event
           }
           else 
               jet_pt_bgsub = jet_pt;

           // REDMER fill dphi, pt histo
           dPhiJets->Fill(dphi(inclusiveJetsCh[iJet].phi(),ep_angle), jet_pt,evt->weights()[0]);

