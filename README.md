# simplerepeater
Simple Radio Repeater with a Link Radio to the Hub Repeater Site

The main purpose of creating this was I had multiple repeater sites. Of course my main hub site, and then three other remote sites. The idea was to provide better coverage since there are lots of hills in my area that block coverage.
Thus the need for the three sites. All of my sites are modified Motorola Maxtrac radios which you can get off ebay for cheap. They are not 100% duty cycle use...so it requires some cooling to increase the duty cycle. External cooling fans, even using expoxy to add heat sink material to improve heat dissipation.

You must modify the radios to bring out a COR signal (active Low to ground), which can be easily accoomplished by using an NPN transistor (e.g. 2n2222 or 2n3904) as an emitter follower setup and front end the base with 4.7Kohm resistor. Collector goes out to one of the unused pins on the RJ45 jack in front of the radio.
For more details, see the repeater builder site @ http://www.repeater-builder.com/motorola/maxtrac/maxtrac-rptr-int.html

This sketch was built using the Xboard. You can use whatever Arduino board you want or have. I just used the Xboard since I had several on the shelf.

Additionally, for cooling, I used a Darylington Pair TIP120 because that is what I had on the shelf. You could use a relay, just make sure you include a snubber diode.

For this revision, it is very simple. It has a global variable you can set for the hang timer for the link radio and for the local repeater. The reason for two separate hang timers is to allow for a little overlap for the hang timer on the hub site.

Additionally, with this setup, all audio inputs/outputs are tied together with AC coupling (1uF capacitor between each radio). This does not allow for audio gain, so you will either have to live with the audio level, or use a potentiometer to adjust. A better way would be to use an OP AMP and build a simple mixer. Since all the radios in  my use case are the same, I'm able to just tie them together with a cap on each feed. Your mileage may vary :-)

The next revision I'll add CWID output.


