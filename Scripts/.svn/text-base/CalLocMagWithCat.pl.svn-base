#!/usr/bin/perl
# Usage: CalLocMagWithCat.pl <NEIC PDE catalog> 
# Catalogs can be obtained from http://neic.usgs.gov/neis/epic/epic_global.html
# these need to be re-formatted for use with GMT

use POSIX;
use Time::Local;

$minmag = $ARGV[1];

$file2 = $ARGV[0];
unless (open(CFL,$file2)) {
        die("Can't open catalog $file2 \n");
}

while ($line = <CFL>) {
# PDE-Q 2007   02 11 113620.29  16.38  -98.58  34 4.40 mb GS   .. . .......      
        chomp($line);
	$emg = substr($line,49,4);
	$eyr = substr($line,7,4);
	$emo = substr($line,14,2);
	$edy = substr($line,17,2);
	$ehr = substr($line,20,2);
	$emn = substr($line,22,2);
	$ese = substr($line,24,2);
	$elt = substr($line,30,6);
	$eln = substr($line,37,7);
	$edp = substr($line,45,3);
	$etm = substr($line,54,2);
	print "$eyr-$emo-$edy"."T"."$ehr:$emn:$ese $elt $eln $edp $emg\n"; 
}
close(CFL);

exit 0;

sub get_gcarc {

#***** given the, ale (lat. & long. of event),
#*****       ths, als (lat. & long. of station)
#            here given as PAS
#***** obtain delta and azdg (clockwise from north)
#***** all the angles are in deg
#*****     revised Jan. 1989

      my($st_lat,$st_lon,$ev_lat,$ev_lon) = @_;
      my($pi,$rad,$als,$ale,$the,$fe,$al,$fs,$delt,$deltdg,$az,$baz,$azdg,$bazdg,$ths);
      $ths=$st_lat;
      $als=$st_lon;
      $the=$ev_lat;
      $ale=$ev_lon;
      $pi = acos(-1.0);
      $rad = $pi/180.0;
      if ($als<0) {$als = 360.0 + $als};
      if ($ale<0) {$ale = 360.0 + $ale};
      $fe = (90.0 - $the ) * $rad;
      $al = ($als - $ale) * $rad;
      $fs = (90.0 - $ths ) * $rad;
      $delt = acos(cos($fs)*cos($fe) + sin($fs)*sin($fe)*cos($al));
      $deltdg =  $delt / $rad;
      $delt = $deltdg * 111;
      if ($fe==0 || $delt==0) {
        $delt=1;
      }
      $az  = acos((cos($fs)-cos($fe)*cos($delt))/(sin($fe)*sin($delt))) ;
      $baz = acos((cos($fe)-cos($fs)*cos($delt))/(sin($fs)*sin($delt))) ;
      $azdg  =  $az / $rad;
      $bazdg = $baz / $rad;
      if(($al>=0 && $al<$pi) || ($al>-($pi*2) && $al<-$pi)) {
                $bazdg = 360.0 - $bazdg;
      } else {
                $azdg = 360.0 - $azdg;
      }
      return($delt);
}

