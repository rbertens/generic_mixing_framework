# do you really want to submit a thousand jobs by hand? 
# no you don't - so here's a little helper script

# file range
files=(21 31)

for ((i = ${files[0]}; i < ${files[1]}+1; ++i)); do
    inci=$(( $i + 1 ))
    ./bsub_ME_jet_analysis.sh $i
done
