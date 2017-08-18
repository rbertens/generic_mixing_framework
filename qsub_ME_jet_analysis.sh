echo " -------------------------------- "
echo "  welcome to the ME jet analyzer "
echo "--------------------------------- "
echo " "

# create a unique folder
mkdir ME_jets_$1
cd ME_jets_$1
echo " -> looking for jets in file ME_jets_$1.root"

# prepare running through bash, priority, name in the queue
echo "#PBS -S /bin/bash" >> ME_jet_analysis_autoscript.sh    
echo "#PBS -N GMF_$1" >> ME_jet_analysis_autoscript.sh    
echo "#PBS -l walltime=05:00:00" >> ME_jet_analysis_autoscript.sh    
export PBS_O_WORKDIR=`pwd`
echo "cd $PBS_O_WORKDIR" >> ME_jet_analysis_autoscript.sh
# set some specific paths. ugly, but no other way
echo "export ROOTSYS=/nics/c/home/rbertens/root/mybuild" >> ME_jet_analysis_autoscript.sh
echo "export PATH=$PATH:$ROOTSYS/bin " >> ME_jet_analysis_autoscript.sh
echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOTSYS/lib " >> ME_jet_analysis_autoscript.sh
echo "export FASTJET=/nics/c/home/rbertens/fastjet/fastjet-3.2.0/build" >> ME_jet_analysis_autoscript.sh
echo "export LD_LIBRARY_PATH=$FASTJET/lib\:$LD_LIBRARY_PATH" >> ME_jet_analysis_autoscript.sh

export PATH_TO_SOURCE=/nics/c/home/rbertens/projects/generic_mixing_framework
echo "export PATH_TO_SOURCE=$PATH_TO_SOURCE" >> ME_jet_analysis_autoscript.sh
echo "export PATH_TO_DATA=/nics/c/home/rbertens/lustre/mixed-events" >> ME_jet_analysis_autoscript.sh

echo "root -q -b '$PATH_TO_SOURCE/runJetFindingOnMixedEvents.C($1)'" >> ME_jet_analysis_autoscript.sh

# change permissions
chmod +x ME_jet_analysis_autoscript.sh

# launch the autolauncher
qsub ME_jet_analysis_autoscript.sh
cd ..    
