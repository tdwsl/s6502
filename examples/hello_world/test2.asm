; print hello world

CHPUT = $F000
Ptr = $BE

*=$3000-2
  !word $3000

Start:
  ldx #<HelloMsg
  ldy #>HelloMsg
  jsr Print

  brk

Print:
  stx Ptr
  sty Ptr+1
  ldy #0

Print_0:
  lda (Ptr),y
  beq Print_1
  iny
  jsr CHPUT
  jmp Print_0

Print_1:
  rts

HelloMsg:
  !text "Hello, world!"
  !byte $0A
