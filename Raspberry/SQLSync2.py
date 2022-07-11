import mysql.connector as mysql
import pandas as pd
import sys


chunkSize = 5000
#skipTables = ['SolarData','Batt_V_old']
skipTables = ['Batt_V_old','Batt_V','Batt_T','BME280_T_2','solarChargeState']
# Synchronize martiaDB local to online DB (1blu)
# Date for program
#hostDbSource = '192.168.178.50'
hostDbSource = 'localhost'
portDbSource = '3306'
loginSource = 'penelopeRead'
passwSource = '83f4cb09061f084ad40286e015470907eae8934f2b6dc5305b21619b3ac72e49'
nameDbSource = 'Penelope'

#hostDbTarget = 'www.harangozo.info'
hostDbTarget = 'mysql.webhosting50.1blu.de'
portDbTarget = '3306'
loginTarget = 's282917_3134306'
passwTarget = 'oPEeTu8)0!#Agz0D%cY@&#Px?'
nameDbTarget = 'db282917x3134306'

def extractSqlAnswer(queryResults):
	listOfResults = []
	for x in queryResults:
		listOfResults.append(x[0])
	return listOfResults

def readSQL(host, port, login, passw, dbName, sqlString):
	connection = mysql.connect(host = host, user = login, passwd = passw, db = dbName)
	cursor = connection.cursor()
	cursor.execute(sqlString)
	myresult = cursor.fetchall()
	cursor.close()
	#connection.sommit()
	connection.close()
	return myresult
	#for x in myresult:
	#	print(x)

def readSQLMulti(host, port, login, passw, dbName, sqlString):
	connection = mysql.connect(host = host, user = login, passwd = passw, db = dbName)
	df = pd.read_sql(sqlString, connection)
	connection.close()
	return df
		
def writeSQL(host, port, login, passw, dbName, sqlString):
	connection = mysql.connect(host = host, user = login, passwd = passw, db = dbName)
	cursor = connection.cursor()
	cursor.execute(sqlString)
	#myresult = cursor.fetchall()
	cursor.close()
	connection.commit()
	connection.close()

def writeSQLMulti(host, port, login, passw, dbName, tableName, df):
	connection = mysql.connect(host = host, user = login, passwd = passw, db = dbName)
	result = df.to_sql(tableName, connection, if_exists='fail')
	print("To_SQL answe = ", end='')
	print(result)
	connection.close()
	return df
	
def dfToSqlString(df, tableName):
	#INSERT INTO 'BME_280_T_test' ('Id', 'EnvSensoNr', 'Temperature', 'Humidity', 'Pressure', 'Altitude', 'timestap') VALUES (1,3, '13.34', 'XX', 'XX'; 'XX', 'XX', '2020-12-18 07:40:36'), (1,3, '13.34', 'XX', 'XX'; 'XX', 'XX', '2020-12-18 07:40:36')
	cols = df.columns
	sqlString  = "INSERT INTO " + tableName + " ("
	for col in cols:
		#sqlString += "'"
		sqlString += col
		#sqlString += "'"
		sqlString += ","
	# Remove last ,
	sqlString = sqlString[:-1]
	sqlString += ") VALUES "
	for index, row in df.iterrows():
		sqlString +="("
		for col in cols:
			sqlString += "'"
			sqlString += str(row[col])
			sqlString += "',"
		sqlString = sqlString[:-1]
		sqlString += "),"
	sqlString = sqlString[:-1]
	return sqlString


# Get tables local
sqlString = "SELECT table_name FROM information_schema.tables WHERE table_schema = '" + nameDbSource + "';"
result = readSQL(hostDbSource, portDbSource, loginSource, passwSource, nameDbSource, sqlString)
listTablesSource = extractSqlAnswer(result)
print("Tables in Source DB = ", end='')
print(listTablesSource)

# Get tables target
sqlString = "SELECT table_name FROM information_schema.tables WHERE table_schema = '" + nameDbTarget + "';"
result = readSQL(hostDbTarget, portDbTarget, loginTarget, passwTarget, nameDbTarget, sqlString)
listTablesTarget = extractSqlAnswer(result)
print("Tables in Target DB = ", end='')
print(listTablesTarget)
print("----------------------------------------")

#listTablesSource = ['SolarData']

# Check every table last id
for table in listTablesSource:
	if table in skipTables:
		print("Table " + table + " not Processed - table available in Skip list")
		print("----------------------------------------")
		continue
	if table in listTablesTarget:
		print("Table " + table + " Exists in Target DB")
		sqlString = "SELECT Id FROM " + table + " ORDER BY Id DESC LIMIT 1"
		result = readSQL(hostDbTarget, portDbTarget, loginTarget, passwTarget, nameDbTarget, sqlString)
		if not result:
			print("Target Table is empty")
			lastIdTarget = 0
		else:
			lastIdTarget = extractSqlAnswer(result)[0]
		print("Last Id of " + table + " = " + str(lastIdTarget))
		# Check last Id of Source db
		sqlString = "SELECT Id FROM " + table + " ORDER BY Id DESC LIMIT 1"
		print(sqlString)
		result = readSQL(hostDbSource, portDbSource, loginSource, passwSource, nameDbSource, sqlString)
		lastIdSource = extractSqlAnswer(result)[0]
		print("Last Id of " + table + " = " + str(lastIdSource))
		if lastIdTarget < lastIdSource:
			idTmp = lastIdTarget + 1
			print("Data of table needes to be mirrored")
			print("Start Row = " + str(lastIdTarget+1))
			# ReadChunk
			sqlString = "SELECT * FROM " + table + " WHERE Id >= " + str(idTmp) + " AND Id < " + str(idTmp + chunkSize) + " ORDER BY Id"
			dfResult = readSQLMulti(hostDbSource, portDbSource, loginSource, passwSource, nameDbSource, sqlString)
			# WriteChunk
			run = 0
			# If Table begins not with Id 1
			while dfResult.empty and (idTmp+chunkSize < lastIdSource):
				print("No Ids available between Id " + str(idTmp) + " - " + str(idTmp + chunkSize))
				idTmp += chunkSize
				sqlString = "SELECT * FROM " + table + " WHERE Id >= " + str(idTmp) + " AND Id < " + str(idTmp + chunkSize) + " ORDER BY Id"
				dfResult = readSQLMulti(hostDbSource, portDbSource, loginSource, passwSource, nameDbSource, sqlString)
				
			while not dfResult.empty:
				print("Write Row: " + str(idTmp) + " - " + str(idTmp+dfResult.shape[0]))
				sqlString = dfToSqlString(dfResult, table)
				#print(sqlString)
				#WriteToSQL
				writeSQL(hostDbTarget, portDbTarget, loginTarget, passwTarget, nameDbTarget, sqlString)
				idTmp += chunkSize
				sqlString = "SELECT * FROM " + table + " WHERE Id >= " + str(idTmp) + " AND Id < " + str(idTmp + chunkSize) + " ORDER BY Id"
				dfResult = readSQLMulti(hostDbSource, portDbSource, loginSource, passwSource, nameDbSource, sqlString)
				run += 1
				if(run == 2):
					#sys.exit()
					pass
			#writeSQLMulti(hostDbTarget, portDbTarget, loginTarget, passwTarget, nameDbTarget, table, dfResult)
			#print(dfResult.dtypes)
			#INSERT INTO 'BME_280_T_test' ('Id', 'EnvSensoNr', 'Temperature', 'Humidity', 'Pressure', 'Altitude', 'timestap') VALUES (1,3, '13.34', 'XX', 'XX'; 'XX', 'XX', '2020-12-18 07:40:36'), (1,3, '13.34', 'XX', 'XX'; 'XX', 'XX', '2020-12-18 07:40:36')
			
		else:
			print("No Action required")
		
	else:
		print("Table " + table + " does NOT Exists in Target DB")
	print("----------------------------------------")
	

# Check if table exists online - if not create

# Check if table has same columns - if not create

# Synchronize only ids which are not in online table


# Main Prog
#sqlString = "SELECT * FROM sensor LIMIT 10"

#readSQL(hostDbSource, portDbSource, loginSource, passwSource, nameDbSource, sqlString)

