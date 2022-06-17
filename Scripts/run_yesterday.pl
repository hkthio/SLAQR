#! /usr/bin/perl

use strict;
use DateTime::Precise;

print"Running SLAQR on yesterday's (GMT) data.\n";

my($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime();
$year = $year+1900; 
$mon  = $mon+1; 
my $dy   = $mday; 
my $hr   = 0;
my $min  = 0;
my $sec  = 0;
my $eventtimestr = sprintf "%d%02d%02d%02d%02d%02d",$year,$mon,$dy,$hr,$min,$sec;
my $event_time = DateTime::Precise->new($eventtimestr);
my $start_time = $event_time-24*60*60;
my $start_timestr = $start_time->dprintf("%^Y%M%D%h%m%s");
my $command = "csh $ENV{'SLAQR_HOME'}/Scripts/SLAQR-manu.csh $start_timestr 24h $eventtimestr";
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

`mv $start_timestr $ENV{'SLAQR_HOME'}/www/documents/daily/$start_timestr`;
`unlink $ENV{'SLAQR_HOME'}/www/documents/daily.mov`;
`ln -s $ENV{'SLAQR_HOME'}/www/documents/daily/$start_timestr/last24hr.mov   $ENV{'SLAQR_HOME'}/www/documents/daily.mov`;
