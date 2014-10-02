#!/usr/bin/perl -w 
#
# Skrypt do podmiany metadanych w plikach
#
# Pseudokod:
# 0 obsluga lini polecen
# 10 zbuduj liste plikow
# 20 dla kazdego pliku
# 30	wczytaj plik
# 35	otworz ten sam plik do zapisu
# 40	dla kazdej linijki 
# 50		wykonaj podstawienie
# 60		wydrukuj do pliku
# 70	wroc do 40
# 75	zamknij plik dla zapisu
# 80 wroc do 20

# 0 obsluga lini polecen
if ($#ARGV<1 or $#ARGV>2) {
	print "Usage:\n";
	print "\tch_meta.pl nr_metadanej \"wartosc\" [katalog_z_metadanymi]\n";
	print "EXAMPLE:\n";
	print "\tch_meta.pl 5 \"test\" .\n";
	exit;
}
print $#ARGV," ", $ARGV[0]," ", $ARGV[1];

if ( not $ARGV[0] =~ /[0-9]+/ ) {
	print "nr_metadanej musi byc liczba w systemie dziesietnym\n"
}

# 10 zbuduj liste plikow
if ($#ARGV==2) {
	@test=<$ARGV[2]/*.mtd>;
} else {
	@test=<*.mtd>;
}
# 20 dla kazdego pliku
foreach $i (@test) {
	print "DUPA $i\n";
# 30	wczytaj plik
	open(MYINPUTFILE, "<$i");
	my $holdTerminator = $/;
	undef $/;
	my($lines) = <MYINPUTFILE>;
	$/ = $holdTerminator;
	close(MYINPUTFILE);

	open(MYOUTFILE, ">$i");
# 40	dla kazdej linijki 
	foreach $line ($lines) {
# 50		wykonaj podstawienie
		$line=~s/$ARGV[0]:.*/5:$ARGV[1]/g;
# 60		wydrukuj do pliku
		print MYOUTFILE $line;
	}
	close(MYOUTFILE);

}

