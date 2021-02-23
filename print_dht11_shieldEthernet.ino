  // Library for sensor DHT 11
  #include "DHT.h"
  // Libary communication SPI with the ethernet Shield
  #include "SPI.h" 
  // Library communication with LAN 
  #include "Ethernet.h" 

  #define DHTPIN 8
  #define DHTTYPE DHT11
  //sensor declaration
  DHT dht(DHTPIN, DHTTYPE);
  
  //give random mac address to ethernet shield
  byte mac[] = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15 }; 

  //give ip address to ethernet shield
  IPAddress ip(192,168,0, 39); // ethernet Shield IP

  // open port 80
  EthernetServer local_server(80); // Shield become a server on the port 80 (port HTTP)

  
void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  // DHT sensor begin
  dht.begin();

  // DHT data
  int temperature=0;
  int humidity=0;
  
  // Start ethernet
  Serial.println("Getting IP");
  Ethernet.begin(mac, ip);
  Serial.println(Ethernet.localIP());

  // Our ethernet client
  EthernetClient client;
  local_server.begin();

}

void loop()
{
    respond_local_server();  
}

void respond_local_server()
{
    EthernetClient local_client = local_server.available();

    if (local_client) 
    {  
        boolean currentLineIsBlank = true;
        while (local_client.connected()) // while connected client
        {
            if (local_client.available()) // if client send request
            {   
                char c = local_client.read(); // read client char
                if (c == '\n' && currentLineIsBlank) // Quand la dernière ligne envoyée par le client est vide et suivi de \n on va lui répondre
                {
                    // send standard html header 
                    local_client.println("HTTP/1.1 200 OK");
                    local_client.println("Access-Control-Allow-Origin: *");
                    local_client.println("Content-Type: application/json");
                    local_client.println("Connection: close");
                    local_client.println("Refresh: 5");
                    local_client.println();

                    // On envoie la valeur des capteur en format JSON
                    char sPostData[150] = "";
                    getJSON_DataFromSensors(sPostData);
                    local_client.println(sPostData);

                    break;
                }
                
                // every line of text received from the client ends with \r\n
                if (c == '\n') // Si le caractère reçu est \n on positionne la variable currentLineIsBlank à vrai
                    currentLineIsBlank = true;
                else 
                    if (c != '\r') // Si le caractère reçu est \r on positionne la variable currentLineIsBlank à faux
                        currentLineIsBlank = false;
            } 
        } 
        
        delay(1000);      // On laisse 1s au browser pour récupérer les données qu'on a envoyées
        local_client.stop(); // ferme la connection
    }
}

// send data on JSON 
void getJSON_DataFromSensors(char *sDataFromSensors)
{   
    // start struct JSON with {
    strcpy(sDataFromSensors, "{");   
      
    // Copy temperature readi in json string
    char tempext[30] = "";
    double dTempExt = dht.readTemperature();
    if(!isnan(dTempExt))  // if temp correctly read 
    {
       dtostrf(dTempExt, 0, 1, tempext);
       strcat(sDataFromSensors, "\"Temperature\":\" ");
       strcat(sDataFromSensors, tempext);
       strcat(sDataFromSensors, "  \",");
    }
    else
       strcat(sDataFromSensors, "\"Temperature\":\" /!\ reading error /!\ \");

    // Copy humydity reading in a json  string
    char humidext[30] = "";
    double dHumidExt = dht.readHumidity();
    if(!isnan(dHumidExt)) // if humidity correctly read
    {
       dtostrf(dHumidExt, 0, 0, humidext);
       strcat(sDataFromSensors, "\"Humidity\":\" ");
       strcat(sDataFromSensors, humidext);
       strcat(sDataFromSensors, "  \"");
    }
    else
        strcat(sDataFromSensors, "\"Humidity\":\"  /!\ reading error /!\ \");

    // stop struct JSON with }
    strcat(sDataFromSensors, "}");  
}
