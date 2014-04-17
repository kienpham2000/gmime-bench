#include <glib.h>
#include <gmime/gmime.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, const char * argv[])
{
    GMimeMessage *message;
	int fd;
	
	if (argc < 2) {
		printf ("Usage: a.out <message file>\n");
		return 0;
	} else {
//		if ((fd = open (argv[1], 0)) == -1) {
//			fprintf (stderr, "Cannot open message `%s': %s\n", argv[1], g_strerror (errno));
//			return 0;
//		}
	}
	
	/* init the gmime library */
//	g_mime_init (0);
	
	/* parse the message */
//	message = parse_message (fd);
	
	/* count the number of parts in the message */
//	count_parts_in_message (message);
    
}

static GMimeMessage *
parse_message (int fd)
{
	GMimeMessage *message;
	GMimeParser *parser;
	GMimeStream *stream;
	
	/* create a stream to read from the file descriptor */
	stream = g_mime_stream_fs_new (fd);
	
	/* create a new parser object to parse the stream */
	parser = g_mime_parser_new_with_stream (stream);
	
	/* unref the stream (parser owns a ref, so this object does not actually get free'd until we destroy the parser) */
	g_object_unref (stream);
	
	/* parse the message from the stream */
	message = g_mime_parser_construct_message (parser);
	
	/* free the parser (and the stream) */
	g_object_unref (parser);
	
	return message;
}