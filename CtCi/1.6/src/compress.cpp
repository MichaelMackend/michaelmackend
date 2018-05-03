#include "compress.h"
#include <exception>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdexcept>
#include <ctype.h>


void recordCharacterAndCount(char letter, int count, char* buffer, int& offset) {
    buffer[offset] = letter;
    offset++;

    if(count < 2)
        return;

    offset += sprintf(buffer + offset,"%i", count);
}

class SimpleCompressor {
    public:

        SimpleCompressor(const char* inputString) : input(inputString) {

            if(inputString == nullptr) {
                throw std::invalid_argument("inputString");
            }

            inputLength = strlen(input) + 1;

        }
        const char* input = nullptr;
        char* compressedString = nullptr;
        char currentChar = '\0';
        int readIndex = 0;
        int writeIndex = 0;
        int currentCount = 0;
        int inputLength = 0;

        void initializeOutput() {
            compressedString = new char[inputLength];
            memset(compressedString,'\0',inputLength);
            currentChar = '\0';
            writeIndex = 0;
            currentCount = 0;
        }

        void forEachCharacterInTheInput(void (*action)(SimpleCompressor* compressor)) {
            for(int i = 0; i < inputLength; ++i) {
                readIndex = i;
                if(input[i] == '\0') {
                    return;
                }
                if(!isalpha(input[i])) {
                    throw std::invalid_argument("string");
                }
                action(this);
            }
        }

        bool newCharacterFoundOrReachedEndOfInput() {
            if(readIndex == 0) {
                return true;
            }

            if(input[readIndex] != currentChar) {
                return true;
            }

            return false;
        }

        bool notTheFirstCharacterFound() {
            return readIndex != 0;
        }

        void recordCharacterAndCount() {
            ::recordCharacterAndCount(currentChar,currentCount,compressedString,writeIndex);
        }

        void startTheNewCharacterCount() {
            currentChar = input[readIndex];
            currentCount = 1;
        }

        void incrementTheCurrentCharacter() {
            currentCount += 1;
        }

        char* returnCompressedString() {
            return compressedString;
        }

        const char* getCompressedString() {

            if(inputLength <= 2) {
                return input;
            }

            initializeOutput();

            forEachCharacterInTheInput([](SimpleCompressor* c) {
                if(c->newCharacterFoundOrReachedEndOfInput()) {
                    if(c->notTheFirstCharacterFound()) {
                        c->recordCharacterAndCount();
                    }
                    c->startTheNewCharacterCount();
                } else {
                    c->incrementTheCurrentCharacter();
                }
            });
            recordCharacterAndCount();

            return (const char*)returnCompressedString();
        }
};

const char* compressString(const char* inputString) {

    SimpleCompressor compressor(inputString);

    return compressor.getCompressedString();
}
