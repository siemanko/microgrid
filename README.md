=== The Microgrid Project ===
This is a project that aims to help electrify villages in India. Right now it does not have that many features but it runs quite reliably on a very low cost hardware. In the future we hope to deply much more advanced demand reponse algorithms here. Right now it uses simple hardcoded logic.

=== Target Audience ===
This project is fairly tied to a corresponding TATA Microgrid Project Effort at MIT is hard to be run on boards different than designed by our group. That being said it has a lot of tiny little pieces of code that makes writing programs on embeded hardware a little bit of a headached. 
Some gems include:

[*] cron job scheduler - allows on to schedule functions to be executed every X units of time. It also has quite advanced performance diagnostics
[*] ethermini - minimalistic implementation of Ethernet like protocol that can be run over UART
[*] memory prober - gets free memory on heap.
[*] state of charge estimate - fairly advanced algorithm that estimates state of charge of led acid battery based on in/out current and voltage.
[*] debugger - allows to send printf like debug message to computer over uart. 
[*] simple implementations of basic datastructures - vector, set, circular buffer and map
[*] data logger - fairly advance data logger with arbitrary data logger schemas.


And many more little things.

=== Code organization ===
Code is divided into 5 different folders
[*] comm_board - code for communication board
[*] interface - graphical python interface
[*] link board - code for link board
[*] load board - code for laod board
[*] p33FJ64GS406_drivers - drivers shared between load and link board
[*] shared - shared between all the repos. 

*comm_board - communication*
[*] computer - controls how messages are sent to computer uses ethermini over uart
[*] phone_line - like the above but for messages between comm_boards
[*] interface - unified interface that decides whether to use phone_line of computer communication based on target uid.
[*] messages - defines message handler interface - many message handlers are defined there, but some message handlers are also defined in relevant places in code. set_message_handler is key function here, which recognizes message type by it's first byte.
[*] network utils - to do with pining and getting list of connected nodes.
[*] otherboards/{link,load}board - defines abstract interface for communication with other boards over SPI.

*comm_board - data logger*
 Fairly self explanatory - it's pretty cool, because one only needs to define names and types of pieces of data, functions to get them and the code takes care of the reast - it's quite easy to modify if we want to get different data. 
*comm_board - demand_reponse*
high level logic for demand reponse on both boxes - again fairly self explanatory. Basically we do a kalman filter based on combination of culomb counting and voltage/current based absolute value of soc estimation. soc_interpolation has the hardcore interpolation based on voltage/messasurement. state_of_charge has the Kalman Filter.

*comm_board - drivers*
All the drivers reside here. In theory if you swap out those files the code should work with arbitrary board.

*comm_board - user_interface* 
[*] balance - updates the value of the blanace. arguable belongs more to demand response.
[*] display - controls what's displayed on LCD and how LEDs are blinking.
[*] hindi - some hindi characters

*comm_board - storage*
Describes how things are layed out in EEPROM

*comm_board - constants*
you should REALLY REALLY read that file



*link board*
[*] drivers/pwm - drivers for pwm
[*] api - defines a set of commands that load board supports and tha comm board can issue.
[*] isr - PID controller for network voltage.
[*] reading_model - controls how readings like voltage/current readings are calculated. Most importantly many noisy readings get averaged.


*load board*
[*] adc_interrupt - happens when fresh readings arrive
[*] drivers/pwm - drivers for pwm
[*] api - defines a set of commands that load board supports and tha comm board can issue.
[*] enable_model - controls enable switches for loads.
[*] reading_model - controls how readings like voltage/current readings are calculated. Most importantly many noisy readings get averaged.

*shared*
[*] algorithms - basic datastructures - set, vector, map and circular buffer (not very efficient but simple and working)
[*] communication - mostly home of ethermini and messagebuilder - ehtermini implements checksums for messages and detects frame starts and ends. MessageBuilder helps build messages that contain various data types. There's also utils.h which has a bunch of helpful functions used all over the place.

*p33FJ64GS406_drivers*
[*] adc - analog/digital converter
[*] board - setting up board oscillator etc.
[*] spi slave - slave mode driver for spi communication to talk to comm_board
[*] timer - used for interrupts etc.

=== About User Interface ===
The user interface is slightly chaotic in its current form. By the looks of it you can see exactly the way it was built. Initially it was well thought out and I had ambition to make it run on a tablet, but in the end I run out of time. There's potential to quite easily port it. The only problem is figuring out serial port communication functionality on Android, but from what I know this can be done. The way it works also requires clean up. It should be much smoother to use if we want to leter give it to villagers (I imagine it should be more like an installation flow of configuring a box with each step described in the UI).

=== User Interface functionality ===
For each piece of UI I indicate wether it works form A box only, B box only or both.

* Debug Tab *
[*] clear screen (A,B) - clears logs from the panel on the right.
[*] ping (A,B) - sends a ping to device - if comm board is functioning correctly you should see a "pong" reply.
[*] test LEDs (B) - blinks the green,yellow,red LEDs
[*] print local time (A,B) - prints time in Jamshedpur timezone - it's and effect of conversion from UTC, which ulink uses internally.
[*] read eeprom (A,B) - allows to read a specific address from EEPROM memory and converts it to float, byte or uint32
[*] print data logs (A,B) - instantanously creates and prints out a line with logs in the format they are used in data logger. This command actually queries load/link board directly every time, rather than using cached data.
[*] data logger status (A,B) - prints data logger status - right now it's just number of logs that have been stored since last wipe
[*] wipe data logs (A,B) - removes all the logs from comm board memory - should be used to make space for new logs.
[*] cron stats (A,B) - useful for performance debugging and testing. Shows number of iterations of main loop per second.
To get detailed performance information about every cron function uncomment 

```c
	#define CRON_STATS
```
from constants.h. (It slows down performance, that's why it's best left commented for production).
[*] Probe memory (A,B) - shows the amount of memory left on device's heap. Useful to debug memory leaks (it should not increase with time).
[*] Override DR (A) - force A box to broadcast on a particular demand response state (on - always on, off -always off, green, yellow, red). None removes the override and lets A box autonomously choose the DR state.
[*] Get connected nodes (A) - lists the UIDs of nodes that are reachable via phone line. Refreses every 20 seconds
[*] Factory reset (A,B) - wipes the state of eeprom memory. WARNING: it changes type of box back to B.
[*] Reboot (A,B) - reboots the comm board


* Settings Tab *
Pressing enter is not enough to set the value - you must click buttons. 

[*] Time (A,B) - shows time in the local time zone of computer that is running the python interface. It gets converted from ulinks internal UTC representation. Sync time with computer syncs the UTC time on uLink with UTC time on computer, i.e. if your computer is showing the right time for the right timezone it will do the right thing.
[*] UID and Box type (A,B) - uid should be different on all devices. Box type is either A or B. Changing any of those properties resets the comm board.
[*] Balance (B) - sets the amout of balance left on the device. 
[*] State and uncertainty of charge (A) - set state of charge and uncertainty - uncertainty is in sigma squared. The units are culombs
[*] Battery capacity (A) - total capacity of battery used in the system.
[*] Thresholds from off, red, yellow states (A) - expressed as ratio of state of charge to battery capacity
[*] Balance Update - balance is updated every day. If the first two numbers are say 4 and 57 this means that value of the third number is added to the balance every day at 4:57am 

*Data logs tab*
Pull logs (A,B) - pulls data logs and saves them to UID_logs.txt. Can take some time. 

*Installing on Fedora Linux*
First of all make sure you have installed all the required linux dependencies:

```bash
	yum install mesa-libGL mesa-libGL-devel python3 libev SDL-* libtool
``` 

Then install all the required Python dependencies

```bash
	pip3 install Cython==0.21.2
	pip3 install pyserial pytz kivy
```

Clone the Pygame repo and install it

```bash
	hg clone [pygame url]
	cd pygame
	python3 setup.py install 
```
Finally you are ready to compile serial communication code. Got to microgrid/interface/communication and run

```bash
	make
```
Once the command terminates succesfully you are ready to run the interface. Go to microgrid/interface and type python3 run.py

*Additional steps for virtual machine*
Make sure that virtual machine has at least 2gb of memory and at least 2 core to operate. Download and install virtualbox extensions and in usb settings tick usb 2.0. Also make sure that in usb settings you add the serial port device to the list of forwarded port.

After you install fedora in your VM, take the following steps.

```bash
	yum install kernel-devel kernel-sources dkms
	# In VM menu klick Devices -> cd/dvd/insert guest additions cd. The names below are typed from memory so they might differ slightly.
	cd /run/media/USER/VBOXGUESTADDITIONSCDNAME/
	./VBoxLinuxAdditions
```
Now after you reboot your machine should suck much less. To check if serial port gets forwarded type 

```bash
	lsusb
```

in terminal (try removing and inserting usb and seeing if output changes).

=== Troubleshooting ===
If you get a permission denied error on any command try using 
```bash
	su -c 'command'
```
instead.

=== Known bugs and todos ===
1. Trying to pull data logs from comm board crashes the comm board
2. Flow of time is too slow on comm board.
3. State of chrage estimate is currently not logged
4. Uncertainty for motion and mesurment step should be squuared (uncertainty_of_charge is correctly considered to be squared, but motion and measurement uncertainty are not).
5. button reacts slowly - maybe decrease the number of debounce readings.

=== Security Issues ===
Current Version of ulink is pathetic secruitywise. Below a just few examples:
[*] If one resets device 10 times during the exact minute and hour the blance update happens they will get 10 times the balance update. 

