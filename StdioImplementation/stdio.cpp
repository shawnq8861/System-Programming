/*****************************************************************************
*
* File stdio.cpp
*
* Author:	Shawn Quinn
* 			quinnms1@uw.edu
*
* Date:		05/13/2015
*
* This is the C++ implementation file for the FILE class for CSS503 programming
* assignment 3.
*
* The FILE class provides methods that emulate the behavior of the C/C++
* stdio.h library files: fopen, fflush, setbuf, setvbuf, fpurge, fread,
* fwrite, fgetc, fputc, fgets, fputs, fseek, feof, fclose, printf.
*
*****************************************************************************/

#include <fcntl.h>     // open
#include <sys/types.h> // read
#include <sys/uio.h>   // read
#include <unistd.h>    // read, close
#include <strings.h>   // bcopy
#include <string.h>    // strlen
#include <stdarg.h>    // format, ...
#include <stdlib.h>    // abs

using namespace std;

char decimal[100];

int recursive_itoa( int arg )
{
	int div = arg / 10;
	int mod = arg % 10;
	int index = 0;
	if ( div > 0 )
    index = recursive_itoa( div );
	decimal[index] = mod + '0';
	return ++index;
}

char *itoa( const int arg )
{
	bzero( decimal, 100 );
	int order = recursive_itoa( arg );
	char *new_decimal = new char[order + 1];
	bcopy( decimal, new_decimal, order + 1 );
	return new_decimal;
}

int printf( const void *format, ... )
{
	va_list list;
	va_start( list, format );

	char *msg = ( char * )format;
	char buf[1024];
	int nWritten = 0;

	int i = 0, j = 0, k = 0;
	while ( msg[i] != '\0') {
		if ( msg[i] == '%' && msg[i + 1] == 'd' ) {
			buf[j] = '\0';
			nWritten += write( 1, buf, j );
			j = 0;
			i += 2;

			int int_val = va_arg( list, int );
			char *dec = itoa( abs( int_val ) );
			if ( int_val < 0 ) {
				nWritten += write( 1, "-", 1 );
			}
			nWritten += write( 1, dec, strlen( dec ) );
			delete dec;
		}
		else
			buf[j++] = msg[i++];
	}
	if ( j > 0 ) {
		nWritten += write( 1, buf, j );
	}
	va_end( list );
}

int setvbuf( FILE *stream, char *buf, int mode, size_t size )
{
	if ( mode != _IONBF && mode != _IOLBF && mode != _IOFBF )
		return -1;
	stream->mode = mode;
	stream->pos = 0;

	if ( stream->buffer != (char *)0 && stream->bufown == true )
		delete stream->buffer;

	switch ( mode ) {
	case _IONBF:
		stream->buffer = (char *)0;
		stream->size = 0;
		stream->bufown = false;
		break;
	case _IOLBF:
	case _IOFBF:
		if ( buf != (char *)0 ) {
			stream->buffer = buf;
			stream->size   = size;
			stream->bufown = false;
		}
		else {
			stream->buffer = new char[BUFSIZ];
			stream->size = BUFSIZ;
			stream->bufown = true;
		}
		break;
	}
  
	return 0;
}

void setbuf( FILE *stream, char *buf )
{
	setvbuf( stream, buf, ( buf != (char *)0 ) ? _IOFBF : _IONBF , BUFSIZ );
}

FILE *fopen( const char *path, const char *mode )
{
	FILE *stream = new FILE( );
	setvbuf( stream, (char *)0, _IOFBF, BUFSIZ );

	// fopen( ) mode
	// r or rb           =  O_RDONLY
	// w or wb           =  O_WRONLY | O_CREAT | O_TRUNC
	// a or ab           =  O_WRONLY | O_CREAT | O_APPEND
	// r+ or rb+ or r+b  =  O_RDWR
	// w+ or wb+ or w+b  =  O_RDWR   | O_CREAT | O_TRUNC
	// a+ or ab+ or a+b  =  O_RDWR   | O_CREAT | O_APPEND

	switch( mode[0] ) {
	case 'r':
		if ( mode[1] == '\0' )            // r
			stream->flag = O_RDONLY;
		else if ( mode[1] == 'b' ) {
			if ( mode[2] == '\0' )          // rb
				stream->flag = O_RDONLY;
			else if ( mode[2] == '+' )      // rb+
				stream->flag = O_RDWR;
		}
		else if ( mode[1] == '+' )        // r+  r+b
			stream->flag = O_RDWR;
		break;
	case 'w':
		if ( mode[1] == '\0' )            // w
			stream->flag = O_WRONLY | O_CREAT | O_TRUNC;
		else if ( mode[1] == 'b' ) {
			if ( mode[2] == '\0' )          // wb
				stream->flag = O_WRONLY | O_CREAT | O_TRUNC;
			else if ( mode[2] == '+' )      // wb+
				stream->flag = O_RDWR | O_CREAT | O_TRUNC;
		}
		else if ( mode[1] == '+' )        // w+  w+b
			stream->flag = O_RDWR | O_CREAT | O_TRUNC;
		break;
	case 'a':
		if ( mode[1] == '\0' )            // a
			stream->flag = O_WRONLY | O_CREAT | O_APPEND;
		else if ( mode[1] == 'b' ) {
			if ( mode[2] == '\0' )          // ab
				stream->flag = O_WRONLY | O_CREAT | O_APPEND;
			else if ( mode[2] == '+' )      // ab+
				stream->flag = O_RDWR | O_CREAT | O_APPEND;
		}
		else if ( mode[1] == '+' )        // a+  a+b
			stream->flag = O_RDWR | O_CREAT | O_APPEND;
		break;
	}

	mode_t open_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

	if ( ( stream->fd = open( path, stream->flag, mode ) ) == -1 ) {
		delete stream;
		printf( "fopen failed\n" );
		stream = NULL;
	}

  return stream;
}

int fpurge( FILE *stream )
{
	// complete it
	return 0;
}

int fflush( FILE *stream )
{
	// complete it
	return 0;
}

size_t fread( void *ptr, size_t size, size_t nmemb, FILE *stream )
{
	// complete it
	char* buffer = (char*)ptr;
	size_t elementsRead = 0;
	int numRead = 1;
	// 	while ! eof, read returns 0 if at end of file
	//		for i = 0; i < size; i++
	while ( elementsRead < nmemb && numRead > 0) {
		// call read() size times to read the element
		for (int i = 0; i <  size; ++i) {
			numRead = read(stream->fd, buffer, 1);
			if ( numRead == 0 ) {
				break;
			}
		}
		if ( numRead == 0 ) {
			stream->eof = true;
		}
		else {
			buffer += size;
			++elementsRead;
		}
	}

	return elementsRead;
}

size_t fwrite( const void *ptr, size_t size, size_t nmemb, FILE *stream )
{
	// complete it
	return 0;
}

int fgetc( FILE *stream )
{
	// complete it
	char buffer;
	int numRead;
	numRead = read(stream->fd, &buffer, 1);
	if ( numRead == 0 ) {
		stream->eof = true;
		return EOF;
	}
	else {
		return (int)buffer;
	}
}

int fputc( int c, FILE *stream )
{
	// complete it
	return 0;
}

char *fgets( char *str, int size, FILE *stream )
{
	// complete it
	return NULL;
}

int fputs( const char *str, FILE *stream )
{
	// complete it
	return 0;
}

int feof( FILE *stream )
{
	return stream->eof == true;
}

int fseek( FILE *stream, long offset, int whence )
{
	// complete it
	// three cases, origin == beginning, current or end, naything else
	// is an error
	switch(whence) {
	case 0: 	// origin == beginning
		if ((stream->size - 1) > offset ) {
			stream->pos = offset;
			stream->eof = false;
			return 0;
		}
		else if ((stream->size - 1) == offset) {
			stream->pos = offset;
			stream->eof = true;
			return 0;
		}
		else {
			return -1;
		}
	case 1: 	// origin == current position
		if ((stream->size - 1) > offset + whence ) {
			stream->pos = offset + whence;
			stream->eof = false;
			return 0;
		}
		else if ((stream->size - 1) == offset + whence ) {
			stream->pos = offset + whence;
			stream->eof = true;
			return 0;
		}
		else {
			return -1;
		}
	case 2: 	// origin == end
		if (offset + whence >= 0 && offset != 0) {
			stream->pos = offset + whence;
			stream->eof = false;
			return 0;
		}
		else if (offset == 0 ) {
			stream->pos = stream->size - 1;
			stream->eof = true;
			return 0;
		}
		else {
			return -1;
		}
	}
}

int fclose( FILE *stream )
{
	// complete it
	return 0;
}
