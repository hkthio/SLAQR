#!/usr/bin/perl
#Old Usage:  get_waveforms_edge lasteventfile stationsfile comp
#New Usage:  getEdge starttime_in_YYYYMMDDHHMMSS  duration_in_sec stationsfile comp
# where comp is LHZ LHN LHE BHZ BHN BHE etc.
# Modified 4/13/05 from TriNet wavepool version
#	Had to change read from lastevent file due to less columns
# Modified 6/17/05 to work with new location code compliant waveman2disk
#       Added v2.0 to start of first line
#       Added default location code of 00 to trigfile
#       Added empty line to end of trigfile
# Modified 07/10/06 to work with the edge
# Modified 01/03/07 to work with generalized inputparameters

use POSIX;
use Time::Local;
use Cwd;

$start     = $ARGV[0];
$dur       = $ARGV[1];
$lag       = $ARGV[2];
$halfwidth = $ARGV[3];
$window = $maxper/2;

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
$time2 = $time - $halfwidth;
($sec,$min,$hour,$day,$mon,$year,$wday,$yday,$istdst) = gmtime($time2);
$mon += 1;
$year += 1900;
$yday += 1;

$dur = $dur + $lag + $halfwidth;

print "Year = $year  Month = $mon  Day = $day Hour = $hour  Min = $min  Sec= $sec  \n";


print "$start $dur $end $dur\n";

$infile = $ARGV[4];
unless (open(STAFILE,$infile)) {
        die("Can't open stationfile $infile \n");
}

$ofile = "i_edgequery";
unless (open(REQFILE,"> $ofile")) {
        die("Can't open waveform request file $ofile \n");
}

$numargs = @ARGV;
$num=0;
while ($num <= ($numargs - 6) ) {
	$comp[$num]=$ARGV[5+$num];
	$num++;	
}

$loc="00";

$line_no=0;

	print "Year = $year  Month = $mon  Day = $day Hour = $hour  Min = $min  Sec= $sec  \n";

$f_mon= sprintf ("%02d",$mon);
$f_day= sprintf ("%02d",$day);
$f_hour= sprintf ("%02d",$hour);
$f_min= sprintf ("%02d",$min);
$f_sec= sprintf ("%02d",$sec);

$o_date="$year/$f_mon/$f_day";
$o_time="$f_hour:$f_min:$f_sec";
	
while ($linesta = <STAFILE>) {
	chomp($linesta);
	($stainfo,$comments) = split("#",$linesta);
	($net,$loc,$sta) = split(" ",$stainfo);
	$len = length($sta);
        for ($pos=5; $pos>$len; $pos--) {
		$sta = $sta." ";
	}
	$no_co=0;

	while ($no_co<=(@comp-1) ) {
		$cmp = $comp[$no_co];
		print "$net $sta $loc $cmp $no_co\n";
		print REQFILE "-nogaps -s \"$net$sta$cmp$loc\" -b \"$o_date $o_time\" -d $dur -sacpz um\n";
#		system("java -jar ./CWBQuery.jar -b \"$o_date $o_time\" -s \"$net$sta$cmp$loc\" -d $dur -hp");
		$no_co++;
	}
        $line_no++;
}

close(REQFILE);
print "Requesting data from the Edge\n";
system("java -jar ~/CWBQuery/CWBQuery.jar -f $ofile > out_edge");

# FFC.LHN.II.00 was the form output by waveserver, use that same format for simplicity
# IIARU__LHN00.sac is edge format

@wavfiles = qx(ls -1dF *.sac);
$nof = @wavfiles;
for ($no=0;$no<$nof;$no++) {
	chomp($wavfiles[$no]);
	$net = substr($wavfiles[$no],0,2);
	$loc = substr($wavfiles[$no],10,2);
	$sta = substr($wavfiles[$no],2,5);
	$sta =~ s/\_//g; #remove the underscores for stations with fewer than 5 letters
	$chan = substr($wavfiles[$no],7,3);
	$sta =~ tr/a-z/A-Z/;
	$chan =~ tr/a-z/A-Z/;
	print "cp $wavfiles[$no] $sta.$loc.$net.$chan\n";
	system("mv $wavfiles[$no] $sta.$loc.$net.$chan");
}

exit 0;

