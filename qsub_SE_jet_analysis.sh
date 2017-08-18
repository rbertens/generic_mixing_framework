echo " -------------------------------- "
echo "  welcome to the ME jet analyzer "
echo "--------------------------------- "
echo " "

# create a unique folder
mkdir SE_jets_$1_$2_$3
cd SE_jets_$1_$2_$3

# prepare running through bash, priority, name in the queue
echo "#PBS -S /bin/bash" >> SE_jet_analysis_autoscript.sh    
echo "#PBS -N GMF_$1_$2_$3" >> SE_jet_analysis_autoscript.sh    
echo "#PBS -l walltime=05:00:00" >> SE_jet_analysis_autoscript.sh    
export PBS_O_WORKDIR=`pwd`
echo "cd $PBS_O_WORKDIR" >> SE_jet_analysis_autoscript.sh
# set some specific paths. ugly, but no other way
echo "export ROOTSYS=/nics/c/home/rbertens/root/mybuild" >> SE_jet_analysis_autoscript.sh
echo "export PATH=$PATH:$ROOTSYS/bin " >> SE_jet_analysis_autoscript.sh
echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOTSYS/lib " >> SE_jet_analysis_autoscript.sh
echo "export FASTJET=/nics/c/home/rbertens/fastjet/fastjet-3.2.0/build" >> SE_jet_analysis_autoscript.sh
echo "export LD_LIBRARY_PATH=$FASTJET/lib\:$LD_LIBRARY_PATH" >> SE_jet_analysis_autoscript.sh

export PATH_TO_SOURCE=/nics/c/home/rbertens/projects/generic_mixing_framework
echo "export PATH_TO_SOURCE=$PATH_TO_SOURCE" >> SE_jet_analysis_autoscript.sh
echo "export PATH_TO_DATA=/nics/c/home/rbertens/lustre/mixed-events" >> SE_jet_analysis_autoscript.sh

echo "root -q -b '$PATH_TO_SOURCE/runJetFindingOnTree.C($1,$2,$3)'" >> SE_jet_analysis_autoscript.sh

# change permissions
chmod +x SE_jet_analysis_autoscript.sh

# launch the autolauncher
#qsub SE_jet_analysis_autoscript.sh
cd ..    
