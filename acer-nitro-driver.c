
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
static int an_uevent(const struct device *dev,struct kobj_uevent_env *env){
  add_uevent_var(env, "DEVMODE=%#o",0666);
  return 0;
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
struct file_operations fops = {.write = device_write

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
static acpi_status call_wmi_method(u32 m_id, u32 m_input, u32 *m_output) {
  union acpi_object *obj;
  const struct acpi_buffer input = {(acpi_size)sizeof(u32), &m_input};
  struct acpi_buffer result = {ACPI_ALLOCATE_BUFFER, NULL};
  u32 tmp = 0;
  acpi_status status =
      wmi_evaluate_method(WMI_GAMING_GUID, 0, m_id, &input, &result);
  if (ACPI_FAILURE(status))
    return status;

  obj = (union acpi_object *)result.pointer;
  if (obj) {
    if (obj->type == ACPI_TYPE_BUFFER && (obj->buffer.length == sizeof(u32) ||
                                          obj->buffer.length == sizeof(u64))) {
      tmp = *((u32 *)obj->buffer.pointer);
    } else if (obj->type == ACPI_TYPE_INTEGER) {
      tmp = (u32)obj->integer.value;
    }
  }

  if (m_output)
    *m_output = tmp;

  kfree(result.pointer);
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
  u32 wmiout[40];
  int a = 0;
  char *token;

  while ((token = strsep(&msg, "-")) != NULL) {
    msgs[a] = (char*) kmalloc(sizeof(token), GFP_KERNEL);
    strcpy(msgs[a], token);
    printk(KERN_INFO "a:%s", token);
    a++;
  }
  if (strcmp(msgs[0], "0") == 0) {
    printk(KERN_INFO "Fan Speed Control Mode");

    if (strcmp(msgs[1], "1") == 0) {
      printk(KERN_INFO "Cpu Fan Speed Control Mode");
      printk(KERN_INFO "awq%lu", strlen(msgs[2]));
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
      printk(KERN_INFO "acbg:%s", msgs[2]);
      printk(KERN_INFO "%c", msgs[2][3]);
      strcpy(&msgs[2][3], "1");

      printk(KERN_INFO "zd%s", msgs[2]);
      if (kstrtoint(msgs[2], 10, &cbcmd) != 0) {
        printk(KERN_ERR "Input value should be integer");
      }
      printk(KERN_INFO "behaivor:%d", cbcmd);

      call_wmi_method(14, cbcmd, NULL);
    }
    if (strcmp(msgs[1], "1") == 0) {
      


      printk(KERN_INFO "Gpu Fan Speed Behaivor Mode");
      

      printk(KERN_INFO "acbg:%s", msgs[2]);
      printk(KERN_INFO "%c", msgs[2][3]);
      strcpy(&msgs[2][5], "1");
      strcpy(&msgs[2][6], "0");
      printk(KERN_INFO "zd%s", msgs[2]);
      if (kstrtoint(msgs[2], 10, &cbcmd) != 0) {
        printk(KERN_ERR "Input value should be integer");
      }
      printk(KERN_INFO "behaivor:%d", cbcmd);

      call_wmi_method(14, cbcmd, NULL);
      printk(KERN_INFO "%c", msgs[2][6]);
    }
  
  }
  
}

module_init(module_start);
module_exit(module_end);
