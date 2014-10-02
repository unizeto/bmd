#!/usr/bin/env perl
$val=100000;
#@test=split("\n",`find @ARGV -name "*.[ch]"`);
@test=<*.[ch]>;
push(@test, <*/*.[ch]>);
push(@test, <*/*/*.[ch]>);
push(@test, <*/*/*/*.[ch]>);
push(@test, <*/*/*/*/*.[ch]>);
push(@test, <*/*/*/*/*/*.[ch]>);
my %mg;
my %erclass;

my $holdTerminator = $/;
undef $/;
open(MYINPUTFILE, "<include/bmd/libbmderr/libbmderr.h"); # open for input
	my($lines) = <MYINPUTFILE>; # read file into list
	while ($lines =~ /(#define\s+(ERR_\w+)\s+(\d+))/g) {

		$tmp=$2;
		if ($tmp ne "") { 
			if ($erclass{$tmp} eq "") {
				$erclass{$tmp}=$3;
			}
		}
	}
close(MYINPUTFILE);

#foreach my $key (sort(keys %erclass)) {
#      print "$key $erclass{$key}\n";
#}

foreach $i (@test) {
#	print "DUPA $i\n";
	open(MYINPUTFILE, "<$i"); # open for input
	my($lines) = <MYINPUTFILE>; # read file into list
#	while ($lines =~ /((typedef)?\s+struct\s+(\w+)?\s+[{].*[}]\s+(\w+);)/g) {
#	while ($lines =~ /((typedef\s+)?struct\s+((\w+)\s+)?{[\w\s;]+}\s+([* ]+)?(\w+)?\s*;)/g) {
#	(typedef\s+)?struct\s+(\w+)\s[{]([\x5B\x5D\s\w;/*<>:-,'!@#$%^&*()-_=+|\\])+[}]\s*([*\s]+\w+)?;

	while ($lines =~ /(ERR\s*\(\s*(\w*)\s*,\s*\"([\w\s]+)\"\s*,\s*\w*\s*\))/g) {

#		print "struct in file ",$i," at ", pos $lines, "\n";
#		print $1,"\n";
#		print $2,"\n";
#		print $3,"\n";
	
		$tmp="$3";
		if ($tmp ne "") { 
			if ($mg{$tmp} eq "") {
				$mg{$tmp}=$2;
			} else {
				if ($mg{$tmp} ne $2) {
					print "QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA\n";
					print "QRWA CONFLICT: $tmp defined as both $mg{$tmp} and $2\n";
					print "QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA\n";
					exit -1;
				}
			}
		}
	}

	close(MYINPUTFILE);
}

my @li;
foreach my $key (sort(keys %mg)) {
	my $crc;
	$crc=`src/libbmderr/str2int $key`;
#	print "gff",$crc,"\n";
	$crc=($crc % 1000);
#	print "gff",$crc,"\n";
#	print "#ssdefine $key $mg{$key} $crc\n";
	if ($erclass{$mg{$key}} eq "") {
		print "QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA\n";
		print "QRWA Error class $mg{$key} not definated.\n";
		print "QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA QRWA\n";
		exit -1;
	} else {
#	print "#define $key $mg{$key} $crc\n";
#	print $erclass{$mg{$key}};
	}
	$li[$crc+$erclass{$mg{$key}}]=$key;
}
$/ = $holdTerminator;
#(typedef\s+)?struct\s+(\w+)\s[{]([\s\w;/*<>:-])+[}]\s+([*]+\s)\w+\s*;
my $dupa;
$dupa=0;
print "const char *bmd_error_strings[] = {";
for(my $i=0; $i<=$#li; $i=$i+1) {
	if ($dupa==1) {
		print ",";
		}
	if ($i>0) {
		print " /* $i */\n";
		}
	if ($li[$i] ne "") {
		$dupa=1; 
		print "\"$li[$i]\""; 
	} else {
		$dupa=1;
		printf "0";	
	}
	
}
print "};";
