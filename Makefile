BINDIR=/usr/bin

all:
	gcc pinknoise8bit.c -Wall -Wextra -o pinknoise8bit -Os \
		-fno-stack-protector -fomit-frame-pointer \
		-ffunction-sections -fdata-sections -Wl,--gc-sections 
	strip pinknoise8bit  -S --strip-unneeded \
		--remove-section=.note.gnu.gold-version \
		--remove-section=.comment --remove-section=.note \
		--remove-section=.note.gnu.build-id \
		--remove-section=.note.ABI-tag 
	ls -al pinknoise8bit
clean:
	rm -f pinknoise8bit

install:
	mkdir -p ${DESTDIR}/${BINDIR}
	cp pinknoise8bit ${DESTDIR}/${BINDIR}
