#!/usr/bin/env perl
$val=100000;
@test=<*.[ch]>;
push(@test, <*/*.[ch]>);
push(@test, <*/*/*.[ch]>);
push(@test, <*/*/*/*.[ch]>);
push(@test, <*/*/*/*/*.[ch]>);
push(@test, <*/*/*/*/*/*.[ch]>);
my %mg;
foreach $i (@test) {
#	print "DUPA $i\n";
	open(MYINPUTFILE, "<$i"); # open for input
	my $holdTerminator = $/;
	undef $/;
	my($lines) = <MYINPUTFILE>; # read file into list

# STRUCTURES
	while ($lines =~ /(((typedef)\s+)?struct\s+(\w+)\s[{][\x5B\x5D\s\w\;\/\*\<\>\:\-\,\'\!\@\#\$\%\^\&\*\(\)\-\_\=\+\|\\]+[}]\s*([*\s]+(\w+))?;)/g) {

#		print "struct in file ",$i," at ", pos $lines, "\n";
		$tmp=$4;
		if ($tmp ne "") { 
			if ($mg{$tmp} eq "") {
				$mg{$tmp}=$val;
				$val=$val+1; 
			}
		}

		$tmp=$6;
		if ($tmp ne "") { 
			if ($mg{$tmp} eq "") {
				$mg{$tmp}=$val;
				$val=$val+1; 
			}
		}
	}

	$/ = $holdTerminator;
close(MYINPUTFILE);
}

foreach my $key (sort(keys %mg)) {
      print "#define BM_$key $mg{$key}\n";
}
#(typedef\s+)?struct\s+(\w+)\s[{]([\s\w;/*<>:-])+[}]\s+([*]+\s)\w+\s*;
