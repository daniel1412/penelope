import wiringpi
import mysql.connector
from decimal import Decimal
import datetime
import sys

wiringpi.wiringPiSetupGpio()
pinSolar = 26 # Test Pin = 19; Real Solar Pin = 26

desiredState = -1
currentState = -1
offTemp = -3.0
onTemp = 2.0
hoursAbove = 2.0

#Open mySQL DB connection
conn = mysql.connector.connect(
	user='penelopeRead', 
	password='83f4cb09061f084ad40286e015470907eae8934f2b6dc5305b21619b3ac72e49', 
	#user='test',
	#password='testtest',
	host='localhost', 
	database='Penelope'
)
#print(conn)
# cursor
cursor = conn.cursor()


def getUptime():
	with open('/proc/uptime', 'r') as f:
		uptime_seconds = float(f.readline().split()[0])
	print("Current Uptime = " + str(uptime_seconds))
	return uptime_seconds

def readState(pinNr):
	statePin = wiringpi.digitalRead(pinNr)
	return statePin

def setPinState(pinNr, state):
	wiringpi.pinMode(pinNr, 1)
	print("Change Pin state to: ", end='')
	print(state)
	wiringpi.digitalWrite(pinNr, state)
	print("State changed!")

def togglePinState():
	#wiringpi.wiringPiSetupPhys() # use BCM pin number (alternative = wiringpi.wiringPiSetup() OR wiringpi.wiringPiSetupGpio())
	#wiringpi.wiringPiSetupSys()
	#wiringpi.wiringPiSetupGpio()

	# Set Pin mode
	wiringpi.pinMode(19, 1) # sets P1 pin 19 to output (0=input, 2=PWM)

	# Read pin state
	#wiringpi.digitalRead(19)
	print("Port 19 Status = ", end='')
	pinState = wiringpi.digitalRead(19)
	print(pinState)

	# Write pin state
	#wiringpi.digitalWrite(19,0) # sets Port to 0
	#wiringpi.digitalWrite(19,1) # sets Port to 1
	wiringpi.digitalWrite(19, not pinState)

def desiredDBstate():
	# Executin an MYSQL function using the execute() method
	#cursor.execute("SELECT MAX(index) FROM 'sensor'")
	#cursor.execute("SELECT * FROM sensor")
	cursor.execute("SELECT solarState FROM solarChargeState ORDER BY Id DESC LIMIT 1;")

	# Fetch a single row using fetchone() method
	data = cursor.fetchone()
	#data = cursor.fetchall()
	#print("Desired pin state: ", end='')
	#print(data[0])	
	#for x in data:
	#	print(x)
	return data[0]

def readMultipleDBValues(sqlQuery):
	cursor.execute(sqlQuery)
	output = []
	for row in cursor:
		row_data = []
		for data in row:
			row_data.append(str(data))
		output.append(row_data)
	#print("Data = ", end = '')
	#for x in output:
	#	print(x)
	return output

def getMinBattCellTemp():
	cursor.execute("SELECT Cell1_T, Cell2_T, Cell3_T, Cell4_T, Cell5_T, Cell6_T, Cell7_T, Cell8_T FROM Batt_T ORDER BY Id DESC LIMIT 1;")
	#data = cursor.fetchall()
	#print(data)
	output = []
	for row in cursor:
		#print("Row: ", end='')
		#print(row)
		row_data = []
		for data in row:
			#print("Data: ", end='')
			#print(data)
			if type(data) is Decimal:
				row_data.append(float(data))
			else:
				row_data.append(str(data))
		output.append(row_data)
	print("Temp Data:", end = '')
	for x in output:
		print(x)
	print("Min Temp = ", end = '')
	minT = min(output[0])
	print(minT)
	return minT

def getMinCellTempLastXHours(minusHours):
	# Get number of row where time > now - hours
	#timestamp = datetime.datetime.now() - datetime.timedelta(hours=minusHours, minutes=0)
	cursor.execute("SELECT Id FROM Batt_T WHERE timestamp < (NOW() - INTERVAL " + str(minusHours) + " HOUR) ORDER BY Id DESC LIMIT 1;")
	idNr = int(cursor.fetchone()[0])
	cursor.execute("SELECT Id FROM Batt_T ORDER BY Id DESC LIMIT 1;")
	lastId = int(cursor.fetchone()[0])
	dataCount = lastId - idNr
	#print("LastId = " + str(lastId) + "; idNr = " + str(idNr) + "; count = " + str(dataCount))
	if(dataCount > 20):
		print("Get Min values of DB for the last " + str(hoursAbove) + " Hours")
		sqlQuery = "SELECT Min(Cell1_T), Min(Cell2_T), Min(Cell3_T), Min(Cell4_T), Min(Cell5_T), Min(Cell6_T), Min(Cell7_T), Min(Cell8_T) FROM Batt_T WHERE Id > " + str(idNr) + ";"
		result = readMultipleDBValues(sqlQuery)
		resultFloat = []
		for x in result[0]:
			resultFloat.append(float(x))
		# Calc min over all Cells
		minT = min(resultFloat)
		print("Min Temp in last " + str(hoursAbove) + " Hours = " + str(minT))
		if minT > offTemp:
			print("Min Temp in last " + str(hoursAbove) + " Hours above 'offTemp' -> switch Solar Charger ON")
			return 1
		else:
			print("Min Temp in last " + str(hoursAbove) + " Hours too low -> do not switch SolarCharger ON")
			return 0
	else:
		print("Not enough Data available -> Do not switch SolarCharger ON")
		return 0

# --------------------------------MAIN Program ---------------------------
#togglePinState()
#readMySql()
#sys.exit()

# Check time and UpTime -> If run direct after boot (within 120s) than skip check states (system and mariadb maybe not completely ready)
now = datetime.datetime.now()
print("Date and Time: ", end = '')
print(now.strftime('%Y-%m-%d %H:%M:%S'))
print("System UpTime: ", end = '')
uptime = getUptime()
if(uptime < 120.0):
	print("System Uptime to short -> skip executing program")
	sys.exit()

# Read actual state of Pin 19 (controles SolarCharger)
currentState = readState(pinSolar)
print("State of Solar Pin = ", end = '')
print(currentState)

# Get desired state from sql DB (0 = charger OFF, 1 = charger ON, 2 = AUTO (temperature controlled)
desiredState = desiredDBstate()
print("Desired State in DB = ", end = '')
print(desiredState)

# Decide if state must be changed
if not (desiredState == currentState):
	print("State must be changed")
	if(desiredState == 0) or (desiredState == 1):
		setPinState(pinSolar, desiredState)
	elif(desiredState == 2):
		print("Run TempDetection and set State")
		minTemp = getMinBattCellTemp()
		#minTemp = 1.2
		if(minTemp < offTemp):
			print("Temp Lower than " + str(offTemp) + " C -> Charger OFF")
			if(currentState is not 0):
				setPinState(pinSolar, 0)
				print("Set Pin to 0")
			else:
				print("No Action needed -> pin already in state 0")
		elif(minTemp > onTemp):
			print("Temp HIGHER than " + str(onTemp) + " C -> Charger ON")
			if(currentState is not 1):
				setPinState(pinSolar, 1)
				print("Set Pin to 1")
			else:
				print("No Action needed -> pin already in state 1")
		else:
			print("Range between " + str(offTemp) + " C and " + str(onTemp) + " C -> check if Temp is above 'offTemp' for more than " + str(hoursAbove) + " hours")
			# Check if temp is more than X hours above 'offTemp' if yes switch SolarCharger ON
			StateSet = getMinCellTempLastXHours(hoursAbove)
			if StateSet:
				print("Set STATE to 1 because aboveHour criteria")
				setPinState(pinSolar, StateSet)
	else:
		print("UNKNOWN STATE!!!!!! NO CHANGE DONE!")
else:
	print("State already set correct - No change needed!")

# Close DB connection
conn.close()
print("------------------------------------------------------------------------------")
