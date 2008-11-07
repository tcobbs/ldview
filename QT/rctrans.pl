#!/usr/bin/perl
sub load {
	($filename,$lang) = @_;
	$langtext=$lang."text";
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
		}
		if ($s =~ /POPUP[ \t]*"([^"]*)"/) {$popup++;$msg=$1;$id="POPUP".$popup;}
		if ($s =~ /CAPTION[ \t]*"([^"]*)"/) {$id=$section."_CAPTION"; $msg=$1;}
		if ($id) {
			$msg =~ s/\&/\&amp;/g;
			if ($msg =~ /(.*)\\t/) { $msg=$1;}
			$$lang{$id}=$msg;
			$$langtext{$msg}=$id;
		}
	}
}

sub dumptrans {
    ($filename,$from,$to) = @_;
	$fromtext=$from."text";
    open(FILE,$filename);
    while($sor=<FILE>)
	{
		$sor =~s/code_page\([0-9]*\)/code_page\(1250\)/;
		if ($sor =~ /"([^"]*)"/) 
		{
			$text=$1;
			if ($$fromtext{$text})
			{
				$sor =~ s/$text/$$to{$$fromtext{$text}}/;
			}
		}
		print $sor;
    }
    close FILE;
}

load("../LDView.rc","english");
load("../Translations/Hungarian/LDView.rc","hungarian");

dumptrans("../LDView.rc","english","hungarian");
