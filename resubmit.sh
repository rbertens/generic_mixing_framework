echo " -------------------------------- "
echo "  welcome to the ME jet analyzer "
echo "--------------------------------- "
echo " "

# create a unique folder
cd SE_jets_$1_$2_$3
if [ ! -f SE_jets_0_5.root ]; then
    echo "i do not exist"
    pwd
    bsub -q 1nd SE_jet_analysis_autoscript.sh
fi
cd ..    
