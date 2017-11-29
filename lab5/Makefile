all:  sim bone
	# Native compile 
sim:
	gcc -o metro_server_sim metro_server.c gpio_led_fu_sim.c serve.c -lrt
	gcc -o metro_client_sim metro_client.c key_input_fu.c clien.c -lrt -pthread
bone:        
	arm-linux-gnueabihf-gcc -o metro_server metro_server.c gpio_led_fu.c serve.c -lrt
	gcc -o metro_client metro_client.c key_input_fu.c clien.c -lrt -pthread

