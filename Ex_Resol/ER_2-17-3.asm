		mov  #8, R5
		call #nib_asc
		jmp $
;
nib_asc:
		mov.b tab_asc(R5), R5
		ret

		 .data
tab_asc: .byte 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
		 .byte 0x38, 0x39, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45
