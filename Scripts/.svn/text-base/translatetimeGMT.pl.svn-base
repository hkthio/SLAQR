#!/usr/bin/perl 
# Usage: translatetimeGMT.pl YYMMDDHHMMSS 
# Output: GMT format time output 
use POSIX; 
use Time::Local; 
$date = $ARGV[0]; 
$dur = $ARGV[1]; 
$year = unpack("a4",$date); 
$mon = unpack("\@4a2",$date); 
$day = unpack("\@6a2",$date); 
$hour = unpack("\@8a2",$date); 
$min = unpack("\@10a2",$date); 
$sec = unpack("\@12a2",$date); 
print "$year-$mon-$day"."T"."$hour:$min:$sec\n"; 
exit 0;

