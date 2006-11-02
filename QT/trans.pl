#!/usr/bin/perl
use Unicode::String qw(utf8 latin1);
use Encode qw(encode decode from_to);
sub zzz {
	($filename,$lang) = @_;
	open(FILE,$filename);
	$text="";
	while($sor=<FILE>){
		$text.=$sor;
	}
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
		}
		if ($s =~ /POPUP[ \t]*"([^"]*)"/) {$popup++;$msg=$1;$id="POPUP".$popup;}
		if ($s =~ /CAPTION[ \t]*"([^"]*)"/) {$id=$section."_CAPTION"; $msg=$1;}
		if ($id) {
			$msg =~ s/\&/\&amp;/g;
#			$msg =~ s/\xfc/\xc3\xbc/g;
#			$msg =~ s/\xe4/\xc3\xa4/g;
#			$msg =~ s/\xf6/\xc3\xb6/g;
#			$msg =~ s/\xc4/\xc3\x84/g;
#			$msg =~ s/\xd6/\xc3\x96/g;
#			$msg =~ s/\xdc/\xc3\x9c/g;
#			$msg =~ s/\xdf/\xc3\x9f/g;
#			$u=decode("iso-8859-2", $msg);
#			$msg=encode("utf8",$u); 
			from_to($msg,"windows-1250","utf8");
			if ($msg =~ /(.*)\\t/) { $msg=$1;}
			#print $id.",".$msg."\n" 
			$$lang{$id}=$msg;
		}
		#print $s."\n";
	}
}


zzz("../AppResources.rc","english");
zzz("../Translations/German/Resources.rc","german");
zzz("../Translations/Italian/Resources.rc","italian");
zzz("../Translations/Czech/Resources.rc","czech");

print "<!DOCTYPE QPH><QPH>\n";
while (($key, $value) = each(%czech)) {
#	printf $key.",".$english{$key}.",".$czech{$key}."\n";
	print "<phrase>\n    <source>".$english{$key}."</source>\n    <target>";
	print $czech{$key}."</target>\n</phrase>\n";
}
print "</QPH>";
