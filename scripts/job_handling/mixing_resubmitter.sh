# create a unique folder
cd m_$1_$2_$3_$4_$5_$6_$7_$8
if [ ! -s mixingQA.root ]; then
    echo "mixingQA.root  does not exist of is empty, resubmitting !"
    pwd
    rm *.root
    bsub -q 1nw SE_jet_analysis_autoscript.sh
fi
cd ..    
