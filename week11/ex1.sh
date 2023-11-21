#!/bin/bash


# function that gets file path as a parameter,
# uses ldd on it and with the use of regular expressions
# extracts the paths of the shared libraries for the file
getlibs(){
	ldd "$1" \
    	| grep so \
	| sed -e '/^[^\t]/ d' \
	| sed -e 's/\t//' \
	| sed -e 's/.*=..//' \
	| sed -e 's/ (0.*)//' \
	| sed -e '/linux-vdso/d' 

}

# creating file and allocate memory for our new file system
fallocate -l 100M lofs.img
# setting up a loop device on the created file
sudo losetup /dev/loop18 lofs.img

# creating a loop file system ext4 on the device
sudo mkfs.ext4 /dev/loop18

# creating a directory to mount the filesystem on
mkdir ./lofsdisk

# mounting the device on the created directory
sudo mount /dev/loop18 ./lofsdisk

# giving the user permissions on the directory and the device itself
sudo chown $USER ./lofsdisk
sudo chown $USER:disk /dev/loop18
sleep 2

# creating file1 and file2 and filling them with my names
sudo echo "Alie" > ./lofsdisk/file1.txt
sudo echo "Ablaeva" > ./lofsdisk/file2.txt

# for the commands bash, cat, echo, ls copy them to the new filesystem,
# get the paths of their shared libraries using getlibs(),
# copy these libraries to the new file system
sudo cp --parents /bin/bash ./lofsdisk
libs=$(getlibs /bin/bash)
sudo cp --parents $libs ./lofsdisk

sudo cp --parents /bin/cat ./lofsdisk
libs=$(getlibs /bin/cat)
sudo cp --parents $libs ./lofsdisk

sudo cp --parents /bin/echo ./lofsdisk
libs=$(getlibs /bin/echo)
sudo cp --parents $libs ./lofsdisk

sudo cp --parents /bin/ls ./lofsdisk
libs=$(getlibs /bin/ls)
sudo cp --parents $libs ./lofsdisk

# compiling c program that prints all files of the root directory 
gcc ex1.c -o ex1
# copying the program to the new filesystem to run it from there
cp --parents ./ex1 ./lofsdisk

echo -e "\n---------------\nrun from lofsdisk:\n" > ex1.txt
# running the program from the new filesystem and printing the output
# to ex1.txt
sudo chroot ./lofsdisk ./ex1 >> ex1.txt
echo -e "---------------\n\nrun from original file system:\n" >> ex1.txt
#running the program from the original main filesystem and printing the
# output to ex1.txt
~/week11/ex1 >> ~/week11/ex1.txt

