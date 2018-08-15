# helper script to recursively to through a collection of ME output files
# and create symlinks to the found files that are given names
# that are easy for another bash script to work on

#! /bin/bash
COUNTER=0

for file in `find . -size +10240c -name 'myMixedEvents*'`; do
    echo "File ME_$COUNTER) found at ${file}"
    if [ -L ME_$COUNTER.root ]; then
          echo "  -> symlink with name ME_$COUNTER.root exists, unlinking first ..."
          unlink ME_$COUNTER.root
    fi
    echo "  -> creating symlink ${file} -> ME_$COUNTER.root"
    ln -s ${file} ME_$COUNTER.root
    COUNTER=$[$COUNTER +1]
done 
