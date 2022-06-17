#!/bin/csh -f
alias acalc '       awk "BEGIN{ print \!* }" '

set width = 4
set maxperiod = 150
set Lag = 1800

@ Window = $width * $maxperiod
set HalfWidth = `acalc $width*$maxperiod/2.`
echo $HalfWidth

set ROOT = ${SLAQR_HOME}
set BIN  = ${ROOT}/Src
set SCR  = ${ROOT}/Scripts
set PAR  = ${ROOT}/Par

set Start  = $1
set StartString = `echo $Start |awk '{printf("%4d/%2.2d/%2.2d-%2.2d:%2.2d:%2.2d\n", substr($0,1,4),substr($0,5,2),substr($0,7,2),substr($0,9,2),substr($0,11,2),substr($0,13,2))}' `
set Duration = $2
echo $StartString
set WorkDir = $1
mkdir  $WorkDir
cd $WorkDir
cp  ${PAR}/groupvel  ${PAR}/station.in ${PAR}/.gmtdefaults4 .
csh ${SCR}/makepar.csh

${SCR}/req_EDGE.pl ${Start} ${Duration} ${Lag} ${HalfWidth}  station.in LHZ LHN LHE
${SCR}/process.pl

ls *.nogap > LIST

${BIN}/locate-mag-median par=wmag.par start=${StartString} span=${Duration} out_file=${1}.${3}.stackamp
${SCR}/GetNEICCatForLoc.pl*
${SCR}/runmany
csh ${SCR}/makemovie.csh
