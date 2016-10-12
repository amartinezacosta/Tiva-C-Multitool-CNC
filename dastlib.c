#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "dastlib.h"
#include "planner.h"
#include "driverlib/interrupt.h"
//PARSIG SECTION-------------------------------------------------
static bool is_White(unsigned char c);
static bool is_Delim(unsigned char c, const unsigned char *delimiters);
static bool is_In(unsigned char c, const unsigned char *s);

uint8_t Token_get(unsigned char *string, const unsigned char *delimiters, unsigned char *token){
	uint8_t count = 0;

	if(*string == 0){
		return 0;
	}

	while(is_White(*string)){						//ignore white spaces
		string++;
		count++;
	}

	if(is_In(*string, delimiters)){
		*token++ = *string++;						//if any delimiter is found point to the element
		count++;
		while(!is_Delim(*string, delimiters)){		//while the next delimiter is not found, continue
			*token++ = *string++;
			count++;
		}
	}

	*token = 0;
	return count;
}

static bool is_White(unsigned char c){
	if(c == ' ' || c == 9){
		return true;
	}

	return false;
}

static bool is_In(unsigned char c, const unsigned char *s){
	while(*s){
		if(*s == c){return true;}
		s++;
	}

	return false;
}

static bool is_Delim(unsigned char c, const unsigned char *delimiters){
	if(is_In(c, delimiters) || c == 9 || c == '\r' || c == 0){
		return true;
	}

	return false;
}



static const float g_pfExponents[] =
{
    1.0e+01,
    1.0e+02,
    1.0e+04,
    1.0e+08,
    1.0e+16,
    1.0e+32,
};

float ustrtof(unsigned char *nptr){
    unsigned long ulNeg, ulExp, ulExpNeg, ulValid, ulIdx;
    float fRet, fDigit, fExp;
    unsigned char *pcPtr;

    fRet = 0;
    ulNeg = 0;
    ulValid = 0;

    pcPtr = nptr;
    pcPtr++;      //skip first letter on the command TODO: this only applies for the Gcode parser!!!!

    while((*pcPtr == ' ') || (*pcPtr == '\t')){
        pcPtr++;
    }

    if(*pcPtr == '-'){
        ulNeg = 1;
        pcPtr++;
    }else if(*pcPtr == '+'){
        pcPtr++;
    }

    while((*pcPtr >= '0') && (*pcPtr <= '9')){
        fRet *= 10;
        fRet += *pcPtr++ - '0';

        ulValid = 1;
    }

    if((*pcPtr == '.') && (pcPtr[1] >= '0') && (pcPtr[1] <= '9')){
        pcPtr++;

        fDigit = 0.1;
        while((*pcPtr >= '0') && (*pcPtr <= '9')){
            fRet += (*pcPtr++ - '0') * fDigit;
            fDigit /= (float)10.0;

            ulValid = 1;
        }
    }

    if(((pcPtr[0] == 'e') || (pcPtr[0] == 'E')) && (ulValid == 1) &&
       (((pcPtr[1] >= '0') && (pcPtr[1] <= '9')) ||
        (((pcPtr[1] == '+') || (pcPtr[1] == '-')) &&
         (pcPtr[2] >= '0') && (pcPtr[2] <= '9')))){


        pcPtr++;

        ulExpNeg = 0;
        if(*pcPtr == '-'){
            ulExpNeg = 1;
            pcPtr++;
        }
        else if(*pcPtr == '+'){
            pcPtr++;
        }

        ulExp = 0;
        while((*pcPtr >= '0') && (*pcPtr <= '9')){
            ulExp *= 10;
            ulExp += *pcPtr++ - '0';
        }

        fExp = 1;
        for(ulIdx = 0; ulIdx < 7; ulIdx++){
            if(ulExp & (1 << ulIdx)){
                fExp *= g_pfExponents[ulIdx];
            }
        }

        if(ulExpNeg == 1){
            fExp = 1 / fExp;
        }

        fRet *= fExp;
    }

    return(ulNeg ? (0 - fRet) : fRet);
}




//QUEUE SECTION--------------------------------------------------
void Queue_init(Queue *queue, Linear_Motion *Buffer, uint16_t Max_Items){
	queue->ItemCount = 0;
	queue->Max_Items = Max_Items;
	queue->front = 0;
	queue->rear = -1;
	queue->Buffer = Buffer;

}

Linear_Motion Queue_peek(Queue *queue){
	return queue->Buffer[queue->front];
}

bool Queue_Empty(Queue *queue){
   return queue->ItemCount == 0 ? true : false;
}

bool Queue_Full(Queue *queue){
   return  queue->ItemCount == queue->Max_Items ? true : false;
}

uint16_t Queue_size(Queue *queue){
   return queue->ItemCount;
}


//return true if queue is full, false if motion was added into the queue
bool Queue_insert(Queue *queue, Linear_Motion motion){
	if(Queue_Full(queue)){
		return true;
	}else{
		if(queue->rear == queue->Max_Items-1){
			queue->rear = -1;
		}
		queue->rear++;
		queue->Buffer[queue->rear] = motion;
		queue->ItemCount++;
		return false;
	}
}

//return true if queue is empty, false if queue extracted motion
bool Queue_extract(Queue *queue , Linear_Motion *motion){
	if(Queue_Empty(queue)){
		return 0;
	}else{
		*motion = queue->Buffer[queue->front];
		queue->front++;
		if(queue->front == queue->Max_Items){
			queue->front = 0;
		}
		queue->ItemCount--;
		return true;
	}
}


//RINGBUFFER SECTION----------------------------------------------------------
static uint16_t Ringbuffer_available_data(RingBuffer *object);
static uint16_t Ringbuffer_available_space(RingBuffer *object);
static void Ringbuffer_commit_read(RingBuffer *object);
static void Ringbuffer_commit_write(RingBuffer *object);
static uint16_t RingBuffer_starts_at(RingBuffer *object);
static uint16_t RingBuffer_ends_at(RingBuffer *object);

void RingBuffer_init(RingBuffer *object, uint16_t lenght, unsigned char *buffer){
	object->end = 0;
	object->start = 0;
	object->lenght = lenght + 1;
	object->buffer = buffer;
}

bool RingBuffer_Read(RingBuffer *object, unsigned char *target){
	uint16_t index;

	if(Ringbuffer_available_data(object) <= 1){ return false; }

	index = RingBuffer_starts_at(object);
	*target = object->buffer[index];

	Ringbuffer_commit_read(object);

	if(object->end == object->start){
		object->start = object->end = 0;
	}

	return true;
}

bool RingBuffer_Write(RingBuffer *object,  unsigned char data){
	uint16_t index;

	if(Ringbuffer_available_data(object) == 0){
		object->start = object->end = 0;
	}

	if(Ringbuffer_available_space(object) <= 1){ return false; }

	index = RingBuffer_ends_at(object);
	object->buffer[index] = data;
	Ringbuffer_commit_write(object);

	return true;
}

static uint16_t Ringbuffer_available_data(RingBuffer *object){
	uint16_t available_data = (object->end + 1) % (object->lenght - object->start - 1 );
	return available_data;
}

static uint16_t Ringbuffer_available_space(RingBuffer *object){
	uint16_t available_space = object->lenght - object->end - 1;
	return available_space;
}

bool Ringbuffer_empty(RingBuffer *object){
	bool empty = Ringbuffer_empty(object) == 0 ? true : false;
	return empty;
}
bool Ringbufer_full(RingBuffer *object){
	bool full = Ringbuffer_available_data(object) - object->lenght == 0 ? true : false;
	return full;
}

static void Ringbuffer_commit_read(RingBuffer *object){
	IntMasterDisable();
	object->start = (object->start + 1) % object->lenght;
	IntMasterEnable();
}

static void Ringbuffer_commit_write(RingBuffer *object){
	IntMasterDisable();
	object->end = (object->end + 1) % object->lenght;
	IntMasterEnable();
}

static uint16_t RingBuffer_starts_at(RingBuffer *object){
	uint16_t index = object->start;
	return index;
}

static uint16_t  RingBuffer_ends_at(RingBuffer *object){
	uint16_t index = object->end;
	return index;
}
