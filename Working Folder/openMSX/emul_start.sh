# OpenMSX Start Script
# The emulator will be started only if it's not already active

#MacOS
if [ "$(uname -s)" = "Darwin" ]; then
	xopenmsx=`ps x | grep "openmsx " | grep -v grep`
	 
	if [ "${xopenmsx}" = "" ]; then
	  echo "...Now Starting OpenMsx "
	  ./openMSX/openMSX.app/Contents/MacOS/openmsx -script ./openMSX/emul_start_config.txt | NULL
	else
	  echo "...openMSX already running "
	fi

#Linux
elif [ "$(uname -s)" = "Linux" ]; then
	xopenmsx=`pgrep "openmsx"`
	
	if [ "${xopenmsx}" = "" ]; then
		echo "...Now Starting OpenMSX "
		/opt/openMSX/bin/openmsx -script ./openMSX/emul_start_config.txt
	else
		echo "...openMSX already running "
	fi

#Other
else
	echo "Failed to identify OS"
fi
