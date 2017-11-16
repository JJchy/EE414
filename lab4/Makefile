all:  sim bone
	# Native compile 
sim:
	gcc -o metronome_hrt_sim metronome_hrt.c gpio_led_fu_sim.c key_input_fu.c -lrt
bone:
	arm-linux-gnueabihf-gcc -o metronome_hrt metronome_hrt.c gpio_led_fu.c key_input_fu.c -lrt

