# do you really want to submit a thousand jobs by hand? 
# no you don't - so here's a little helper script



# centrality range
cen=(0 10)

# vertex arrays
vertex=(-10 -6 -2 2 6 10)

# event plane array
ep=(-1.6 -.785 0, .785 1.6)

# multiplicity array

mult=(1280 1720 2000 2160 2600)

for ((i = 0; i < ${#vertex[@]}-1; ++i)); do
    inci=$(( $i + 1 ))
    for ((j = 0; j < ${#ep[@]}-1; ++j)); do
        incj=$(( $j + 1 ))
        for ((k = 0; k < ${#mult[@]}-1; ++k)); do
            inck=$(( $k + 1 ))
            ./qsub_create_mixed_events.sh ${mult[$k]} ${mult[$inck]} ${vertex[$i]} ${vertex[$inci]} ${ep[$j]} ${ep[$incj]} ${cen[0]} ${cen[1]}
        done
    done
done
