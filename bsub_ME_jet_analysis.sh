echo " -------------------------------- "
echo "  welcome to the ME jet analyzer "
echo "--------------------------------- "
echo " "

# create a unique folder
mkdir ME_jets_$1
cd ME_jets_$1
echo " -> looking for jets in file ME_jets_$1.root"

# prepare running through bash, priority, name in the queue
echo "#!/bin/bash" >> ME_jet_analysis_autoscript.sh    
echo "kinit -R" >> ME_jet_analysis_autoscript.sh
echo "klist" >> ME_jet_analysis_autoscript.sh
export WORKDIR=`pwd`
echo "cd $WORKDIR" >> ME_jet_analysis_autoscript.sh
# set some specific paths. ugly, but no other way
echo "source /eos/user/r/rbertens/env_aliroot.sh" >> ME_jet_analysis_autoscript.sh
export PATH_TO_SOURCE=/eos/user/r/rbertens/projects/generic_mixing_framework
echo "export PATH_TO_SOURCE=$PATH_TO_SOURCE" >> ME_jet_analysis_autoscript.sh

export TDIR=`mktemp -u`
echo "mkdir -p $TDIR" >> ME_jet_analysis_autoscript.sh
echo "cd $TDIR" >> ME_jet_analysis_autoscript.sh

echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliAnalysisTaskTTreeFilter.h ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFEventContainer.h ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFEventCuts.h ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFEventReader.h ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFHistogramManager.h ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFMixingManager.h ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFSimpleEventCuts.h ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFSimpleJetFinder.h ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFSimpleTrackCuts.h ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFTrackCuts.h ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFTTreeHeader.h ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFTTreeTrack.h ." >> ME_jet_analysis_autoscript.sh

echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliAnalysisTaskTTreeFilter.cxx ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFEventContainer.cxx ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFEventCuts.cxx ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFEventReader.cxx ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFHistogramManager.cxx ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFMixingManager.cxx ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFSimpleEventCuts.cxx ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFSimpleJetFinder.cxx ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFSimpleTrackCuts.cxx ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFTrackCuts.cxx ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFTTreeHeader.cxx ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/AliGMFTTreeTrack.cxx ." >> ME_jet_analysis_autoscript.sh

echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/readTTree.C ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/runEventMixer.C ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/runJetFindingOnMixedEvents.C ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/runJetFindingOnTree.C ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/runTTreeFilter.C ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/runTTreeFilterOnGrid.C ." >> ME_jet_analysis_autoscript.sh
echo "xrdcp root://eosuser.cern.ch/$PATH_TO_SOURCE/TTreeFilterOnGrid.C ." >> ME_jet_analysis_autoscript.sh

echo "root -q -b 'runJetFindingOnMixedEvents.C($1, 0, 0, 1e9, 3, 3, kTRUE)'" >> ME_jet_analysis_autoscript.sh
echo "xrdcp ME_jets.root root://eosuser.cern.ch/$WORKDIR" >> ME_jet_analysis_autoscript.sh
echo "rm -rf $TDIR" >> ME_jet_analysis_autoscript.sh
# change permissions
chmod +x ME_jet_analysis_autoscript.sh

# launch the autolauncher
#bsub -q 8nh ME_jet_analysis_autoscript.sh
cd .. 
