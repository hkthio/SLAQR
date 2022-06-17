#!/usr/local/bin/perl -w
use Carp;

# C. Scrivner
# February 21, 1995
# A class for Seismogram::Hiroo format files

# Methods:
#  $hiroo = new Seismogram::Hiroo (%initialvalues);
#  $error = Read $hiroo ($filehandle);
#  $error = Write $hiroo ($filehandle);
#  $check = Change $hiroo (%values);     - returns number of successful changes
#  @list  = Fetch $hiroo (@headerkeys);  - returns values to header keys
#  $gram  = Translate $hiroo ($newtype); - translates Hiroo to $newtype record

#use strict;
package Seismogram::Hiroo;

require Seismogram;
require "util.pl";
@Seismogram::Hiroo::ISA = qw( Seismogram::Seismogram );


@Seismogram::Hiroo::hirookeys = ('filename','traceid','cmp','station','year',
				 'jday','hour','minute','sec','msec','b','npts','delta',
				 'data');
%Seismogram::Hiroo::proxykeys = ('nzyear' => 'year','nzjday' => 'jday','nzhour' => 'hour',
				 'nzmin' => 'minute','nzsec' => 'sec','nzmsec' => 'msec',
				 'kstnm' => 'station','kcmpnm' => 'cmp');
@Seismogram::Hiroo::traceid   = ('Unknown','V','N','E');
%Seismogram::Hiroo::traceid   = ('V' => 1,'Z' => 1,'N' => 2,'E' => 3);
%Seismogram::Hiroo::defaults = ('numtraces' => 1,
                                'format' => '(6E12.5)',
                                'b'         => 0);

sub new {
  my $class = shift;
  # the additional arguments are not required at time of construction
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
  @_ == 2 or ::croak "$0 Seismogram::Hiroo::Read: passed wrong number of arguments";
  
  my $this       = shift;
  my $filehandle = shift;
  my $linepack   = "";
  my $check      = "";
  my($perline,$width);
  my($i,$j,$blank,@time,@data,$npts);
  my($traceid,$station,$cmp,$npts,$delta,$format);
  my($year,$jday,$hour,$minute,$sec,$msec);
  
  $i = 0;
  while ($check = <$filehandle>) {
    ($blank,$traceid,$station) = split(' ',$check); # 9/11 - possible problem here.  Was using /\s+/, still need $blank?
    $cmp = $traceid[$traceid];
    ($blank,@time)             = split(' ',<$filehandle>);
    ($blank,$npts,$delta)      = split(' ',<$filehandle>);
    ($perline,$width) = $format =~ /(\d+)[efg](\d+)/i;
    for ($j=0;$j<$perline;$j++) {
      $linepack .= "a$width";
    }
    @data = ();
    while (@data < $npts) {
      push(@data,unpack($linepack,<$filehandle>));
    }
    # there may be some extra empty values that need to be removed
    for ($j=@data;$j>$npts;$j--) {
      pop(@data);
    }
    ($year,$jday,$hour,$minute,$sec,$msec) = @time;
    #make sure msec is positive
    if ($msec < 0) {
      $sec--;
      $msec += 1000;
    }
    $this->Change("traceid-$i" => $traceid,"station-$i" => $station,
                  "cmp-$i"     => $cmp,    "npts-$i"    => $npts,
                  "delta-$i"   => $delta,  "format-$i"  => $format,
                  "year-$i"    => $year,   "jday-$i"    => $jday,
                  "hour-$i"    => $hour,   "minute-$i"  => $minute,
                  "second-$i"  => $second, "msec-$i"    => $msec,
                  "data-$i"    => \@data);
    $i++;
  }
  return(0) if !$i;    # nothing read
  
  $this->Change('numtraces' => $i,'format' => $format);
  
  1; # successful completion
}


sub Write {
  @_ or ::croak "$0 Seismogram::Hiroo::Write: passed wrong number of arguments";
  
  my $this       = shift;
  my $filehandle = shift;
  my($perline,$notation,$width,$precision);
  my($i,$j,$k,$format,$numtraces,$check,$shortcmp);
  my($cmp,$traceid,$station,$npts,$delta,
     $year,$jday,$hour,$minute,$sec,$msec,$data);
  
  ($numtraces,$format) = $that->Fetch('numtraces','format');
  $numtraces = $defaults{'numtraces'} if !defined($numtraces);
  $format    = $defaults{'format'}    if !defined($format);
  $format = "($format)" if substr($format,0,1) ne '(';
  ($perline,$notation,$width,$precision) = 
      $format =~ /(\d+)([efgEG])(\d+)\.(\d+)/ or return(0);
  $format = "+${width}.${precision}${notation}";
  for ($i=0;$i<$numtraces;$i++) {
    ($cmp,$traceid,$station,$npts,$delta,
     $year,$jday,$hour,$minute,$sec,$msec,$data) =
    	 $this->Fetch("cmp-$i", "traceid-$i","station-$i","npts-$i",  "delta-$i",
    	              "year-$i","jday-$i",   "hour-$i",   "minute-$i","second-$i",
    	              "msec-$i","data-$i");
    $shortcmp = substr($cmp,0,1);
    $check = $traceid{"\U$shortcmp\E"} or return(0);
    if ($check != $traceid) {
      warn "Seismogram::Hiroo::Write: $cmp conflicts with traceid.  Using $cmp\n";
      $traceid = $check;
    }
    printf $filehandle ("%5d %-16s\n",$traceid,$station) or return(0);
    printf $filehandle ("%5d %6d %6d %6d %6d %6d\n",$year,
			$jday,$hour,$minute,$sec,$msec)              or return(0);
    printf $filehandle ("%10d   %9.4e\n",$npts,$delta)   or return(0);
    for ($j=0;$j<$npts;) {
      for ($k=0;$k<$perline && $j<$npts;$k++) {
	printf $filehandle ("%$format",$data->[$j])      or return(0);
	$j++;
      }
      print $filehandle "\n";
    }
  }
  
  1;
}


sub Translate {
  @_ == 2 or ::croak "$0 Seismogram::Hiroo::Translate: passed wrong number of arguments";
  
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
