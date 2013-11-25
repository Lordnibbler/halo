arecord --format=S16_LE -r 24000 -D plughw:0,0 -c 1 -t raw | ./lights
