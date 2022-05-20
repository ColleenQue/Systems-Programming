#!/bin/bash
# getopts.sh

#I pledge my honor that I have abided by the Stevens Honor System. 
#Colleen Que

#cd /media/sf_cs392/hw1

h=0;
l=0;
p=0;
readonly x=/home/user/.junk;

if [ ! -d "$x" ]    
  #cant find directory 
    then  
    mkdir $x
fi 

#print unknown first
#if there is no unknown then print message 

#here document
if [ -z "$1" ] #when no argument supplied
then cat << EOF 
Usage: $(basename $0) [-hlp] [list of files]
  -h: Display help.
  -l: List junked files.
  -p: Purge all files.
  [list of files] with no other arguments to junk those files.
EOF
exit 0
fi

#####
usage()
 {
   echo "usage: <command> options:<w|l|h>"
 }


#checks only for flags
while getopts ":hlp" options; do 
#while getopts "n:t:" options; do 
  case "${options}" in
    h)
      h=1
      ;;
    l)
      l=1
      ;;
    p) 
     p=1
      ;;
    *)
      cat << EOF
Error: Unknown option '-${OPTARG}'.
Usage: $(basename $0) [-hlp] [list of files]
  -h: Display help.
  -l: List junked files.
  -p: Purge all files.
  [list of files] with no other arguments to junk those files.
EOF
      exit 1 #exit error
  esac
  done

#!/bin/bash
# here.sh
total=$(( $p + $h + $l ))

#too many arguments
if [[ $total > 1 ]]
then cat << EOF
Error: Too many options enabled.
Usage: $(basename $0) [-hlp] [list of files]
  -h: Display help.
  -l: List junked files.
  -p: Purge all files.
  [list of files] with no other arguments to junk those files.
EOF
exit 1
fi


#check for the presence of the ~/.junk directory 


if [ $h = 1 ]
  #display message
  then cat << EOF
Usage: $(basename $0) [-hlp] [list of files]
  -h: Display help.
  -l: List junked files.
  -p: Purge all files.
  [list of files] with no other arguments to junk those files.
EOF
exit 0
fi

if [ $p = 1 ]
#purge files
then rm -r $x
exit 0
fi

if [ $l = 1 ]
  then  
  ls -lAF $x
  exit 0
fi



for word in "$@"; do
#check if file exists
#move to junk bin
if [[ ! -f $word ]] && [[ ! -d $word ]] 
then
    echo "Warning: '$word' not found."
else
    mv $word $x
fi
#tree -a ~/.junk
done

exit 0;


