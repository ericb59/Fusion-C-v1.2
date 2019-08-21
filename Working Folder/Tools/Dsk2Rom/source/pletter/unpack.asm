  module uitpakker

uitpakmode=9

@pakuit
  ld iy,.loop
  ld a,128
  exx
  ld de,1
  exx

.looplit
  ldi
.loop
  add a,a
  jp nz,.hup
  ld a,(hl)
  inc hl
  rla
.hup
  jr nc,.looplit

  exx
  ld l,e
  ld h,d
.getlen
  add a,a
  call z,.getbyteexx
  jr nc,.lenok
  add a,a
  call z,.getbyteexx
  adc hl,hl
  jp nc,.getlen
  exx
  ret
.lenok
  inc hl
  exx

  ld c,(hl)
  inc hl
  ld b,0
  if uitpakmode !=8
  bit 7,c
  jp z,.offsok
  add a,a
  call z,.getbyte
  if uitpakmode !=9
  rl b
  add a,a
  call z,.getbyte
  if uitpakmode !=0
  rl b
  add a,a
  call z,.getbyte
  if uitpakmode !=1
  rl b
  add a,a
  call z,.getbyte
  if uitpakmode !=2
  rl b
  add a,a
  call z,.getbyte
  if uitpakmode !=3
  rl b
  add a,a
  call z,.getbyte
  endif
  endif
  endif
  endif
  endif
  rl b
  add a,a
  call z,.getbyte
  jr nc,.offsok
  or a
  inc b
  res 7,c
.offsok
  endif
  inc bc

  push hl
  exx
  push hl
  exx
  ld l,e
  ld h,d
  sbc hl,bc
  pop bc
  ldir
  pop hl
  jp iy

.getbyte
  ld a,(hl)
  inc hl
  rla
  ret

.getbyteexx
  exx
  ld a,(hl)
  inc hl
  exx
  rla
  ret

  endmodule
