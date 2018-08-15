(echo "auto_overflow_0_5_20overflow_1e9gev_preserve_flow ";
echo "auto_overflow_40_50_20overflow_1e9gev_preserve_flow";
echo "auto_overflow_0_5_20overflow_5gev_preserve_flow";
echo "auto_overflow_40_50_20overflow_5gev_preserve_flow";
echo "auto_overflow_0_5_20overflow_5gev_coll_split";
echo "auto_overflow_40_50_20overflow_5gev_coll_split";
echo "auto_overflow_0_5_20overflow_3gev_coll_split";
echo "auto_overflow_40_50_20overflow_3gev_coll_split";
echo "auto_overflow_0_5_20overflow_7gev_coll_split";
echo "auto_overflow_40_50_20overflow_7gev_coll_split";
echo "golden_0_5_run_novtx";
echo "golden_40_50_run_novtx") | xargs -I % sh -c 'echo %;  sshpass -p $(cat password) rsync -rav -e ssh --include "'*/'" --include="'*.root'" --exclude="'*'" rbertens@lxplus.cern.ch:/eos/user/r/rbertens/analyzed_events/% .'

