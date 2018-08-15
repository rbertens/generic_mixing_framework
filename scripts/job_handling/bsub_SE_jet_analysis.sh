echo " -------------------------------- "
echo "  welcome to the SE jet analyzer "
echo "--------------------------------- "
echo " "

# create a unique folder
mkdir SE_jets_$1_$2_$3
cd SE_jets_$1_$2_$3

# prepare running through bash, priority, name in the queue
echo "#!/bin/bash" >> SE_jet_analysis_autoscript.sh    
echo "kinit -R" >> SE_jet_analysis_autoscript.sh
echo "klist" >> SE_jet_analysis_autoscript.sh
export WORKDIR=`pwd`
#echo "cd $WORKDIR" >> SE_jet_analysis_autoscript.sh
# set some specific paths. ugly, but no other way
echo "source ~/env_aliroot.sh" >> SE_jet_analysis_autoscript.sh
export PATH_TO_SOURCE=/eos/user/r/rbertens/projects/generic_mixing_framework
echo "export PATH_TO_SOURCE=$PATH_TO_SOURCE" >> SE_jet_analysis_autoscript.sh

export TDIR=`mktemp -u`
echo "mkdir -p $TDIR" >> SE_jet_analysis_autoscript.sh
echo "cd $TDIR" >> SE_jet_analysis_autoscript.sh


#grab data from eos, hardcoded path here to avoid typos 
echo "xrdcp root://eosuser.cern.ch//eos/user/r/rbertens/sandbox/merge/$3.root ." >> SE_jet_analysis_autoscript.sh

# make sources available
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliAnalysisTaskTTreeFilter.h ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFEventContainer.h ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFEventCuts.h ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFEventReader.h ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFHistogramManager.h ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFMixingManager.h ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFSimpleEventCuts.h ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFSimpleJetFinder.h ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFSimpleTrackCuts.h ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFTrackCuts.h ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFTTreeHeader.h ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFTTreeTrack.h ." >> SE_jet_analysis_autoscript.sh

echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliAnalysisTaskTTreeFilter.cxx ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFEventContainer.cxx ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFEventCuts.cxx ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFEventReader.cxx ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFHistogramManager.cxx ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFMixingManager.cxx ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFSimpleEventCuts.cxx ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFSimpleJetFinder.cxx ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFSimpleTrackCuts.cxx ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFTrackCuts.cxx ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFTTreeHeader.cxx ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFTTreeTrack.cxx ." >> SE_jet_analysis_autoscript.sh

echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/runEventMixer.C ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/runJetFindingOnMixedEvents.C ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/runJetFindingOnTree.C ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/runTTreeFilter.C ." >> SE_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/runTTreeFilterOnGrid.C ." >> SE_jet_analysis_autoscript.sh

echo "root -q -b 'runJetFindingOnTree.C($1,$2,$3)'" >> SE_jet_analysis_autoscript.sh
echo "xrdcp SE_jets_$1_$2.root root://eosuser.cern.ch/$WORKDIR" >> SE_jet_analysis_autoscript.sh

echo "rm -rf $TDIR" >> SE_jet_analysis_autoscript.sh
# change permissions
chmod +x SE_jet_analysis_autoscript.sh

echo " --------- here's your JDL ------------ "
cat SE_jet_analysis_autoscript.sh
echo " ---------- it's on its way ! --------- "


# launch the autolauncher
#bsub -q 1nd SE_jet_analysis_autoscript.sh
cd ..    
