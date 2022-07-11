//#include <SoftwareSerial.h>
//SoftwareSerial mySerial(2,3);
SoftwareSerial mySerial(4,5);

int generateChecksumPostValue(String stringData){
  // calculates the last 8-bit hex value for checksum -> sum of all chars (incl. checksum) gives 0
  uint8_t sumAll = 0;
  uint8_t rest = 0;
//  Serial.print("Length = ");
//  Serial.println(stringData.length());
//  Serial.print("Data = ");
//  Serial.println(stringData);
  for(int i=0; i<stringData.length(); i++){
    sumAll = sumAll + char(stringData[i]);
//    Serial.print(stringData[i]);
//    Serial.print("----");
//    Serial.print(stringData[i],DEC);
//    Serial.print("----");
//    Serial.println(sumAll,DEC);
  }
  rest = 255 - sumAll;
//  Serial.print("Sum of all chars = ");
//  Serial.println(sumAll);
//  Serial.print("value to add to array = ");
//  Serial.println(rest);
  return rest;
}

void sendSqlStatement(String preChar, int countValues, float values[], int decPrecision){
  uint8_t checksum = 0;
  String sqlStatement;
  
  //sprintf(sqlQuery, INSERT_SQL_V, voltageValue[0], voltageValue[1], voltageValue[2], voltageValue[3], voltageValue[4], voltageValue[5], voltageValue[6], voltageValue[7], voltageValue[8]);
  Serial.println("Start");
  mySerial.println("Start");

  //sqlStatement += preChar;
  for (int i = 0; i < countValues; i++){
    //char result[completeLength];
    //dtostrf(values[i], completeLength, lengthAfterComma, result);
    //sqlStatement += String(values[i],decPrecision);
    sqlStatement.concat(String(values[i],decPrecision));
    if(i < countValues - 1){
      sqlStatement += String(",");
    }
    else{
      sqlStatement += String(")");
    }
  }
  
  //preChar.concat(sqlStatement);
  //Serial.println(preChar);

  Serial.print(preChar);
  Serial.println(sqlStatement);
  mySerial.print(preChar);
  mySerial.println(sqlStatement);
  
  checksum = generateChecksumPostValue(preChar);
  checksum += generateChecksumPostValue(sqlStatement);
  Serial.println(checksum);
  mySerial.println(checksum);
  //sqlStatement = String(preChar) + sqlStatement;
  //Serial.println(sqlStatement);
  //Serial.println(generateChecksumPostValue(preChar));
  //mySerial.println(generateChecksumPostValue(completeStatement);
  
  Serial.println("Stop");
  mySerial.println("Stop");
}
