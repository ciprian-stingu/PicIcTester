/**
 * 
 * PicIcTester Database
 * 
 * Copyright (C) 2020-2021 Ciprian Stingu
 * ciprian dot stingu at gmail dot com
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
 * or see <http://www.gnu.org/licenses/>
 * 
 */
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
//------------------------------------------------------------------------------
#define INVALID_INDEX 0xFFFF
#define MAX_RECORDS 0x7FFF
#define MAX_DATA_SIZE 0x7FFF

void ShowHelp(void);
uint16_t GetNextRecordIndex(FILE *dbFileHandler);
uint16_t ConvertRecord(FILE *dbFileHandler, int StartOffset, int EndOffset, uint8_t *outData, uint16_t *deviceCount);

int main(int argc, char **argv) 
{
    if(argc <= 1) 
    {
        ShowHelp();
        exit(EXIT_FAILURE);
    }
    
	int option;
    uint16_t dbVersion = 0;
    char dbFile[PATH_MAX];
    
    memset(dbFile, 0x00, PATH_MAX);
    
    while((option = getopt(argc, argv, "v:f:h")) != -1)
    {
        switch(option) 
        {
            case 'v':
                dbVersion = atoi(optarg);
                break;
            case 'f':
				strncpy(dbFile, optarg, PATH_MAX - 1);
                break;
            case 'h':
            default:
                ShowHelp();
                exit(EXIT_FAILURE);
        }
    }
    
    if(strlen(dbFile) == 0 || dbVersion <= 0)
    {
        ShowHelp();
        exit(EXIT_FAILURE);
    }
    
    char dbFilePath[PATH_MAX];
    memset(dbFilePath, 0x00, PATH_MAX);
    
    fprintf(stdout, "Opening input file...\n");
    char *realPathResult = realpath(dbFile, dbFilePath);
    if(!realPathResult)
    {
        fprintf(stderr, "Invalid input file path\n");
        exit(EXIT_FAILURE);
    }
    
    FILE *dbFileHandler = fopen(dbFilePath, "r");
    if(!dbFileHandler)
    {
        fprintf(stderr, "Can't open the input file\n");
        exit(EXIT_FAILURE);
    }
    fseek(dbFileHandler, 0L, SEEK_SET);
    
    uint16_t indexes[MAX_RECORDS], newIndexes[MAX_RECORDS], indexCount = 0, deviceCount = 0;
    for(int i = 0; i < MAX_RECORDS; i++) {
        indexes[i] = newIndexes[i] = INVALID_INDEX;
    }
    
    fprintf(stdout, "Reading tests indexes...\n");
    uint16_t indexOffset = INVALID_INDEX;
    while((indexOffset = GetNextRecordIndex(dbFileHandler)) != INVALID_INDEX) 
    {
        if(indexCount >= MAX_RECORDS) {
            break;
        }
        indexes[indexCount++] = indexOffset;
        //fprintf(stdout, "Found index at %d\n", indexOffset);
    }
    
    if(indexCount == 0)
    {
        fclose(dbFileHandler);
        fprintf(stderr, "No data found in input file\n");
        exit(EXIT_FAILURE);
    }
    
    fseek(dbFileHandler, 0L, SEEK_SET);
    
    fprintf(stdout, "Found %d tests...\n", indexCount);
    
    
    char newDbFilePath[PATH_MAX];
    memset(newDbFilePath, 0x00, PATH_MAX);
    strcpy(newDbFilePath, dbFilePath);
    strcat(newDbFilePath, ".bin");
    
    fprintf(stdout, "Opening output file...\n");
    FILE *newDbFileHandler = fopen(newDbFilePath, "wa");
    if(!newDbFileHandler)
    {
        fprintf(stderr, "Can't open the output file\n");
        exit(EXIT_FAILURE);
    }
    fseek(newDbFileHandler, 0L, SEEK_SET);
    
    fprintf(stdout, "Writing data...\n");
    int writeResult = fwrite(&dbVersion, 2, 1, newDbFileHandler);
    if(writeResult != 1)
    {
        fclose(dbFileHandler);
        fclose(newDbFileHandler);
        fprintf(stderr, "Can't write version\n");
        exit(EXIT_FAILURE);
    }
    writeResult = fwrite(&deviceCount, 2, 1, newDbFileHandler);
    if(writeResult != 1)
    {
        fclose(dbFileHandler);
        fclose(newDbFileHandler);
        fprintf(stderr, "Can't write device count\n");
        exit(EXIT_FAILURE);
    }
    writeResult = fwrite(&indexCount, 2, 1, newDbFileHandler);
    if(writeResult != 1)
    {
        fclose(dbFileHandler);
        fclose(newDbFileHandler);
        fprintf(stderr, "Can't write index count\n");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < indexCount; i++)
    {
        writeResult = fwrite(&newIndexes[i], 2, 1, newDbFileHandler);
        if(writeResult != 1)
        {
            fclose(dbFileHandler);
            fclose(newDbFileHandler);
            fprintf(stderr, "Can't write indexes\n");
            exit(EXIT_FAILURE);
        }
    }
    
    uint8_t data[MAX_DATA_SIZE];
    uint16_t newOffset = 0;
    for(int i = 0; i < indexCount; i++)
    {
        newIndexes[i] = newOffset;
        
        memset(data, 0x00, MAX_DATA_SIZE);
        uint16_t newSize  = ConvertRecord(dbFileHandler, indexes[i], indexes[i + 1], data, &deviceCount);
        if(newSize <= 0) 
        {
            fclose(dbFileHandler);
            fclose(newDbFileHandler);
            fprintf(stderr, "Invalid record size\n");
            exit(EXIT_FAILURE);
        }
        
        writeResult = fwrite(data, 1, newSize, newDbFileHandler);
        if(writeResult != newSize)
        {
            fclose(dbFileHandler);
            fclose(newDbFileHandler);
            fprintf(stderr, "Fail to write record\n");
            exit(EXIT_FAILURE);
        }
        newOffset += newSize;
    }
    
    fclose(dbFileHandler);
    
    fprintf(stdout, "Found %d devices...\n", deviceCount);
    fseek(newDbFileHandler, 2L, SEEK_SET);
    writeResult = fwrite(&deviceCount, 2, 1, newDbFileHandler);
    if(writeResult != 1)
    {
        fclose(newDbFileHandler);
        fprintf(stderr, "Can't update device count\n");
        exit(EXIT_FAILURE);
    }
    fseek(newDbFileHandler, 6L, SEEK_SET);
    for(int i = 0; i < indexCount; i++)
    {
        newIndexes[i] += 6 + indexCount * 2; // add 2 bytes version. 2 bytes device count, 2 bytes index count, indexCount * 2
        writeResult = fwrite(&newIndexes[i], 2, 1, newDbFileHandler);
        if(writeResult != 1)
        {
            fclose(newDbFileHandler);
            fprintf(stderr, "Can't update indexes\n");
            exit(EXIT_FAILURE);
        }
    }
    
    fclose(newDbFileHandler);
    
    fprintf(stdout, "Done.\n");
    
	exit(EXIT_SUCCESS);
}
//------------------------------------------------------------------------------

void ShowHelp(void)
{
    fprintf(stderr, "Usage: DbCreate -v <db version number> -f <db file name path> [-h]\n");
}
//------------------------------------------------------------------------------

uint16_t GetNextRecordIndex(FILE *dbFileHandler)
{
    uint8_t data = 0x00;
    int readResult = 0, position = 0;
    bool endOfData = true;
    
    while((readResult = fread(&data, 1, 1, dbFileHandler)) == 1)
    {
        if(data == '$' || data == '&') 
        {
            position = ftell(dbFileHandler);
            if(data == '$') {
                endOfData = false;
            }
            break;
        }
    }
    
    if(endOfData || readResult == 0 || position >= 0xFFFF) {
        return INVALID_INDEX;
    }
    
    return (position - 1) & 0xFFFF;
}
//------------------------------------------------------------------------------

uint16_t ConvertRecord(FILE *dbFileHandler, int StartOffset, int EndOffset, uint8_t *outData, uint16_t *deviceCount)
{
    uint8_t data[MAX_DATA_SIZE];
    memset(data, 0x00, MAX_DATA_SIZE);
    int size = EndOffset - StartOffset;
    
    fseek(dbFileHandler, StartOffset, SEEK_SET);
    int readResult = fread(data, 1, size, dbFileHandler);
    if(readResult != size) 
    {
        if(!feof(dbFileHandler)) {
            return 0;
        }
    }
    
    uint16_t outSize = 0;
    
    for(int i = 0; i < readResult; i++)
    {
        if(data[i] == '&') // end of data
        {
            outData[outSize++] = '&';
            outData[outSize++] = '\n';
            break;
        }
        else
        {
            bool eol = false;
            while(data[i] == '\r' || data[i] == '\n') 
            {
                i++;
                eol = true;
            }
            if(eol) 
            {
                i--;
                outData[outSize++] = '\n';
            }
            else {
                outData[outSize++] = data[i];
            }
        }
    }
    
    *deviceCount = *deviceCount + 1;
    for(int i = 0; i < outSize; i++)
    {
        if(outData[i] == '\n')
        {
            break;
        }
        if(outData[i] == '/') {
            *deviceCount = *deviceCount + 1;
        }
    }
    
    return outSize;
}
//------------------------------------------------------------------------------
