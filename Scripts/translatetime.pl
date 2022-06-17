#!/usr/bin/perl
# Usage: translatetime.pl YYMMDDHHMMSS duration_in_sec
# Output: YY/MM/DD HH:MM:SS of two times added together

use POSIX;
use Time::Local;

$date = $ARGV[0];
$dur = $ARGV[1];

$year = unpack("a4",$date);
$mon  = unpack("\@4a2",$date);
$day  = unpack("\@6a2",$date);
$hour = unpack("\@8a2",$date);
$min  = unpack("\@10a2",$date);
$sec  = unpack("\@12a2",$date);

#print "Year = $year  Month = $mon  Day = $day Hour = $hour  Min = $min  Sec= $sec  \n";

$time = timegm($sec,$min,$hour,$day,$mon-1,$year-1900);
$time2 = $time + $dur;
($sec2,$min2,$hour2,$day2,$mon2,$year2,$wday2,$yday2,$istdst2) = gmtime($time2);
$mon2 += 1;
$year2 += 1900;
$yday2 += 1;

$f_mon2 = sprintf ("%02d",$mon2);
$f_day2 = sprintf ("%02d",$day2);
$f_hour2 = sprintf ("%02d",$hour2);
$f_min2 = sprintf ("%02d",$min2);
$f_sec2 = sprintf ("%02d",$sec2);
$f_year2 = unpack("\@0a4",$year2);

$o_date="$f_year2/$f_mon2/$f_day2";
$o_time="$f_hour2:$f_min2:$f_sec2";

print "$o_date $o_time $f_year2$f_mon2$f_day2$f_hour2$f_min2$f_sec2\n";

exit 0;



