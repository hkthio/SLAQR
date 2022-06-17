#!/usr/local/bin/perl -w
use Carp;

# Seismogram::Helm.pm 1.2
# C. Scrivner
# January 28 - February 8, 1995
# A class for Helmberger format files

# Methods:
#  $helm  = new Seismogram::Helm (%initialvalues);
#  $error = Read $helm ($filehandle);
#  $error = Write $helm ($filehandle);
#           Translate Seismogram::Helm ($other);
# The following are Seismogram methods (a superclass of Helm)
#  @list  = Fetch $helm (@headerkeys);   - returns values to header keys
#           Change $helm (%values);      - returns number of successful changes
#  @list  = Split Seismogram::Helm ($sac[,$number]); - split record into pieces
#  $helm  = Join Seismogram::Helm (@list);           - join records into single record

package Seismogram::Helm;

require Seismogram;
require "util.pl";
@Seismogram::Helm::ISA = qw( Seismogram::Seismogram );


@Seismogram::Helm::helmkeys = ('numtraces','format','dist','b',
			       'npts','delta','unknown1','unknown2','data');
%Seismogram::Helm::defaults = ('numtraces' => 1,'format' => '6e11.4');

sub new {
  my $class = shift;
  # the following is not required at time of construction
  my %initial = @_;
  my(%mark);
  
  my $self = {};
  bless $self;
  
  $self->Change(%defaults);
  if (%initial) {
    $self->Change(%initial);
  }
  
  return $self;
}


sub Read {
  @_ == 2 or ::croak("$0 Seismogram::Helm::Read: expects 2 arguments");
  
  my $this       = shift;
  my $filehandle = shift;
  my $linepack   = "";
  my $check      = "";
  my($perline,$width);
  my($numtraces,$format);
  my($distance,$b,$unknown1,$unknown2);
  my($i,$j,$npts,$delta,$blank);
  
  
  $check = <$filehandle>;
  return(0) if !$check;    # nothing to read
  
  chomp($numtraces = $check);
  chomp($format    = <$filehandle>);
  $this->Change('numtraces' => $numtraces,'format' => $format);
  ($perline,$width) = $format =~ /(\d+)[efgEG](\d+)/ or return(0);
  for ($i=0;$i<$perline;$i++) {
    $linepack .= "a$width";
  }
  for ($i=0;$i<$numtraces;$i++) {
    ($distance,$b,$unknown1,$unknown2) = split(' ',<$filehandle>);
    ($npts,$delta) = split(' ',<$filehandle>);
    my @data = (); # start a new data array
    while (@data < $npts) {
      push(@data,unpack($linepack,<$filehandle>));
    }
    # there may be some extra empty values that need to be removed
    for ($j=@data;$j>$npts;$j--) {
      pop(@data);
    }
    $this->Change("b-$i" =>$b,"npts-$i" => $npts,
		  "delta-$i" => $delta,"dist-$i" => $distance,
		  "unknown1-$i" => $unknown1,"unknown2-$i" => $unknown2,
		  "data-$i" => \@data);
  }
  
  1; # successful completion
}


sub Write {
  @_ == 2 or ::croak("$0 Seismogram::Helm::Write: expects 2 arguments");
  
  my $this     = shift;
  my $filehandle = shift;
  my($perline,$notation,$width,$precision);
  my($format,$numtraces,$dist,$b,$npts,$dt,$data);
  my($i,$j,$k,$form);
  
  ($format,$numtraces) = $this->Fetch('format','numtraces');
  $format    = $defaults{'format'}    if !defined($format);
  $numtraces = $defaults{'numtraces'} if !defined($numtraces);
  $format = "($format)" if substr($format,0,1) ne '(';
  ($perline,$notation,$width,$precision) =
      $format =~ /(\d+)([efgEG])(\d+)\.(\d+)/ or return(0);
  $form = "${width}.${precision}$notation";
  printf $filehandle ("%8d\n",$numtraces) or return(0);
  printf $filehandle ("%s\n",$format) or return(0);
  for ($i=0;$i<$numtraces;$i++) {
    ($dist,$b,$npts,$dt,$data) = 
	$this->Fetch("dist-$i","b-$i","npts-$i","delta-$i","data-$i");
    printf $filehandle ("%15.4e%15.4e\n",$dist,$b) or return(0);
    printf $filehandle ("%8d%10.5f\n",$npts,$dt) or return(0);
    for ($j=0;$j<$npts;) {
      for ($k=0;$k<$perline && $j<$npts;$k++) {
	printf $filehandle ("%$form",$data->[$j]) or return(0);
	$j++;
      }
      print $filehandle "\n" or return(0);
    }
  }
  
  1;
}


sub Translate {
  @_ == 2 or ::croak("$0 Seismogram::Helm::Translate: expects 2 arguments");
  
  my $class = shift;
  my $that  = shift;
  
  ($numtraces,$format) = $that->Fetch('numtraces','format');
  $numtraces = $defaults{'numtraces'} if !defined($numtraces);
  $format    = $defaults{'format'}    if !defined($format);
  $that->Change('numtraces' => $numtraces,
		'format' => $format);
  
  my %translating = %$that;
  my $translated = \%translating;
  
  bless $translated;
}


1;
