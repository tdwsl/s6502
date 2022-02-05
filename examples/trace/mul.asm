; multiplies 3 by 9

*=$2000-2
  !word $2000

TempMul = $D000

Start:
  lda #3
  ldx #9
  jsr MulAX

  brk

MulAX:
  sta TempMul
  lda #0

MulAX_0:
  adc TempMul
  dex
  bne MulAX_0

  rts
