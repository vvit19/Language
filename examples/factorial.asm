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
		call factorial 
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

factorial: 
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
		push rcx 
		push [rax + 0] 
		push 1 
		sub 
		push rax + 1 
		pop rax 
		call factorial 
		pop rcx 
		push rax
		push 1
		sub
		pop rax 
		push rbx 
		; initialize res 
		pop [rax + 1] 
		push [rax + 0] 
		push [rax + 1] 
		mult 
		pop rbx 
		push rcx 
		ret ; function end

