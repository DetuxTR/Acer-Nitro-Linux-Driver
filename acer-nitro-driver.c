

#include "linux/acpi.h"

#include "linux/gfp_types.h"
#include "linux/kern_levels.h"

#include "linux/kobject.h"
#include "linux/kstrtox.h"
#include "linux/printk.h"

#include "linux/slab.h"

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/wmi.h>



#define ZERO "0"
#define CPUFANID "1"
#define GPUFANID "4"
#define WMI_GAMING_GUID "7A4DDFE7-5B5D-40B4-8595-4408E0CC7F56"
#define DEV_NAME "acer-nitro"

MODULE_LICENSE("GPL");
static struct class *cls;
int major;

static void handle_cmd(char * Message);
static acpi_status call_wmi_method(u32 m_id, u64 m_input, u32 *m_output);
static ssize_t device_read(struct file *, char * __user ,size_t ,loff_t *);
static int an_uevent(const struct device *dev,struct kobj_uevent_env *env){
  add_uevent_var(env, "DEVMODE=%#o",0666);
  return 0;
}
static ssize_t device_read(struct file *filp, char *__user buffer,size_t length,loff_t *offset){
  u32 cfanspout[10];
  u32 gfanspout[10];
  call_wmi_method( 17, 1, cfanspout);
  call_wmi_method( 17, 4, gfanspout);
  int bytes_read = 0;
  char msg[22];
  
  snprintf(msg,21,"%d-%d",*cfanspout,*gfanspout);
  const char *msg_ptr = msg;
  printk("%s",msg);
   if (!*(msg+ *offset)) { 
        *offset = 0; 
        return 0; 
    } 
    msg_ptr += *offset;
        while (length && *msg_ptr) { 
        put_user(*(msg_ptr++), buffer++); 
        length--; 
        bytes_read++; 
    } 
    *offset += bytes_read; 
  return bytes_read;
}
static ssize_t device_write(struct file *file, const char *buffer,
                            size_t length, loff_t *offset) {
                              char Message[20];
  int i;
  for (i = 0; i < length && i < 20; i++)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 2, 0)
    get_user(Message[i], buffer + i);
#else
    Message[i] = get_user(buffer + i);
#endif
  printk(KERN_INFO "%s", Message);
  handle_cmd(Message);
  return i;
}
struct file_operations fops = {
  .write = device_write,
  .read= device_read

};

static void create_chardev(void) {
  major = register_chrdev(0, DEV_NAME, &fops);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)

  cls = class_create(DEV_NAME);
  cls->dev_uevent=an_uevent;

#else

  cls = class_create(THIS_MODULE, DEVICE_NAME);

#endif

  device_create(cls, NULL, MKDEV(major, 0), NULL, DEV_NAME);
}
static acpi_status call_wmi_method(u32 m_id, u64 m_input, u32 *m_output) {
  union acpi_object *obj; 
  const struct acpi_buffer input = {(acpi_size)sizeof(u32), &m_input};
  struct acpi_buffer result = {ACPI_ALLOCATE_BUFFER, NULL};
  u32 tmp = 0;
  acpi_status status =
      wmi_evaluate_method(WMI_GAMING_GUID, 0, m_id, &input, &result);
  if (ACPI_FAILURE(status)){
    return status;}
 
  obj=(union acpi_object *)result.pointer;
  if(obj && obj->type == ACPI_TYPE_BUFFER){
    printk(KERN_INFO"im here");
     
      if (obj->buffer.length == sizeof(u32)){
				tmp = *((u32 *) obj->buffer.pointer);}
			if (obj->buffer.length == sizeof(u64)){
				tmp = *((u64 *) obj->buffer.pointer);}
      
  }
  else if(obj && obj->type == ACPI_TYPE_INTEGER){
    tmp = (u64) obj->integer.value;
  }

  if (m_output){
    *m_output =  tmp;
  }
  return status;
}
static int __init module_start(void) {
  printk(KERN_INFO "Module started");
  create_chardev();
  return 0;
}

static void __exit module_end(void) {
  printk(KERN_INFO "Module exit");
  device_destroy(cls, MKDEV(major, 0));
  class_destroy(cls);
  unregister_chrdev(major, DEV_NAME);
}

static void handle_cmd(char * Message) {
  
  char *msg = Message;
  char *msgs[10];
  u32 cbcmd = 0;
  
  int a = 0;
  char *token;

  while ((token = strsep(&msg, "-")) != NULL) {
    msgs[a] = (char*) kmalloc(sizeof(token), GFP_KERNEL);
    strcpy(msgs[a], token);
    
    a++;
  }
  if (strcmp(msgs[0], "0") == 0) {
    printk(KERN_INFO "Fan Speed Control Mode");

    if (strcmp(msgs[1], "1") == 0) {
      printk(KERN_INFO "Cpu Fan Speed Control Mode");
     
      strcpy(&msgs[2][strlen(msgs[2]) - 1], "1");

      if (kstrtoint(msgs[2], 10, &cbcmd) != 0) {
        printk(KERN_ERR "Input value should be integer");
      }
      printk(KERN_INFO "%d", cbcmd);
      call_wmi_method(16, cbcmd, NULL);
    }
    if (strcmp(msgs[1], "4") == 0) {
      printk(KERN_INFO "Gpu Fan Speed Control Mode");
      strcpy(&msgs[2][strlen(msgs[2]) - 1], "4");

      if (kstrtoint(msgs[2], 10, &cbcmd) != 0) {
        printk(KERN_ERR "Input value should be integer");
      }
      printk(KERN_INFO "%d", cbcmd);
      call_wmi_method(16, cbcmd, NULL);
    }
  } else if (strcmp(msgs[0], "1") == 0) {
    printk(KERN_INFO "Fan Speed Behaivor Mode");
    if (strcmp(msgs[1], "0") == 0) {
      printk(KERN_INFO "Cpu Fan Speed Behaivor Mode");
      
      
      strcpy(&msgs[2][3], "1");

      if (kstrtoint(msgs[2], 10, &cbcmd) != 0) {
        printk(KERN_ERR "Input value should be integer");
      }
      printk(KERN_INFO "behaivor:%d", cbcmd);

      call_wmi_method(14, cbcmd, NULL);
    }
    if (strcmp(msgs[1], "1") == 0) {
      printk(KERN_INFO "Gpu Fan Speed Behaivor Mode");

      strcpy(&msgs[2][5], "1");
      strcpy(&msgs[2][6], "0");
      
      if (kstrtoint(msgs[2], 10, &cbcmd) != 0) {
        printk(KERN_ERR "Input value should be integer");
      }
      printk(KERN_INFO "behaivor:%d", cbcmd);

      call_wmi_method(14, cbcmd, NULL);
      
    }
  
  }
  
}

module_init(module_start);
module_exit(module_end);
