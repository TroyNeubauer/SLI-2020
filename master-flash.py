import subprocess
import sys
import time

if(len(sys.argv) < 3):
	print('Specify arguments')
	print('python master-flash [s/c] [d/r]')
	exit()

def dots_print(s):
	for i in range(4):
		print(s + ' ' + ('.' * i) + (' ' * (4 - i)), end='\r')
		time.sleep(0.1)

chipid = ''

if(sys.argv[1] == 'c'):
	chipid = b'0x0410'
	dots_print('Writing to the STM32F103')
elif(sys.argv[1] == 's'):
	chipid = b'0x0411'
	dots_print('Writing to the STM32F205')
else:
	print('Invalid Arguments')

while(chipid not in subprocess.check_output(['st-info','--probe'])):
	dots_print('Waiting for Chip ID')

while(b'flash: 0' in subprocess.check_output(['st-info','--probe'])):
	dots_print('Waiting for reset pin removal')

sys.stdout.buffer.write((subprocess.check_output(['st-info','--probe'])))

time.sleep(0.1)
dots_print('Writing now ')
print('')

command = ''
if(sys.argv[1] == 's' and sys.argv[2] == 'd'):
	command='st-flash write Sensors/Debug/Sensors.bin 0x8000000'
elif(sys.argv[1] == 's' and sys.argv[2] == 'r'):
	command='st-flash write Sensors/Release/Sensors.bin 0x8000000'
elif(sys.argv[1] == 'c' and sys.argv[2] == 'd'):
	command='st-flash write Communications/Debug/Communications.bin 0x8000000'
elif(sys.argv[1] == 'c' and sys.argv[2] == 'r'):
	command='st-flash write Communications/Release/Communications.bin 0x8000000'

while(True):
	try:
		output = subprocess.check_output(command.split(' '), stderr=subprocess.STDOUT)
	except(subprocess.CalledProcessError):
		output = b''

	if b'jolly good' in output:
		print('JOLLY GOOD!')
		break
	else:
		sys.stdout.buffer.write(output)
		input()

