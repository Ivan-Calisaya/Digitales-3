/*
===============================================================================
 Name        : Trabajo Final ED3.c
 Author      : Calisaya Ivan, Menendez Federico
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_gpdma.h"
//definicion de funciones
void configpio ();
void configpio2();
void configint ();
void retardo ();
void scan (int tecla);
void ingresocodigo ();
void confUart();
void transmision1 ();
void transmisio2 ();
void confiextint ();
void configADC();
void configtimer ();
void configdma ();
void Buffer_Verify(void);
//variables auxiliares
volatile uint32_t Channel0_TC;
volatile uint32_t Channel0_Err;
uint8_t j=0;
uint32_t datos [10];
uint32_t DMADest_Buffer[10];
uint32_t codigo [4]= {7,3,0,4};
uint8_t auxiliar=0;

int main() {
	configpio ();	//llamado a las funciones para configurar los perifericos
	configpio2();
	configint ();
	confiextint ();
	confUart();
	configtimer ();
	while(1) {
    	for (int i =0;i<4;i++){		//bucle para rotar la posicion de un bit de los pines del teclado
    	GPIO_SetValue(0, 1<<i);		//para asi detectar la tecla que fue presionada
    	GPIO_ClearValue(0, 1<<i);
    	}
    }
    return 0 ;
}
void configpio (){
	PINSEL_CFG_Type PIN_CONFIGURATION;
	//configuro puerto 0 pin 0 a 7 como gpio
	for (int i=0;i<8;i++){
		PIN_CONFIGURATION.Portnum=0;
		PIN_CONFIGURATION.Pinnum=i;
		PIN_CONFIGURATION.Funcnum=0; 	//configuro puerto 0 pin 0 a 7 como gpio
		PINSEL_ConfigPin(&PIN_CONFIGURATION);
	}
	// Configuración pines ADC
		PIN_CONFIGURATION.Portnum =0;	//puerto 0
		PIN_CONFIGURATION.Pinnum=25;	//pin 25
		PIN_CONFIGURATION.Funcnum=1;	//funcion adc
		PIN_CONFIGURATION.OpenDrain=0;	//sin resistencias de pull-up y pull-down
		PIN_CONFIGURATION.Pinmode=0;
		PINSEL_ConfigPin (&PIN_CONFIGURATION);

	//configuracion pines comunicacion serie
		PIN_CONFIGURATION.Funcnum = 1;		//funcion uart
		PIN_CONFIGURATION.OpenDrain = 0;	//sin resistencias de pull-up y pull-down
		PIN_CONFIGURATION.Pinmode = 0;
		PIN_CONFIGURATION.Pinnum = 10;	//pin 10
		PIN_CONFIGURATION.Portnum = 0;	//puerto 0
		PINSEL_ConfigPin(&PIN_CONFIGURATION);
		PIN_CONFIGURATION.Pinnum = 11;	//pin 11
		PINSEL_ConfigPin(&PIN_CONFIGURATION);
	for (int i=0;i<8;i++){
			GPIO_ClearValue(0, 1<<i);	//limpio los bits 0 a 7
		if (i<4){
			GPIO_SetDir(0, 1<<i, 1); //coloco los bit 0-3 del puerto 0 como salida
		}
		else {
			GPIO_SetDir(0,1<<i, 0);//coloco los bits 4-7 del puerto 0 como entradas
		}
	}
	for (int i=0;i<8;i++){
			GPIO_SetValue(2, 0<<i);	//coloco los bits 0 a 7 del puerto 2 como saalida
	}
	return;
}
void configint (){
	GPIO_IntCmd(0,0xF0<<0, 0); //habilito interrupcion por gpio por flanco de subida en los pines P0.4 a P0.7

	GPIO_ClearInt(0,1<<4);		//limpio la bandera de interrupcion del bit 4
	GPIO_ClearInt(0,1<<5);		//limpio la bandera de interrupcion del bit 5
	GPIO_ClearInt(0,1<<6);		//limpio la bandera de interrupcion del bit 6
	GPIO_ClearInt(0,1<<7);		//limpio la bandera de interrupcion del bit 7
	NVIC_EnableIRQ(EINT3_IRQn); //habilito el vector por gpio
	return;
}

void configpio2 (){
	PINSEL_CFG_Type PIN_CONFIGURATION;

		for (int i=0;i<10;i++){
			PIN_CONFIGURATION.Portnum=2;	//Puerto 2
			PIN_CONFIGURATION.Pinnum=i;		//Numero de pin i
			PIN_CONFIGURATION.Funcnum=0; 	//configuro puerto 2 pin 0 a 8 como gpio
			PINSEL_ConfigPin(&PIN_CONFIGURATION);//cargo los valores configurados
		}

		GPIO_ClearValue(2, 0x3F<<0);

		PIN_CONFIGURATION.Portnum=2;	//Puerto 2
		PIN_CONFIGURATION.Pinnum=11;	// Numero de pin 11
		PIN_CONFIGURATION.Funcnum=1;	//configuro puerto para interrupcion externa
		PINSEL_ConfigPin(&PIN_CONFIGURATION);//cargo los valores configurados
		GPIO_SetDir(2, 1<<11, 0);	//coloco los bits 11 del puerto 2 como entrada
		for (int i=0;i<10;i++){
			GPIO_SetDir(2, 1<<i, 1); //coloco los bit 0-10 del puerto 2 como salida
		}
		GPIO_SetValue(2, 1<<8); //BIT 8 DEL PUERTO 2 ALARMA DESACTIVADA
		return;
}
void confiextint (){
	EXTI_InitTypeDef extint;
	extint.EXTI_Line=EXTI_EINT1;	//selecciono interrupcion externa eint1
	extint.EXTI_Mode=EXTI_MODE_EDGE_SENSITIVE;	//selecciono  por flanco
	extint.EXTI_polarity=EXTI_POLARITY_HIGH_ACTIVE_OR_RISING_EDGE;	//selecciono flanco de subida
	EXTI_Config (&extint);//cargo los valores configurados
	EXTI_ClearEXTIFlag(EXTI_EINT1_BIT_MARK);	//limpio la bandera de interrupcion
	NVIC_EnableIRQ(EINT1_IRQn);					//habilito el vector de interrupcion
	return;
}
void confUart(){
	UART_CFG_Type UARTConfigStruct;
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
	UART_ConfigStructInit(&UARTConfigStruct);	//configuracion por defecto:
	UART_Init(LPC_UART2, &UARTConfigStruct);	//inicializa periferico
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	UART_FIFOConfig(LPC_UART2, &UARTFIFOConfigStruct);	//Inicializa FIFO
	UART_TxCmd(LPC_UART2, ENABLE); 		//Habilita transmision
	return;
}

void EINT1_IRQHandler (){
	if(FIO_ByteReadValue(2,0) & (1<<7)){	//pregunto si la alarma esta activada
	GPIO_SetValue(2, 1<<9); //BIT CON EL QUE SE VA A ENCENDER LA SIRENA
	transmision1();
	}
	EXTI_ClearEXTIFlag(EXTI_EINT1_BIT_MARK);	//limpio bandera de interrupcion
	return;
}
void EINT3_IRQHandler (){

	if (GPIO_GetIntStatus(0, 4, 0)){	//pregunto si la interrupcion es por el bit 4
		if (FIO_ByteReadValue(0, 0)&(1<<0)){	//pregunto si fue la tecla 1
			GPIO_SetValue(2, 0X06<<0); // Pongo 1 (Funciona)
			scan (1);
			retardo();	//apago led de display
		}
		if (FIO_ByteReadValue(0, 0)&(1<<1)){	//pregunto si fue la tecla 4
			GPIO_SetValue(2, 0X66<<0); // Pongo 4 (Funciona)
			scan (4);
			retardo();	//apago led de display
		}
		if (FIO_ByteReadValue(0, 0)&(1<<2)){	//pregunto si fue la tecla 7
			GPIO_SetValue(2, 0X07<<0); // Pongo 7 (Funciona)
			scan (7);
			retardo();	//apago led de display
		}
	}
	if (GPIO_GetIntStatus(0, 5, 0)){
		if (FIO_ByteReadValue(0, 0)&(1<<0)){	//pregunto si fue la tecla 2
			GPIO_SetValue(2, 0X5B<<0); // Pongo 2 (Funciona)
			scan (2);
			retardo();	//apago led de display
		}
		if (FIO_ByteReadValue(0, 0)&(1<<1)){	//pregunto si fue la tecla 5
			GPIO_SetValue(2, 0X6D<<0); // Pongo 5 (Funciona)
			scan (5);
			retardo();	//apago led de display
		}
		if (FIO_ByteReadValue(0, 0)&(1<<2)){	//pregunto si fue la tecla 8
			GPIO_SetValue(2, 0X7F<<0); // Pongo 8 (Funciona)
			scan (8);
			retardo();	//apago led de display
		}
		if (FIO_ByteReadValue(0, 0)&(1<<3)){	//pregunto si fue la tecla 0
			GPIO_SetValue(2, 0X3F<<0); // Pongo 0 (Funciona)
			scan (0);
			retardo();	//apago led de display
		}
	}
	if (GPIO_GetIntStatus(0, 6, 0)){	//pregunto si fue la tecla 3
		if  (FIO_ByteReadValue(0, 0)&(1<<0)){
			GPIO_SetValue(2, 0X4F<<0); // Pongo 3 (Funciona)
			scan (3);
			retardo();	//apago led de display
			}
		if (FIO_ByteReadValue(0, 0)&(1<<1)){	//pregunto si fue la tecla 6
			GPIO_SetValue(2, 0X7D<<0); // Pongo 6 (Funciona)
			scan (6);
			retardo();	//apago led de display
		}
		if (FIO_ByteReadValue(0, 0)&(1<<2)){	//pregunto si fue la tecla 9
			GPIO_SetValue(2, 0X67<<0); // Pongo 9 (Funciona)
			scan (9);
			retardo();	//apago led de display
		}
	}
	GPIO_ClearInt(0,1<<4);		//limpio la bandera de interrupcion del bit 4
	GPIO_ClearInt(0,1<<5);		//limpio la bandera de interrupcion del bit 5
	GPIO_ClearInt(0,1<<6);		//limpio la bandera de interrupcion del bit 6
	GPIO_ClearInt(0,1<<7);		//limpio la bandera de interrupcion del bit 7
	return;
}

// Retardo para que se apague el display
void retardo () {
    for (uint8_t bit=0; bit<7; bit++){
	   GPIO_ClearValue(2, 1<<bit);
   }
   return;
}
void scan (int tecla){
	// 7304
	if ((tecla==(codigo[0])) & (auxiliar==0)){	//pregunto si la tecla pulsada es la primera del codigo
		auxiliar++; // variable auxiliar para tener en cuenta la posicion
		return;
	}
	if ((tecla==(codigo[1])) & (auxiliar==1)){	//pregunto si la tecla pulsada es la segunda del codigo
		auxiliar++;	// variable auxiliar para tener en cuenta la posicion
		return;
	}
	if ((tecla==(codigo[2])) & (auxiliar==2)){	//pregunto si la tecla pulsada es la tercera del codigo
		auxiliar++;	// variable auxiliar para tener en cuenta la posicion
		return;
	}
	if ((tecla==(codigo[3])) & (auxiliar==3)){	//pregunto si la tecla pulsada es la cuarta del codigo
		if (FIO_ByteReadValue(2,1) & (1<<0)){	//pregunto si el la alarma esta desactivada
			configADC();			//configuro adc
			GPIO_SetValue(2, 1<<7);		 //prendo led para indicar que la alarma esta activada
			GPIO_ClearValue(2, 1<<8);	//apago el led indicador de alarma desactivada
			auxiliar=0;	// variable auxiliar para tener en cuenta la posicion
			return;
		}
		if (FIO_ByteReadValue(2,0) & (1<<7)){
			GPIO_ClearValue(2, 1<<7);	//apago el led indicador de alarma activada
			GPIO_SetValue(2, 1<<8);		//prendo led para indicar que la alarma esta desactivada
			auxiliar=0;	// variable auxiliar para tener en cuenta la posicion
			GPIO_ClearValue(2, 1<<9); //BIT CON EL QUE SE VA A APAGAR LA SIRENA
			return;
		}
	}
	else{	// en caso de que se ingreso erroneamente una tecla del codigo se vuelve a comenzar
		auxiliar=0;	// variable auxiliar para tener en cuenta la posicion
		return;
	}
}
void transmision1 (){
	uint8_t info1[] = "alarmas grupo 4\t-\tElectr?nica Digital 3\t-\tFCEFyN-UNC \n\r";
	uint8_t info2[]= "su alarma se ha activado\t-\estamos enviando personal de seguridad a su domicilio\n\r";
	UART_Send(LPC_UART2, info1, sizeof(info1), BLOCKING);
	UART_Send(LPC_UART2, info2, sizeof(info2), BLOCKING);
	return;
}
void transmision2 (){
	uint8_t info1[]= "alarmas grupo 4\t-\tElectr?nica Digital 3\t-\tFCEFyN-UNC \n\r";
	uint8_t info2[]= "su alarma ha detectado un incendio\t-\estamos enviando personal\n\r";
	uint8_t info3[]= "de seguridad y bomberos a su domicilio\n\r";
	UART_Send(LPC_UART2, info1, sizeof(info1), BLOCKING);
	UART_Send(LPC_UART2, info2, sizeof(info2), BLOCKING);
	UART_Send(LPC_UART2, info3, sizeof(info3), BLOCKING);
	return;
}

void configADC(){
	ADC_Init(LPC_ADC, 200000);	//enciendo el adc y selecciono la frecuencia
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN2, ENABLE);	//activo interrupcion por canal 2
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_2, ENABLE);	//activo el canal 2
	ADC_StartCmd(LPC_ADC,ADC_START_ON_MAT01);		//habilito para que empiece la conversion con el match
	ADC_EdgeStartConfig(LPC_ADC,ADC_START_ON_FALLING);//por flanco descendente
	ADC_BurstCmd (LPC_ADC,DISABLE); //deshabilito el mmodo burst
	NVIC_EnableIRQ(ADC_IRQn);	//habilito el vector de interrupcion
	return;
}
void configtimer (){
	TIM_TIMERCFG_Type TIMER_CONFIGURATION;
	TIMER_CONFIGURATION.PrescaleOption=TIM_PRESCALE_USVAL;// selecciono al prescaler en microsegundos
	TIMER_CONFIGURATION.PrescaleValue= 15;	//cargo con el valor
	TIM_MATCHCFG_Type MATCH_CONFIGURATION;
	MATCH_CONFIGURATION.ResetOnMatch=ENABLE;	//habilito el reset al ocurrir el match
	MATCH_CONFIGURATION.MatchChannel=1;	//selecciono el canal
	MATCH_CONFIGURATION.MatchValue=1000000;	//cargo el valor del match
	MATCH_CONFIGURATION.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;//haga un toggle cuando el match ocurre
	MATCH_CONFIGURATION.StopOnMatch=DISABLE;	//selecciono para que no se detenga luego del match
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &TIMER_CONFIGURATION);//cargo los valores configurados
	TIM_ConfigMatch(LPC_TIM0, &MATCH_CONFIGURATION);//cargo los valores configurados
	TIM_Cmd(LPC_TIM0, ENABLE);	//habilito el timer0
	return;
}
void configdma (){
	GPDMA_Channel_CFG_Type GPDMACfg;
	NVIC_DisableIRQ(DMA_IRQn);//deshabilito el vector de interrupciones
	GPDMA_Init();
	GPDMACfg.ChannelNum = 0;	//selecciono el canal del dma
	GPDMACfg.SrcMemAddr = (uint32_t) datos ;	//determino la fuente
	GPDMACfg.DstMemAddr = (uint32_t) DMADest_Buffer;//determino el destino
	GPDMACfg.TransferSize = 10;	//cantidad de datos a transferir
	GPDMACfg.TransferWidth = GPDMA_WIDTH_WORD; //tamaño de palabra de 16 bits
	GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2M; //transferencia de memoria a memoria
	GPDMACfg.SrcConn = 0 ;
	GPDMACfg.DstConn = 0;
	GPDMACfg.DMALLI = 0;
	GPDMA_Setup(&GPDMACfg);

	return;
}
void ADC_IRQHandler(){
	if(FIO_ByteReadValue(2,0) & (1<<7)){ //pregunto si la alarma esta activada
	static uint16_t ADC0Value;
	uint16_t aux=0;
	ADC0Value = ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_2);
	datos [j]=ADC0Value; //cargo en un array los datos obtenidos del adc
	j++;
	if (j>0){
			aux = datos [j-1]-datos [j-2];	//resto los valores para verificar si se produce un incendio
	}

	if (aux>2000){	//si se produce un gran cambio en el valor ingreso
		j=0;
		for(int w=0; w < 10; w++){ //limpio el array de datos
			datos [w] =0;
		}
		transmision2();	//realizo una transmision de que se detecto el incendio
		ADC_DeInit(LPC_ADC);	//apago el adc
		//llamar a la funcion transmision2 ya que se ha detectado incendio
	}

	if (j>10){	//pregunto si se lleno el array
		configdma ();	//configuro el dma
		Channel0_TC = 0;
		Channel0_Err = 0;
		GPDMA_ChannelCmd(0, ENABLE);	//habilito el canal de dma
		NVIC_EnableIRQ(DMA_IRQn);	//hbilito el vector de interrupcion
		while ((Channel0_TC == 0) && (Channel0_Err == 0)); //espero que el dma complete el proceso
		Buffer_Verify();
		datos[0]=datos[10]; //al ultimo dato lo coloco en la posicion 0
		j=1;
	}
	}
	return;

}
void DMA_IRQHandler (void)
{
	// check GPDMA interrupt on channel 0
	if (GPDMA_IntGetStatus(GPDMA_STAT_INT, 0)){ //check interrupt status on channel 0
		// Check counter terminal status
		if(GPDMA_IntGetStatus(GPDMA_STAT_INTTC, 0)){
			// Clear terminate counter Interrupt pending
			GPDMA_ClearIntPending (GPDMA_STATCLR_INTTC, 0);
			Channel0_TC++;
		}
		if (GPDMA_IntGetStatus(GPDMA_STAT_INTERR, 0)){
			// Clear error counter Interrupt pending
			GPDMA_ClearIntPending (GPDMA_STATCLR_INTERR, 0);
			Channel0_Err++;
		}
	}
	return;
}
void Buffer_Verify(void)
{
	uint8_t i;
	uint32_t *src_addr = (uint32_t *)datos;
	uint32_t *dest_addr = (uint32_t *)DMADest_Buffer;
	for ( i = 0; i < 10; i++ )
	{
		if ( *src_addr++ != *dest_addr++ )
		{
			while(1){}
		}
	}
	return;
}
