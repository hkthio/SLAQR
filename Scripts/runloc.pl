#!/usr/bin/perl
# Usage: translatetime.pl YYMMDDHHMMSS duration_in_sec
# Output: YY/MM/DD HH:MM:SS of two times added together

use POSIX;
use Time::Local;

$start = $ARGV[0];
$dur = $ARGV[1];
$lag=1800;
$window=600;

#decode start time
$year = unpack("a4",$start);
$mon  = unpack("\@4a2",$start);
$day  = unpack("\@6a2",$start);
$hour = unpack("\@8a2",$start);
$min  = unpack("\@10a2",$start);
$sec  = unpack("\@12a2",$start);

#decode duration
$unit = substr($dur,-1);
$l_dur=length($dur);
print "$unit\n";
if ($unit eq "0") {
   $dur=$dur;
   }
elsif ($unit eq "h") {
   print "$dur\n";
   $dur = substr($dur,0,$l_dur-1);
   $dur *= 3600;
   }
elsif ($unit eq 'm') {
   $dur = substr($dur,0,$l_dur-1);
   $dur *= 60;
   }
elsif ($unit eq 's') {
   $dur = substr($dur,0,$l_dur-1);
   }
   
#find end time
$time = timegm($sec,$min,$hour,$day,$mon-1,$year-1900);
$time2 = $time + $dur;
$dur = $dur + $lag + $window/2.;
($sec2,$min2,$hour2,$day2,$mon2,$year2,$wday2,$yday2,$istdst2) = gmtime($time2);
$mon2 += 1;
$year2 += 1900;
$yday2 += 1;
$end = sprintf("%04d%02d%02d%02d%02d%02d",$year2,$mon2,$day2,$hour2,$min2,$sec2);

print "Year = $year  Month = $mon  Day = $day Hour = $hour  Min = $min  Sec= $sec  \n";


print "$start $dur $end $dur\n";

exit 0;



