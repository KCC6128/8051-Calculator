all: final_main.hex final_main.ihx delay.rel LED_Display.rel Keypad4x4.rel kclib.lib

delay.rel: delay.c
	sdcc -c delay.c

LED_Display.rel: LED_Display.c
	sdcc -c LED_Display.c

Keypad4x4.rel: Keypad4x4.c
	sdcc -c Keypad4x4.c

kclib.lib: delay.rel LED_Display.rel Keypad4x4.rel
	sdar -rs kclib.lib delay.rel LED_Display.rel Keypad4x4.rel
	sdranlib kclib.lib

final_main.ihx: final_main.c kclib.lib
	sdcc final_main.c kclib.lib

final_main.hex: final_main.ihx
	packihx final_main.ihx > final_main.hex
clean:
	del *.hex *.ihx *.lk *.lst *.map *.mem *.rel *.rst *.sym *.asm *.lib
