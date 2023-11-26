#
# Regular cron jobs for the netfleet package
#
0 4	* * *	root	[ -x /usr/bin/netfleet_maintenance ] && /usr/bin/netfleet_maintenance
