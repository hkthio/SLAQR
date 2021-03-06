#!/bin/csh -f
set SLAQR_HOME =  /Users/hongkie/Develop/SLAQR
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
cp  ${PAR}/SLAQR-dispersion.par  ${PAR}/.gmtdefaults4 .
#cp  ${PAR}/station.in-lownoise station.in
cp  ${PAR}/station.in-usgs station.in
csh ${SCR}/makepar-lp.csh

${SCR}/req_EDGE.pl ${Start} ${Duration} ${Lag} ${HalfWidth}  station.in LHZ LHN LHE
${SCR}/process.pl
csh ${SCR}/reform_resp.csh

ls *.nogap > LIST

echo ${BIN}/locate-mag-median-dev par=wmag.par start=${StartString} span=${Duration} out_file=${1}.${3}.stackamp output_amp=1
${BIN}/locate-mag-median-dev par=wmag.par start=${StartString} span=${Duration} out_file=${1}.${3}.stackamp output_amp=1
${SCR}/GetNEICCatForLoc.pl*
${SCR}/runmany
csh ${SCR}/makemovie.csh
