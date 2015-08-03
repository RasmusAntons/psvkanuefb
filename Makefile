psvkanuefb: main.c efb.c calendar.c
	gcc `pkg-config --cflags gtk+-3.0 webkit2gtk-4.0` -o psvkanuefb main.c efb.c calendar.c `pkg-config --libs gtk+-3.0 webkit2gtk-4.0`

run: clean psvkanuefb
	if [ -f psvkanuefb ]; then ./psvkanuefb; fi

clean:
	rm -f psvkanuefb
