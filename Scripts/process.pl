#!/usr/bin/perl
#
use POSIX;
use Time::Local;
use Cwd;
use lib "$ENV{SLAQR_HOME}/Scripts/Craig";
use Seismogram::SAC;
use IO::File;

@wavfiles = qx(ls -1dF *.LHZ);
$nof = @wavfiles;
for ($no=0;$no<$nof;$no++) {
                  print " $wavfiles[$no]\n";

		  chomp($wavfiles[$no]);
                  $sacfile = $wavfiles[$no];
                  $fh = IO::File->new($sacfile,'r') or die "can't open sacfile $sacfile\n";
                  $gram = Seismogram::SAC->new;
                  $gram->Read($fh) or die "can't read $sacfile as SAC format file\n";
                  $npts = $gram->npts;
                  $dataref = $gram->data;
                  @data = @$dataref;
                  $fh->close;
		  $gap=0;
                  for ($pt=0;($pt<=$npts);$pt++) {
                                if ($data[$pt]==-12345) {
                                        if ($data[$pt+1]==-12345) {
						$gap=1;
						last;
					}
				}
		  }
		  if ($gap==0) {
        		open(SAC, "|/Applications/sac/bin/sac");
                	print SAC "echo on\n";
                	print SAC "r $sacfile\n";
                	print SAC "rtrend\n";
                	print SAC "decimate 2\n";
                	print SAC "decimate 5\n";
                	print SAC "write $sacfile.nogap \n";
			print SAC "quit\n";
                        close(SAC);
		  }
}
