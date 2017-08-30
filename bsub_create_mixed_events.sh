echo " -------------------------------- "
echo "    welcome to the event mixer "
echo "--------------------------------- "
echo " "
if [ $# -lt 8 ]; then
  echo 1>&2 "FATAL - not enough arguments given"
  echo " " 
  echo " -  please provide 8 arguments: "
  echo "   > multiplicity range [1 - 2]" 
  echo "   > vertex range [3 - 4]"
  echo "   > event plane range [5 - 6]"
  echo "   > centrality range [7 - 8]"
  echo "------------------------------- " 
  exit 2
elif [ $# -gt 8 ]; then
  echo 1>&2 "FATAL - too many arguments"
  echo " " 
  echo " -  please provide 8 arguments: "
  echo "   > multiplicity range [1 - 2]" 
  echo "   > vertex range [3 - 4]"
  echo "   > event plane range [5 - 6]"
  echo "   > centrality range [7 - 8]"
  echo "------------------------------- " 
  exit 2
fi


echo " - configuration: "
echo "   > multiplicity range [$1 - $2]" 
echo "   > vertex range [$3 - $4]"
echo "   > event plane range [$5 - $6]"
echo "   > centrality range [$7 - $8]"
echo "  "
echo "  - I wish you a happy mixing -"


# create a unique folder
mkdir m_$1_$2_$3_$4_$5_$6_$7_$8
cd m_$1_$2_$3_$4_$5_$6_$7_$8


# prepare running through bash, priority, name in the queue
echo "#!/bin/bash" >> create_mixed_events_autoscript.sh    
export WORKDIR=`pwd`
echo "cd $WORKDIR" >> create_mixed_events_autoscript.sh

# set some specific paths. ugly, but no other way
echo "source /eos/user/r/rbertens/env_aliroot.sh" >> create_mixed_events_autoscript.sh
export PATH_TO_SOURCE=/eos/user/r/rbertens/projects/generic_mixing_framework
echo "export PATH_TO_SOURCE=$PATH_TO_SOURCE" >> create_mixed_events_autoscript.sh
echo "export PATH_TO_DATA=/eos/user/r/rbertens/sandbox/merge" >> create_mixed_events_autoscript.sh

export TDIR=`mktemp -u`
echo "mkdir -p $TDIR" >> create_mixed_events_autoscript.sh
echo "cd $TDIR" >> create_mixed_events_autoscript.sh

echo "root -q -b '$PATH_TO_SOURCE/runEventMixer.C($1,$2,$3,$4,$5,$6,$7,$8)'" >> create_mixed_events_autoscript.sh
echo "mv *.root $WORKDIR" >> create_mixed_events_autoscript.sh
echo "rm -rf $TDIR" >> create_mixed_events_autoscript.sh
# change permissions
chmod +x create_mixed_events_autoscript.sh

# launch the autolauncher
bsub -q8nh create_mixed_events_autoscript.sh
cd ..    
