while !(st-info --probe | grep '0x041' &> /dev/null);
do
	echo -ne '\rWaiting for Chip ID    '
	sleep 0.1
	echo -ne '\rWaiting for Chip ID .  '
	sleep 0.1
	echo -ne '\rWaiting for Chip ID .. '
	sleep 0.1
	echo -ne '\rWaiting for Chip ID ...'
done

echo -ne '\r'

while (st-info --probe | grep 'flash: 0' &> /dev/null);
do
	echo -ne '\r'
	echo -ne '\rWaiting for Flash    '
	sleep 0.1
	echo -ne '\rWaiting for Flash .  '
	sleep 0.1
	echo -ne '\rWaiting for Flash .. '
	sleep 0.1
	echo -ne '\rWaiting for Flash ...'
done

echo -ne '\r'

st-info --probe

sleep 2
echo ''

if [[ $1 == 's' ]]; then
	if [[ $2 == 'd' ]]; then
		command="st-flash write Sensors/Debug/Sensors.bin 0x8000000"
	else
		command="st-flash write Sensors/Release/Sensors.bin 0x8000000"
	fi
else
	if [[ $2 == 'd' ]]; then
		command="st-flash write Communications/Debug/Communications.bin 0x8000000"
	else
		command="st-flash write Communications/Release/Communications.bin 0x8000000"
	fi
fi

echo $command

sleep 1

while !($command 2>&1 | grep 'jolly good' &> /dev/null);
do
	$command
	sleep 0.2
	clear
done

echo 'JOLLY GOOD!'