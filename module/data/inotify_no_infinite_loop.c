#include <sys/inotify.h>
#include <limits.h>
#include "tlpi_hdr.h"

int files_accessed = 0, permissions_changed = 0, files_modified = 0, files_opened = 0;
//General activity statistics

int ACCESS_ALERT = 0, PERM_CHANGE_ALERT = 0, MODIFY_ALERT = 0, READ_ALERT;
//Sensitive activity statistics

struct inotify_event {
	__s32		wd;		/* watch descriptor */
	__u32		mask;		/* watch mask */
	__u32		cookie;		/* cookie to synchronize two events */
	__u32		len;		/* length (including nulls) of name */
	char		name[];	/* stub for possible name */
};

static void             /* Display information from inotify_event structure */
displayInotifyEvent(struct inotify_event *i)
{
    printf("    wd =%2d; ", i->wd);
    if (i->cookie > 0)
        printf("cookie =%4d; ", i->cookie);

    printf("mask = ");
    if (i->mask & IN_ACCESS)        printf("IN_ACCESS ");
    if (i->mask & IN_ATTRIB)        printf("IN_ATTRIB ");
    if (i->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (i->mask & IN_CLOSE_WRITE)   printf("IN_CLOSE_WRITE ");
    if (i->mask & IN_CREATE)        printf("IN_CREATE ");
    if (i->mask & IN_DELETE)        printf("IN_DELETE ");
    if (i->mask & IN_DELETE_SELF)   printf("IN_DELETE_SELF ");
    if (i->mask & IN_IGNORED)       printf("IN_IGNORED ");
    if (i->mask & IN_ISDIR)         printf("IN_ISDIR ");
    if (i->mask & IN_MODIFY)        printf("IN_MODIFY ");
    if (i->mask & IN_MOVE_SELF)     printf("IN_MOVE_SELF ");
    if (i->mask & IN_MOVED_FROM)    printf("IN_MOVED_FROM ");
    if (i->mask & IN_MOVED_TO)      printf("IN_MOVED_TO ");
    if (i->mask & IN_OPEN)          printf("IN_OPEN ");
    if (i->mask & IN_Q_OVERFLOW)    printf("IN_Q_OVERFLOW ");
    if (i->mask & IN_UNMOUNT)       printf("IN_UNMOUNT ");
    printf("\n");

    if (i->len > 0)
        printf("        name = %s\n", i->name);
}

static void
analyzeInotifyEvent(struct inotify_event *i)
{
	if (i->mask & IN_ACCESS);        //File was accessed
		files_accessed++;
	
	if (i->mask & IN_ATTRIB);		//Permissions were changed
		permissions_changed++;
	
	if (i->mask & IN_MODIFY);		//File was modified
		files_modified++;
	
	if (i->mask & IN_OPEN);			//File was opened
		files_opened++;
	
}

static void		//Specific event handler for editing of the etc/shadow file (password hashes)
analyzeInotifyEvent_shadow(struct inotify_event *i)
{
	if (i->mask & IN_ACCESS){					//Nobody but root should access this
		printk(KERN_INFO "etc/shadow was accessed\n");
		ACCESS_ALERT++;
		//Any way to tell the permissions or identify of the user who committed the action????????
			//I.E. if a daemon user accessed shadow
	}
	
	if (i->mask & IN_ATTRIB){
		printk(KERN_INFO "etc/shadow had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){
		printk(KERN_INFO "etc/shadow was modified!\n");
		MODIFY_ALERT++;
	}
}

static void		//Specific event handler for editing of the etc/ssh/ssh_config file
analyzeInotifyEvent_ssh_config(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){
		printk(KERN_INFO "etc/ssh/ssh_config had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){
		printk(KERN_INFO "etc/ssh/ssh_config was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_sudoers(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){		
		printk(KERN_INFO "etc/sudoers had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){	//Changing this is VERY delicate. Perhaps add 2 to modify alert
		printk(KERN_INFO "etc/sudoers was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_passwd(struct inotify_event *i)
{
	if (i->mask & IN_ACCESS){
		printk(KERN_INFO "etc/passwd was accessed\n");
		ACCESS_ALERT++;
		//Any way to tell the permissions or identify of the user who committed the action????????
			//I.E. if a daemon user accessed shadow
	}
	
	if (i->mask & IN_ATTRIB){
		printk(KERN_INFO "etc/passwd had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){		//VERY delicate
		printk(KERN_INFO "etc/passwd was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_group(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){		//EXTREMELY DELICATE. WHY WOULD YOU NEED TO DO THAT?
		printk(KERN_INFO "etc/group had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){		//VERY delicate
		printk(KERN_INFO "etc/group was modified!\n");
		MODIFY_ALERT++;
	}
}

analyzeInotifyEvent_hosts(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){
		printk(KERN_INFO "etc/host had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){
		printk(KERN_INFO "etc/host was modified!\n");
		MODIFY_ALERT++;
	}
}


analyzeInotifyEvent_host_allow(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){		//EXTREMELY DELICATE. WHY WOULD YOU NEED TO DO THAT?
		printk(KERN_INFO "etc/host.allow had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){		//VERY delicate
		printk(KERN_INFO "etc/host.allow was modified!\n");
		MODIFY_ALERT++;
	}
}

analyzeInotifyEvent_host_deny(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){		//EXTREMELY DELICATE. WHY WOULD YOU NEED TO DO THAT?
		printk(KERN_INFO "etc/host_deny had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){		//VERY delicate
		printk(KERN_INFO "etc/host.deny was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_securetty(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){		//EXTREMELY DELICATE. WHY WOULD YOU NEED TO DO THAT?
		printk(KERN_INFO "etc/securetty had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){		//VERY delicate
		printk(KERN_INFO "etc/securetty was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_sources_list(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){		
		printk(KERN_INFO "etc/securetty had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){	
		printk(KERN_INFO "etc/securetty was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_cron_tab(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){		
		printk(KERN_INFO "etc/crontab had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){	
		printk(KERN_INFO "etc/crontab was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_bashrc(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){		
		printk(KERN_INFO ".bashrc had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){	
		printk(KERN_INFO ".bashrc was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_bash_aliases(struct inotify_event *i)
{
	if (i->mask & IN_ATTRIB){		
		printk(KERN_INFO ".bash_aliases had its permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){	
		printk(KERN_INFO ".bash_asiases was modified!\n");
		MODIFY_ALERT++;
	}
}

static void
analyzeInotifyEvent_id_rsa(struct inotify_event *i)
{
	if (i->mask & IN_ACCESS){
		printk(KERN_INFO "ssh private keys were accessed\n");
		ACCESS_ALERT++;
	}
	
	if (i->mask & IN_ATTRIB){		
		printk(KERN_INFO "ssh private keys had their permissions changed!\n");
		PERM_CHANGE_ALERT++;
	}
	
	if (i->mask & IN_MODIFY){	
		printk(KERN_INFO "ssh private keys were modified!\n");
		MODIFY_ALERT++;
	}
}

//Call inotifyevent with "etc" and "etc/ssh" as directories to add to the watchlist

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))
int
inotify(int watch_list_size, char *files_and_directories[])		//Sets up the watchlist
{
    int inotifyFd, wd;
    char buf[BUF_LEN] __attribute__ ((aligned(8)));
    ssize_t numRead;
    char *p;
    struct inotify_event *event;

    if (watch_list_size < 2 || strcmp(files_and_directories[1], "--help") == 0)
        usageErr("%s pathname...\n", argv[0]);

    inotifyFd = inotify_init();   /* Create an inotify instance and return a file 
									descriptor referring to the inotify instance */
    if (inotifyFd == -1)
        errExit("inotify_init");

//SHOULDN'T IT START AT 0?????????
    for (j = 1; j < watch_list_size; j++) {	//Enter all of the input pathnames into the watchlist
        wd = inotify_add_watch(inotifyFd, files_and_directories[j], IN_ALL_EVENTS);
	/*manipulates the "watch list" associated with an inotify instance. Each item ("watch") in the watch list specifies the 
	pathname of a file or directory, along with a set of events that the kernel should monitor for the file/directory*/
		  
	/*The IN_ALL_EVENTS macro is defined as a bit mask of all of the above events*/
		
        if (wd == -1)
            errExit("inotify_add_watch");

        printf("Watching %s using wd %d\n", argv[j], wd);
    }

    return 0;
	
}


//Somehow call this every time file activity is recorded????????????????????? Or every time a certain time interval passes??????
void analyze_activity(int inotifyFd, char buf[BUF_LEN]){

        numRead = read(inotifyFd, buf, BUF_LEN);	//Finds out how many file activity events have been detected		
        
	if (numRead == 0)
            fatal("read() from inotify fd returned 0!");

        if (numRead == -1)
            errExit("read");

        printf("Read %ld bytes from inotify fd\n", (long) numRead);

        /* Process all of the events in buffer returned by read() */

        for (int p = buf; p < buf + numRead; ) {		//Print out the event details 1 event at a time
            event = (struct inotify_event *) p;
            displayInotifyEvent(event);
			
   	    //So far, we're analyzing /etc/shadow, /etc/ssh/ssh_config, /etc/sudoers, and /etc/passwd

	    if(strcmp(event->name, "shadow") == 0)	//Password hashes
	    	anaylyzeInotifyEvent_shadow(event);	
			
	    else if (strcmp(event->name, "ssh_config") == 0)	
	       analyzeInotifyEvent_ssh_config(event);
			
	    else if (strcmp(event->name, "sudoers") == 0)	//Who can sudo
	       analyzeInotifyEvent_sudoers(event);

      	    else if (strcmp(event->name, "passwd") == 0)	//Information regarding registered system users
      		analyzeInotifyEvent_passwd(event);
      		
      	    else if (strcmp(event->name, "group") == 0)		//Information regarding security group definitions
      		analyzeInotifyEvent_group(event);
      	
      	    else if (strcmp(event->name, "securetty") == 0)	//List of terminals where root can login
      		analyzeInotifyEvent_securetty(event);	
      		
            else if (strcmp(event->name, "hosts") == 0)	//Contains a list of hosts used for name resolution
	        analyzeInotifyEvent_hosts(event);
      		
      	    else if (strcmp(event->name, "hosts.allow") == 0)	//Contains a list of hosts allowed to access services
	        analyzeInotifyEvent_host_allow(event);	
	
	    else if (strcmp(event->name, "hosts.deny") == 0)   //Contains a list of hosts forbidden to access services
	      	analyzeInotifyEvent_host_deny(event);
	    
	    else if (strcmp(event->name, "sources.list") == 0)   //List of sources accessed during updates
	      	analyzeInotifyEvent_sources_list(event);
	      	
	    else if (strcmp(event->name, "crontab") == 0)   //Contains the list of cron jobs
	      	analyzeInotifyEvent_crontab(event);
	      	
	    else if (strcmp(event->name, "crontab") == 0)   //Contains the list of cron jobs
	      	analyzeInotifyEvent_crontab(event);
	      	
	    else if (strcmp(event->name, ".bashrc") == 0)   //Shell script that Bash runs whenever it is started interactively
	      	analyzeInotifyEvent_bashrc(event);
	      	
	    else if (strcmp(event->name, ".bash_aliases") == 0)   //Stores command aliases
	      	analyzeInotifyEvent_bashrc(event);
	      	
	    else if ((strcmp(event->name, "id_rsa") == 0) || (strcmp(event->name, "authorized_key") == 0)
	    		|| (strcmp(event->name, "authorized_key2") == 0))   //ssh private keys
	      	analyzeInotifyEvent_id_rsa(event);
	   
			
	    //The name field is present only when an event is returned for a file inside a watched directory
				
	    anaylyzeInotifyEvent(event);		//General analysis
			
            p += sizeof(struct inotify_event) + event->len;
        }
}
