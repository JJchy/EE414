all:  sq snb algo
# Native compile 
sq: 
	gcc -o test_single_key test_single_key.c
snb: 
	gcc -o test_single_key_nb test_single_key_nb.c
algo:
	gcc -o algo_metro_tui_pc algo_metronome_tui.c
metro:
	arm-linux-gnueabihf-gcc -o metronome_tui_thread_sim metronome_tui_thread.c gpio_led_fu.c key_input_fu.c -pthread

