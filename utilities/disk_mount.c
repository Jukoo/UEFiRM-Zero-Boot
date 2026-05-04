//!SPDX-License-Identifier :GPL-3.0 
/* 
 * - Analyse image disk in specific way  and mount it 
 * Author <Umar Ba <jUmarB@protonmail.com> 
 */
#define _GNU_SOURCE 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/ioctl.h> 
#include <linux/loop.h>
#include <sys/stat.h> 
#include <sys/mount.h>
#include <fcntl.h>
#include <string.h> 
#include <errno.h> 


#define  perr_r(fcall, ...) \
  EXIT_FAILURE;do{perror(#fcall); fprintf(stderr , __VA_ARGS__);}while(0) 

#define  DEVICE_LOOP_CTRL  "/dev/loop-control" 
#define  DEVICE_LOOP_NAMING  "/dev/loop"  
#define  VIRTUAL_FILE_ALLOCATION_TABLE "vfat" 

typedef struct  __loopdev_t loop_dev_t; 
typedef struct  __loopitem_t loop_it  ; 

struct __loopitem_t {
  unsigned int _lo_fd, 
               _backing_fd; 
}; 

struct __loopdev_t{
   unsigned int  _loopfd,
                 _devnum;
   struct __loopitem_t   *_link;  
   union { 
     char *  _loopname;  
   }; 
} ; 

typedef  void (*dm_release_function)(void*) ;

static void lpdevctr_free(void * lpdevctr) 
{
   struct __loopdev_t  * lpdev = (struct __loopdev_t *) lpdevctr; 
   close (lpdev->_loopfd); 
   free(lpdev->_loopname);

}

static void dm_emergency_release(void *data,  dm_release_function cleaner) 
{
  cleaner(data); 
}

static  int dm_check_loopctrl(void) 
{
  if(access(DEVICE_LOOP_CTRL, F_OK)){
    return ~0 ;  
  }

  //!check if chardev 

  return 0 ; 
}

static int dm_link_loop_device_iterface(struct __loopitem_t * restrict ldevinterface) 
{
  return ioctl(ldevinterface->_lo_fd , LOOP_SET_FD, ldevinterface->_backing_fd); 
}

static  int  dm_allocate_loopdev(struct __loopdev_t * restrict  lpdevctr)  
{
    
  lpdevctr->_loopfd = open(DEVICE_LOOP_CTRL,  O_RDWR) ;
   if(-1 == lpdevctr->_loopfd) 
     return ~0 ; 

   lpdevctr->_devnum = ioctl(lpdevctr->_loopfd, LOOP_CTL_GET_FREE , 0) ; 
   if(-1 ==  lpdevctr->_devnum){
     close(lpdevctr->_loopfd) ; 
     return  ~0 ; 
   }
   asprintf(&lpdevctr->_loopname , DEVICE_LOOP_NAMING"%i", lpdevctr->_devnum) ; 
   return 0 ;  
}

static  loop_it *  dm_link(const char *  image_disk ,  const char * devname)
{ 
  unsigned int status  = 0x10000; 
  struct __loopitem_t   *ldevinterface =(struct __loopitem_t *) malloc(sizeof(*ldevinterface))  ; 
  
  if(!ldevinterface) return  0;  
  
  ldevinterface->_backing_fd =  open(image_disk , O_RDWR), status |=errno ; 
  ldevinterface->_lo_fd      =  open(devname, O_RDWR) ,    status |=errno<<8 ; 

  if(status &(0x7fff))    
  {  
    perr_r(open , "Error while opening file \012") ; 
    return 0 ; 
  }
  
  if(dm_link_loop_device_iterface(ldevinterface)) 
  {
    perr_r(dm_link_loop_device_iterface ,"Fail to link image disk to loop interface \012") ; 
    return 0 ; 

  }

  return  ldevinterface; 

}

static int dm_auto_remove_allocated_loopdev(struct __loopdev_t * lpdevctr)  
{ 

 
  if(ioctl(lpdevctr->_link->_lo_fd  ,  LOOP_CLR_FD  , lpdevctr->_link->_backing_fd)){
    perr_r(ioctl, "@%s :" , __func__ ,strerror(*__errno_location())) ; 
    return  -1;    
  } 
  
  if(ioctl(lpdevctr->_loopfd , LOOP_CTL_REMOVE , lpdevctr->_devnum)) 
  {
     perr_r(ioctl::LOOP_CTL_REMOVE , "Fail to remove loopdevice \012")  ; 
     return -1 ; 
  }
  
  return 0 ;   
} 

static int dm_mount(struct  __loopdev_t *  lpdevctr) 
{
  if(mount(lpdevctr->_loopname ,"/mnt/efi/" , VIRTUAL_FILE_ALLOCATION_TABLE, 0, 0)) 
  {
     perr_r(mount, "Fail to mount this device  :%s\012",strerror(*__errno_location())) ; 
     return  -1 ; 
  } 

  return 0;  
  
}

static int dm_parse_argument(char * const * av)
{
  unsigned int status = 0, 
               i  = ~0 ; 
  while(*(av + ++i) || !status ){
    char *arg =  *(av +i) ; 
    if(!strlen(arg))
      continue ; 
    status ^=((*arg & 0xff) == '-' && (*(arg+1) & 0xff ) == 'u'); 
     
  }

  return status ; 

}

static int dm_save_loopdev(struct  __loopdev_t * restrict  lpdevctr)
{
  
}


int main(int ac , char *const *av) 
{
  unsigned int  pstatus = EXIT_SUCCESS, 
                has_umount_flag =0 ; 
  const char *fat_image_disk = 00 ; 

  loop_dev_t lpdevctr = {0}  ; 

  if(!(ac &~(1)))
  {
     fprintf(stderr , " Require img disk as entry \012") ;
     goto  _eplg ;
  }

  has_umount_flag = dm_parse_argument(av) ;  
   

  fat_image_disk = *(av+(ac-1)) ; 

  //TODO :Add more verification  

  if(dm_allocate_loopdev(&lpdevctr)){
    pstatus ^= perr_r(dm_check_loopctrl, "Fail to allocated  loop device \012") ; 
    goto _eplg;  
  }
 
  lpdevctr._link = dm_link(fat_image_disk,  lpdevctr._loopname) ;

  if(!lpdevctr._link){
    dm_emergency_release((void *) &lpdevctr , lpdevctr_free) ; 
    pstatus^=perr_r(dm_link,"Fail to link  image disk to loopdevice:%s \012",strerror(*__errno_location())) ; 
    goto  _eplg ; 
  }  

  if(dm_mount(&lpdevctr)) 
  {
    dm_auto_remove_allocated_loopdev(&lpdevctr) ; 
    goto _eplg ;  
  }

  //TODO : create  a temporary file to save  the information of the pheripheral  
 
  //dm_save_loopdev(&lpdevctr) ; 
 
  dm_auto_remove_allocated_loopdev(&lpdevctr) ; 

_eplg:
  dm_emergency_release(&lpdevctr ,  lpdevctr_free) ;  
  return pstatus ;  
}
