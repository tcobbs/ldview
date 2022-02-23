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
		if ($s =~ /pragma code_page\(([0-9]+)\)/) {$id="codepage";$msg=$1;}
		if ($s =~ /([_A-Z0-9]*)[ \t]*DIALOG/) {$section=$1; $cnt=0; }
		if ($s =~ /"([^"]+)"[ \t]*\,[ \t]*(ID[A-Z0-9_]+)/) {
			$msg=$1;
			$id=$2;
			if ($id =~ /IDC_STATIC/) {
				$cnt++;
				$id=$section."_IDC_STATIC".$cnt;
			}
		}
		if ($s =~ /[ \t]*POPUP[ \t]*"([^"]*)"/) {$popup++;$msg=$1;$id="POPUP".$popup;}
		if ($s =~ /CAPTION[ \t]*"([^"]*)"/) {$id=$section."_CAPTION"; $msg=$1;}
		if ($id && !$$lang{$id}) {
			$msg2=$msg;
			$msg2 =~ s/[&\.\+]//g;
			$$lang{$id}=$msg;
			$$langtext{$msg}=$id;
#			print STDERR $id,":\t",$msg,":",$msg2,"\n";
		}
	}
}

sub dumptrans {
    ($filename,$from,$to,$outfile) = @_;
	$fromtext=$from."text";
    open(FILE,$filename);
	open(OFILE,'>'.$outfile) || die "open error";
    while($sor=<FILE>)
	{
		if ($sor =~ /code_page\([0-9]*\)/) {$sor ="#pragma code_page(".$$to{codepage}.")";}
		if ($sor =~ /"([^"]+)"/) 
		{
			$text=$1;
			if ($$fromtext{$text} && $$to{$$fromtext{$text}})
			{
				$text2=$text;
				$text2 =~ s/\\/\\\\/g;
				$text2 =~ s/\+/\\\+/g;
				$text2 =~ s/\(/\\\(/g;
				$text2 =~ s/\)/\\\)/g;
				$text3 = $$to{$$fromtext{$text}};
				$sor =~ s/$text2/$text3/;
				print STDERR $text2,"->",$text3,",",$$fromtext{$text},"\n";
			}
		}
		$sor=~ s/\"Icons/\"..\\\\..\\\\Icons/  ;
		$sor=~ s/\"Textures\\\\/\"..\/..\/Textures\\\\/ ;
		$sor=~ s/^\#include \"([A-Z])/\#include \"..\/..\/$1/ ;
		print OFILE $sor;
    }
    close FILE;
	close OFILE;
}

load("../LDView.rc","english");
load("../Translations/Hungarian/LDView.rc","hungarian");
load("../Translations/German/LDView.rc","german");
load("../Translations/Italian/LDView.rc","italian");
load("../Translations/Czech/LDView.rc","czech");

dumptrans("../LDView.rc","english","hungarian","/tmp/LDView-hu.rc");
dumptrans("../LDView.rc","english","czech","/tmp/LDView-cz.rc");
dumptrans("../LDView.rc","english","italian","/tmp/LDView-it.rc");
dumptrans("../LDView.rc","english","german","/tmp/LDView-de.rc");

