#!/usr/bin/perl

# use Device::SerialPort;
use Time::HiRes qw(usleep);
use POSIX ":sys_wait_h";

$cmddir = "/home/root/halo/commands/";
$seqdir = "/home/root/halo/sequences/";
$playdir = "/home/root/halo/playlists/";

our $NUM_LIGHTS = 10;
my $SYSTEM_ON = 0;

my $playlist = ();
my $playistLength;
my $playlistIndex = 0;

my $openSequence_flag = 0;
my $sequence = ();
my $sequenceLength;
my $sequenceIndex = 0;


@oldRgb;
for($j = 0;$j < $NUM_LIGHTS; $j++){
	push(@{$oldRgb[$j]},(0,0,0,0));
}
my $timeslotSlope = ();
my $fadeTime = 250;


open(SERIAL, "> /dev/tty01");

{
	my $previous_default = select(STDOUT);  # save previous default
	select(SERIAL);
	$|++;                                   # autoflush STDERR, to be sure
	select($previous_default);              # restore previous default
}
sub maxDiff
{
	my($r,$g,$b,$er,$eg,$eb)= @_;
	my $max = 0;
	my $distance = 0;
	if(abs($er - $r) >= $max){
		$max = abs($er - $r);
		$distance = $er - $r;
	}
	if(abs($eg - $g) >= $max){
		$max = abs($eg - $g);
		$distance = $eg - $g;
	}
	if(abs($eb - $b) >= $max){
		$max = abs($eb - $b);
		$distance = $eb - $b;
	}
	return $distance;
}

sub findSlope
{
	my($r,$g,$b,$violet,$er,$eg,$eb,$ev, $steps)= @_;
	my $rdiff,$gdiff,$bdiff,$vdiff;
	
	$rdiff = ($er - $r) / $steps;
	$gdiff = ($eg - $g) / $steps;
	$bdiff = ($eb - $b) / $steps;
	
	$vdiff = ($ev - $violet)/ $steps;
	
	return ($rdiff,$gdiff,$bdiff,$vdiff);
}
sub interpolate
{
	my($m,$t,$b) = @_;

	return int($m * $t + $b);
}

sub sendColor
{
	my($address,$r,$g,$b,$v)= @_;
	$address = $address + 1;
	print SERIAL "4,$address,$r,$g,$b,$v;";
#	print "4,$address,$r,$g,$b,$v;\n";
}

sub turnOffAll
{
	while(waitpid(-1,WNOHANG ) >= 0) {}
	for(my $address = 0; $address < $NUM_LIGHTS; $address ++){
	 	&sendColor($address,0,0,0,0);
		@{$oldRgb[$address]} = (0,0,0,0);
	}
	
}
sub openTimeline{
	my($timeline_name) = @_;
	
	open(TIMELINE, "< ${seqdir}${timeline_name}.html");
	print "open timeline $timeline_name\n";
	@html = split(/\<div|\<td|\<input/,<TIMELINE>);
	undef(@sequence); #empty timeline
	my $light_index = 0;
	my $sequence_index = -1;
	my $timeslot= ();
	
	foreach(@html){
		if($_=~/.*selector_circle.*rgb\(([0-9]+)\,\s([0-9]+)\,\s([0-9]+)\)/)#rgb\(([0-9]+)\s\,([0-9]+)\s\,([0-9]+)\)
		{
			my @rgb = ($1,$2,$3,0);
			print "row = $sequence_index addr = $light_index @rgb \n";
			push(@{$timeslot[$light_index]},@rgb);
			$light_index++;
		}
		if($_=~/.*selector_circle.*background-image/)#rgb\(([0-9]+)\s\,([0-9]+)\s\,([0-9]+)\)
		{
			my @rgb = (0,0,0,255);
			print "row = $sequence_index addr = $light_index @rgb \n";
			push(@{$timeslot[$light_index]},@rgb);
			$light_index++;
		}
		if($_=~/.*time_box.*value="([0-9]+)".*id="time/)
		{
			print("Time $1\n");
			#print "row = $sequence_index addr = $light_index @rgb \n";
			push(@{$timeslot[$light_index]},$1);
			#$light_index++;
		}
		if($_=~/.*time_box.*value="([0-9]+)".*id="fade/)
		{
			print("Fade $1\n");
			#print "row = $sequence_index addr = $light_index @rgb \n";
			push(@{$timeslot[$light_index]},$1);
			#$light_index++;
		}
		if($_=~/scene_header/){
			if($sequence_index >=0){
				push(@{$sequence[$sequence_index]},@timeslot);
				undef(@timeslot);
			}
			$sequence_index ++;
			$light_index =0;
		}

	}

	push(@{$sequence[$sequence_index]},@timeslot);
	undef(@timeslot);	
	
	close(TIMELINE);
	$sequenceLength = @sequence;
	$sequenceIndex = 0;
}

sub openPlaylist{
	my($playlist_name) = @_;
	
	open(PLAYLIST, "< ${playdir}${playlist_name}.txt");
	
	undef(@playlist); #empty timeline
	
	@playlist = split(/\,/,<PLAYLIST>);
	print "Opened playlist $playlist_name: \n";
	foreach $seq_name (@playlist){
		print "$seq_name\n";
	}
	close(PLAYLIST);
	$playlistLength = @playlist;
	$playlistIndex = 0;
	$openSequence_flag = 1;
}
sub handleCommands{
	opendir my($dh), $cmddir or die "Couldn't open dir '$dirname': $!";
	my @files =  grep { !/^\./ } readdir $dh;
	closedir $dh;
	usleep(1000);
	foreach $filename (@files){
		print "$filename found, opening...\n";
		$fullname =  "$cmddir$filename";
		open (COMMAND_FILE, "< $fullname");
		$command = <COMMAND_FILE>;
		print "read $command\n";
		if($command =~ /ACTIVE_SEQUENCE\s+([\w0-9]*)/){
			
			openTimeline($1);
		}elsif ($command =~ /POWER\s+([0-1])/){
			$SYSTEM_ON = $1;
			if($SYSTEM_ON == 0){
				&turnOffAll();
			}
		}elsif ($command =~ /ACTIVE_PLAYLIST\s+([\w0-9]*)/){
			openPlaylist($1);	
		}elsif ($command =~ /CHANGE_LIGHT/){
			print "Previewing\n";
			$SYSTEM_ON = 0;
			for(my $address = 0; $address < $NUM_LIGHTS; $address ++){
				$color = <COMMAND_FILE>;
				if($color =~ /([0-9]+)\,([0-9]+)\,([0-9]+)\,([0-9]+)/){
					print "$1, $2, $3, $4 found\n";
					&sendColor($address,$1,$2,$3,$4);
				}
				
			}
		
			
		}
		close(COMMAND_FILE);
		unlink($fullname);
	  
	}
	
}






#printf("sequence 0, address 0 R= %d G=%d B=%d\n",$sequence[0][2][0],$sequence[0][2][1],$sequence[0][2][2]);


# do {
#   print "parent checking \n";
#   $kid = waitpid(-1, WNOHANG);
#   print "kid was $kid\n";
# } while($kid>0);
sub forkFade
{
	if( waitpid(-1,WNOHANG ) >= 0){
		return 0;
	}
	
	my $pid = fork();

	if($pid){
		print "Parent here\n";
	}elsif($pid == 0) {
		&fadeColor();
		print "Changed color!\n";
		exit(0);
	}else {
		die "couldn’t fork: $!\n";
	}	
	
	for(my $address = 0; $address < $NUM_LIGHTS; $address ++){
		my ($r,$g,$b,$v)= @{$sequence[$sequenceIndex][$address]};
		@{$oldRgb[$address]} = ($r,$g,$b,$v);
	}
	
	return 1;
}


sub fadeColor {
	
	my $maxSteps = 0;	
	# for($address = 0; $address < $NUM_LIGHTS; $address ++){
	# 	my $curMax = abs(&maxDiff(@{$oldRgb[$address]},@{$sequence[$sequenceIndex][$address]}));
	# 	if($curMax > $maxSteps){
	# 		$maxSteps = $curMax;
	# 	}
	# }
	
	my @timing= @{$sequence[$sequenceIndex][$NUM_LIGHTS]};
	
	$fadeTime = @timing[1];
	my $sleepTime = @timing[0];
	
	print "sleep = $sleepTime fade = $fadeTime\n";
	
	$maxSteps = int(($fadeTime*15)/1000 );

	if($maxSteps == 0){
		$maxSteps = 1;
	}

	my $timePerSlot = $fadeTime / $maxSteps;
	my $ms = $timePerSlot;
	print "$maxSteps $ms\n";
	for(my $address = 0; $address < $NUM_LIGHTS; $address ++){
		push(@{$timeslotSlope[$address]},findSlope(@{$oldRgb[$address]},@{$sequence[$sequenceIndex][$address]},$maxSteps));
	}
	
	for(my $x = 0; $x <=$maxSteps ; $x++){
		#print("starting step $x\n");
		for(my $address = 0; $address < $NUM_LIGHTS ; $address ++){
			my ($r,$g,$b,$v)= @{$oldRgb[$address]};
		
			 my $writeRed = &interpolate($timeslotSlope[$address][0],$x,$r);
			 my $writeGreen = &interpolate($timeslotSlope[$address][1],$x,$g);
			 my $writeBlue =  &interpolate($timeslotSlope[$address][2],$x,$b);
		 	 my $writeViolet = &interpolate($timeslotSlope[$address][3],$x,$v);
		 	&sendColor($address,$writeRed,$writeGreen,$writeBlue,$writeViolet);
		}
		usleep(20000);	
	}
	undef(@timeslotSlope);
	
	sleep($sleepTime);
}


while(1){
	
	usleep(20000);
	if($openSequence_flag){ #Either we have a new playlist or the current sequence just ended
		$openSequence_flag = 0; #clear open timeline flag
		&openTimeline($playlist[$playlistIndex]);
		$playlistIndex++;
		if($playlistIndex == $playlistLength){ #wrap around 
			$playlistIndex = 0;
		}
	}
	&handleCommands();
	if($SYSTEM_ON == 1){
		if(&forkFade()) {
			$sequenceIndex ++;
			if($sequenceIndex == $sequenceLength){
				$sequenceIndex = 0;
				$openSequence_flag = 1;
			}
		}
	}
}

