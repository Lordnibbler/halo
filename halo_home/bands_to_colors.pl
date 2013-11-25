#!/usr/bin/perl -w
use strict;

# BEGIN {
#     eval {
#         require Audio::PortAudio;
#     } || die "Can't find Audio::PortAudio. 
# If you've build Audio::PortAudio but haven't installed it yet, use
# perl -Mblib vumeter.pl [program options]
# ";
# }
use Getopt::Long;

system("echo 20 > /sys/kernel/debug/omap_mux/uart1_rxd");
system("echo 0 > /sys/kernel/debug/omap_mux/uart1_txd");
system("stty -F /dev/ttyO1 speed 115200 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts");

my $samplefrequency = 24000;
my $updatefrequency = 200;
my $help = 0;
my $avgchar = "#";
my $peakchar = "|";
my $blankchar = " ";
my $verbose=0;
my $channelcount = 1;
my $api_name = "";
my $device_name = "";
my $s = GetOptions(
    "samplefrequency=i",\$samplefrequency,
    "updatefrequency=i",\$updatefrequency,
    "avgchar=s",\$avgchar,
    "peakchar=s",\$peakchar,
    "blankchar=s",\$blankchar,
    "verbose",\$verbose,
    "channelcount=i",\$channelcount,
    "api=s",\$api_name,
    "device=s",\$device_name,
    "help",\$help
);
if (!$s || $help) {
    print "usage: $0 [options]
options:
  --samplefrequency VALUE   audio input rate. default = 22050
  --updatefrequency VALUE   display update rate. default = 40
  --peakchar        CHAR    character to display peak value. default = |
  --avgchar         CHAR    character to display average power. default = #
  --blankchar       CHAR    character to fill. default = ' ' (space)
  --channelcount    NUM     number of input channels. default = 1
  --api             NAME    API to use (ALSA, OSS ...)
  --device          NAME    input device to use
  --verbose                 be verbose
  --help                    show this message

example (default values):
   vumeter.pl -u 40 -s 22050 -p '|' -av '#' -b ' '
";
    exit $s;
}


$|=1;

open(SERIAL, "> /dev/ttyO1");

{
my $previous_default = select(STDOUT);  # save previous default
select(SERIAL);
$|++;                                   # autoflush STDERR, to be sure
select($previous_default);              # restore previous default
}

sub sendColor
{
my($address,$r,$g,$b,$v)= @_;
$address = $address + 1;
print SERIAL "4,$address,$r,$g,$b,$v;";		#	print "4,$address,$r,$g,$b,$v;\n";
}


while (<STDIN>) {
    print "$_";
    print SERIAL "$_";
    # my @peaks = $_ =~ /(\w+),(\w+),(\w+),(\w+),(\w+)/;
    #     
    #     #foreach my $val(@peaks){
    # #      print "val = $val, ";
    # #   }
    #  #   print "\n";
    #     for(my $i = 0; $i < scalar(@peaks);$i++){
    #       $logged = $peaks[$i]? $log10_10 * log($peaks[$i]): 0;
    #       #    print "$logged, ";
    #      my $level = (($logged-10)>=35)? 34: ($logged-10);
    #    if($level <0){
    #       $level = 0;
    #    }
    #    $level = int($level / 3);
    #    if($level >6 ){
    #    }
    #    if($peak_count > 20){
    #       push(@color_order,shift(@color_order));
    #    $peak_count = 0;
    #    }
    #    #print "level = $level\n";
    #    my $address = $address_table[$i];
    # #for(my $address = 0; $address < $NUM_LIGHTS; $address ++){
    #       my($r,$g,$b) =@{$color_array[11-$level][$color_order[$address]]};
    #       #print "$r,$g,$b\n";
    #       &sendColor($address,$r,$g,$b,0);
    # #  }  
    #    }
    #    # print "\n";
}

exit;
