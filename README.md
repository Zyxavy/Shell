# Shell
Building a simple shell in C following the guide on "https://brennan.io/2015/01/16/write-a-shell-in-c/". Built this on windows 10 using WSL to compile to linux.
<br>
<h2>
Note: This lsh only works on Linux/Unix Systems or WSL, it will not work natively on Windows since POSIX system calls does not exist in Windows.
</h2>
<br>

<h4>current commands:</h4>
<ul>
<li>cd - change directory</li>
<li>help - list all commands</li>
<li>exit - exit out of shell</li>
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
