=== Xvnc_killer.sh ===
Exiting a remote desktop results in an incorrect termination of Xvnc on the ODROID
(it will remain in the memory), this causes a memory leak.
xvnc_killer.sh was developed to kill these remaining processes. It boots on startup
and ejects the oldest Xvnc client whenever there is more than 4 open (up to one every 
minute). This script can be found in /home/odroid/Documents/Bash/SUNRISE-Odroid/Bash. To stop 
this script from running on startup, remove /home/odroid/.config/upstart/xvnc_killer.conf

=== Unclutter ===
Unclutter is a package that is used to hide the mouse after 5 seconds of inactivity. This is
to ensure that the mouse does not remain on-screen on the local display of the charging
station. The script that runs this package is found in 
/home/odroid/Documents/SUNRISE-Odroid/Bash. To undo this for the current session, end 
the 'unclutter' task in the task manager. To never boot this on startup, remove 
/home/odroid/.config/upstart/hide_mouse.conf.

=== Browser_booter ===
Browser_booter is booted on startup to display information on the local screen of the charging 
station. To Disable this on startup, remove /home/odroid/.config/upstart/browser_booter.conf.
//Firefox also has two add-ons installed: mFull (to boot full-screen) and HideScrollbars
(to hide scrollbars). To exit full screen press F11.