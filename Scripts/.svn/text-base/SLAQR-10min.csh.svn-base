#!/bin/tcsh -f
set Start  = $1
set Time = `echo $Start |awk '{printf("%4d %2.2d %2.2d %2.2d %2.2d %2.2d  ", substr($0,1,4),substr($0,5,2),substr($0,7,2),substr($0,9,2),substr($0,11,2),substr($0,13,2))}' `
set Time = `date "+ %Y %m %d %H %M %S"`
#echo $Time[1]

cd ${SLAQR_RUN}

if (! -e $Time[1] ) then
   mkdir $Time[1]
endif
cd $Time[1]

if (! -e $Time[2] ) then
   mkdir $Time[2]
endif
cd $Time[2]

if (! -e $Time[3] ) then
   mkdir $Time[3]
endif
cd $Time[3]
date >>out

