clear
echo " -------------------------------- "
echo "  welcome to the ME jet analyzer "
echo "--------------------------------- "
echo " "
echo " centrality [1 - 2]"
echo " input file range [3 - 4]"

# create a unique folder
mkdir SE_jets_$1_$2_$3_$4
cd SE_jets_$1_$2_$3_$4


# prepare running through bash, priority, name in the queue
echo "#!/bin/bash" >> SE_jet_analysis_autoscript.sh    
echo "#$ -N SE_jets_$1_$2_$3_$4" >> SE_jet_analysis_autoscript.sh    
echo "#$ -q medium*" >> SE_jet_analysis_autoscript.sh    
echo "#$ -S /bin/bash" >> SE_jet_analysis_autoscript.sh    
echo "#$ -cwd" >> SE_jet_analysis_autoscript.sh    

# set some specific paths. ugly, but no other way
echo "export rd=/nics/c/home/rbertens" >> SE_jet_analysis_autoscript.sh
echo "export ROOTSYS=/nics/c/home/rbertens/root/mybuild" >> SE_jet_analysis_autoscript.sh
echo "export PATH=$PATH:$ROOTSYS/bin " >> SE_jet_analysis_autoscript.sh
echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOTSYS/lib " >> SE_jet_analysis_autoscript.sh
echo "export FASTJET=/nics/c/home/rbertens/fastjet/fastjet-3.2.0/build" >> SE_jet_analysis_autoscript.sh
echo "export LD_LIBRARY_PATH=$FASTJET/lib\:$LD_LIBRARY_PATH" >> SE_jet_analysis_autoscript.sh

export PATH_TO_SOURCE=/nics/c/home/rbertens/projects/generic_mixing_framework
echo "export PATH_TO_SOURCE=$PATH_TO_SOURCE" >> SE_jet_analysis_autoscript.sh
echo "export PATH_TO_DATA=/nics/c/home/rbertens/projects/generic_mixing_framework/merge" >> SE_jet_analysis_autoscript.sh

echo "root -q -b '$PATH_TO_SOURCE/runJetFindingOnTree.C($1,$2,$3,$4)'" >> SE_jet_analysis_autoscript.sh

# change permissions
chmod +x SE_jet_analysis_autoscript.sh

# launch the autolauncher
qsub SE_jet_analysis_autoscript.sh
cd ..    
