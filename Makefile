# Adjust the run address to match the .org in the source code
all: talking_clock.hex

talking_clock.hex: a.out
	bin2hex.py a.out talking_clock.hex
#	bin2hex.py --offset=0x8000 a.out talking_clock.hex

# config.o
a.out: interrupt.o vectors.o delay.o m6242.o mc6840.o mos6551.o spo256.o parser.o io.o main.o 
	ld65 -C ./lib/ethergeiger.cfg -m main.map interrupt.o vectors.o delay.o m6242.o mc6840.o mos6551.o spo256.o parser.o io.o main.o ./lib/ethergeiger.lib	

main.s: main.c
	cc65 -t none -O --cpu 65c02 main.c
	
main.o: main.s
	ca65 --cpu 65c02 main.s
	
spo256.s: spo256.c
	cc65 -t none -O --cpu 65c02 spo256.c
	
spo256.o: spo256.s
	ca65 --cpu 65c02 spo256.s
	
m6242.s: m6242.c
	cc65 -t none -O --cpu 65c02 m6242.c	

m6242.o: m6242.s
	ca65 --cpu 65c02 m6242.s		

mc6840.s: mc6840.c
	cc65 -t none -O --cpu 65c02 mc6840.c	

mc6840.o: mc6840.s
	ca65 --cpu 65c02 mc6840.s	
	
mos6551.s: mos6551.c
	cc65 -t none -O --cpu 65c02 mos6551.c	

mos6551.o: mos6551.s
	ca65 --cpu 65c02 mos6551.s		

parser.s: parser.c
	cc65 -W -unused-param -t none -O --cpu 65c02 parser.c	

parser.o: parser.s
	ca65 --cpu 65c02 parser.s	
	
io.s: io.c
	cc65 -t none -O --cpu 65c02 io.c	

io.o: io.s
	ca65 --cpu 65c02 io.s	

delay.o: delay.s
	ca65 --cpu 65c02 delay.s
	
interrupt.o: interrupt.s
	ca65 --cpu 65c02 interrupt.s
	
vectors.o: vectors.s
	ca65 --cpu 65c02 vectors.s	
	
clean:
	$(RM) *.o *.lst *.map *.out, *.hex a.out main.s spo256.s m6242.s mc6840.s mos6551.s parser.s io.s
