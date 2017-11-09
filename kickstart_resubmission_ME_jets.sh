# do you really want to submit a thousand jobs by hand? 
# no you don't - so here's a little helper script

# file range
files=(0 557)

for ((i = 0; i < ${files[1]}+1; ++i)); do
    inci=$(( $i + 1 ))
    cd ME_jets_$i
    if [ ! -s ME_jets.root ]; then
        echo "i do not exist"
        pwd
        bsub -q 1nd ME_jet_analysis_autoscript.sh
    fi
    cd .. 
done
