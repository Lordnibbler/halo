#!/usr/bin/perl

my $halo_path = "/home/root/halo";

local ($buffer, @pairs, $pair, $name, $value, %FORM);
    # Read in text
    $ENV{'REQUEST_METHOD'} =~ tr/a-z/A-Z/;
    if ($ENV{'REQUEST_METHOD'} eq "POST")
    {
        read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'});
    }else {
	$buffer = $ENV{'QUERY_STRING'};
    }
    
    # Split information into name/value pairs
    @pairs = split(/&/, $buffer);
    foreach $pair (@pairs)
    {
	($name, $value) = split(/=/, $pair);
	$value =~ tr/+/ /;
	$value =~ s/%(..)/pack("C", hex($1))/eg;
	$FORM{$name} = $value;
    }
    

print "Content-type:text/html\r\n\r\n";

if($FORM{command} eq "POWER_ON"){
	
	
	system("echo 'ACTIVE_PLAYLIST $FORM{playlist}' > $halo_path/commands/set_playlist");
	system("echo 'POWER 1' > $halo_path/commands/poweron");
	
	print "$FORM{playlist}";
	
}
if($FORM{command} eq "POWER_OFF"){
	
	system("echo 'POWER 0' > $halo_path/commands/poweroff");
	
}

if($FORM{command} eq "change_light"){
	
	system("echo 'CHANGE_LIGHT\n$FORM{data}' > $halo_path/commands/change");
	
#	print FILE "CHANGE_LIGHT\n";
#	print FILE $FORM{data};
#	close (FILE);
	
	
}

if($FORM{command} eq "save_timeline"){
	open(FILE, "> $halo_path/sequences/$FORM{title}.html");

	print FILE $FORM{data};
	close(FILE);
	
	@files = `ls -1 $halo_path/sequences/`;
	
	foreach $filename(@files){
		if($filename =~/(\w+)\.html/){
			print "$1,";
		}
	}
	
}

if($FORM{command} eq "open_timeline"){
	open(FILE, "< $halo_path/sequences/$FORM{title}.html");
	
	my(@lines) = <FILE>; # read file into list
	my($line);
	foreach $line (@lines) # loop thru list
 	{
 		print "$line"; # print in sort order
 	}
	close(FILE);
	
	
}

if($FORM{command} eq "list_timelines"){

	@files = `ls -1 $halo_path/sequences/`;
	
	foreach $filename(@files){
		if($filename =~/(\w+)\.html/){
			print "$1,";
		}
	}
	
}

if($FORM{command} eq "delete_timeline"){
   
   $result = `rm $halo_path/sequences/$FORM{title}.html`;
   
	@files = `ls -1 $halo_path/sequences/`;
	
	foreach $filename(@files){
		if($filename =~/(\w+)\.html/){
			print "$1,";
		}
	}
	
}

if($FORM{command} eq "save_playlist"){
	open(FILE, "> $halo_path/playlists/$FORM{title}.txt");

	print FILE $FORM{data};
	close(FILE);	
}

if($FORM{command} eq "list_playlists"){

	@files = `ls -1 $halo_path/playlists/`;
	
	foreach $filename(@files){
		if($filename =~/(\w+)\.txt/){
			print "$1,";
		}
	}
	
}


if($FORM{command} eq "read_playlists"){

	@files = `ls -1 $halo_path/playlists/`;
	$not_first = 0;
	foreach $filename(@files){
		if($filename =~/(\w+)\.txt/){
			if($not_first == 1){
				print"\*";
				
			}
			print "$1,";
			$contents = `cat $halo_path/playlists/$1.txt`;
			print "$contents";
			$not_first = 1;
		}
	}
	
}

if($FORM{command} eq "delete_playlist"){
   
   $result = `rm $halo_path/playlists/$FORM{title}.txt`;	
   print "$FORM{title}";
	
}

print "${halo_path}/playlists/\n";