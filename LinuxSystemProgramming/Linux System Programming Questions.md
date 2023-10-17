Linux System Programming Questions


#include<unistd.h>
#include<stdio.h>

int globvar = 10;
char buff[] = "write to std out buff";

int main(int gc, void *av)
{
   int autovar = 88;
   pid_t pid;

   if(write(STDOUT_FILENO, buff, sizeof(buff)-1))
        printf("error writing to STDOUT\n");

    printf("before fork\n");

    if((pid = fork()) < 0) {
        printf("fork error\n");
    } else if(pid == 0) {   /* child process */
        ++globvar;
        ++autovar;
    } else {
        sleep(2);
    }

    printf("pid = %d, global = %d, autoval = %d\n", pid, globvar, autovar);
    return 0;
}

=================================================================================

#include<unistd.h>
#include<stdio.h>
#include<sys/wait.h>
#include<stdlib.h>

int main(int gc, void *av)
{
    int childstatus;
    int k = 10;
    pid_t pid;

    printf("My pid = %d\n", getpid());
    getchar();

    if((pid = fork()) < 0) {
        printf("fork error\n");
    } else if(pid == 0) {   /* child process */
        printf("child : My pid = %d\n", getpid());
        k = 44;
        printf("end of child  k = %d\n", k);
        exit(100);
    } else {
        /* Parent */
        printf("parent : My pid = %d\n", getpid());
        printf("parent : pid of newly created child = %d\n", pid);
        printf("parent : value of k = %d\n", k);
        wait(&childstatus);
        printf("end of parent  k = %d\n", k);
        printf("child status = %d\n", childstatus);
        printf("Child Status = %d\n", WIFEXITED(childstatus));
    }
    return 0;
}

=================================================================================

#include<unistd.h>
#include<sys/types.h>
#include<stdio.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<signal.h>

#define CHILD 0

void signal_handler_for_child_exit(int signum) {
    // Control reaches here, if child exits normally
    printf("Inside signal handler of child exiting normally with signum = %d\n", signum);
}

int main(void)
{
    pid_t pid;
    int childstatus;


    pid = fork();
    if(pid == CHILD) {
        // child process
        printf("child pid = %d\n", getpid());
        sleep(10);  /* sleep for 10 seconds before exiting */
        printf("Child process exiting after 10 seconds.\n");
        exit(1);
    } else {
        // parent process
        signal(SIGCHLD, signal_handler_for_child_exit);
        wait(&childstatus);
        printf("parent : child status = %d", childstatus); // control never reaches here if user presses ctrl+c
        if(WIFSIGNALED(childstatus)){  // checking if child process terminated due to a signal
            printf("child was killed due to uncaught signal (normal termination)\n");
        }
    }
}


=================================================================================

#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include <fcntl.h>


#define CHILD 0

int main(void)
{
    pid_t pid;
    int fd;
    char buff[2];
    int childstatus;


    fd = open("./dummy_data.txt", O_RDONLY);
    if(fd == NULL) {
        printf("Unable to open file, exiting program");
        exit(1);
    }
        
    pid = fork();
    if(pid == CHILD) {
        // child process
        read(fd, buff, sizeof(buff));
        printf("child : buff[0] : %c\n", buff[0]);
        printf("child : buff[1] : %c\n", buff[1]);
        close(fd);      // both parent and child have to close respective file handles
    } else {
        // parent process
        wait(&childstatus);     // wait for child to finish before reading file in parent
        read(fd, buff, sizeof(buff));
        printf("parent : buff[0] : %c\n", buff[0]);
        printf("parent : buff[1] : %c\n", buff[1]);
        close(fd);      // both parent and child have to close respective file handles
    }

}

=================================================================================

#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>        // needed for wait()
#include<sys/types.h>       // needed for pid_t
#include<fcntl.h>           // needed for O_RDONLY in open()


#define CHILD 0

int main(void)
{
    pid_t pid;


    pid = fork();
    if(pid == CHILD) {
        /* CHILD process */
        printf("child : pid = %d\n", getpid());
        exit(0);
    } else {
        /* PARENT process */
        printf("parent : pid = %d\n", getpid());
        /* since we are not "wait"-ing for child, child becomes a zombie...
            this can be checked via another shell
            $ ps -ef 
            child pid process should show zombie / defunct against it 

            sample output 
            =============
                8549 pts/3    S+     0:00      |   |   \_ ./test SHELL=/bin/bash COLORTERM=truecolor TERM_PROGRAM_VERSION=1.80.1 GIT_PS1_SHOWUPSTREAM=auto GIT_PS1_SHOWDIRTYSTATE=1
                8550 pts/3    Z+     0:00      |   |       \_ [test] <defunct>
        */
       sleep(50);
    }
}
=================================================================================

#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>        // needed for wait()
#include<sys/types.h>       // needed for pid_t
#include<fcntl.h>           // needed for O_RDONLY in open()


#define CHILD 0
/* Example 1 of Orphan process */
int main(void)
{
    pid_t pid = -1;

    pid = fork();

    if(pid > 0){
        printf("In parent process, %d", getpid());
    }

    if(pid == CHILD){
        sleep(30);
        printf("In child process, %d", getpid());
    }

    return 0;
}


=================================================================================
// Q: What would be PID of parent process (PPID) in child and why? 
#include<stdio.h>
#include<stdlib.h>          // needed for exit()
#include<unistd.h>
#include<sys/wait.h>        // needed for wait()
#include<sys/types.h>       // needed for pid_t
#include<fcntl.h>           // needed for O_RDONLY in open()


#define CHILD 0

int main(void)
{
    /* Orphan child process example - Parent exits first before child exits */
    pid_t pid = -1;

    pid = fork();

    if(pid == CHILD) {
        /* Child process */
        printf("CHILD: I am child process (%d), my parent process is (%d)\n", getpid(), getppid());
        sleep(2);
        printf("CHILD: I should be an orphan by now...\n");
        printf("CHILD: My parent has exited, my ppid now is = %d\n ", getppid());   /* ppid now becomes 1 - INIT process id */
        printf("CHILD: I am exiting now\n");

        exit(EXIT_SUCCESS);
    } else {
        /* Parent process */
        /* sleep for a second, so that child has time to get parent's 
            pid, before parent exits */
        sleep(1);
        printf("PARENT: I am parent process (%d)\n", getpid());
        printf("PARENT: I am out of here child, you are onn your own...\n");
        exit(EXIT_SUCCESS);
    }

}

=================================================================================
// Q: What would be the output of read() in child process??
#include<stdio.h>
#include<stdlib.h>          // needed for exit()
#include<unistd.h>
#include<sys/wait.h>        // needed for wait()
#include<sys/types.h>       // needed for pid_t
#include<fcntl.h>           // needed for O_RDONLY in open()


#define CHILD 0

int main(void)
{
    int fd = 10;
    pid_t pid = -1;
    char buff[2];
    int child_status = -1;

    pid = fork();
    if(pid == CHILD) {
        /* child */
        sleep(4);
        read(fd, buff, sizeof(buff));
        perror("child: ");  /* prints "bad file descriptor error" */
        printf("in child: buff[0] = %c\n", buff[0]);
        printf("in child: buff[1] = %c\n", buff[1]);
        close(fd);
    } else {
        /* parent */
        fd = open("./test", O_RDONLY);
        printf("parent waiting for child to finish...\n");
        wait(&child_status);
        read(fd, buff, sizeof(buff));
        printf("in child: buff[0] = %c\n", buff[0]);
        printf("in child: buff[1] = %c\n", buff[1]);
        close(fd);
    }

    return 0;
}

=================================================================================
// Q : value of "global" variable in parent? And why ?
// CHILD: value of global = 10
// PARENT: value of global = 1
#include<stdio.h>
#include<stdlib.h>          // needed for exit()
#include<unistd.h>
#include<sys/wait.h>        // needed for wait()
#include<sys/types.h>       // needed for pid_t
#include<fcntl.h>           // needed for O_RDONLY in open()


#define CHILD 0

int global;

int main(void)
{
    pid_t pid = -1;
    global = 1;
    int child_status;
    pid = fork();

    if(pid == CHILD) {
        global = 10;
        printf("CHILD: value of global = %d\n", global);
    } else {
        wait(&child_status);
        printf("PARENT: value of global = %d\n", global);
    }

    exit(EXIT_SUCCESS);
}


=================================================================================
// Q : What is (Program Break). 
// A: Program Break is the address in memory just after the  un-initialized data(BSS) ie. end of data segment

// Q : What would be the output of following program (assume some values for sbrk(0) (Program Break). 
// A: Hint : after program execution, value of Program break of parent is equal to Child's program break value before malloc.
// After malloc , child's program break value is incremented by the amount of heap memory allocated. 

#include<stdio.h>
#include<stdlib.h>          // needed for exit()
#include<unistd.h>
#include<sys/wait.h>        // needed for wait()
#include<sys/types.h>       // needed for pid_t
#include<fcntl.h>           // needed for O_RDONLY in open()

#define CHILD 0

static int func(int arg) {
    int i;
    printf("Child : Program break is at %p before malloc...\n", sbrk(0));

    for(i = 0; i < 0x100; i++) {
        if(malloc(0x800) == 0) {
            perror("malloc error");
            exit(1);
        }
    }

    printf("Child : Program break is at %p after malloc...\n", sbrk(0));
}

int main(void)
{
    pid_t pid = -1;
    int child_status;
    int arg = 0;

    printf("Parent : Program break at begining of program is at %p...\n", sbrk(0));

    pid = fork();
    switch(pid) {
        case -1: /* fork error occured */
        	perror("fork");
        	exit(1);
        case 0: /* Child Process */
            exit(func(0));  /* pass arg = 0 */
            break;
        default: 
            wait(&child_status);
            break;
    }

    printf("Parent : Program break at end of the program is at %p...\n", sbrk(0));
}



=================================================================================
// Q : What would be the value of lseek() and O_APPEND flag in parent process (please note lseek() has modified the cursor to 1000 bytes in file and fcntrl()
// A: Value of cursor modified in child is seen even in parent, 
// however, when tested on Raspberry pi Linux (Linux sudhanshu 6.1.21-v8+ #1642 SMP PREEMPT Mon Apr  3 17:24:16 BST 2023 aarch64 GNU/Linux),  file flags modified in child were not reflected in parent.
/* |  803  ~/src/linux_programs 
└─▶ $ ./test 
Child has exited.
Parent : File offset is 1000
Parent : O_APPEND flag in parent is turned off */

#include<stdio.h>
#include<stdlib.h>          // needed for exit()
#include<unistd.h>
#include<sys/wait.h>        // needed for wait()
#include<sys/types.h>       // needed for pid_t
#include<fcntl.h>           // needed for O_RDONLY in open()

#define CHILD 0


int main(void)
{
    pid_t pid = -1;
    int child_status, flags, fd;
    /* template file name has to end with 6X's - XXXXXX */
    char template[] = "./templateXXXXXX";

    fd = mkstemp(template);
    if(fd == -1) {
        perror("mkstemp");
        exit(1);
    }

    pid = fork();
    switch(pid) {
        case -1: /* fork error occured */
        	perror("fork");
            exit(1);
        case 0: /* Child Process */
            /* Change file offset in child to 1000 and status flags */
            if(lseek(fd, 1000, SEEK_SET) == -1) {
                perror("lseek");
                exit(1);
            }
            
            flags = fcntl(fd, F_GETFL); // Fetch current file flags
            if(flags == -1) {
                perror("fcntl");
                exit(1);
            }

            flags |= O_APPEND; // change / modify file flags
            if(fcntl(fd, F_SETFL) == -1) {
                perror("fnctl - F_SETFL");
                exit(1);
            }
            break;
        default: 
            wait(&child_status);
            printf("Child has exited.\n");
            printf("Parent : File offset is %lld\n", (long long)lseek(fd, 0, SEEK_CUR));
            /* Lets test if parent can check flag changes done in child */
            flags = fcntl(fd, F_GETFL);
            printf("Parent : O_APPEND flag in parent is turned %s\n", (flags & O_APPEND)? "on" : "off");

            break;
    }
}

=================================================================================
// Q : Output of following program?
// A : Infinite prints of "Can you see me [ONE]". Program respawns itself continuously and from the new program loaded again respawns itself
// code will never reach this point - Can you see me [TWO]

#include<stdio.h>
#include<stdlib.h>          // needed for exit()
#include<string.h>          // needed for strcpy() 
#include<unistd.h>
#include<sys/wait.h>        // needed for wait()
#include<sys/types.h>       // needed for pid_t
#include<fcntl.h>           // needed for O_RDONLY in open()

#define CHILD 0

char *argstochild[10];
char *envtochild[10];

void prepare_args(){
    argstochild[0] = (char *)malloc(20);
    strcpy(argstochild[0], "./progname");
    argstochild[1] = (char *)malloc(20);
    strcpy(argstochild[1], "param1");
    argstochild[2] = (char *)malloc(20);
    strcpy(argstochild[2], "param2");
    argstochild[3] = NULL;
}

void prepare_env() {
    envtochild[0] = (char *)malloc(20);
    strcpy(envtochild[0], "env1=value1");
    envtochild[1] = (char *)malloc(20);
    strcpy(envtochild[0], "env2=value2");
    envtochild[2] = NULL;
}

int main(void)
{
    printf("Can you see me [ONE]\n");
    prepare_args();
    prepare_env();
    execve("./test", argstochild, envtochild);

    printf("Can you see me [TWO]\n");

}



=================================================================================
// Q : How many times do you see "Embedded Systems" printed on console.
// A : first fork creates two thread, 2nd fork() creates 2x2 = 4 threads, finally 3rd fork() will create 4x2 = 8 threads, and each thread prints "Embedded Systems". Hence the printf() is invoked 8 times.

#include<stdio.h>
#include<stdlib.h>          // needed for exit()
#include<string.h>          // needed for strcpy() 
#include<unistd.h>
#include<sys/wait.h>        // needed for wait()
#include<sys/types.h>       // needed for pid_t
#include<fcntl.h>           // needed for O_RDONLY in open()

#define CHILD 0


int main(void)
{
    fork();
    fork();
    fork();
    printf("Embedded Systems\n");

}


=================================================================================
// Stack Frames section 
#include<stdio.h>
// #include<stdlib.h>          // needed for exit()
// #include<string.h>          // needed for strcpy() 
// #include<unistd.h>
// #include<sys/wait.h>        // needed for wait()
// #include<sys/types.h>       // needed for pid_t
// #include<fcntl.h>           // needed for O_RDONLY in open()

#define CHILD 0

void func2(){
    int a  = 20;
    printf("In func2 : a = %d\n", a);
}

void func1(){
    int a  = 10;
    func2();
    printf("In func1 : a = %d\n", a);

}

int main(void)
{
    int a  = 30;
    register int w = 4;

    func1();
    printf("In main : a = %d\n", a);
    printf("Value of w = %d", w);


    return 0;
}

=================================================================================
// Seg Fault example for coredump analysis

#include<stdio.h>
// #include<stdlib.h>          // needed for exit()
// #include<string.h>          // needed for strcpy() 
// #include<unistd.h>
// #include<sys/wait.h>        // needed for wait()
// #include<sys/types.h>       // needed for pid_t
// #include<fcntl.h>           // needed for O_RDONLY in open()

#define CHILD 0


int main(void)
{
    int *p = NULL;
    *p = 10;        // This is invalid statement

    printf("Value of pointer p is = %d", *p);

    return 0;
}

=================================================================================
// Error code / string example 

#include<stdio.h>
#include<stdlib.h>              // needed for exit(), malloc()
#include<string.h>              // needed for strcpy() , strerror()
// #include<unistd.h>
// #include<sys/wait.h>        // needed for wait()
// #include<sys/types.h>       // needed for pid_t
#include<fcntl.h>           // needed for O_RDONLY in open()
#include <errno.h>              // needed for errno

#define CHILD 0



int main(void)
{
    int fd;
    fd = open("./hello.txt", O_RDWR);
    if(fd < 0) {
        printf("File could not be opened. Error code : %d\n", errno);
        printf("File could not be opened. Error string : %s\n", strerror(errno));
    } else {
        printf("File could be openend\n");
        close(fd);
    }
}

=================================================================================

// Thread ID example (using syscall() - directly invoking system call)
#include<stdio.h>
#include<stdlib.h>              // needed for exit(), malloc()
#include<string.h>              // needed for strcpy() , strerror()
#include<unistd.h>              // getpid()
// #include<sys/wait.h>         // needed for wait()
// #include<sys/types.h>        // needed for pid_t
#include <sys/syscall.h>        // syscall(system_call_number) - directly calling system call
#include <fcntl.h>              // needed for O_RDONLY in open()
#include <errno.h>              // needed for errno
#include <ctype.h>              // tolower()



#define CHILD 0

int main() {
    printf("Process ID : %d \n", getpid());
    printf("Thread ID : %ld \n", syscall(SYS_gettid));

    return 0;
}

=================================================================================

// Simple Pthread (POSIX Thread) example code
// gcc -g test.c -o test -lpthread

#include<stdio.h>               // printf()
#include<stdlib.h>              // needed for exit(), malloc()
#include<string.h>              // needed for strcpy() , strerror()
#include<unistd.h>              // getpid()
// #include<sys/wait.h>         // needed for wait()
// #include<sys/types.h>        // needed for pid_t
#include <sys/syscall.h>        // syscall(system_call_number) - directly calling system call
#include <fcntl.h>              // needed for O_RDONLY in open()
#include <errno.h>              // needed for errno
#include <ctype.h>              // tolower()
#include <pthread.h>            // pthread_create(), pthread_t

void *entry_func(void *arg) {
    printf("Hello world...\n");
    return NULL;
}


int main()
{
    pthread_t tid;

    pthread_create(&tid, NULL, entry_func, NULL);
    while(1);   // loop forever

}

=================================================================================

// Pthread (POSIX Thread) code to get parent and child thread id(s)
// gcc -g test.c -o test -lpthread

#include<stdio.h>               // printf()
#include<stdlib.h>              // needed for exit(), malloc()
#include<string.h>              // needed for strcpy() , strerror()
#include<unistd.h>              // getpid()
// #include<sys/wait.h>         // needed for wait()
// #include<sys/types.h>        // needed for pid_t
#include <sys/syscall.h>        // syscall(system_call_number) - directly calling system call
#include <fcntl.h>              // needed for O_RDONLY in open()
#include <errno.h>              // needed for errno
#include <ctype.h>              // tolower()
#include <pthread.h>            // pthread_create()


void *entry_func(void *arg) {
    printf("Hello world...\n");

    printf("[New Thread] Process ID : %d \n", getpid());
    printf("[New Thread] Thread ID : %ld \n", syscall(SYS_gettid));
    return NULL;
}


int main()
{
    pthread_t tid;

    printf("[Main Thread] Process ID : %d \n", getpid());
    printf("[Main Thread] Thread ID : %ld \n", syscall(SYS_gettid));

    pthread_create(&tid, NULL, entry_func, NULL);

    while(1);   // loop forever
}

=================================================================================

// Pthread (POSIX Thread) code to to pass multiple parameters to thread entry function
// gcc -g test.c -o test -lpthread

#include<stdio.h>               // printf()
#include<stdlib.h>              // needed for exit(), malloc()
#include<string.h>              // needed for strcpy() , strerror()
#include<unistd.h>              // getpid()
// #include<sys/wait.h>         // needed for wait()
// #include<sys/types.h>        // needed for pid_t
#include <sys/syscall.h>        // syscall(system_call_number) - directly calling system call
#include <fcntl.h>              // needed for O_RDONLY in open()
#include <errno.h>              // needed for errno
#include <ctype.h>              // tolower()
#include <pthread.h>            // pthread_create()


struct test {
    int a;
    int b;
};

void *entry_func(void *arg) {
    struct test *ptr = arg;

    printf("Inside entry function with params ptr->a = %d and ptr->b = %d\n", ptr->a, ptr->b);

    return NULL;
}

int main()
{
    pthread_t tid;
    struct test myTestParam = {1,2};

    pthread_create(&tid, NULL, entry_func, &myTestParam);

    while(1);   // loop forever
}

=================================================================================
// Pthread (POSIX Thread) pthread_join() example
// gcc -g test.c -o test -lpthread


#include<stdio.h>               // printf()
#include<stdlib.h>              // needed for exit(), malloc()
#include<string.h>              // needed for strcpy() , strerror()
#include<unistd.h>              // getpid()
// #include<sys/wait.h>         // needed for wait()
// #include<sys/types.h>        // needed for pid_t
#include <sys/syscall.h>        // syscall(system_call_number) - directly calling system call
#include <fcntl.h>              // needed for O_RDONLY in open()
#include <errno.h>              // needed for errno
#include <ctype.h>              // tolower()
#include <pthread.h>            // pthread_create()


#define CHILD 0

void *vpThreadFunc(void *arg) {
    sleep(1);
    printf("Inside new thread...\n");
    return NULL;
}

int main() {

    pthread_t tid;

    printf("In main thread...\n");
    printf("Before Thread create...\n");
    pthread_create(&tid, NULL, vpThreadFunc, NULL);

    /* pthread_join blocks the main thread, 
        till the thread function is done executing */
    pthread_join(tid, NULL);

    printf("After Thread create...\n");

    return 0;
}


=================================================================================
// Pthread (POSIX Thread) pthread example of passing argument and returning value from thread
// gcc -g test.c -o test -lpthread
#include<stdio.h>               // printf()
#include<stdlib.h>              // needed for exit(), malloc()
#include<string.h>              // needed for strcpy() , strerror()
#include<unistd.h>              // getpid()
// #include<sys/wait.h>         // needed for wait()
// #include<sys/types.h>        // needed for pid_t
#include <sys/syscall.h>        // syscall(system_call_number) - directly calling system call
#include <fcntl.h>              // needed for O_RDONLY in open()
#include <errno.h>              // needed for errno
#include <ctype.h>              // tolower()
#include <pthread.h>            // pthread_create()


#define CHILD 0

void *vpThreadFunc(void *arg) {
    char *arg_input = arg;
    sleep(1);
    printf("Inside new thread...\n");
    printf("Argument passed to thread = %s\n", arg_input);
    return (void *)strlen(arg_input);
}

int main() {

    pthread_t tid;
    void *thread_rv;

    printf("In main thread...\n");
    printf("Before Thread create...\n");
    pthread_create(&tid, NULL, vpThreadFunc, "hello world");

    /* pthread_join blocks the main thread, 
        till the thread function is done executing */
    pthread_join(tid, &thread_rv);
    printf("Thread returned = %d\n", (long)thread_rv);
    printf("After Thread exit...\n");

    return 0;
}

=================================================================================
// pthread_join : try to join self thread. not possible 
// Output : 
// ┌─ User:sudhanshu   Host:sudhanshu   Date:Sat Aug 19   Time:10:54:20   
// |  Git:     Branch:   Tag:  
// |  936  ~/src/linux_programs 
// └─▶ $ g./test 
// pthread_join error = 35
// pthread_join error string = Resource deadlock avoided
// main:8061

#include<stdio.h>               // printf()
#include<stdlib.h>              // needed for exit(), malloc()
#include<string.h>              // needed for strcpy() , strerror()
#include<unistd.h>              // getpid()
// #include<sys/wait.h>         // needed for wait()
// #include<sys/types.h>        // needed for pid_t
#include <sys/syscall.h>        // syscall(system_call_number) - directly calling system call
#include <fcntl.h>              // needed for O_RDONLY in open()
#include <errno.h>              // needed for errno
#include <ctype.h>              // tolower()
#include <pthread.h>            // pthread_create()


#define CHILD 0



int main() {
    int ret;

    if((ret = pthread_join(pthread_self(), NULL)) != 0){
        printf("pthread_join error = %d\n", ret);
        printf("pthread_join error string = %s\n", strerror(ret));
    }

    printf("%s:%d\n", __func__, getpid());    
    return 0;
}


=================================================================================
// pthread_kill example : should not be called from main thread.
// Output : 
// ┌─ User:sudhanshu   Host:sudhanshu   Date:Sat Aug 19   Time:11:40:31   
// |  Git:     Branch:   Tag:  
// |  940  ~/src/linux_programs 
// └─▶ $ g./test 
// Hi, I am thread 0
// Hi, I am thread 1
// Hi, I am thread 2
// Hi, I am thread 3

#include<stdio.h>               // printf()
#include<stdlib.h>              // needed for exit(), malloc()
#include<string.h>              // needed for strcpy() , strerror()
#include<unistd.h>              // getpid()
// #include<sys/wait.h>         // needed for wait()
// #include<sys/types.h>        // needed for pid_t
#include <sys/syscall.h>        // syscall(system_call_number) - directly calling system call
#include <fcntl.h>              // needed for O_RDONLY in open()
#include <errno.h>              // needed for errno
#include <ctype.h>              // tolower()
#include <pthread.h>            // pthread_create()


#define CHILD 0

void *print(void *arg){
    int *input = (int *)arg;
    printf("Hi, I am thread %d\n", *input);
    pthread_exit(NULL);
}

int main()
{
    #define NUM_OF_THREADS  4
    pthread_t tid[NUM_OF_THREADS];
    int val[NUM_OF_THREADS] = {0,};
    int i;
    void *retvalue;

    for(i = 0; i < NUM_OF_THREADS; i++) {
        val[i] = i;
        pthread_create(&tid[i], NULL, print, &val[i]);
    }

    // Caution : this should NOT be done. 
    // pthread_exit() is to be called from newly created threads and not from main thread.
    // else the pthread_join() below shall not be invoked.
    pthread_exit(NULL);

    for(i = 0; i < NUM_OF_THREADS; i++) {
        printf("Trying to join thread id %d\n", i);
        pthread_join(tid[i], &retvalue);
    }


}

=================================================================================
// Question : calling exit() from thread , instead of pthread_exit(). what will happen? 
// process will exit or single thread? 
// Answer : whole program (process) will terminate. and threads will be 
#include<stdio.h>               // printf()
#include<stdlib.h>              // needed for exit(), malloc()
#include<string.h>              // needed for strcpy() , strerror()
#include<unistd.h>              // getpid()
// #include<sys/wait.h>         // needed for wait()
// #include<sys/types.h>        // needed for pid_t
#include <sys/syscall.h>        // syscall(system_call_number) - directly calling system call
#include <fcntl.h>              // needed for O_RDONLY in open()
#include <errno.h>              // needed for errno
#include <ctype.h>              // tolower()
#include <pthread.h>            // pthread_create()


#define CHILD 0

void *print(void *arg){
    int *input = (int *)arg;
    printf("Hi, I am thread %d\n", *input);
    
    /* Calling exit() instead of pthread_exit(), what will happen? <<-----------
        Thread will be exited or complete process? */
    //pthread_exit(NULL);
    exit(0);
}

int main()
{
    #define NUM_OF_THREADS  4
    pthread_t tid[NUM_OF_THREADS];
    int val[NUM_OF_THREADS] = {0,};
    int i;
    void *retvalue;

    for(i = 0; i < NUM_OF_THREADS; i++) {
        val[i] = i;
        pthread_create(&tid[i], NULL, print, &val[i]);
    }

    for(i = 0; i < NUM_OF_THREADS; i++) {
        printf("Trying to join thread id %d\n", i);
        pthread_join(tid[i], &retvalue);
    }


}


=================================================================================
// Q : which is faster ? Fork creation or Thread creation?
// Lets time the two programs below and find out : 

// fork program execution time (5000 instances)
// real    0m6.432s
// user    0m3.287s
// sys     0m2.483s

// pthread_create program execution time (5000 instances) --> FASTER
// real    0m2.731s
// user    0m0.369s
// sys     0m2.805s



// Program 1 : fork()
#include<stdio.h>               // printf()
#include<stdlib.h>              // needed for exit(), malloc()
#include<string.h>              // needed for strcpy() , strerror()
#include<unistd.h>              // getpid()
#include<sys/wait.h>         // needed for wait(), wait pid
#include <sys/types.h>        // needed for pid_t
#include <sys/syscall.h>        // syscall(system_call_number) - directly calling system call
#include <fcntl.h>              // needed for O_RDONLY in open()
#include <errno.h>              // needed for errno
#include <ctype.h>              // tolower()
#include <pthread.h>            // pthread_create()


#define CHILD 0

#define NUMBER_OF_CREATIONS 5000

static int thread_id = 0;
void child_process()
{
    int i = 0;
    i++;
    exit(EXIT_SUCCESS);
}

int main()
{
    int i, status;
    pid_t pid;

    for(i = 0; i < NUMBER_OF_CREATIONS; i++){
        pid = fork();
        switch(pid){
            case -1:
                printf("fork failed...\n");
                break;
            case CHILD: // child
                child_process();
                break;
            default:    // parent
                waitpid(pid, &status, 0);
        };
    }   // for

    exit(EXIT_SUCCESS);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Program 2 : pthread_create() (5000 instances)
#include<stdio.h>               // printf()
#include<stdlib.h>              // needed for exit(), malloc()
#include<string.h>              // needed for strcpy() , strerror()
#include<unistd.h>              // getpid()
#include<sys/wait.h>         // needed for wait(), wait pid
#include <sys/types.h>        // needed for pid_t
#include <sys/syscall.h>        // syscall(system_call_number) - directly calling system call
#include <fcntl.h>              // needed for O_RDONLY in open()
#include <errno.h>              // needed for errno
#include <ctype.h>              // tolower()
#include <pthread.h>            // pthread_create()


#define CHILD 0

#define NUMBER_OF_CREATIONS 5000

void *print(void *arg)
{
    int *param = (int *)arg;
    printf("Thread number %d\n", *param);
    pthread_exit(NULL);
}


int main()
{
    int i;
    int val[NUMBER_OF_CREATIONS] = {0,};
    pthread_t tid[NUMBER_OF_CREATIONS];

    for(i = 0; i < NUMBER_OF_CREATIONS; i++)
    {
        val[i] = i;
        pthread_create(&tid[i], NULL, print, &val[i]);
    }

    for(i = 0; i < NUMBER_OF_CREATIONS; i++)
    {
        pthread_join(tid[i], NULL);
    }


}

=================================================================================

// Question : Matrix 1024x1024 multiplication over several cores
// c[][] = a[][] x b[][]
#include<stdio.h>               // printf()
#include <pthread.h>            // pthread_create()

#define N   1024        // number of rows / columns in matrix
int a[N][N], b[N][N], c[N][N];
#define N_CORES     4   // adjusted as per system its running on

void *entry_func(void *arg) 
{   
    int *p = (int *)arg;
    int i = 0, j = 0, k = 0;

    for(i = *p; i < (*p + N/N_CORES); i++) {
        for(j = 0; j < N; j++) {
            for(k = 0; k < N; k++) {
                c[i][j] = c[i][j] + a[i][k] * b[k][j]; 
            }
        }
    }

    pthread_exit(NULL);
}

int main()
{
    int i = 0, j = 0;
    int row[N_CORES];
    pthread_t tid[N_CORES];

    // initialize matrices 'a' and 'b'
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            a[i][j] = 2;
            b[i][j] = 3;
        }
    }

    // create a thread per row
    for(i = 0; i < N_CORES; i++) {
        row[i] = i * (N / N_CORES);
        pthread_create(&tid[i], NULL, entry_func, &row[i]);
    }

    // pthread_join for all the threads created.
    for(i = 0; i < N_CORES; i++) {
        pthread_join(tid[i], NULL);
    }

    pthread_exit(NULL);
}


=================================================================================
// Question : how to know thread id of thread from within the thread handler
// Answer : pthread_self() will return thread id of currently running thread.
// Output : 
// ┌─ User:sudhanshu   Host:sudhanshu   Date:Sun Aug 20   Time:08:50:30   
// |  Git:     Branch:   Tag:  
// |  997  ~/src/linux_programs 
// └─▶ $ ./test 
// main: Thread id of newly created thread = 548088922560
// entry_func: Thread id of newly created thread = 548088922560

#include<stdio.h>               // printf()
#include <pthread.h>            // pthread_create()

void *entry_func(void *arg) 
{   
    printf("entry_func: Thread id of newly created thread = %lu\n", pthread_self());
    pthread_exit(NULL);
}

int main()
{
    pthread_t tid;

    pthread_create(&tid, NULL, entry_func, NULL);
    printf("main: Thread id of newly created thread = %lu\n", tid);

    pthread_join(tid, NULL);        // wait for newly created thread to complete
    pthread_exit(NULL);
}


=================================================================================
// pthread_self() usage example to identify which thread is executing
#include<stdio.h>               // printf()
#include <pthread.h>            // pthread_create()

// thread ids for two threads that are created.
#define NUM_OF_THREADS  2
pthread_t tid[NUM_OF_THREADS];

void *entry_func(void *arg) 
{   
    if(pthread_equal(tid[0], pthread_self()))
        printf("Thread 1 is executing\n");
    else
        printf("Thread 2 is executing\n");

    pthread_exit(NULL);
}

int main()
{
    for(int i = 0; i < NUM_OF_THREADS; i++){
        // create two threads
        pthread_create(&tid[i], NULL, entry_func, NULL);
        printf("Thread %d created\n", i+1);
    }
 
    for(int i = 0; i < NUM_OF_THREADS; i++){
        // wait for two threads to finish
        pthread_join(tid[i], NULL);
    }

    exit(EXIT_SUCCESS);
    // OR
    pthread_exit(NULL);
}

=================================================================================
// Q : Example of : passing stack variables from one thread to another.
// A : you can pass stack variables from one thread to another, however, 
//     you need to be very careful about the lifetime of stack variables
#include<stdio.h>               // printf()
#include <pthread.h>            // pthread_create()

void *func1(void *arg1) {
    int *val = (int *)arg1;
    printf("Value passed from func() to func1() is %d\n", *val);
    pthread_exit(NULL);         // no exit value (NULL)
}

void *func(void *arg) {
    pthread_t tid;
    int val = 20;

    // NOTE : it is ok to pass stack variable in this case, as the 
    // thread func() will be alive (and its stack) till func1() returns
    pthread_create(&tid, NULL, func1, &val);

    // func() is blocked till func1() is returned. If this line is omitted, 
    // we will see random values passed to func1(), since the current thread 
    // is exited by the time pointer to val is passed to func1()
    pthread_join(tid, NULL);    
    pthread_exit(NULL);         // no exit value (NULL)
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, func, NULL);
    pthread_join(tid, NULL);    // no exit status from thread (NULL)  
    pthread_exit(NULL);         // no exit value (NULL)
}

=================================================================================
// Q: Does a joinable thread result in memory leak if not pthread_join() ? 
// check with valgrind
// $ valgrind --tool=memcheck --leak-check=yes ./test_bin
// Output 
// ┌─ User:sudhanshu   Host:sudhanshu   Date:Sun Aug 20   Time:13:17:20   
// |  Git:     Branch:   Tag:  
// |  1013  ~/src/linux_programs 
// └─▶ $ valgrind_memcheck ./test
// ==9862== Memcheck, a memory error detector
// ==9862== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
// ==9862== Using Valgrind-3.16.1 and LibVEX; rerun with -h for copyright info
// ==9862== Command: ./test
// ==9862== 
// Hello world
// ==9862== 
// ==9862== HEAP SUMMARY:
// ==9862==     in use at exit: 272 bytes in 1 blocks
// ==9862==   total heap usage: 7 allocs, 6 frees, 3,072 bytes allocated
// ==9862== 
// ==9862== 272 bytes in 1 blocks are possibly lost in loss record 1 of 1
// ==9862==    at 0x484C164: calloc (vg_replace_malloc.c:760)
// ==9862==    by 0x40118CB: allocate_dtv (dl-tls.c:348)
// ==9862==    by 0x40118CB: _dl_allocate_tls (dl-tls.c:594)
// ==9862==    by 0x4881047: allocate_stack (allocatestack.c:622)
// ==9862==    by 0x4881047: pthread_create@@GLIBC_2.17 (pthread_create.c:660)
// ==9862==    by 0x1088C7: main (test.c:25)
// ==9862== 
// ==9862== LEAK SUMMARY:
// ==9862==    definitely lost: 0 bytes in 0 blocks
// ==9862==    indirectly lost: 0 bytes in 0 blocks
// ==9862==      possibly lost: 272 bytes in 1 blocks
// ==9862==    still reachable: 0 bytes in 0 blocks
// ==9862==         suppressed: 0 bytes in 0 blocks
// ==9862== 
// ==9862== For lists of detected and suppressed errors, rerun with: -s
// ==9862== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)

#include <stdio.h>               // printf()
#include <unistd.h>              // getpid(), sleep()
#include <pthread.h>            // pthread_create()

void *func(void *arg) {
    printf("Hello world\n");
    pthread_exit(NULL);
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, func, NULL);
    sleep(1);
    //pthread_join(tid, NULL);    // will this result in memory leak (thread function not freeing its resources?)
}


// with pthread_join() enabled, there were no memory leaks
// ┌─ User:sudhanshu   Host:sudhanshu   Date:Sun Aug 20   Time:13:25:45   
// |  Git:     Branch:   Tag:  
// |  1025  ~/src/linux_programs 
// └─▶ $ valgrind_memcheck ./test
// ==11443== Memcheck, a memory error detector
// ==11443== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
// ==11443== Using Valgrind-3.16.1 and LibVEX; rerun with -h for copyright info
// ==11443== Command: ./test
// ==11443== 
// Hello world
// ==11443== 
// ==11443== HEAP SUMMARY:
// ==11443==     in use at exit: 0 bytes in 0 blocks
// ==11443==   total heap usage: 7 allocs, 7 frees, 3,072 bytes allocated
// ==11443== 
// ==11443== All heap blocks were freed -- no leaks are possible
// ==11443== 
// ==11443== For lists of detected and suppressed errors, rerun with: -s
// ==11443== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)


=================================================================================
// Q : pthread_detach() example, check if there is any memory leak when 
// thread is in "detached mode" and there is no pthread_join() called

#include <stdio.h>               // printf()
#include <stdlib.h>              // needed for exit(), malloc()
#include <unistd.h>              // getpid(), sleep()
#include <pthread.h>            // pthread_create()


void *func(void *arg){
    // convert "joinable thread" to "detached thread"
    pthread_detach(pthread_self());

    printf("Hello world\n");
    pthread_exit(NULL);
}

int main()
{
    pthread_t tid;

    int ret = pthread_create(&tid, NULL, func, NULL);
    if(ret != 0){
        perror("Thread creation error...");
        exit(0);
    }

    printf("After thread creation in main\n");
    sleep(1);
    return 0;
}

// Output of valgrid mem test - $ valgrind --tool=memcheck --leak-check=yes ./test_bin

// ┌─ User:sudhanshu   Host:sudhanshu   Date:Sun Aug 20   Time:13:34:56   
// |  Git:     Branch:   Tag:  
// |  1027  ~/src/linux_programs 
// └─▶ $ valgrind_memcheck ./test
// ==12191== Memcheck, a memory error detector
// ==12191== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
// ==12191== Using Valgrind-3.16.1 and LibVEX; rerun with -h for copyright info
// ==12191== Command: ./test
// ==12191== 
// Hello world
// After thread creation in main
// ==12191== 
// ==12191== HEAP SUMMARY:
// ==12191==     in use at exit: 0 bytes in 0 blocks
// ==12191==   total heap usage: 7 allocs, 7 frees, 3,072 bytes allocated
// ==12191== 
// ==12191== All heap blocks were freed -- no leaks are possible
// ==12191== 
// ==12191== For lists of detected and suppressed errors, rerun with: -s
// ==12191== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)

=================================================================================
// Q : setting a thread as detached using thread's attribute - example code
// convert "joinable thread" to "detached thread" using "THREAD ATTRIBUTES"

// Valgrind output 
// ┌─ User:sudhanshu   Host:sudhanshu   Date:Sun Aug 20   Time:13:51:19   
// |  Git:     Branch:   Tag:  
// |  1029  ~/src/linux_programs 
// └─▶ $ gvalgrind_memcheck ./test
// ==13276== Memcheck, a memory error detector
// ==13276== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
// ==13276== Using Valgrind-3.16.1 and LibVEX; rerun with -h for copyright info
// ==13276== Command: ./test
// ==13276== 
// Inside entry_func
// After thread created in main
// ==13276== 
// ==13276== HEAP SUMMARY:
// ==13276==     in use at exit: 0 bytes in 0 blocks
// ==13276==   total heap usage: 2 allocs, 2 frees, 1,296 bytes allocated
// ==13276== 
// ==13276== All heap blocks were freed -- no leaks are possible
// ==13276== 
// ==13276== For lists of detected and suppressed errors, rerun with: -s
// ==13276== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)



#include <stdio.h>               // printf()
#include <stdlib.h>              // needed for exit(), malloc()
#include <unistd.h>              // getpid(), sleep()
#include <pthread.h>            // pthread_create()

void *entry_func(void *arg) {
    printf("Inside entry_func\n");
}

int main()
{
    int ret;
    pthread_t tid;
    pthread_attr_t attr;

    // initialize attributes with default values.
    ret = pthread_attr_init(&attr);
    if(ret != 0) {
        perror("Error init attributes");
        exit(1);
    }

    // set attribute to create a detached thread
    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if(ret != 0) {
        perror("Error setting attributes");
        exit(1);
    }

    // now create the actual thread with attributes
    ret = pthread_create(&tid, &attr, entry_func, NULL);
    if(ret != 0) {
        perror("Error creating thread");
        exit(1);
    }

    printf("After thread created in main\n");
    sleep(1);
    return 0;
}


=================================================================================
// Q: clone() is called by both fork() and pthread_create() for creating child process / thread. 
// what are the flags / parameters passed to clone() library call 

// Sample -- fork() program
#include <stdio.h>                  // printf()
#include <unistd.h>              // getpid(), sleep(), fork()

int main(int argc, char *argv[]) 
{
    pid_t pid = 10;

    if(fork()) {
        printf("Hello world from parent!!!\n");
    }
    
    printf("Hello world!!!\n");
}
// $ strace ./fork
//        ==> clone(child_stack=NULL, 
//                  flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, 
//                  child_tidptr=0x7f845e9d10) = 5501


// Sample -- pthread_create() program
#include <stdio.h>                  // printf()
#include <stdlib.h>              // needed for exit(), malloc()
#include <pthread.h>              // pthread_create()

void *thrdfn(void *arg) {
    printf("Hello world from child\n");
}

int main(int argc, char *argv[]) 
{
    pthread_t tid;

    pthread_create(&tid, NULL, thrdfn, NULL);
    pthread_join(tid, NULL);

    printf("Hello world from parent!!!\n");
}


// $ strace ./thread
//      ==> clone(child_stack=0x7fa4e2dac0, 
//                flags=CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_THREAD|CLONE_SYSVSEM|CLONE_SETTLS|CLONE_PARENT_SETTID|CLONE_CHILD_CLEARTID, 
//                parent_tid=[7963], tls=0x7fa4e2e8c0, 
//                child_tidptr=0x7fa4e2e290) = 7963


=================================================================================
// CLONE_VM Flag example - how this flag affects the changes done in parent / child to be visible to child / parent


#define _GNU_SOURCE                // CLONE_VM
#include <stdio.h>                  // printf()
#include <stdlib.h>              // needed for exit(), malloc()
#include <string.h>              // needed for strcpy(), strcat(), strerror()
#include <sys/wait.h>            // needed for wait(), wait pid
#include <sched.h>                  // clone()
#include <signal.h>

#define STACK_SIZE  65536
#define HEAP_SIZE  1024

char *heap = NULL;
int global_value = -1;

static int child_func(void *args) {

    char *buff = (char *)args;
    printf("Child sees buff = %s\n", buff);
    printf("Child sees global_value = %d\n", global_value);
    printf("Child sees heap = %s\n", heap);

    strcpy(buff, "hello from child");
    global_value = 10;
    strcpy(heap, "bye");
    return 0;
}

int main(int argc, char *argv[])
{
    // allocate stack for child task
    char *stack = (char *) malloc(STACK_SIZE);
    unsigned long flags = 0;
    char buff[256];
    int status;

    if(!stack) {
        perror("Failed to allocate memory for stack...");
        exit(1);
    }

    heap = malloc(HEAP_SIZE);
    if(!heap) {
        perror("Failed to allocate memory for heap...");
        exit(2);
    }

    if(argc == 2 && !strcmp(argv[1], "vm")) {
        // CLONE_VM - lets the changes done on either parent / child side to be 
        // visible over the other side 
        // ie. changes done in parent are visible to child and vice versa 
        flags |= CLONE_VM;
    }
        

    strcpy(buff, "hello from parent!!");
    strcpy(heap, "hello");
    global_value = 5;

    if(clone(child_func, stack + STACK_SIZE, flags | SIGCHLD, buff) == -1) {
        perror("Clone");
        exit(1);
    }

    if(wait(&status) == -1) {
        perror("Wait");
        exit(1);
    } 

    printf("Parent : Child exited with status = %d\t", status);
    printf("buff = %s\t", buff);
    printf("global_value = %d\t", global_value);
    printf("Parent heap = %s\n", heap);

    return 0;
}

// Output : 
// ┌─ User:sudhanshu   Host:sudhanshu   Date:Sun Aug 27   Time:16:23:45   
// |  Git:     Branch:   Tag:  
// |  1060  ~/src/linux_programs 
// └─▶ $ ./test 
// Child sees buff = hello from parent!!
// Child sees global_value = 5
// Child sees heap = hello
// Parent : Child exited with status = 0   buff = hello from parent!!      global_value = 5        Parent heap = hello

// ┌─ User:sudhanshu   Host:sudhanshu   Date:Sun Aug 27   Time:16:23:49   
// |  Git:     Branch:   Tag:  
// |  1061  ~/src/linux_programs 
// └─▶ $ ./test vm
// Child sees buff = hello from parent!!
// Child sees global_value = 5
// Child sees heap = hello
// Parent : Child exited with status = 0   buff = hello from child global_value = 10       Parent heap = bye


=================================================================================
// CLONE_FILES Flag example - file descriptor closed in child affects read() in parent


#define _GNU_SOURCE                // CLONE_VM
#include <stdio.h>                  // printf()
#include <stdlib.h>              // needed for exit(), malloc()
#include <string.h>              // needed for strcpy(), strcat(), strerror()
#include <unistd.h>                 // getpid(), sleep(), fork()
#include <sys/wait.h>            // needed for wait(), wait pid
#include <fcntl.h>               // needed for O_RDONLY, O_RDWR in open(), open(), close()
#include <sched.h>                  // clone()
#include <signal.h>

#define STACK_SIZE  65536
#define HEAP_SIZE  1024

char *heap = NULL;
int global_value = -1;
int fd;

static int child_func(void *args) {
    // Note : when called with "files" command line parameter, 
    // the child and parent process are sharing the file descriptor table, 
    // and if child closes any file descriptor then the closed-file-description is not visible in 
    // parent anymore.
    close(fd);
    return 0;
}

int main(int argc, char *argv[])
{
    // allocate stack for child task
    char *stack = (char *) malloc(STACK_SIZE);
    unsigned long flags = 0;
    char buff[256];
    int status;

    // Open file in parent (and close in child and see if parent can 
    // still read from the file)
    fd = open("file.txt", O_RDWR);
    if(fd == -1) {
        perror("failed to open file");
        exit(1);
    }

    if(!stack) {
        perror("Failed to allocate memory for stack...");
        exit(1);
    }

    heap = malloc(HEAP_SIZE);
    if(!heap) {
        perror("Failed to allocate memory for heap...");
        exit(2);
    }

    if(argc == 2 && !strcmp(argv[1], "files")) {
        // CLONE_FILES - lets the changes done to a file on either 
        // parent / child side to be visible over the other side 
        // ie. changes done in file in parent are visible to child and vice-versa 
        flags |= CLONE_FILES;
    }

    strcpy(buff, "hello from parent!!");
    strcpy(heap, "hello");
    global_value = 5;

    if(clone(child_func, stack + STACK_SIZE, flags | SIGCHLD, buff) == -1) {
        perror("Clone");
        exit(1);
    }

    if(wait(&status) == -1) {
        perror("Wait");
        exit(1);
    } 

    printf("Parent : Child exited with status = %d\t", status);
    status = read(fd, buff, 100);                                   // <-- this call fails if child has closed the fd (and child was created with flags=CLONE_FILES)
    if(status < 0){
        perror("Read failed");
        exit(1);
    }
    printf("Parent read : %s\n", buff);
    close(fd);

    return 0;
}

// Output : 
// ┌─ User:sudhanshu   Host:sudhanshu   Date:Sun Aug 27   Time:17:12:05   
// |  Git:     Branch:   Tag:  
// |  1071  ~/src/linux_programs 
// └─▶ $ cat file.txt 
// Hello there, my name is Sudhanshu

// ┌─ User:sudhanshu   Host:sudhanshu   Date:Sun Aug 27   Time:17:11:46   
// |  Git:     Branch:   Tag:  
// |  1070  ~/src/linux_programs 
// └─▶ $ ./test 
// Parent : Child exited with status = 0   Parent read : Hello there, my name is Sudhanshu

// ┌─ User:sudhanshu   Host:sudhanshu   Date:Sun Aug 27   Time:17:11:53   
// |  Git:     Branch:   Tag:  
// |  1070  ~/src/linux_programs 
// └─▶ $ ./test files
// Read failed: Bad file descriptor
// Parent : Child exited with status = 0


=================================================================================
// CLONE_FS Flag example - 
// To showcase that if a child process is created with CLONE_FS flag, any FileSystem 
// related changes done in child are reflected in parent process and vice-versa

#define _GNU_SOURCE                // CLONE_VM, CLONE_FS
#include <stdio.h>                  // printf(), NULL
#include <stdlib.h>              // needed for exit(), malloc()
#include <string.h>              // needed for strcpy(), strcat(), strerror(), strcmp()
#include <unistd.h>                 // getpid(), sleep(), fork(), get_current_dir_name(), chdir()
#include <sys/wait.h>            // needed for wait(), wait pid
#include <sched.h>                  // clone()
#include <signal.h>                 // SIGCHLD

#define STACK_SIZE  65536


static int child_func(void *args) {
    // Get the current directory in child process
    printf("Child: Get current directory name : %s\n", get_current_dir_name());
    
    // Change the directory in child
    chdir("/opt");

    // lets print the cwd after chdir() has been executed in child
    printf("Child: Get current directory name : %s\n", get_current_dir_name());
    return 0;
}

int main(int argc, char *argv[])
{
    // allocate stack for child task
    char *stack = (char *) malloc(STACK_SIZE);
    unsigned long flags = 0;
    char buff[256];
    int status;

    printf("Parent: Get current directory name : %s\n", get_current_dir_name());

    if(!stack) {
        perror("Failed to allocate memory for stack...");
        exit(1);
    }

    if(argc == 2 && !strcmp(argv[1], "fs")) {
        // CLONE_FS 
        flags |= CLONE_FS;
    }

    if(clone(child_func, stack + STACK_SIZE, flags | SIGCHLD, NULL) == -1) {
        perror("Clone");
        exit(1);
    }

    if(wait(&status) == -1) {
        perror("Wait");
        exit(1);
    } 

    // See if the chdir() executed in child process affects cwd in parent 
    // process as well (when executed with "fs" cli option)
    printf("Parent : Child exited with status = %d\t   cwd: %s\n", status, get_current_dir_name());
    return 0;
}


// ╭─ 👨‍💻 sudhanshu   💻 sudhanshu   📅 Tue Sep 05   ⏰ 20:59:18   
// ┊  Git:     Branch:   Tag:  
// ┊  1172  ~/src/linux_programs 
// ╰─▶ $ ./test 
// Parent: Get current directory name : /home/sudhanshu/src/linux_programs
// Child: Get current directory name : /home/sudhanshu/src/linux_programs
// Child: Get current directory name : /opt
// Parent : Child exited with status = 0      cwd: /home/sudhanshu/src/linux_programs

// ╭─ 👨‍💻 sudhanshu   💻 sudhanshu   📅 Tue Sep 05   ⏰ 20:59:29   
// ┊  Git:     Branch:   Tag:  
// ┊  1173  ~/src/linux_programs 
// ╰─▶ $ ./test fs
// Parent: Get current directory name : /home/sudhanshu/src/linux_programs
// Child: Get current directory name : /home/sudhanshu/src/linux_programs
// Child: Get current directory name : /opt
// Parent : Child exited with status = 0      cwd: /opt


=================================================================================
// CLONE_SIGHAND Flag example - 

#define _GNU_SOURCE                // CLONE_VM, CLONE_FS
#include <stdio.h>                  // printf(), NULL
#include <stdlib.h>              // needed for exit(), malloc()
#include <string.h>              // needed for strcpy(), strcat(), strerror(), strcmp()
#include <unistd.h>                 // getpid(), sleep(), fork(), get_current_dir_name(), chdir()
#include <sys/wait.h>            // needed for wait(), wait pid
#include <sched.h>                  // clone()
#include <signal.h>                 // SIGCHLD

#define STACK_SIZE  65536

// Signal Handler triggered when a thread / process is self killed
int catch_signal;
struct sigaction sa;
static void sighandler(int sig){
    printf("Caught Signal %d (%s)\n", sig, strsignal(sig));
}

// Child process 
static int child_func(void *args) {

    signal(catch_signal, sighandler);   // register signal handler
    kill(getpid(), SIGTERM);            // send a kill signal to self
    return 0;
}

int main(int argc, char *argv[])
{
    // allocate stack for child task
    char *stack = (char *) malloc(STACK_SIZE);
    unsigned long flags = 0;
    char buff[256];
    int status;

    // ignore the self-kill signal to parent process
    catch_signal = SIGTERM;
    if(signal(catch_signal, SIG_IGN) == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    if(!stack) {
        perror("Failed to allocate memory for stack...");
        exit(1);
    }

    if(argc == 2 && !strcmp(argv[1], "signal")) {
        // CLONE_SIGHAND - this option allows both parent 
        // and child process to share the signal handler
        flags |= CLONE_SIGHAND;
    }

    // create a child process with CLONE_VM & CLONE_SIGHAND options
    if(clone(child_func, stack + STACK_SIZE, flags | CLONE_VM | SIGCHLD, NULL) == -1) {
        perror("Clone");
        exit(1);
    }

    // wait for child to end
    if(wait(&status) == -1) {
        perror("Wait");
        exit(1);
    } 

    printf("Child exited with status : %d\n", status);
    kill(getpid(), SIGTERM);    // send a kill signal to self
    return 0;
}



// ╭─ 👨‍💻 sudhanshu   💻 sudhanshu   📅 Tue Sep 05   ⏰ 21:54:04   
// ┊  Git:     Branch:   Tag:  
// ┊  1183  ~/src/linux_programs 
// ╰─▶ $ ./test 
// Caught Signal 15 (Terminated)
// Child exited with status : 0

// ╭─ 👨‍💻 sudhanshu   💻 sudhanshu   📅 Tue Sep 05   ⏰ 21:57:26   
// ┊  Git:     Branch:   Tag:  
// ┊  1183  ~/src/linux_programs 
// ╰─▶ $ ./test signal
// Caught Signal 15 (Terminated)
// Child exited with status : 0
// Caught Signal 15 (Terminated)


=================================================================================== Quiz

Q1. Output for following program ?? 
—————————————————————————————————————————————
#include <stdio.h>                   // printf(), NULL
#include <pthread.h>              // pthread_create()

void *func(void *cntx) {
    int ret;
    pthread_exit(&ret);

    // <-- this line is not executed, nothing is printed as thread exits from line above
    printf("%d\n", ret); 
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    void *res;

    if(pthread_create(&tid, NULL, func, NULL) != 0)
        perror("pthread_create");
    
    if(pthread_join(tid, &res) != 0)
        perror("pthread_join");

    return 0;
}
—————————————————————————————————————————————
// Answer : Nothing is printed as the pthread_exit() is invoked before printf() and 
// thread exits from pthread_exit() call itself and no other line beyond pthread_exit() 
// is executed.

===================================================================================



Q2. Output for following program ?? 
—————————————————————————————————————————————

#include <stdio.h>                   // printf(), NULL
#include <pthread.h>              // pthread_create()

void *func(void *cntx)
{
    printf("LSP\n");
    pthread_exit("Bye");
}

int main(int argc, char *argv[])
{
    pthread_t tid;

    if(pthread_create(&tid, NULL, func, NULL) != 0)
        perror("pthread_create");

    return 0;
}

—————————————————————————————————————————————
// Answer :  Depends on scheduler, if child executes first, then “LSP” is printed, 
// else if parent is executed first and exited then child also gets exited and hence 
// child doesn’t get CPU cycles to execute and no “LSP” is printed in that case.

===================================================================================


Q2. Output for following program ?? 
—————————————————————————————————————————————


#include <stdio.h>                   // printf(), NULL
#include <pthread.h>              // pthread_create()

// int a = 20;         // global definition 
void *func(void *cntx) {

    printf("a = %d\n", a);
    pthread_exit("Bye");

}

int main(int argc, char *argv[])
{
    pthread_t tid;
    int a = 10;       // local definition
    void *res;

    if(pthread_create(&tid, NULL, func, NULL) != 0)
        perror("pthread_create");

    if(pthread_join(tid, &res) != 0)
        perror("pthread_join");

    return 0;
}

—————————————————————————————————————————————
// Answer :  In case of local variable, the code won’t compile, as it gives undeclared 
// error in child thread function - func().
// However, if variable ‘a’ is declared as global, then child process will 
// print value of ‘a’ as 20.

===================================================================================


Q3. Output for following program ?? 


#include <stdio.h>                   // printf(), NULL
#include <pthread.h>              // pthread_create()

int a;         // global definition 
void *func(void *cntx) {
    printf("a = %d\n", a);
    pthread_exit("Bye");
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    void *res;

    a = 10;       // modifying global variable here.
    if(pthread_create(&tid, NULL, func, NULL) != 0)
        perror("pthread_create");

    if(pthread_join(tid, &res) != 0)
        perror("pthread_join");

    return 0;
}

—————————————————————————————————————————————
// Answer :  Since the child thread is created with flag CLONE_VM, so variables in 
// parent are visible in child process as well. 
// Hence, when a variable that is visible to both parent and child is modified in child variable.
// Hence, output would be - “10”

===================================================================================


Q4. Output for following program ?? 
—————————————————————————————————————————————


#include <stdio.h>                   // printf(), 
#include <unistd.h>               // getpid(), sleep(), fork(), get_current_dir_name(), chdir()
#include <pthread.h>              // pthread_create()


void *func(void *cntx) {
    printf("child: getpid() = %d\n", getpid());
    pthread_exit("Bye");
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    void *res;


    if(pthread_create(&tid, NULL, func, NULL) != 0)
        perror("pthread_create");

    if(pthread_join(tid, &res) != 0)
        perror("pthread_join");

    printf("parent: getpid() = %d\n", getpid());

    return 0;
}


—————————————————————————————————————————————
// Answer :  both parent and child thread will return same process ID (main process contains parent thread)

// ╭─ 👨‍💻 sudhanshu   💻 sudhanshu   📅 Thu Sep 07   ⏰ 21:33:14   
// ┊  Git:     Branch:   Tag:  
// ┊  1167  ~/src/linux_programs 
// ╰─▶ $ ./test 
// child: getpid() = 31079
// parent: getpid() = 31079


=================================================================================

Q5. Output for following program ?? 



#include <stdio.h>                   // printf(), NULL
#include <unistd.h>               // getpid(), sleep(), fork(), get_current_dir_name(), chdir()
#include <fcntl.h>                // needed for O_RDONLY, O_RDWR in open(), open(), close()
#include <pthread.h>              // pthread_create()

int fd;
void *func(void *cntx) {
    char buff[100];

    int read_count = read(fd, buff, sizeof(buff));
    printf("%d\n", read_count);
    printf("%s\n", buff);
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    void *res;

    // open a file to read 
    fd = open("./test.c", O_RDONLY);
    if(fd < 0) {
        perror("file open");
        return -1;
    }

    if(pthread_create(&tid, NULL, func, NULL) != 0)
        perror("pthread_create");

    if(pthread_join(tid, &res) != 0)
        perror("pthread_join");

    return 0;
}

—————————————————————————————————————————————
// Answer :  since threads are created via system call clone(), with flags set 
// as CLONE_FILES, the file descriptors are visibile between parent and child thread. 
// Hence the contents of file could be read via FD created in parent thread.

// ╭─ 👨‍💻 sudhanshu   💻 sudhanshu   📅 Sun Sep 10   ⏰ 20:59:07   
// ┊  Git:     Branch:   Tag:  
// ┊  1170  ~/src/linux_programs 
// ╰─▶ $ g./test 
// 100

// // #define _GNU_SOURCE                  // CLONE_VM, CLONE_FS
// #include <stdio.h>   



=================================================================================

Q6. Output for following program ?? 

#include <stdio.h>                // printf(), NULL
#include <unistd.h>               // getpid(), sleep(), fork(), get_current_dir_name(), chdir()
#include <fcntl.h>                // needed for O_RDONLY, O_RDWR in open(), open(), close()
#include <pthread.h>              // pthread_create()


void *func(void *cntx) {
    pthread_exit("BYE");
    printf("LSP\n");
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    void *res;

    if(pthread_create(&tid, NULL, func, NULL) != 0)
        perror("pthread_create");

    if(pthread_join(tid, &res) != 0)
        perror("pthread_join");

    // try to print result returned by child thread
    printf("%s\n", res);
    
    return 0;
}

—————————————————————————————————————————————
// Answer :  since we are returning "BYE" from child thread, the same is captured 
// in parent thread in "res" variable. And thus printed in parent thread

// ╭─ 👨‍💻 sudhanshu   💻 sudhanshu   📅 Sun Sep 10   ⏰ 21:08:09   
// ┊  Git:     Branch:   Tag:  
// ┊  1172  ~/src/linux_programs 
// ╰─▶ $ g./test 
// BYE




=================================================================================

Q7. Output for following program ?? 


#include <stdio.h>                   // printf(), NULL
#include <unistd.h>               // getpid(), sleep(), fork(), get_current_dir_name(), chdir()
#include <fcntl.h>                // needed for O_RDONLY, O_RDWR in open(), open(), close()
#include <pthread.h>              // pthread_create()

void *func(void *cntx) {
    sleep(1);
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    void *res;


    if(pthread_create(&tid, NULL, func, NULL) != 0)
        perror("pthread_create");

    if(pthread_join(tid, &res) != 0)
        perror("pthread_join");

    // try to print result returned by child thread
    printf("%s\n", res);

    return 0;
}


—————————————————————————————————————————————
// Answer :  results in SEGMENTATION FAULT. 
// Since there is no return value from child thread [ie. no call to pthread_exit()], 
// and we try to print the value pointed by "void *res" which isn't initialized yet. 
// So incorrect memory access results in SEGFAULT


================================================================================= Thread Attributes
// Question : Default Attributes of pthread thread (when no attribute is set 
// and default values are considered)


#define _GNU_SOURCE                  // CLONE_VM, CLONE_FS
#include <stdio.h>                   // printf(), NULL
#include <stdlib.h>                  // needed for exit(), malloc()
#include <pthread.h>                 // pthread_create()


void attr_dump(pthread_t thread_id)
{
    pthread_attr_t attr;
    void *stack_addr;
    size_t stack_size, guard_size;
    int thread_state, scheduling_policy;

    /* pthread_getattr_np() 
        gets the attributes of the thread specified in argument 
    */
    pthread_getattr_np(thread_id, &attr);
    printf("Printing default attributes : \n");

    /* Read stack size and stack address */
    pthread_attr_getstack(&attr, &stack_addr, &stack_size);
    printf("Stack addr : %p\n", stack_addr);
    printf("Stack Size : %ld bytes\n", stack_size);

    /* Read if thread state is detached or joinable */
    pthread_attr_getdetachstate(&attr, &thread_state);
    if(thread_state == PTHREAD_CREATE_JOINABLE) {
        printf("Thread State : Joinable\n");
    } else {
        printf("Thread State : Detached\n");
    }
    
    /* Read stack guard size */
    pthread_attr_getguardsize(&attr, &guard_size);
    printf("Guard Size : %ld bytes\n", guard_size);

    /* Read scheduling policy */
    pthread_attr_getschedpolicy(&attr, &scheduling_policy);
    switch (scheduling_policy)
    {
        case SCHED_OTHER:
            printf("Scheduling Policy : SCHED_OTHER (Pre-emptive Fair Share Scheduler) \n");
            break;
        case SCHED_RR:
            printf("Scheduling Policy : SCHED_RR (Static Priority Round Robin Scheduler) \n");
            break;
        case SCHED_FIFO:
            printf("Scheduling Policy : SCHED_FIFO (Static Priority Pre-emptive Cooperative Scheduler) \n");
            break;
        default:
            break;
    };
}

void *func(void *cntx)
{
    pthread_detach(pthread_self()); // change thread state from joinable -> detached
    attr_dump(pthread_self());      // dump attribute settings for current thread

    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    void *res;

    if(pthread_create(&tid, NULL, func, NULL) != 0)
    {
        perror("pthread_create");
        exit(1);
    }

    if(pthread_join(tid, &res) != 0)
    {
        perror("pthread_join");
    }

    pthread_exit(NULL);

    return 0;
}


// Answer : 
// ╭─ 👨‍💻 sudhanshu   💻 sudhanshu   📅 Tue Sep 12   ⏰ 08:12:24   
// ┊  Git:     Branch:   🏷 〘〙  
// ┊  1179  ~/src/linux_programs 
// ╰─▶ $ ./test 
// Printing default attributes : 
// Stack addr : 0x7fab0e0000
// Stack Size : 8388608 bytes
// Thread State : Detached
// Guard Size : 4096 bytes
// Scheduling Policy : SCHED_OTHER (Pre-emptive Fair Share Scheduler) 



=================================================================================
// Question : Default Attributes of pthread thread (when attribute(s) are set 
// and default values are ignored)


#define _GNU_SOURCE                  // CLONE_VM, CLONE_FS
#include <stdio.h>                   // printf(), NULL
#include <stdlib.h>                  // needed for exit(), malloc()
#include <pthread.h>              // pthread_create()


void attr_dump(pthread_t thread_id)
{
    pthread_attr_t attr;
    void *stack_addr;
    size_t stack_size, guard_size;
    int thread_state, scheduling_policy;

    /* pthread_getattr_np() 
        gets the attributes of the thread specified in argument 
    */
    pthread_getattr_np(thread_id, &attr);
    printf("Printing default attributes : \n");

    /* Read stack size and stack address */
    pthread_attr_getstack(&attr, &stack_addr, &stack_size);
    printf("Stack addr : %p\n", stack_addr);
    printf("Stack Size : %ld bytes\n", stack_size);

    /* Read if thread state is detached or joinable */
    pthread_attr_getdetachstate(&attr, &thread_state);
    if(thread_state == PTHREAD_CREATE_JOINABLE) {
        printf("Thread State : Joinable\n");
    } else {
        printf("Thread State : Detached\n");
    }
    
    /* Read stack guard size */
    pthread_attr_getguardsize(&attr, &guard_size);
    printf("Guard Size : %ld bytes\n", guard_size);

    /* Read scheduling policy */
    pthread_attr_getschedpolicy(&attr, &scheduling_policy);
    switch (scheduling_policy)
    {
        case SCHED_OTHER:
            printf("Scheduling Policy : SCHED_OTHER (Pre-emptive Fair Share Scheduler) \n");
            break;
        case SCHED_RR:
            printf("Scheduling Policy : SCHED_RR (Static Priority Round Robin Scheduler) \n");
            break;
        case SCHED_FIFO:
            printf("Scheduling Policy : SCHED_FIFO (Static Priority Pre-emptive Cooperative Scheduler) \n");
            break;
        default:
            break;
    };
}

pthread_t tid;      // thread 1
pthread_t tid1;     // thread 2

void *func(void *cntx)
{
    int ret; 
    if(pthread_equal(pthread_self(), tid)) {
        printf("\nPrinting Thread 1 Contents\n");
    } else {
        // wait for thread 1st to complete, then only start with 2nd thread
        ret = pthread_join(tid, NULL);
        if(ret != 0) {
            perror("Error in joining thread");
            exit(1);
        }
        printf("\nPrinting Thread 2 Contents\n");
    }
    attr_dump(pthread_self());  // print attribute values for both parent and child thread.

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_attr_t attr;

    /* initialize thread attributes with default values */
    if(pthread_attr_init(&attr)) {
        perror("pthread_attr_init");
        exit(1);
    }

    /* Thread 1 creation */
    if(pthread_create(&tid, &attr, func, NULL)) {
        perror("pthread_create");
        exit(1);
    }

    /* Modify attributes for 2nd thread  - thread state & scheduling policy */
    if(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) {
        perror("Error setting state of thread");
        exit(1);
    }

    // NOTE : THIS OPTION WAS NOT WORKING ON RASPBIAN OS ON RPI4
    // WASN'T ABLE TO SET THE SCHEDULING POLICY TO ROUND ROBIN
    if(pthread_attr_setschedpolicy(&attr, SCHED_RR)) {
        perror("Error setting scheduling policy of thread");
        exit(1);
    }

    /* Thread 2 creation */
    if(pthread_create(&tid1, &attr, func, NULL)) {
        perror("pthread_create");
        exit(1);
    } 

    pthread_exit(NULL);
    return 0;
}



// Answer : 
// ╭─ 👨‍💻 sudhanshu   💻 sudhanshu   📅 Tue Sep 12   ⏰ 20:55:42   
// ┊  Git:     Branch:   🏷 〘〙  
// ┊  1226  ~/src/linux_programs 
// ╰─▶ $ ./test

// Printing Thread 1 Contents
// Printing default attributes : 
// Stack addr : 0x7f98406000
// Stack Size : 8388608 bytes
// Thread State : Joinable
// Guard Size : 4096 bytes
// Scheduling Policy : SCHED_OTHER (Pre-emptive Fair Share Scheduler) 

// Printing Thread 2 Contents
// Printing default attributes : 
// Stack addr : 0x7f97c05000
// Stack Size : 8388608 bytes
// Thread State : Detached
// Guard Size : 4096 bytes
// Scheduling Policy : SCHED_OTHER (Pre-emptive Fair Share Scheduler) 


=================================================================================

// Question : Destroying Attributes of pthread thread 
// 
// /* Any thread attribute initialization needs be to be destroyed before exiting the program. 
//     Any threads running with the attributes shall have no effect on already created threads. */
// if(pthread_attr_destroy(&attr)) {
//     perror("pthread_create");
// }


#define _GNU_SOURCE                  // CLONE_VM, CLONE_FS
#include <stdio.h>                   // printf(), NULL
#include <stdlib.h>                  // needed for exit(), malloc()
#include <pthread.h>              // pthread_create()


void attr_dump(pthread_t thread_id)
{
    pthread_attr_t attr;
    void *stack_addr;
    size_t stack_size, guard_size;
    int thread_state, scheduling_policy;

    /* pthread_getattr_np() 
        gets the attributes of the thread specified in argument 
    */
    pthread_getattr_np(thread_id, &attr);
    printf("Printing default attributes : \n");

    /* Read stack size and stack address */
    pthread_attr_getstack(&attr, &stack_addr, &stack_size);
    printf("Stack addr : %p\n", stack_addr);
    printf("Stack Size : %ld bytes\n", stack_size);

    /* Read if thread state is detached or joinable */
    pthread_attr_getdetachstate(&attr, &thread_state);
    if(thread_state == PTHREAD_CREATE_JOINABLE) {
        printf("Thread State : Joinable\n");
    } else {
        printf("Thread State : Detached\n");
    }
    
    /* Read stack guard size */
    pthread_attr_getguardsize(&attr, &guard_size);
    printf("Guard Size : %ld bytes\n", guard_size);

    /* Read scheduling policy */
    pthread_attr_getschedpolicy(&attr, &scheduling_policy);
    switch (scheduling_policy)
    {
        case SCHED_OTHER:
            printf("Scheduling Policy : SCHED_OTHER (Pre-emptive Fair Share Scheduler) \n");
            break;
        case SCHED_RR:
            printf("Scheduling Policy : SCHED_RR (Static Priority Round Robin Scheduler) \n");
            break;
        case SCHED_FIFO:
            printf("Scheduling Policy : SCHED_FIFO (Static Priority Pre-emptive Cooperative Scheduler) \n");
            break;
        default:
            break;
    };
}

pthread_t tid;      // thread 1
pthread_t tid1;     // thread 2

void *func(void *cntx)
{
    int ret; 
    if(pthread_equal(pthread_self(), tid)) {
        printf("\nPrinting Thread 1 Contents\n");
    } else {
        // wait for thread 1st to complete, then only start with 2nd thread
        ret = pthread_join(tid, NULL);
        if(ret != 0) {
            perror("Error in joining thread");
            exit(1);
        }
        printf("\nPrinting Thread 2 Contents\n");
    }
    attr_dump(pthread_self());  // print attribute values for both parent and child thread.

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_attr_t attr;

    /* initialize thread attributes with default values */
    if(pthread_attr_init(&attr)) {
        perror("pthread_attr_init");
        exit(1);
    }

    /* Thread 1 creation */
    if(pthread_create(&tid, &attr, func, NULL)) {
        perror("pthread_create");
        exit(1);
    }

    /* Modify attributes for 2nd thread  - thread state & scheduling policy */
    if(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) {
        perror("Error setting state of thread");
        exit(1);
    }

    // NOTE : THIS OPTION WAS NOT WORKING ON RASPBIAN OS ON RPI4
    // WASN'T ABLE TO SET THE SCHEDULING POLICY TO ROUND ROBIN
    if(pthread_attr_setschedpolicy(&attr, SCHED_RR)) {
        perror("Error setting scheduling policy of thread");
        exit(1);
    }

    /* Thread 2 creation */
    if(pthread_create(&tid1, &attr, func, NULL)) {
        perror("pthread_create");
        exit(1);
    } 

    /* Any thread attribute initialization needs be to be destroyed before exiting the program. 
        Any threads running with the attributes shall have no effect on already created threads. */
    if(pthread_attr_destroy(&attr)) {
        perror("attribute_destroy failed");
    }

    pthread_exit(NULL);
    return 0;
}


=================================================================================
// Question : pthread_yield() - is used to relinquish the present thread (calling thread), 
// and give CPU to other threads. Current thread is pushed back to the READY queue and 
// next available thread is picked from the READY queue and given CPU cycles.


#define _GNU_SOURCE                  // CLONE_VM, CLONE_FS
#include <stdio.h>                   // printf(), NULL
#include <stdlib.h>                  // needed for exit(), malloc()
#include <pthread.h>              // pthread_create()

pthread_t tid[2];

void *func_t1(void *args)
{
    pthread_yield();
    perror("pthread_yield");

    printf("Thread 1 in execution\n");
    pthread_exit(NULL);
}

void *func_t2(void *args) 
{
    printf("Thread 2 in execution\n");
    pthread_exit(NULL);
}


int main(int args, char *argv[]) 
{
    if(pthread_create(&tid[0], NULL, func_t1, NULL) == 0) {
        printf("Thread 1 created successfuly..\n");
    } else {
        printf("Thread 1 creation failed\n");
    }

    if(pthread_create(&tid[1], NULL, func_t2, NULL) == 0) {
        printf("Thread 2 created successfuly..\n");
    } else {
        printf("Thread 2 creation failed\n");
    }

    pthread_exit(NULL);
}

// Answer : 
// ╭─ 👨‍💻 sudhanshu   💻 sudhanshu   📅 Tue Sep 12   ⏰ 21:34:55   
// ┊  Git:     Branch:   🏷 〘〙  
// ┊  1230  ~/src/linux_programs 
// ╰─▶ $ c./test 
// Thread 1 created successfuly..
// Thread 2 created successfuly..
// Thread 2 in execution                        << Thread 2 starts execution first as thread 1 relinquished itself, then perror message is displayed and finally thread 1 gets CPU cycles to execute.
// pthread_yield: Success
// Thread 1 in execution



=================================================================================
// Question : Can you fork a process with multiple threads.
// OR
// Question : How many THREADs would be there in the following program ? (2 threads or 4 threads)

// Answer : only 2 threads would be there, in the child PROCESS created, the THREADs will not be 
// created again (as the threads were created before the fork() call was made)



#define _GNU_SOURCE                  // CLONE_VM, CLONE_FS
#include <stdio.h>                   // printf(), NULL
#include <stdlib.h>                  // needed for exit(), malloc()
#include <unistd.h>               // getpid(), sleep(), fork(), get_current_dir_name(), chdir()
#include <pthread.h>              // pthread_create()


pid_t child = -1;

void *thread_fn(void *arg) {
    printf("%d: %s starting up...\n", getpid(), (char *)arg);
    while(child == -1)
        sleep(2);
    printf("%d: %s finishing...\n", getpid(), (char *)arg);
    return NULL;
}


int main() 
{
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, thread_fn, "New Thread 1");
    pthread_create(&tid2, NULL, thread_fn, "New Thread 2");

    child = fork();
    printf("%d: %s\n", getpid(), "forking commplete");
    sleep(2);
    printf("%d: %s\n", getpid(), "main thread finished");
    pthread_exit(NULL);
}


// ╭─ 👨‍💻 sudhanshu   💻 sudhanshu   📅 Tue Sep 12   ⏰ 21:56:31   
// ┊  Git:     Branch:   🏷 〘〙  
// ┊  1234  ~/src/linux_programs 
// ╰─▶ $ ./test 
// 22960: New Thread 1 starting up...
// 22960: New Thread 1 finishing...
// 22963: forking commplete
// 22960: forking commplete
// 22960: New Thread 2 starting up...
// 22960: New Thread 2 finishing...
// 22963: main thread finished
// 22960: main thread finished

=================================================================================
// Question : What would be the value of 'glob' variable after two threads are executed.



#include <stdio.h>                   // printf(), NULL
#include <stdlib.h>                  // needed for exit(), malloc()
#include <unistd.h>               // getpid(), sleep(), fork(), get_current_dir_name(), chdir()
#include <pthread.h>              // pthread_create()


int glob = 0;


void *threadFn(void *args)
{
    int loop = *(int *)args;
    int i = 0;
    for(i = 0; i < loop; i++) {
        glob++;
    }
    return NULL;
}

int main()
{
    pthread_t tid1, tid2;
    int loops = 100000;
    int ret = 0;

    ret = pthread_create(&tid1, NULL, threadFn, &loops);
    if(ret != 0)
        perror("pthread_create");

    ret = pthread_create(&tid2, NULL, threadFn, &loops);
    if(ret != 0)
        perror("pthread_create");
    
    ret = pthread_join(tid1, NULL);
    if(ret != 0)
        perror("pthread_join");

    ret = pthread_join(tid2, NULL);
    if(ret != 0)
        perror("pthread_join");

    printf("glob = %d\n", glob);
    return 0;
}

// Answer : Random increment of variable 'glob', since glob++ is not atomic and there would be race condition when two threads would be running....
// On Assembly level "glob++" is divided into three statements 
    // int tmp =  glob
    // tmp = tmp + 1
    // glob = tmp
// During execution, the CPU can prempt these "increment" statements anytime and give cpu cycles to other thread, hence we see an inconsistency in the value of 'glob' variable.
// What we need is "Atomic" operations (provided by GNU) to solve this issue.  

// ╭─ 👨‍💻 sudhanshu   💻 sudhanshu   📅 Sun Sep 17   ⏰ 07:17:05   
// ┊  Git:     Branch:   🏷 〘〙  
// ┊  1183  ~/src/linux_programs 
// ╰─▶ $ ./test
// glob = 159494

// ╭─ 👨‍💻 sudhanshu   💻 sudhanshu   📅 Sun Sep 17   ⏰ 07:17:08   
// ┊  Git:     Branch:   🏷 〘〙  
// ┊  1183  ~/src/linux_programs 
// ╰─▶ $ ./test
// glob = 112590

// ╭─ 👨‍💻 sudhanshu   💻 sudhanshu   📅 Sun Sep 17   ⏰ 07:17:10   
// ┊  Git:     Branch:   🏷 〘〙  
// ┊  1183  ~/src/linux_programs 
// ╰─▶ $ ./test
// glob = 118394

=================================================================================
// Question : What would be the value of 'glob' variable after two threads are executed.


#define _GNU_SOURCE                  // CLONE_VM, CLONE_FS, gettid()
#include <stdio.h>                   // printf(), NULL
#include <stdlib.h>                  // needed for exit(), malloc()
#include <unistd.h>               // getpid(), sleep(), fork(), get_current_dir_name(), chdir()
#include <pthread.h>              // pthread_create()


int glob = 0;


void *threadFn(void *args)
{
    int loop = *(int *)args;
    int i = 0;
    for(i = 0; i < loop; i++) {
        // making increment operation atomic
        __sync_fetch_and_add(&glob, 1); /* gnu atomic op macro */
    }
    return NULL;
}

int main()
{
    pthread_t tid1, tid2;
    int loops = 100000;
    int ret = 0;

    ret = pthread_create(&tid1, NULL, threadFn, &loops);
    if(ret != 0)
        perror("pthread_create");

    ret = pthread_create(&tid2, NULL, threadFn, &loops);
    if(ret != 0)
        perror("pthread_create");
    
    ret = pthread_join(tid1, NULL);
    if(ret != 0)
        perror("pthread_join");

    ret = pthread_join(tid2, NULL);
    if(ret != 0)
        perror("pthread_join");

    printf("glob = %d\n", glob);
    return 0;
}

// Answer : This program eliminates the race condition encountered earlier by using 
// GNU's __sync_fetch_and_add(&glob, 1) atomic api 
// Using __sync_fetch_and_add() , compiler will make sure that the increment operation is completed in one go, 
// by disabling context switch while the atomic instruction is ongoing.


// ╭─ 👨‍💻 sudhanshu   💻 sudhanshu   📅 Mon Sep 18   ⏰ 07:26:11   
// ┊  Git:     Branch:   🏷 〘〙  
// ┊  1186  ~/src/linux_programs 
// ╰─▶ $ ./test 
// glob = 200000

// ╭─ 👨‍💻 sudhanshu   💻 sudhanshu   📅 Mon Sep 18   ⏰ 07:26:14   
// ┊  Git:     Branch:   🏷 〘〙  
// ┊  1187  ~/src/linux_programs 
// ╰─▶ $ ./test 
// glob = 200000

=================================================================================

// Question : How can I create 10 different thread with 10 different starting values.


#define _GNU_SOURCE                  // CLONE_VM, CLONE_FS, gettid()
#include <stdio.h>                   // printf(), NULL
#include <stdlib.h>                  // needed for exit(), malloc()
#include <unistd.h>               // getpid(), sleep(), fork(), get_current_dir_name(), chdir()
#include <pthread.h>              // pthread_create()



void *threadFn(void *args)
{
    int arg = *((int *)args);
    printf("%d: input arg = %d\n", gettid(), arg);
    pthread_exit(NULL);
}

#define THREAD_COUNT    10

int main()
{
    pthread_t tid[THREAD_COUNT] = {0,};
    int i = 0;

    // each thread gets a differnt value of "i" to process
    for(i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&tid[i], NULL, threadFn, &i);
    }

    pthread_exit(NULL);
}

// Output : 
// This program doesn't work properly due to race condition, this is because 
// when the thread is created it doesn't get schecduled / starts running instantly. 
// And since we are passing address of local variable "i" (ie. on stack), the value 
// picked up during execution of the thread would be different as per the then value 
// of "i" on stack. Hence a random value is seen inside child thread rather than the 
// expected incremental value.
// see output section below :

// ╭─ 👨‍💻 sudhanshu   💻 sudhanshu   📅 Mon Sep 18   ⏰ 07:16:35   
// ┊  Git:     Branch:   🏷 〘〙  
// ┊  1182  ~/src/linux_programs 
// ╰─▶ $ ./test 
// 6780: input arg = 1
// 6781: input arg = 3
// 6782: input arg = 3
// 6786: input arg = 7
// 6783: input arg = 7
// 6784: input arg = 7
// 6785: input arg = 7
// 6787: input arg = 8
// 6788: input arg = 9
// 6789: input arg = 10

// ╭─ 👨‍💻 sudhanshu   💻 sudhanshu   📅 Mon Sep 18   ⏰ 07:17:14   
// ┊  Git:     Branch:   🏷 〘〙  
// ┊  1184  ~/src/linux_programs 
// ╰─▶ $ c./test 
// 6954: input arg = 1
// 6955: input arg = 4
// 6960: input arg = 10
// 6957: input arg = 10
// 6956: input arg = 7
// 6959: input arg = 10
// 6958: input arg = 7
// 6961: input arg = 10
// 6962: input arg = 10
// 6963: input arg = 10


=================================================================================
// Question : How can I create 10 different thread with 10 different starting values. [Solution 1]


#define _GNU_SOURCE                  // CLONE_VM, CLONE_FS, gettid()
#include <stdio.h>                   // printf(), NULL
#include <stdlib.h>                  // needed for exit(), malloc()
#include <unistd.h>               // getpid(), sleep(), fork(), get_current_dir_name(), chdir()
#include <pthread.h>              // pthread_create()


void *threadFn(void *args)
{
    int arg = *((int *)args);
    printf("%d: input arg = %d\n", gettid(), arg);
    pthread_exit(NULL);
}

#define THREAD_COUNT    10

int main()
{
    pthread_t tid[THREAD_COUNT] = {0,};
    int i = 0;

    // each thread gets a differnt value of "i" to process
    for(i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&tid[i], NULL, threadFn, &i);
        // approach 1: is to wait for thread to complete before spawning second thread
        pthread_join(tid[i], NULL);
    }

    pthread_exit(NULL);
}

// Answer : one solution is to wait for the thread to complete first, before spawning subsequent threads.

// ╭─ 👨‍💻 sudhanshu   💻 sudhanshu   📅 Mon Sep 18   ⏰ 07:49:38   
// ┊  Git:     Branch:   🏷 〘〙  
// ┊  1190  ~/src/linux_programs 
// ╰─▶ $ c./test 
// 9674: input arg = 0
// 9675: input arg = 1
// 9676: input arg = 2
// 9677: input arg = 3
// 9678: input arg = 4
// 9679: input arg = 5
// 9680: input arg = 6
// 9681: input arg = 7
// 9682: input arg = 8
// 9683: input arg = 9


// ╭─ 👨‍💻 sudhanshu   💻 sudhanshu   📅 Mon Sep 18   ⏰ 07:49:42   
// ┊  Git:     Branch:   🏷 〘〙  
// ┊  1191  ~/src/linux_programs 
// ╰─▶ $ ./test 
// 9723: input arg = 0
// 9724: input arg = 1
// 9725: input arg = 2
// 9726: input arg = 3
// 9727: input arg = 4
// 9728: input arg = 5
// 9729: input arg = 6
// 9730: input arg = 7
// 9731: input arg = 8
// 9732: input arg = 9

=================================================================================







=================================================================================



=================================================================================



=================================================================================




=================================================================================



=================================================================================



=================================================================================




=================================================================================



=================================================================================



=================================================================================



=================================================================================




=================================================================================



=================================================================================



=================================================================================



=================================================================================




=================================================================================



=================================================================================



=================================================================================



=================================================================================




=================================================================================



=================================================================================



=================================================================================
































