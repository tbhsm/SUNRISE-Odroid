=== Xvnc_killer.sh ===
Exiting a remote desktop results in an incorrect termination of Xvnc on the ODROID
(it will remain in the memory), this causes a memory leak.
xvnc_killer.sh was developed to kill these remaining processes. It boots on startup
and ejects the oldest Xvnc client whenever there is more than 4 open (up to one every 
minute). This script can be found in /home/odroid/Documents/SUNRISE-Odroid/Bash. To stop 
this script from running on startup, remove /home/odroid/.config/upstart/xvnc_killer.conf

=== Unclutter ===
Unclutter is a package that is used to hide the mouse after 5 seconds of inactivity. This is
to ensure that the mouse does not remain on-screen on the local display of the charging
station. The script that runs this package is found in 
/home/odroid/Documents/SUNRISE-Odroid/Bash. To undo this for the current session, end 
the 'unclutter' task in the task manager. To never boot this on startup, remove 
/home/odroid/.config/upstart/hide_mouse.conf.

=== Browser ===
Chromium is booted on startup to display information on the local screen of the charging 
station. To Disable this on startup, remove /home/odroid/.config/upstart/browser_boot.conf.
To exit full screen use ALT+F4 or ALT-TAB.

=== C-code ===
The C-code is booted by the ODROID_booter.conf found in the same folder as the previous applications. It boots the C-code on startup and whenever the code has stopped running as a process. Remove the .conf file to stop it from booting on startup.