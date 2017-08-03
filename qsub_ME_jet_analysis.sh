clear
echo " -------------------------------- "
echo "  welcome to the ME jet analyzer "
echo "--------------------------------- "
echo " "

# create a unique folder
mkdir ME_jets_$2_$3
cd ME_jets_$2_$3


# prepare running through bash, priority, name in the queue
echo "#!/bin/bash" >> ME_jet_analysis_autoscript.sh    
echo "#$ -N GMF_$1" >> ME_jet_analysis_autoscript.sh    
echo "#$ -q medium*" >> ME_jet_analysis_autoscript.sh    
echo "#$ -S /bin/bash" >> ME_jet_analysis_autoscript.sh    
echo "#$ -cwd" >> ME_jet_analysis_autoscript.sh    

# set some specific paths. ugly, but no other way
echo "export rd=/nics/c/home/rbertens" >> ME_jet_analysis_autoscript.sh
echo "export ROOTSYS=/nics/c/home/rbertens/root/mybuild" >> ME_jet_analysis_autoscript.sh
echo "export PATH=$PATH:$ROOTSYS/bin " >> ME_jet_analysis_autoscript.sh
echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOTSYS/lib " >> ME_jet_analysis_autoscript.sh
echo "export FASTJET=/nics/c/home/rbertens/fastjet/fastjet-3.2.0/build" >> ME_jet_analysis_autoscript.sh
echo "export LD_LIBRARY_PATH=$FASTJET/lib\:$LD_LIBRARY_PATH" >> ME_jet_analysis_autoscript.sh

export PATH_TO_SOURCE=/nics/c/home/rbertens/projects/generic_mixing_framework
echo "export PATH_TO_SOURCE=$PATH_TO_SOURCE" >> ME_jet_analysis_autoscript.sh
echo "export PATH_TO_DATA=/nics/c/home/rbertens/projects/generic_mixing_framework/merge" >> ME_jet_analysis_autoscript.sh

echo "root -q -b '$PATH_TO_SOURCE/runJetFindingOnMixedEvents.C(\"$1/$2/$3\")'" >> ME_jet_analysis_autoscript.sh

# change permissions
chmod +x ME_jet_analysis_autoscript.sh

# launch the autolauncher
#qsub ME_jet_analysis_autoscript.sh
cd ..    
