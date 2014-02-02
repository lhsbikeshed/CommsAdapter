==Adapter for keyboard on comms machine==

Requires:
avr-gcc and all that
LUFA
minimus AVR board

Keymapping:
keypad - as expected

top row : rstabcel
bottom row: hgmnopqji   k

leds:
send 4 bytes followed by ',' to set leds. 4bytes = 32 bit big endian number
