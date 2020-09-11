;
main:
            mov #vetor1, R4
            mov #vetor2, R5
            mov #vetor2, R10
            call #comp_vec
            jmp $
            nop

comp_vec:
            mov 0(R4), R6 ; n elemtnos v1
            mov 0(R5), R7 ; n elemtnos v2
            incd R4
            incd R5
            mov R4, R8 ; pont v1
            mov R5, R9 ; pont v2

loop_anda:  cmp @R8,0(R9)
            jeq igual
            incd R8
            dec R6
            jz fim
            jmp loop_anda

igual:      dec R7
            jz set_carry
            incd R9
            incd R8
            cmp @R8,0(R9)
            jeq igual
            mov 0(R10), R7
            mov R5,R9
            dec R6
            jmp loop_anda

set_carry:  setc

fim:        ret

        .data
vetor1: .word 8,"ezequiel"
vetor2: .word 3,"eza"