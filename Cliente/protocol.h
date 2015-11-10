#ifndef protocolostpte_practicas_headerfile
#define protocolostpte_practicas_headerfile
#endif

// COMANDOS DE APLICACION
#define SC "USER"  // SOLICITUD DE CONEXION USER usuario 
#define PW "PASS"  // Password del usuario  PASS password

#define HE	"HELO"	// Identifica remitente
#define MA	"MAIL FROM:"	//Comienza la transacción de correo
#define RC	"RCPT TO:"	//Identifica al destinatario
#define DA	"DATA"	//El remitente esta listo para transmitir
#define SD  "QUIT"  // Finalizacion de la conexion de aplicacion
#define SD2 "EXIT"  // Finalizacion de la conexion de aplicacion y finaliza servidor

// RESPUESTAS A COMANDOS DE APLICACION
#define OK  "OK"
#define ER  "ERROR"

//FIN DE RESPUESTA
#define CRLF "\r\n"

//ESTADOS
#define S_HELO 0
#define S_MAIL 1
#define S_RCPT 2
#define S_DATA 3
#define S_MENSAJE 4
#define S_QUIT 5
#define S_EXIT 6

//PUERTO DEL SERVICIO
#define TCP_SERVICE_PORT	25