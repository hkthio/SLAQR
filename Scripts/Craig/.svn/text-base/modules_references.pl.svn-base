#!/usr/bin/perl -w

# This code does the same thing as subroutines_processes.pl, but operating
# directly on the SAC file, not by starting up the SAC code

# The IO::File module handles opening up a filehandle to a file
# The SAC module knows how to read, write and manipulate SAC files

@ARGV or die "usage: $0 files < resdat.TK\n";

use lib '/home/scrivner/lib/perl'; # Add a path to where my modules are
require IO::File;                  # Part of the standard distribution
require Seismogram::SAC;           # The SAC module is homebrew

@files = @ARGV;

%parameter = &parse(STDIN);

$gram = new Seismogram::SAC;
foreach $file (@files) {
  @bits = split(/\./,$file);
  $station = $bits[1];
  $gain = $parameter{$station}{'vbb'}{'gain'};

  $fh = new IO::File;
  $fh->open($file,'r');
  $gram->Read($fh);
  $fh->close;

  ($data,$npts) = $gram->Fetch('data','npts');
  
  for ($i=0;$i<$npts;$i++) {
    $data->[$i] = $data->[$i] / $gain;
  }

  $fh->open($file,'w');
  $gram->Write($fh);
  $fh->close;
}


sub parse {
  my $filehandle = shift(@_);
  my(@file,$line,@items,$name,$stream,$key,@KEYS,%parameter);

  @KEYS = ('frequency','damping','gain','nyquist');

  @file = <$filehandle>;
  
  foreach $line (@file) {
    @items = split(/\s+/,$line);
    $name   = shift(@items);
    $stream = shift(@items);
    foreach $key (@KEYS) {
      $parameter{$name}{$stream}{$key} = shift(@items);
    }
  }

  return(%parameter);
}

0;
