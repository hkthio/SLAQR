cat <<END >wmag.par
list=LIST
start=2008/08/09-00:00:00
#end=2008/08/09-23:59:30
span=24h
step=60
#resp_path=${SLAQR_HOME}/Resp
resp_path=.
prev_file=none
out_file=20080809000000.20080809235930.stackamp
overwrite_out=1

tlag=1800 
maxperiod=350
minperiod=60
w=4
window=4
nwindow=600
xor0=0
dxor=100
nxor=400
tor0=0
dtor=60
ncreg=6
ncub=90
nslice=1
target=1
median=0
output_amp=1
END
