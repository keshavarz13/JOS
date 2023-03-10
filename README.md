# COMP 790: Operating System Implementation

This is my implementation of the labs from [University of North Carolina's operating system graduate class](https://www.cs.unc.edu/~porter/courses/comp790/s20/index.html). (UNC gracefully offers their course material to the public).

The following is quoted from the website.

> The primary objective of this course is to gain a detailed understanding of how computer systems work. For instance, when one types a command at the console, what is the chain of hardware and software events that lead to the command returning the correct value? This deep understanding is of practical and philosophical importance. It is practically immportant to understand how computer systems work when you are trying to make them do something new, either for research or industry. More philosophically, a computer scientist with an advanced degree should not view any part of the computer as "magic," but should either understand how it works or have the tools to figure it out.
>
> This course will focus on implementing key OS kernel features in the JOS kernel. JOS provides skeleton code for much of the less interesting components of the OS, allowing you to focus on key implementation details. The JOS lab was developed at MIT, and has been used at several other universities, including Stanford, Texas, and UCLA.
>
> Lectures and readings in the course will serve to draw out general principles, add needed background for the labs, and map details from the JOS implementation to real-world OSes, like Linux and Windows. In my own experience, most of the mapping is fairly intuitive: once you understand the simple code in JOS, the same pattern is clear in the much more complicated Linux source code.
> 
> The operating system you will build, called JOS, will have Unix-like functions (e.g., fork, exec), but is implemented in an exokernel style (i.e., the Unix functions are implemented mostly as user-level library instead of built-in to the kernel). The major parts of the JOS operating system are:
> * Booting
> * Memory management
> * User environments
>  * Preemptive multitasking
>  * File system, spawn, and shell 
>  * Network driver
>  * Open-ended project
>  
> We will provide skeleton code for pieces of JOS, but you will have to do all the hard work. 

# INSTALLATION

See the lab webpage for the full configuration. 

This open a shell in an environement with the right compiler and qemu.

    make
    make qemu-nox


  