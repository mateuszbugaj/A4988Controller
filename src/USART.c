#include "inc/USART.h"

uint8_t usartIndex = 0;
bool commandProcessed;

void usartInit(void) {
    UBRR0H = UBRRH_VALUE;                    // Set USART baud rate
    UBRR0L = UBRRL_VALUE;
    // UCSR0A |= (1 << U2X0);                   // Use double speed mode
    UCSR0B |= (1 << TXEN0) | (1 << RXEN0);   // Enable USART transmitter and receiver
    UCSR0B |= (1 << RXCIE0);                 // Receive complete interrut enable
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // Set the size of data in frame to 8 bits

    sei();
}

void transmitByte(unsigned char data) {
    loop_until_bit_is_set(UCSR0A, UDRE0); // while ( ! ( (port) & (1 << (bitnum)) ) ) ;
    UDR0 = data;
}

void usartPrint(char* content){
    uint8_t i = 0;
    while (content[i]) {
        transmitByte(content[i]);
        i++;
    }
}

void usartPrintNum(int16_t number){
    uint8_t digit = 0;
    char buffer[5] = {0};

    if(number == 0){ 
        transmitByte('0');
        return;
    }

    if(number < 0){
        transmitByte('-');
        number *= -1;
    }

    while(number){
        buffer[digit++] = '0' + (number%10);
        number /= 10;
    }

    while(digit > 0){
        transmitByte(buffer[digit-1]);
        digit--;
    }
}

void usartPrintFloat(float number, int precision){
    usartPrintNum(number);
    usartPrint(".");

    if(number > 0){
        int numberInt = number;
        int rest = number*precision - numberInt*precision;
        usartPrintNum(rest);
    } else {
        int numberInt = number * -1;
        int rest = number * -1 * precision - numberInt*precision;
        usartPrintNum(rest);
    }

}

void clear(char* arr, uint8_t size){
    while(size--){
        arr[size] = 0;
    }
}

uint8_t strLen(char str[]){
    uint8_t len = 0;
    while(str[len] != '\0') len++;
    return len;
}

bool usartCompareStr(char* s1, char* s2){
    int index = 0;
    while(s1[index] != '\0'){ 
        if(s1[index] != s2[index]) return 0;
        index++;
    }

    int len1 = index;
    index = 0;

    while(s2[index] != '\0') index++;
    if(len1 != index) return false;

    return true;
}

bool usartContatinsStr(char* s1, char* s2){
    int index = 0;
    int counter = 0;
    if(strLen(s1) < strLen(s2)) return false;

    while(s1[index] != '\0'){ 
        if(s1[index] == s2[counter])counter++; else counter = 0;
        if(counter == strLen(s2)) return true;
        index++;
    }

    return false;
}

bool isDigit(char c){
    if(c >= '0' && c <= '9') return true;
    return false;
}

bool contains(char str[], char c){
    uint8_t index = 0;
    while(str[index]){
        if(str[index] == c) return true;
        index++;
    } 

    return false;
}

float getNumber(char str[]){
    uint8_t i = strLen(str);
    int16_t wholeNumberMultiplier = str[0] == '-'? -1 : 1;
    int16_t decimalNumberMultiplier = 1;
    int16_t wholeNumber = 0;
    float decimalNumber = 0;
    float resultingNumber = 0;
    bool decimalNumberPortion = contains(str, '.');

    while (i--) {
        uint8_t byte = str[i];
        if(byte == '.'){
            decimalNumberPortion = false;
            continue;
        }

        if(decimalNumberPortion == false){
            if(!isDigit(byte)) byte = '0';
            wholeNumber += (byte - '0') * wholeNumberMultiplier;
            wholeNumberMultiplier *= 10;
        } else {
            if(!isDigit(byte)) byte = '0';
            decimalNumber += (byte - '0') * decimalNumberMultiplier;
            decimalNumberMultiplier *= 10;
        }
    }

    if(decimalNumber != 0){
        if(wholeNumberMultiplier > 0){
            resultingNumber = wholeNumber + decimalNumber/decimalNumberMultiplier;
        } else {
            resultingNumber = wholeNumber - decimalNumber/decimalNumberMultiplier;
        }
    } else {
        resultingNumber = wholeNumber;
    }

    return resultingNumber;
}

int16_t* usartGetNumbers(char str[], float* buffer){
    uint8_t i = 0;
    uint8_t numberIndex = 0;
    uint8_t digitIndex = 0;
    bool numberReading = false;
    char tempBuffer[6] = {0};

    while(str[i]){
        if(isDigit(str[i]) || str[i] == '-' || str[i] == '.'){
            numberReading = true;
            tempBuffer[digitIndex] = str[i];
            digitIndex++;
        } else if(numberReading){
            buffer[numberIndex] = getNumber(tempBuffer);
            clear(tempBuffer, 6);
            digitIndex = 0;
            numberReading = 0;
            numberIndex++;
        }
        i++;
    }

    if(numberReading){
        buffer[numberIndex] = getNumber(tempBuffer);
        clear(tempBuffer, 5);
    }

    return buffer;
}

/* Interrupt routine activated by the incoming data */
ISR(USART_RX_vect){
    uint8_t byte = UDR0;

    if(byte == 127){ // backspace 
        usartPrint("\b ");
        usartPrint("\b");
        usartLastCommand[usartIndex] = '\0';
        usartIndex--;
        return;
    }

    if(byte!='\r')
    transmitByte(byte);

    if(byte == '\r'){
        usartPrint("\r\n");

        usartLastCommand[usartIndex] = '\0';

        usartIndex = 0;
        commandProcessed = false;
    } else {
        usartLastCommand[usartIndex] = byte;
        usartIndex++;
    }
}

bool usartCommandProcessed(){
    if(!commandProcessed){
        commandProcessed = true;
        return false;
    }

    return true;
}