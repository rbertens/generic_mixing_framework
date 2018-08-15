# do you really want to submit a thousand jobs by hand? 
# no you don't - so here's a little helper script



# centrality range
cen=(0 5)

# vertex arrays
vertex=(-10 -4 -2 2 4 10)

# event plane array
ep=(-1.6 -.785 0 .785 1.6)

# multiplicity array
#mult=(1280 1610 1940 2270 2600)
#mult=(100 300 500 700 900)
mult=(1600 1900 2200 2500 2800)
for ((i = 0; i < ${#vertex[@]}-1; ++i)); do
    inci=$(( $i + 1 ))
    for ((j = 0; j < ${#ep[@]}-1; ++j)); do
        incj=$(( $j + 1 ))
        for ((k = 0; k < ${#mult[@]}-1; ++k)); do
            inck=$(( $k + 1 ))
            ./mixing_resubmitter.sh ${mult[$k]} ${mult[$inck]} ${vertex[$i]} ${vertex[$inci]} ${ep[$j]} ${ep[$incj]} ${cen[0]} ${cen[1]}
        done
    done
done
