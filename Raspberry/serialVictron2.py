from __future__ import print_function
import serial
import time
import hashlib
import datetime
import sys
import signal
from numpy import median
import mysql
import mysql.connector
import binascii

def signal_handler(sig, frame):
	print('You pressed Ctrl+C!')
	serCon.close()
	connection.close()
	sys.exit(0)

def md5(str_md5):
	hash_md5 = hashlib.md5(str_md5.encode())
	return hash_md5

def getChecksum(str_chksum):
	checksumNew = 0
	for c in str_chksum:
		checksumNew += ord(c)
	checksumNew += checksumTxtAsciiVal
	checksumNew = checksumNew & 0xff
	checksumNew = (0xff - checksumNew + 1) & 0xff
	return checksumNew

def startSerial():
	serialConnection = serial.Serial(
			#port='/dev/ttyUSB0',\
			port='/dev/ttyS0',\
			baudrate=19200,\
			parity=serial.PARITY_NONE,\
			stopbits=serial.STOPBITS_ONE,\
			bytesize=serial.EIGHTBITS,\
			timeout=10
			)
	serialConnection.flushInput()
	return serialConnection

def stopSerial(serialConnection):
	serialConnection.close()

def startDbConnection():
	connection = mysql.connector.connect(host = "localhost", user = "penelopeWrite", passwd = "6c3a9fdf8b1177c64a0073269fa9e4d69f1cd54cb3d13aee7bc398379be1b665", db = "Penelope")
	#connection =  mysql.connector.connect(host = "localhost", user = "test", passwd = "testtest", db = "penelope")
	return connection

print("Wait for 60 seconds")
time.sleep(5)
signal.signal(signal.SIGINT, signal_handler)

NumAvg = 1200	# bei 1min (120) entspricht das ca. 700kB pro Tag. Besser ca. alle 5-10 min (600-1200)
checksumTxt = "Checksum\t\n\r"
checksumTxtAsciiVal = 851

envSensorNr = 4

connection = startDbConnection()
sqlStart = "INSERT INTO Penelope.SolarData(EnvSensorNr, PID, FW, SER, V, I, VPV, PPV, CS, MPPT, sOR, ERR, sLOAD, RELAY, H19, H20, H21, H22, H23, HSDS, sCHECKSUM) VALUES('"
sqlEnd = "')"
sqlAppend = "', '"
print(serial)
#ser = serial.Serial(
#		port='/dev/ttyUSB0',\
#		baudrate=19200,\
#		parity=serial.PARITY_NONE,\
#		stopbits=serial.STOPBITS_ONE,\
#		bytesize=serial.EIGHTBITS)		#timeout=100

#ser.flushInput()

ser = startSerial()

# values expected = PID, FW, SER#, V, I, VPV, PPV, CS, MPPT, OR, ERR, LOAD, RELAY, H19, H20, H21, H22, H23, HSDS, Checksum
dComplete = ""
dPID = list()
dFW = list()
dSER = list()
dV = list()
dI = list()
dVPV = list()
dPPV = list()
dCS = list()
dMPPT = list()
dOR = list()
dERR = list()
dLOAD = list()
dRELAY = list()
dH19 = list()
dH20 = list()
dH21 = list()
dH22 = list()
dH23 = list()
dHSDS = list()
dCHECKSUM = list()
dDATE = list()
sumValues = 20
countValues = 0
countNoData = 0


def delValues():
	print("DELETE values")
	del dPID[:]
	del dFW[:]
	del dSER[:]
	del dV[:]
	del dI[:]
	del dVPV[:]
	del dPPV[:]
	del dCS[:]
	del dMPPT[:]
	del dOR[:]
	del dERR[:]
	del dLOAD[:]
	del dRELAY[:]
	del dH19[:]
	del dH20[:]
	del dH21[:]
	del dH22[:]
	del dH23[:]
	del dHSDS[:]
	del dCHECKSUM[:]
	global countValues
	countValues = 0

# Empty first received Data (dpendent from starttime it can occure that the datapacket is corrupted)
for i in range(2):
	while(ser.inWaiting() == 0):
		print("No Data")
		time.sleep(0.1)
	time.sleep(0.1)
	ser.read(ser.inWaiting())
	print("Read and clear data")
	
while True:
	try:
		print("While start")
		bytesToRead = ser.inWaiting()
		print("Bytes to read = ", end = '')
		print(bytesToRead)
		if(bytesToRead > 0):
			#textRead = ser.read(bytesToRead)
			#print(textRead)
			#if not (b'\n' in textRead):
			#	print("No lineFeed found in data -> skip")
			#	continue
			print("start read until")
			ser_bytes1 = ser.read_until(b'\n')
			print("stop read until")
			# Convert bytes to String
			print("countValues = ", end = '')
			print(countValues)
			print("Received String = ", end='')
			print(ser_bytes1)
			countNoData = 0
			ser_bytes2 = ser_bytes1.decode(encoding='latin-1',errors='strict')  #'utf-8' 'latin-1' - 'replace' 'strict'
			#decoded_bytes = float(ser_bytes[0:len(ser_bytes)-2].decode("utf-8"))
			if(len(ser_bytes2) > 3):
				print("Check received data:")
				# Check if data contains \r\n -> if not drop data
				if not ('\r\n' in ser_bytes2):
					print("No lineFeed found in Data")
					delValues()
					continue
				if not('\t' in ser_bytes2):
					print("No Tab found in Data")
					delValues()
					continue
				parts = ser_bytes2.split('\t')
				if(len(parts) < 2):
					print("recieved data does not contain enough tab positios")
					delValues()
					continue
				print("First data Check done")
				valueName = ser_bytes2.split('\t', 1)[0]
				valueData = ser_bytes2.split('\t', 1)[1]
				valueData = valueData.replace('\r\n', '')
				if(valueName == "PID"):
					dPID.append(valueData)
					countValues += 1
					dComplete += "PID\t" + valueData + "\r\n"
				elif(valueName == "FW"):
					dFW.append(valueData)
					countValues += 1
					dComplete += "FW\t" + valueData + "\r\n"
				elif(valueName == "SER#"):
					dSER.append(valueData)
					countValues += 1
					dComplete += "SER#\t" + valueData + "\r\n"
				elif(valueName == "V"):
					dV.append(int(valueData))
					countValues += 1
					dComplete += "V\t" + valueData + "\r\n"
				elif(valueName == "I"):
					dI.append(int(valueData))
					countValues += 1
					dComplete += "I\t" + valueData + "\r\n"
				elif(valueName == "VPV"):
					dVPV.append(int(valueData))
					countValues += 1
					dComplete += "VPV\t" + valueData + "\r\n"
				elif(valueName == "PPV"):
					dPPV.append(int(valueData))
					countValues += 1
					dComplete += "PPV\t" + valueData + "\r\n"
				elif(valueName == "CS"):
					dCS.append(int(valueData))
					countValues += 1
					dComplete += "CS\t" + valueData + "\r\n"
				elif(valueName == "MPPT"):
					dMPPT.append(int(valueData))
					countValues += 1
					dComplete += "MPPT\t" + valueData + "\r\n"
				elif(valueName == "OR"):
					dOR.append(valueData)
					countValues += 1
					dComplete += "OR\t" + valueData + "\r\n"
				elif(valueName == "ERR"):
					dERR.append(int(valueData))
					countValues += 1
					dComplete += "ERR\t" + valueData + "\r\n"
				elif(valueName == "LOAD"):
					if(valueData == "1"):
						dLOAD.append(int(1))
					else:
						dLOAD.append(int(0))
					countValues += 1
					dComplete += "LOAD\t" + valueData + "\r\n"
				elif(valueName == "Relay"):
					dRELAY.append(valueData)
					countValues += 1
					dComplete += "Relay\t" + valueData + "\r\n"
				elif(valueName == "H19"):
					dH19.append(int(valueData))
					countValues += 1
					dComplete += "H19\t" + valueData + "\r\n"
				elif(valueName == "H20"):
					dH20.append(int(valueData))
					countValues += 1
					dComplete += "H20\t" + valueData + "\r\n"
				elif(valueName == "H21"):
					dH21.append(int(valueData))
					countValues += 1
					dComplete += "H21\t" + valueData + "\r\n"
				elif(valueName == "H22"):
					dH22.append(int(valueData))
					countValues += 1
					dComplete += "H22\t" + valueData + "\r\n"
				elif(valueName == "H23"):
					dH23.append(int(valueData))
					countValues += 1
					dComplete += "H23\t" + valueData + "\r\n"
				elif(valueName == "HSDS"):
					dHSDS.append(int(valueData))
					countValues += 1
					dComplete += "HSDS\t" + valueData + "\r\n"
				elif(valueName == "Checksum"):
					# Check if Checksum is correct, if not trash values
					#dCHECKSUM.append(valueData)
					#countValues += 1
					print("Complete Message = ")
					print(dComplete)
					print("Received Checksum = ", end='')
					print(ord(valueData))
					print("Received Checksum readable = ", end='')
					print(repr(valueData))


					dCHECKSUM.append(ord(valueData))
					countValues += 1


					#print(len(dComplete))
					#print(repr(str(ord(valueData))))  # HIER OEFTERS MAL EIN ERROR
					#print(str(valueData))
					checksumNew = getChecksum(dComplete)
					print("Checksum Calculated = ", end = '')
					print(checksumNew)
					print("Checksum Calculated readable = ", end='')
					print(chr(checksumNew))
					#dCHECKSUM.append(checksumNew)
					#print("ChecksumNew\t" + chr(checksumNew))
					#print("Repr Checksum Received = ", end = '')
					#print(repr(dCHECKSUM[-1]))	#repr gives printable reprensentation of given object
					#print("Rpr Checksum Calculated = ", end = '')
					#print(repr(chr(checksumNew)))
					#try:
					#	print(ord(dCHECKSUM[-1]))
					#except:
					#	print("Problem:")
					#	print(dCHECKSUM[-1])
					#	time.sleep(5)
					#print("DONE")
					# If Checksum is not correct than delete all values from stack
					#print(str(checksumNew))
					#print(type(str(checksumNew)))
					#print(repr(dCHECKSUM[-1]))
					#print(type(repr(dCHECKSUM[-1])))
					if checksumNew != dCHECKSUM[-1]:	# chr(i) - Retrun the string representing a character whose Unicode code point is the interger i
					#if 0:
						print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
						print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!WRONG CHECKSUM!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
						print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
						delValues()
					else:
						dDATE.append(datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S"))
						print("CHECKSUM OK")
						
					dComplete = ""
					
					# Check amount of collected Values, if ok continue
					if(countValues == sumValues):
						print("Correct Amount of Values")
						print("Sum of collected Data packets = ", end='')
						print(len(dCHECKSUM))
						# Write Data to MySQL db
						if(len(dCHECKSUM) == 1):
							print ("Write to MySQL DataBase")
							sqlString = sqlStart
							sqlString += str(envSensorNr)
							sqlString += sqlAppend
							sqlString += str(dPID[0])# PID
							sqlString += sqlAppend
							sqlString += str(dFW[0])
							sqlString += sqlAppend
							sqlString += str(dSER[0])
							sqlString += sqlAppend
							sqlString += str(dV[0])
							sqlString += sqlAppend
							sqlString += str(dI[0])
							sqlString += sqlAppend
							sqlString += str(dVPV[0])
							sqlString += sqlAppend
							sqlString += str(dPPV[0])
							sqlString += sqlAppend
							sqlString += str(dCS[0])
							sqlString += sqlAppend
							sqlString += str(dMPPT[0])
							sqlString += sqlAppend
							sqlString += str(dOR[0])
							sqlString += sqlAppend
							sqlString += str(dERR[0])
							sqlString += sqlAppend
							sqlString += str(dLOAD[0])
							sqlString += sqlAppend
							sqlString += str(dRELAY[0])
							sqlString += sqlAppend
							sqlString += str(dH19[0])
							sqlString += sqlAppend
							sqlString += str(dH20[0])
							sqlString += sqlAppend
							sqlString += str(dH21[0])
							sqlString += sqlAppend
							sqlString += str(dH22[0])
							sqlString += sqlAppend
							sqlString += str(dH23[0])
							sqlString += sqlAppend
							sqlString += str(dHSDS[0])
							sqlString += sqlAppend
							sqlString += str(dCHECKSUM[0])
							sqlString += sqlEnd
							print(sqlString)
							cursor = connection.cursor()
							cursor.execute(sqlString)
							cursor.close()
							connection.commit()
						# Clear all Lists
						delValues()
					else:
						print("WRONG Amount of Values -> Delete Data")
						print(countValues)
						delValues()
					print("##################################################")
				else:
					print("UNKNOWN!!!!!!!!!!")
					delValues()
		else:
			if(countNoData > 5):
				stopSerial(ser)
				time.sleep(1.0)
				ser = startSerial()
				countNoData = 0
			print("No Data")
			countNoData += 1
			time.sleep(0.5)
	except:
		e = sys.exc_info()[0]
		print("ERROR:")
		print(e)
		delValues()
		#connection.close()
		#sys.exit()

