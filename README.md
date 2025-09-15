# Shell
🐚 Shell in C
A simple Unix shell built in C, inspired by Stephen Brennan's tutorial.

🛠️ About
This project is a minimalist shell implementation that handles basic command execution. It was developed on Windows 10 using WSL (Windows Subsystem for Linux) to compile and run in a Linux environment.

    ⚠️ Note: This shell (lsh) is designed for Linux/Unix systems or WSL. It will not run natively on Windows, as it relies on POSIX system calls that are not supported in the Windows environment.

🚀 Getting Started
To compile and run:

gcc -o lsh lsh.c
./lsh

Make sure you're using a POSIX-compliant environment like Linux or WSL.
</h2>
<br>

<h4>📦 current commands:</h4>
<ul>
<li>cd - change directory</li>
<li>help - list all commands</li>
<li>exit - exit out of shell</li>
<li>pwd - print current working directory</li>
<li>echo - echo or print your arguments</li>
<li>clear - clear the terminal</li>
<li>history - prints the command history</li>
</ul>

<hr>
<h4> To be implemented </h4>
<ul> 
<li><s>pwd, echo, clear</s></li>
<li>Better error handling</li>
<li>Background execution (&)</li>
<li>Redirection (<, >, >>)</li>
<li>Piping (|)</li>
<li>History + history builtin?</li>
<li>Aliases ?(alias ll="ls -l")</li>
<li>Environment variables ($PATH, export)?</li>
<li>Globbing (*.c)?</li>
<li>Job control (fg, bg, jobs)?</li>
<li>Tab completion?</li>
<li>Scripting support?</li>
</ul>
<br>
STATUS = UNFINISHED

📚 Reference
Original tutorial: "https://brennan.io/2015/01/16/write-a-shell-in-c/"
