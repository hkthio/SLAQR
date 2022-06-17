set list = `ls *.pz`

foreach respfile ($list)
   set sta = `echo $respfile | awk '{IND =  index($1,"_")-3;{print substr($1,3,IND)}}'`
   set sacpz = SAC_PZs_${sta}_LHZ_00
   echo $sacpz
   awk '$1 !~/\*/' $respfile  | awk ' { if ( $1 ~/C/ ) {printf("%s %12.6e\n", $1, $2*1.e6)} else {print}}' >$sacpz
end

