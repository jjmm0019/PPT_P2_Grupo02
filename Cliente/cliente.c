/*******************************************************
Protocolos de Transporte
Grado en Ingeniería Telemática
Dpto. Ingeníería de Telecomunicación
Univerisdad de Jaén

Fichero: cliente.c
Versión: 1.0
Fecha: 23/09/2012
Descripción:
	Cliente de eco sencillo TCP.

Autor: Juan Carlos Cuevas Martínez

*******************************************************/
#include <stdio.h>
#include <winsock.h>
#include <time.h>
#include <conio.h>

#include "protocol.h"





int main(int *argc, char *argv[])
{
	SOCKET sockfd;
	struct sockaddr_in server_in;
	char buffer_in[1024], buffer_out[1024],input[1024] , buffer[1024];
	char subject[1000], date[1000],to[1000],from[1000],fecha[1000];
	//char mensaje[1000];
	int recibidos=0,enviados=0;
	
	int estado=S_HELO;
	char option;

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	char numero1[4]="", numero2[4]=" ";
	//int contador=0;

    char ipdest[16];
	char default_ip[16]="127.0.0.1";

	//Inicialización Windows sockets
	wVersionRequested=MAKEWORD(1,1);
	err=WSAStartup(wVersionRequested,&wsaData);
	if(err!=0){
		return(0);}

	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1)
	{
		WSACleanup();
		return(0);
	}
	//Fin: Inicialización Windows sockets

	do{
		sockfd=socket(AF_INET,SOCK_STREAM,0);

		if(sockfd==INVALID_SOCKET)
		{
			printf("CLIENTE> ERROR AL CREAR SOCKET\r\n");
			exit(-1);
		}
		else
		{
			printf("CLIENTE> SOCKET CREADO CORRECTAMENTE\r\n");


			printf("CLIENTE> Introduzca la IP destino (pulsar enter para IP por defecto): ");
			gets(ipdest);
		}
			if(strcmp(ipdest,"")==0)
				strcpy(ipdest,default_ip);


			server_in.sin_family=AF_INET;
			server_in.sin_port=htons(TCP_SERVICE_PORT);
			server_in.sin_addr.s_addr=inet_addr(ipdest);

			estado=S_HELO;

			// establece la conexion de transporte
			if(connect(sockfd,(struct sockaddr*)&server_in,sizeof(server_in))==0)
			{
				printf("CLIENTE> CONEXION ESTABLECIDA CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT);

				recibidos=recv(sockfd,buffer_in,512,0);

				//Inicio de la máquina de estados
				do{
					switch(estado)
					{
					case S_HELO:
						printf("Entramos en la maquina de estados\r \n");
						sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",HE,CRLF);


						break;
					case S_MAIL:
						// establece la conexion de aplicacion
						printf("Entramos en S_MAIL\r \n");
						printf("CLIENTE> Mail from: (enter para salir): ");
						gets(input);
						if(strlen(input)==0)
						{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",SD,CRLF);
							estado=S_QUIT;
						}
						else
							sprintf_s (buffer_out, sizeof(buffer_out), "%s %s%s",MA,input,CRLF);

						break;
					case S_RCPT:
						printf("Entramos en S_RCPT\r \n");
						printf("CLIENTE> Mail to: (enter para salir): ");
						gets(input);
						if(strlen(input)==0)
						{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",SD,CRLF);
							estado=S_QUIT;
						}
						else
							sprintf_s (buffer_out, sizeof(buffer_out), "%s %s%s",RC,input,CRLF);
							
						break;
					case S_DATA:
						printf("Entramos en S_DATA\r \n");


							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",DA,CRLF);
						


						break;
					case S_MENSAJE:
						printf("Entramos en S_MENSAJE\r \n");


						//printf("Introduce fecha: \r\n");
						hora(fecha);
						sprintf(date,"date:%s%s",fecha,CRLF);
						//sprintf(buffer,"date: %s%s",date,CRLF);
						printf("Introduce asunto: \r\n");
						gets(input);
						sprintf(subject, "subject: %s%s",input,CRLF);
						printf("Introduce destinatario: \r\n");
						gets(input);
						sprintf (to, "to: %s%s",input,CRLF);
						printf("Introduce remitente: \r\n");
						gets(input);
						sprintf(from, "from: %s%s",input,CRLF);

						sprintf(buffer_out,"%s%s%s%s%s",date,subject,to,from,CRLF);
						printf("Introduce mensaje:( para finalizar introduce solo un punto) \r\n");
						//gets(input);

						do{
							gets(input);
							strcpy(buffer,input);
							strcat(buffer,CRLF);
							strcat(buffer_out,buffer);
							/******************************************************* 
							NOTA DEL PROFESOR:
							Esta comprobación está mal, debería ser: 
							strcmp(input,".\r\n") == 0 
							para asegurar que input tiene los el punto y CRLF, 
							pero la variable que deberíais haber utilzado es buffer, 
							a la que sí le concatenéis el CRLF.
							Al haber puesto ==1 en vez de ==0 sí os sale del bucle con un punto,
							pero también con un espacio o un punto, porque la comprobación es
							lexicográfica.

							Además, no podríais enviar un correo más largo de la longitud
							de buffer_out, lo cual no cumple con los objetivos.
							******************************************************/
						}while(strcmp(input,".\r\n") == 1);

						//sprintf(mensaje, "%s",input);
						//sprintf(buffer_out,"%s%s%s%s%s",date,subject,to,from,mensaje);
						//montamos el mensaje


						break;


					}//fin switch

					//Envio
					//if(estado!=S_HELO){
					// Ejercicio: Comprobar el estado de envio
					enviados=send(sockfd,buffer_out,(int)strlen(buffer_out),0);

					if(enviados<=0)
					{
						DWORD error=GetLastError();
						if(enviados<0)
						{
							printf("CLIENTE> Error %d en el envio de datos\r\n",error);
							estado=S_QUIT;
						}
						else
						{
							printf("CLIENTE> Conexión con el servidor cerrada\r\n");
							estado=S_QUIT;


						}
					}

					//Recibo
					recibidos=recv(sockfd,buffer_in,512,0);


					if(recibidos<=0)
					{
						DWORD error=GetLastError();
						if(recibidos<0)
						{
							printf("CLIENTE> Error(prueba) %d en la recepción de datos\r\n",error);
							estado=S_QUIT;
						}
						else
						{
							printf("CLIENTE> Conexión con el servidor cerrada\r\n");
							estado=S_QUIT;


						}
					}
					else
					{
						buffer_in[recibidos]=0x00;
						printf(buffer_in);
						//if(estado!=S_DATA && strncmp(buffer_in,OK,2)==0)
						if(estado==S_MENSAJE && strncmp(buffer_in,"2",1)==0){
							printf("CLIENTE> ¿Desea enviar otro mensaje?: (si/no): ");
							gets(input);
							if(strcmp(input,"si")==0){
								estado=S_MAIL;
							}
							else{

								estado=S_QUIT;}
						}
						else if (estado==S_DATA && strncmp(buffer_in,"3",1)==0){
							estado++;
						}
						else if(estado!=S_DATA && strncmp(buffer_in,"2",1)==0){
							estado++;
						}
						else{
							printf("CLIENTE> Error\r\n");
							
						}
					}

				}while(estado!=S_QUIT);



			}
			else
			{
				printf("CLIENTE> ERROR AL CONECTAR CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT);
			}
			// fin de la conexion de transporte
			closesocket(sockfd);


		printf("-----------------------\r\n\r\nCLIENTE> Volver a conectar (S/N)\r\n");
		option=_getche();

	}while(option!='n' && option!='N');



	return(0);

}

int getTimeZone(){
   TIME_ZONE_INFORMATION tziOld;
   DWORD dwRet;
   dwRet = GetTimeZoneInformation(&tziOld);
   
   if(dwRet == TIME_ZONE_ID_STANDARD || dwRet == TIME_ZONE_ID_UNKNOWN)    
		tziOld.StandardBias/60;
   else if( dwRet == TIME_ZONE_ID_DAYLIGHT )
		return tziOld.DaylightBias/60;
   else{
      printf("GTZI failed (%d)\n", GetLastError());
      return 0;
   }


}

char* hora(char* output){

	    time_t tiempo = time(0);
        struct tm *tlocal = localtime(&tiempo);
        strftime(output,128,"%d/%m/%y %H:%M:%S",tlocal);
		sprintf(output,"%s %d",output,getTimeZone());


        return output;
}
