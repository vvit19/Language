push 2 
pop rax 
call main 
hlt 

main: 
		pop rcx 
		push 0 
		; initialize a 
		pop [rax + 0] 
		push 0 
		; initialize b 
		pop [rax + 1] 
		push 0 
		; initialize c 
		pop [rax + 2] 
		push 0 
		; initialize x_1 
		pop [0] 
		push 0 
		; initialize x_2 
		pop [1] 
		in 
		pop [rax + 0] 
		in 
		pop [rax + 1] 
		in 
		pop [rax + 2] 
		push rcx 
		push [rax + 2] 
		push [rax + 1] 
		push [rax + 0] 
		push rax + 3 
		pop rax 
		call solve_quadratic 
		pop rcx 
		push rax
		push 3
		sub
		pop rax 
		push rbx 
		; initialize n_roots 
		pop [rax + 3] 
		push [rax + 3] 
		out 
		; if: 
		push [rax + 3] 
		push 8 
		jne label_0 
		push 0 
		pop rbx 
		push rcx 
		ret ; function end

		jump label_0 
label_0: 
		; if: 
		push [rax + 3] 
		push 0 
		je label_1 
		push [0] 
		out 
		jump label_1 
label_1: 
		; if: 
		push [rax + 3] 
		push 2 
		jne label_2 
		push [1] 
		out 
		jump label_2 
label_2: 
		push 0 
		pop rbx 
		push rcx 
		ret ; function end

solve_quadratic: 
		pop rcx 
		pop [rax + 0] 
		pop [rax + 1] 
		pop [rax + 2] 
		; if: 
		push [rax + 0] 
		push 0 
		jne label_3 
		push rcx 
		push [rax + 2] 
		push [rax + 1] 
		push rax + 3 
		pop rax 
		call solve_linear 
		pop rcx 
		push rax
		push 3
		sub
		pop rax 
		push rbx 
		; initialize n_roots 
		pop [rax + 3] 
		push [rax + 3] 
		pop rbx 
		push rcx 
		ret ; function end

		jump label_3 
label_3: 
		push [rax + 1] 
		push [rax + 1] 
		mult 
		push 4 
		push [rax + 0] 
		mult 
		push [rax + 2] 
		mult 
		sub 
		; initialize discr 
		pop [rax + 4] 
		; if: 
		push [rax + 4] 
		push 0 
		jb label_4 
		push 0 
		pop rbx 
		push rcx 
		ret ; function end

		jump label_4 
label_4: 
		push 0 
		push [rax + 1] 
		sub 
		push [rax + 4] 
		sqrt 
		add 
		push 2 
		push [rax + 0] 
		mult 
		div 
		; new value of x_1 
		pop [0] 
		; if: 
		push [rax + 4] 
		push 0 
		jne label_5 
		push 1 
		pop rbx 
		push rcx 
		ret ; function end

		jump label_5 
label_5: 
		push 0 
		push [rax + 1] 
		sub 
		push [rax + 4] 
		sqrt 
		sub 
		push 2 
		push [rax + 0] 
		mult 
		div 
		; new value of x_2 
		pop [1] 
		push 2 
		pop rbx 
		push rcx 
		ret ; function end

solve_linear: 
		pop rcx 
		pop [rax + 0] 
		pop [rax + 1] 
		push [rax + 0] 
		push 0 
		jne label_6 
		push [rax + 1] 
		push 0 
		jne label_7 
		push 8 
		pop rbx 
		push rcx 
		ret ; function end

		; if: 
		jump label_8 
		; else: 
		label_7: 
		push 0 
		pop rbx 
		push rcx 
		ret ; function end

		; end if/else 
		jump label_8 
label_8: 
		; if: 
		jump label_9 
		; else: 
		label_6: 
		push 0 
		push [rax + 1] 
		sub 
		push [rax + 0] 
		div 
		; new value of x_1 
		pop [0] 
		push 1 
		pop rbx 
		push rcx 
		ret ; function end

		; end if/else 
		jump label_9 
label_9: 
