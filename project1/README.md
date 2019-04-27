# OS 2019 Project 1

## Install System Call

### 1. Setting up build environment
```
$ sudo apt-get update
$ sudo apt-get install gcc vim ssh wget libncurses5-dev libssl-dev libelf-dev bison flex
```
### 2. Getting the source archive
```
$ cd ${HOME}/Documents/
$ wget https://www.kernel.org/pub/linux/kernel/v4.x/linux-4.17.4.tar.xz
```
### 3. Extracting the archive
```
$ sudo tar -xvf linux-4.17.4.tar.xz -C/usr/src/
$ cd /usr/src
```
### 4. Copy our kernel files
```
$ sudo cp /media/sf_OS-Project/project1/kernel_files/sys.c /usr/src/linux-4.17.4/kernel/
```

#### 4.1. What did we add in `/usr/src/linux-4.17.4/kernel/sys.c`
```
$ sudo vim /usr/src/linux-4.17.4/kernel/sys.c
```
```c
#include <linux/ktime.h>
#include <linux/timekeeping.h>
```
```c
SYSCALL_DEFINE1(stephen, char *, msg)
{
    char buf[256];
    long copied = strncpy_from_user(buf, msg, sizeof(buf));
    if (copied < 0 || copied == sizeof(buf))
        return -EFAULT;
    printk(KERN_INFO "%s\n", buf);
    return 0;
}
```
```c
SYSCALL_DEFINE2(my_time, long *, t_sec, long *, t_nsec)
{
    int retval;
    struct timespec t;
    getnstimeofday(&t);
    retval = put_user(t.tv_sec, t_sec);
    if (retval) {
        printk(KERN_ALERT "put_user(t.tv_sec, t_sec) failed.");
    }
    retval = put_user(t.tv_nsec, t_nsec);
    if (retval) {
        printk(KERN_ALERT "put_user(t.tv_nsec, t_nsec) failed.");
    }
    return retval;
}
```

<!-- ### 5. Add new files to `kernel/Makefile`
```
$ sudo vim /usr/src/linux-4.17.4/kernel/Makefile
```
`obj-y += sys_my_time.o`
![](https://i.imgur.com/UNNKqD7.png) -->

<!-- ### 6. Declare the prototype of your system calls
```
$ sudo vim /usr/src/linux-4.17.4/include/linux/syscalls.h
```
```c
asmlinkage int sys_my_time(int isStart, unsigned long *start_sec, unsigned long *start_nsec, unsigned long *end_sec, unsigned long *end_nsec, int *pid);
```
![](https://i.imgur.com/g4sQxag.png) -->

### 5. Register a system call number
```
$ sudo vim /usr/src/linux-4.17.4/arch/x86/entry/syscalls/syscall_64.tbl
```
`333    common            stephen    __x64_sys_stephen`
`334    common            my_time    __x64_sys_my_time`
![](https://i.imgur.com/uvn0Pir.png)
### 6. Compile and install kernel
```
$ cd /usr/src/linux-4.17.4
$ sudo make menuconfig
$ sudo make -j 6
$ sudo make modules_install install
$ reboot
```

## Usage
```
$ cd OS-Project/project1/
$ make clean
$ make
$ make run < testfileFIFO.in && dmesg
```