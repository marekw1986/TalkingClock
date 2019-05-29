# Adjust the run address to match the .org in the source code
all: main.hex

main.hex: a.out
	bin2hex.py --offset=0x8000 a.out main.hex

# config.o
a.out: interrupt.o vectors.o delay.o main.o 
	ld65 -C ./lib/ethergeiger.cfg -m main.map interrupt.o vectors.o delay.o main.o ./lib/ethergeiger.lib	

main.s: main.c
	cc65 -t none -O --cpu 65c02 main.c
	
main.o: main.s
	ca65 --cpu 65c02 main.s
	
delay.o: delay.s
	ca65 --cpu 65c02 delay.s
	
interrupt.o: interrupt.s
	ca65 --cpu 65c02 interrupt.s
	
vectors.o: vectors.s
	ca65 --cpu 65c02 vectors.s	
	
clean:
	$(RM) *.o *.lst *.map *.out, *.hex a.out main.s
