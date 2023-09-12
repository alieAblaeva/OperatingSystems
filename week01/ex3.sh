mkdir root_file
date
sleep 3
ls -lt --reverse / > root_file/root.txt
date
sleep 3
mkdir home_file
date
sleep 3
ls -lt --reverse ~ > home_file/home.txt
