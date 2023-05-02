#!/usr/bin/perl
#use Unicode::String qw(utf8 latin1);
use Encode qw(encode decode from_to);
sub zzz {
	($filename,$lang,$inifile) = @_;
	open(FILE,$filename);
	$text="";
	while($sor=<FILE>){
		$text.=$sor;
	}
	close FILE;
	$text =~ s/,[\t ]*\n[\t ]*/,/g;
	$text =~ s/\|[\t ]*\n[\t ]*/\|/g;
	$popup=0;
	@line = split(/\n/g,$text);
	foreach(@line){
		$s=$_;
		$id=$msg="";
		if ($s =~ /VIRTKEY/) { $s="";}
		if ($s =~ /(ID[_A-Z0-9]*)[ \t]*DIALOG/) {$section=$1; $cnt=0; }
		if ($s =~ /"([^"]*)"[ \t]*\,[ \t]*(ID[A-Z0-9_]*)/) {
			$msg=$1;
			$id=$2;
			if ($id =~ /IDC_STATIC/) {
				$cnt++;
				$id=$section."_IDC_STATIC".$cnt;
			}
			$msg =~ s/<//g;
		}
		if ($s =~ /POPUP[ \t]*"([^"]*)"/) {$popup++;$msg=$1;$id="POPUP".$popup;}
		if ($s =~ /CAPTION[ \t]*"([^"]*)"/) {$id=$section."_CAPTION"; $msg=$1;}
		if ($id) {
			$msg =~ s/\&/\&amp;/g;
#			$u=decode("iso-8859-2", $msg);
#			$msg=encode("utf8",$u); 
			from_to($msg,"windows-1250","utf8");
			if ($msg =~ /(.*)\\t/) { $msg=$1;}
			#print $id.",".$msg."\n" 
			$$lang{$id}=$msg;
		}
		#print $s."\n";
	}
    open(FILE,$inifile);
    while($sor=<FILE>){
		$key=$value="";
		$seq=0;
		$sor =~ s/;.*//g;
		if ($sor=~/(.*)=(.*)/) {$key=$1;$value=$2;}
		if ($key=~/^(.*)[\s]@$/) {$key=$1;}
		if ($key=~/^([a-zA-Z_]*)([0-9][0-9]*)[\s]*$/) {$key=$1;$seq=$2;}
		if ($key) {
			from_to($value,"windows-1250","utf8");
			$key="ini_".$key;
#			if ($$lang{$key}) {$$lang{$key}.="\n";}
			$value =~ s/</&lt;/g;
			$value =~ s/>/&gt;/g;
			$value =~ s/\&/\&amp;/g;
			$$lang{$key}.=$value;
#			print $key,",",$seq,",",$value,"\n";
		}
    }
	close(FILE);
}

sub dumptrans {
	($lang,$filename,$langcode) = @_;
	open FILE, '>'.$filename || die "open error";
	print FILE "<!DOCTYPE QPH><QPH sourcelanguage=\"en\" language=\"$langcode\">\n";
	while (($key, $value) = each(%$lang)) {
		if ($english{$key} ne $$lang{$key}) {
			print FILE "<phrase>\n    <source>".$english{$key}."</source>\n";
			print FILE "    <target>";
			print FILE $$lang{$key}."</target>\n</phrase>\n";
		}
	}
	print FILE "</QPH>";
	close(FILE);
}

zzz("../LDView.rc","english","../LDViewMessages.ini");
zzz("../Translations/German/LDView.rc","german","../Translations/German/LDViewMessages.ini");
zzz("../Translations/Italian/LDView.rc","italian","../Translations/Italian/LDViewMessages.ini");
zzz("../Translations/Czech/LDView.rc","czech","../Translations/Czech/LDViewMessages.ini");
zzz("../Translations/Hungarian/LDView.rc","hungarian","../Translations/Hungarian/LDViewMessages.ini");
dumptrans("czech","/tmp/czech.qph","cs");
dumptrans("german","/tmp/german.qph","de");
dumptrans("italian","/tmp/italian.qph","it");
dumptrans("hungarian","/tmp/hungarian.qph","hu");
