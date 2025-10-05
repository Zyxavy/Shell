# Shell
<h3>🐚 Shell in C</h3>
<br>
A simple Unix shell built in C, inspired by Stephen Brennan's tutorial.

<h3>🛠️ About:</h3>
<br>
This project is a minimalist shell implementation that handles basic command execution. It was developed on Windows 10 using WSL (Windows Subsystem for Linux) to compile and run in a Linux environment.

    ⚠️ Note: This shell (lsh) is designed for Linux/Unix systems or WSL. It will not run natively on Windows, as it relies on POSIX system calls that are not supported in the Windows environment.

<h3>🚀 Getting Started</h3>
<br>
To compile and run:
<br>
1. gcc lsh.c -o lsh
<br>
2. ./lsh

Make sure you're using a POSIX-compliant environment like Linux or WSL.

<br>

<h4>📦 current commands/features:</h4>
<ul>
<li>cd - change directory</li>
<li>help - list all commands</li>
<li>exit - exit out of shell</li>
<li>pwd - print current working directory</li>
<li>echo - echo or print your arguments</li>
<li>clear - clear the terminal</li>
<li>history - prints the command history</li>
<li>mkdir - make a directory</li>
<li>rmdir - remove a directory</li>
<li>Piping - (|)</li>
<li>Background Execution - (&)</li>
</ul>

<hr>
<h4> To be implemented </h4>
<ul> 
<li>Aliases ?(alias ll="ls -l")</li>
</ul>
<br>
STATUS = UNFINISHED

📚 Reference
<br>
Original tutorial: "https://brennan.io/2015/01/16/write-a-shell-in-c/"
