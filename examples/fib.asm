push 0 
pop rax 
call main 
hlt 

main: 
		pop rcx 
		push 0 
		; initialize n 
		pop [rax + 0] 
		in 
		pop [rax + 0] 
		push rcx 
		push [rax + 0] 
		push rax + 1 
		pop rax 
		call fib 
		pop rcx 
		push rax
		push 1
		sub
		pop rax 
		push rbx 
		; initialize res 
		pop [rax + 1] 
		push [rax + 1] 
		out 
		push 0 
		pop rbx 
		push rcx 
		ret ; function end

fib: 
		pop rcx 
		pop [rax + 0] 
		; if: 
		push [rax + 0] 
		push 1 
		jne label_0 
		push 1 
		pop rbx 
		push rcx 
		ret ; function end

		jump label_0 
label_0: 
		; if: 
		push [rax + 0] 
		push 2 
		jne label_1 
		push 1 
		pop rbx 
		push rcx 
		ret ; function end

		jump label_1 
label_1: 
		push 1 
		; initialize f_1 
		pop [rax + 1] 
		push 1 
		; initialize f_2 
		pop [rax + 2] 
		push 0 
		; initialize fib 
		pop [rax + 3] 
		push 0 
		; initialize i 
		pop [rax + 4] 
		push [rax + 4] 
		push [rax + 0] 
		push 2 
		sub 
		jb label_2 
		; while 
		jump label_3 
label_3: 
		push [rax + 1] 
		push [rax + 2] 
		add 
		; new value of fib 
		pop [rax + 3] 
		push [rax + 2] 
		; new value of f_1 
		pop [rax + 1] 
		push [rax + 3] 
		; new value of f_2 
		pop [rax + 2] 
		push [rax + 4] 
		push 1 
		add 
		; new value of i 
		pop [rax + 4] 
		push [rax + 4] 
		push [rax + 0] 
		push 2 
		sub 
		jb label_2 
		jump label_3 
label_2: 
		push [rax + 3] 
		pop rbx 
		push rcx 
		ret ; function end

