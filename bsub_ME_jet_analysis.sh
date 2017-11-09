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
echo "export PATH_TO_DATA=/eos/user/r/rbertens/sandbox/mixed-events" >> ME_jet_analysis_autoscript.sh

export TDIR=`mktemp -u`
echo "mkdir -p $TDIR" >> ME_jet_analysis_autoscript.sh
echo "cd $TDIR" >> ME_jet_analysis_autoscript.sh
echo "cp $PATH_TO_SOURCE/*.C ." >> ME_jet_analysis_autoscript.sh
echo "cp $PATH_TO_SOURCE/*.cxx ." >> ME_jet_analysis_autoscript.sh
echo "cp $PATH_TO_SOURCE/*.h ." >> ME_jet_analysis_autoscript.sh

echo "root -q -b 'runJetFindingOnMixedEvents.C($1, 0, 0, 1e9, 3, 3, kTRUE)'" >> ME_jet_analysis_autoscript.sh
echo "mv *.root $WORKDIR" >> ME_jet_analysis_autoscript.sh
echo "rm -rf $TDIR" >> ME_jet_analysis_autoscript.sh
# change permissions
chmod +x ME_jet_analysis_autoscript.sh

# launch the autolauncher
bsub -q 1nd ME_jet_analysis_autoscript.sh
cd .. 
