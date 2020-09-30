; ---------------------------------------------------------------------------
; interrupt.s
; ---------------------------------------------------------------------------
;
; Interrupt handler.
;
; Checks for a BRK instruction and returns from all valid interrupts.

.import   _init, _dcf_analyze, _mos6551_rxrb, _mos6551_rxrb_head, _uptime_value, _milliseconds
.export   _irq_int, _nmi_int

M6242_STA = $640D
MC6840_STA = $6101
MC6840_TIMER1 = $6102
MC6840_TIMER2 = $6104
ACIA_RXD = $6000
ACIA_STS = $6001


.segment  "CODE"

.PC02                             ; Force 65C02 assembly mode

; ---------------------------------------------------------------------------
; Non-maskable interrupt (NMI) service routine

_nmi_int:  RTI                    ; Return from all NMI interrupts

; ---------------------------------------------------------------------------
; Maskable interrupt (IRQ) service routine

_irq_int:  PHX                    ; Save X register contents to stack
           TSX                    ; Transfer stack pointer to X
           PHA                    ; Save accumulator contents to stack
           INX                    ; Increment X so it points to the status
           INX                    ;   register value saved on the stack
           LDA $100,X             ; Load status register contents
           AND #$10               ; Isolate B status bit
           BNE break              ; If B = 1, BRK detected

; ---------------------------------------------------------------------------
; IRQ detected, return

irq_chk_acia_rx:
           LDA ACIA_STS
           AND #$80
           BEQ irq_chk_t2
           LDA ACIA_RXD
           LDX _mos6551_rxrb_head
           STA _mos6551_rxrb, X
           INC _mos6551_rxrb_head
irq_chk_t2:
           LDA MC6840_STA         ; Load TIMER status register
           AND #$02                ; Check if TIMER2 IRQ flag is set
           BEQ irq_chk_rtc         ; If flag is cleared, go to the next stage
           LDA MC6840_TIMER2      ; You must read T2 to clear interrupt flag
           LDA MC6840_TIMER2+1
           INC _milliseconds      ; Increment milliseconds variable
irq_chk_t1:
           LDA MC6840_STA         ; Load TIMER status register
           AND #$01               ; Check if TIMER1 IRQ flag is set
           BEQ irq_chk_rtc        ; If flag is cleared, go to the next stage
           LDA MC6840_TIMER1      ; You must read T1 to clear interrupt flag
           TAX					  ; This is MSB, transfer it to X 						
           LDA MC6840_TIMER1+1	  ; This is LSB, it stays in A
           JSR _dcf_analyze		  ; DCF77 being processed here
           LDA #$FF				  ; Reload timer
           STA MC6840_TIMER1	  ; MSB Fir
           STA MC6840_TIMER1 + 1  ; Then LSB      
irq_chk_rtc:           
           LDA M6242_STA	      ; Load RTC status register
           AND #$04                ; Check if IRQ flag is set
           BEQ irq_ret            ; If not, this is not RTC interrupt, so continue
           LDA #$00               ; Otherwise clear flag
           STA M6242_STA
           INC _uptime_value      ; Increment uptime variable
           BNE irq_ret
           INC _uptime_value+1
           BNE irq_ret
           INC _uptime_value+2
           BNE irq_ret
           INC _uptime_value+3
irq_ret:   PLA                    ; Restore accumulator contents
           PLX                    ; Restore X register contents
           RTI                    ; Return from all IRQ interrupts

; ---------------------------------------------------------------------------
; BRK detected, stop

break:     JMP _init              ; If BRK is detected, something very bad
                                   ; has happened, restart
