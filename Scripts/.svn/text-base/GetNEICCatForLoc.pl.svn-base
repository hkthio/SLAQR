#!/usr/bin/perl
# Output: Catalog data from NEIC website in format suitable for plotting in Locator plots with 
#	GMT-format date

use POSIX;
use Time::Local;

$minmag=4.0;

`rm eqs7day-M1.txt*`;
`/sw/bin/wget http://earthquake.usgs.gov/eqcenter/catalogs/eqs7day-M1.txt`;

$catfile = "eqs7day-M1.txt";
$ofile = "LocNEICCat.txt";

unless (open(CATIN,$catfile)) {
        die("Can't open NEIC web catalog file $catfile\n");
}
unless (open(CATO," > $ofile")) {
        die("Can't open quake file $ofile \n");
}

my %monthnum = qw( January 01 February 02 March 03 April 04 May 05 June 06 July 07 August 08 September 09 October 10 November 11 December 12 );

$header=<CATIN>;
while ($line=<CATIN>){
        chomp($line);
        @words = split("\"",$line);
#       print "words 0: $words[0]\n $words[1]\n $words[2]\n $words[3]\n\n";
	($sp,$evlat,$evlon,$evmag,$evdp,$evnst) = split("\,",$words[2]);
#	print "evlat = $evlat evlon = $evlon mag=$evmag dp = $evdp nst $evnst\n";
	if ($evmag>=$minmag) {
		$region = $words[3];
		@date = split("\,",$words[1]);
#       	print "date 0: $date[0]\n $date[1]\n $date[2]\n\n";
		($monname,$day) = split(" ",$date[1]);
		if ($day<10) {$day = "0".$day;}
#		print "monname = $monname day = $day\n";
		($year,$time,$type) = split(" ",$date[2]);
		($hour,$min,$sec) = split(":",$time);
#		print "year = $year mon = $monthnum{$monname} day = $day hour = $hour min = $min sec = $sec\n";
		print CATO "$year-$monthnum{$monname}-$day"."T"."$hour:$min:$sec $evlon $evlat $evmag $evdp $region\n";
	}
}
close(CATO);
exit 0;



