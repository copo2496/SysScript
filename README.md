# SysScript

SysScript is a basic scripting language built around system calls
The basics of the language are set up:

	$ followed by a statement executes a bash command

	example: $ls -l

	call followed by an interrupt number executes a system call

	example: call 0x01

Issues:

	Currently the system calls are being executed in a separate process... 
	Need to implement this differently so it continues on the same process
	using the ASM mechanism in C may work to this end

More features to come

To install simply clone the repository and run the setup script, and then call sysscript from the shell
