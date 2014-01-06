/*
   This piece of C code implements a RAM-based logging function in the Linux kernel. 
   It is my first piece of kernel code I developed when I was working on the wirelessly assisted TCP for high-bandwidth data center networks (WSTCP). 
*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/unistd.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/mm.h>
#include <asm/uaccess.h>

int to_print_flush = 0;
module_param(to_print_flush, int, 0000);
MODULE_PARM_DESC(to_print_flush, "0 to print and 1 to flush");
mm_segment_t oldfs;

#define MAX_LOG_CNT 1000000
#define MAX_LOG_LEN 100
extern int log_cnt;
extern char log_buf[MAX_LOG_CNT][MAX_LOG_LEN]; //100000000B < 100MB
extern void flush_log();

struct file *openFile(char *path, int flag, int mode)
{
   struct file *fp;
   fp = filp_open(path, flag, 0);
   if (fp)
      return fp;
   else
      return NULL;
}

int writeFile(struct file *fp, char *buf, int writelen, int writepos)
{
   if (fp->f_op && fp->f_op->write) {
       fp->f_pos = writepos;
       return fp->f_op->write(fp, buf, writelen, &fp->f_pos);
   }
   else {
       return -1;
   }
}

int closeFile(struct file *fp)
{
   filp_close(fp, NULL);
   set_fs(oldfs);
   return 0;
}

void initKernelEnv(void)
{
   oldfs = get_fs();
   set_fs(KERNEL_DS);
}

void fprint_log(struct file *fp)
{
   int i;
   long byte_write = 0;
   for (i = 0; i < log_cnt; i++) {
       if (writeFile(fp, log_buf[i], strlen(log_buf[i]), byte_write) < 0)   {
           printk("ram log print fail at line %d\n", i+1);
           break;
       }
       byte_write += strlen(log_buf[i]);
   }
}

static int __init ram_log_init(void)
{
    struct file *fp;
    if (to_print_flush == 0)  {
        /* init ram log file */
        initKernelEnv();
        fp = openFile("/etc/myconfig", O_RDWR|O_CREAT, 0);
        if (fp != NULL)
            printk("ram log file created\n");
        else
            printk("ram log file creation fails\n");
        
        fprint_log(fp);
        closeFile(fp);
    }
    flush_log();    
    return 0;
}

static void __exit ram_log_exit(void)
{
    printk("ram_log_file module remove successfully\n");
}

module_init(ram_log_init);
module_exit(ram_log_exit);

MODULE_DESCRIPTION("ram log in kernel module"); 
MODULE_AUTHOR("Ke Hong<hongkedavid@gmail.com>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ram log module");
