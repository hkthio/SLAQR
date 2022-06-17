#! /usr/bin/perl

use strict;
use File::Basename;
use DateTime::Precise;
use XML::Simple;
use Data::Dumper;

my $tbeforeevent = 3600;

if($#ARGV != 0){
  print"This script runs SLAQR on events in the 7 day QDM merge file\nusage: runevent_id event_id ";
  exit(0);
}

my $eid = $ARGV[0];

# get most recent QDM file
unlink 'merged_catalog.xml.gz';
unlink 'merged_catalog.xml';
`wget http://earthquake.usgs.gov/eqcenter/catalogs/merged_catalog.xml.gz`;
`gunzip merged_catalog.xml.gz`;
my $xml = new XML::Simple;
my $quakes = $xml->XMLin("merged_catalog.xml");
unlink 'merged_catalog.xml.gz';
unlink 'merged_catalog.xml';


my $year = $quakes->{event}->{$eid}->{param}->{year}->{value};
my $mo   = $quakes->{event}->{$eid}->{param}->{month}->{value};
my $dy   = $quakes->{event}->{$eid}->{param}->{day}->{value};
my $hr   = $quakes->{event}->{$eid}->{param}->{hour}->{value};
my $min  = $quakes->{event}->{$eid}->{param}->{minute}->{value};
my $sec  = $quakes->{event}->{$eid}->{param}->{second}->{value};
my $eventtimestr = sprintf "%d%02d%02d%02d%02d%02d",$year,$mo,$dy,$hr,$min,$sec;
my $event_time = DateTime::Precise->new($eventtimestr);
my $start_time = $event_time-$tbeforeevent;
my $start_timestr = $start_time->dprintf("%^Y%M%D%h%m%s");
my $end_time = $event_time+$tbeforeevent;
my $end_timestr = $end_time->dprintf("%^Y%M%D%h%m%s");
my $command = "csh $ENV{'SLAQR_HOME'}/Scripts/SLAQR-manu-median.csh $start_timestr 2h $end_timestr";
print "runing: $command\n";
my $output = `$command`;
sleep(3);
`mv $start_timestr/last24hr $start_timestr/last24hr.mov`; 

open(INDX,">$start_timestr/index.html");

print INDX 
'<HTML>
<head> 
<title>
About SLAQR 
</title>
</head>
<body text="#000000" bgcolor="#ffffff" link="#0062CE" vlink="#6C9EFF" ali

<br><p><table width="100%" cellpadding="1" border="0"><tr>
<td align="left" bgcolor="#52799e">
<font color="#ffffff" face="Helvetica, Arial"> <big><b> <center>
Surfacewave Locator Associator in Quasi Realtime (SLAQR)
</center> </b></big></font></td></tr> </table>
<p>

<embed src="./last24hr.mov" Pluginspage="http://www.apple.com/quicktime/" width="595" height="860" CONTROLLER="true" LOOP="false" AUTOPLAY="false" name="IBM Video"></embed>

<hr>

</body>
</html>';

`mv $start_timestr $ENV{'SLAQR_HOME'}/www/documents/events/$eid`;
