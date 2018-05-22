echo " -------------------------------- "
echo "  welcome to the ME jet analyzer "
echo "--------------------------------- "
echo " "

# create a unique folder
cd SE_jets_$1_$2_$3
if [ ! -s SE_jets_$1_$2.root ]; then
    echo "file SE_jets_$1_$2.root does not exist of is empty, resubmitting !"
    pwd
    bsub -q 2nd SE_jet_analysis_autoscript.sh
fi
cd ..    
