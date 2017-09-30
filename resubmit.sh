for dir in */; do
    if [ -f $dir/mixingQA.root ]; then
           echo "job $dir was a success"
       else
           echo " !!!!!!!!!!!!! job $dir failed, resubmitting !!!!!!!!!!!!!!!!!!!!!!!" 
           ls -l "$dir"
           bsub -q 8nh $dir/create_mixed_events_autoscript.sh 
       fi
   done
