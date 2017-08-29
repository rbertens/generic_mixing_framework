echo " -------------------------------- "
echo "  welcome to the ME jet analyzer "
echo "--------------------------------- "
echo " "

# create a unique folder
mkdir SE_jets_$1_$2_$3
cd SE_jets_$1_$2_$3

# prepare running through bash, priority, name in the queue
echo "#!/bin/bash" >> SE_jet_analysis_autoscript.sh    
export WORKDIR=`pwd`
echo "cd $WORKDIR" >> SE_jet_analysis_autoscript.sh
# set some specific paths. ugly, but no other way
echo "source /eos/user/r/rbertens/env_aliroot.sh" >> SE_jet_analysis_autoscript.sh
export PATH_TO_SOURCE=/eos/user/r/rbertens/projects/generic_mixing_framework
echo "export PATH_TO_SOURCE=$PATH_TO_SOURCE" >> SE_jet_analysis_autoscript.sh
echo "export PATH_TO_DATA=/eos/user/r/rbertens/sandbox/mixed-events" >> SE_jet_analysis_autoscript.sh

export TDIR=`mktemp -u`
echo "mkdir -p $TDIR" >> SE_jet_analysis_autoscript.sh
echo "cd $TDIR" >> SE_jet_analysis_autoscript.sh
echo "root -q -b '$PATH_TO_SOURCE/runJetFindingOnTree.C($1,$2,$3)'" >> SE_jet_analysis_autoscript.sh
echo "mv *.root $WORKDIR" >> SE_jet_analysis_autoscript.sh
echo "rm -rf $TDIR" >> SE_jet_analysis_autoscript.sh
# change permissions
chmod +x SE_jet_analysis_autoscript.sh

# launch the autolauncher
bsub -q8nh SE_jet_analysis_autoscript.sh
cd ..    
