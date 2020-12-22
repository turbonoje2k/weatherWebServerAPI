  #include "DHT.h"
  #define DHTPIN 8
  #define DHTTYPE DHT11
  DHT dht(DHTPIN, DHTTYPE);//sensor declaration
  #include "SPI.h" // Libairie communicatin SPI avec le Shield éthernet
  #include "Ethernet.h" // Librairie communication Ethernet sur un réseau local 

  //give random mac address to ethernet shield
  byte mac[] = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15 }; // Adresse MAC du shield Ethernet

  //On affecte l'adresse IP du Shield Ethernet
  IPAddress ip(192,168,0, 39); // Adresse IP donné au Shield Ethernet

  // On attribue la fonction serveur au Shield Ethernet sur le port 80
  EthernetServer local_server(80); // Le Shield devient un serveur sur le port 80 (port HTTP)

  
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
        while (local_client.connected()) // tant qu'un client est connecté
        {
            if (local_client.available()) // Si un client a envoyé une requête
            {   
                char c = local_client.read(); // lire un caractère du client
                if (c == '\n' && currentLineIsBlank) // Quand la dernière ligne envoyée par le client est vide et suivi de \n on va lui répondre
                {
                    // On envoie un entête http standard en réponse
                    local_client.println("HTTP/1.1 200 OK");
                    local_client.println("Access-Control-Allow-Origin: *");
                    local_client.println("Content-Type: application/json");
                    local_client.println("Connection: close");
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

// renvoie les données des capteurs en format JSON
void getJSON_DataFromSensors(char *sDataFromSensors)
{   
    // Une structure JSON commence par une accolade
    strcpy(sDataFromSensors, "{");   
      
    // Copy la température de l'air dans la chaine de caractère JSON de retour
    char tempext[30] = "";
    double dTempExt = dht.readTemperature();
    if(!isnan(dTempExt))  // Si la température a été correctement relevé
    {
       dtostrf(dTempExt, 0, 1, tempext);
       strcat(sDataFromSensors, "\"Temperature\":\" ");
       strcat(sDataFromSensors, tempext);
       strcat(sDataFromSensors, " C \",");
    }
    else
       strcat(sDataFromSensors, "\"Temperature\":\" erreur de lecture \",");

    // Copy l'humidité de l'air dans la chaine de caractère JSON de retour
    char humidext[30] = "";
    double dHumidExt = dht.readHumidity();
    if(!isnan(dHumidExt)) // Si l'humidité a été correctement relevé
    {
       dtostrf(dHumidExt, 0, 0, humidext);
       strcat(sDataFromSensors, "\"Humidity\":\" ");
       strcat(sDataFromSensors, humidext);
       strcat(sDataFromSensors, " % \"");
    }
    else
        strcat(sDataFromSensors, "\"Humidity\":\" erreur de lecture \"");

    // Une structure JSON se termine par une accolade
    strcat(sDataFromSensors, "}");  
}
