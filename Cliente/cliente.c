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
	char subject[1000], date[1000],to[1000],from[1000],mensaje[1000];
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


						printf("Introduce fecha: \r\n");
						gets(input);
						sprintf (date,"date: %s%s",input,CRLF);
						printf("Introduce asunto: \r\n");
						gets(input);
						sprintf(subject, "subject: %s%s",input,CRLF);
						printf("Introduce destinatario: \r\n");
						gets(input);
						sprintf (to, "to: %s%s",input,CRLF);
						printf("Introduce remitente: \r\n");
						gets(input);
						sprintf(from, "from: %s%s",input,CRLF);
						printf("Introduce mensaje: \r\n");
						gets(input);
						sprintf(mensaje, "\r\n %s%s",input,CRLF);
						sprintf(buffer_out,"%s%s%s%s%s %s.%s",date,subject,to,from,mensaje,CRLF,CRLF);
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
