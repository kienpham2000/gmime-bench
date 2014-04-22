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
#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>

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

static void
write_message_to_screen (GMimeObject *part)
{
	GMimeStream *stream;
    GMimeDataWrapper *dw;
	
	/* create a new stream for writing to stdout */
//	stream = g_mime_stream_file_new (stdout);
    stream = g_mime_stream_mem_new();
//	g_mime_stream_file_set_owner ((GMimeStreamFile *) stream, FALSE);
	
    dw = g_mime_part_get_content_object( (GMimePart*) part);
    g_mime_data_wrapper_write_to_stream(dw, stream);
    
	/* write the message to the stream */
//	g_mime_object_write_to_stream (part, stream);
	
	/* flush the stream (kinda like fflush() in libc's stdio) */
	g_mime_stream_flush (stream);
    
    GByteArray *ba;
    ba = g_mime_stream_mem_get_byte_array( (GMimeStreamMem*) stream);
   
	
	/* free the output stream */
	g_object_unref (stream);
}

static void
part_callback (GMimeObject *parent, GMimeObject *part, gpointer user_data)
{
	int *count = user_data;
	
	(*count)++;
	
	/* 'part' points to the current part node that
	 * g_mime_message_foreach() is iterating over */
	
	/* find out what class 'part' is... */
	if (GMIME_IS_MESSAGE_PART (part)) {
		/* message/rfc822 or message/news */
		GMimeMessage *message;
		
		/* g_mime_message_foreach() won't descend into
         child message parts, so if we want to count any
         subparts of this child message, we'll have to call
         g_mime_message_foreach() again here. */
		
		message = g_mime_message_part_get_message ((GMimeMessagePart *) part);
		g_mime_message_foreach (message, part_callback, count);
	} else if (GMIME_IS_MESSAGE_PARTIAL (part)) {
		/* message/partial */
		
		/* this is an incomplete message part, probably a
         large message that the sender has broken into
         smaller parts and is sending us bit by bit. we
         could save some info about it so that we could
         piece this back together again once we get all the
         parts? */
	} else if (GMIME_IS_MULTIPART (part)) {
		/* multipart/mixed, multipart/alternative,
		 * multipart/related, multipart/signed,
		 * multipart/encrypted, etc... */
		
		/* we'll get to finding out if this is a
		 * signed/encrypted multipart later... */

        

	} else if (GMIME_IS_PART (part)) {
		/* a normal leaf part, could be text/plain or
		 * image/jpeg etc */
        
        if (GMIME_OBJECT (part)->disposition) {
//            GMimeParam *param;
            write_message_to_screen(part);
            
//            printf ("%s", GMIME_OBJECT (part)->disposition->disposition);
            
//			if ((param = GMIME_OBJECT (part)->disposition->params)) {
//
//				while (param) {
//					printf ("\"%s\" \"%s\"", param->name, param->value);
//                    param = param->next;
//				}
//
//			}
        }
        
            


        
	} else {
		g_assert_not_reached ();
	}
}

int main (int argc, char **argv)
{
    time_t start_time = time(NULL);
    
    // init the lib
    g_mime_init (0);
    
    FILE *fd;

    int loops = 10000;
//    char file_path[] = "/Users/Kien/SendGrid/go/src/github.com/sendgrid/go-gmime/gmime/fixtures/text_attachment.eml";
//    char file_path[] = "/Users/Kien/SendGrid/go/src/github.com/sendgrid/go-gmime/gmime/fixtures/text-only.eml";
    char file_path[] = "/Users/Kien/SendGrid/go/src/github.com/sendgrid/go-gmime/gmime/fixtures/test2.eml";
    
    printf("Loops: %d, Benchmarking file: %s\n\n", loops, file_path);
    
    if (!(fd = fopen(file_path, "r+"))) {
        fprintf (stderr, "Cannot open message `%s': %s\n", file_path, g_strerror (errno));
        return 0;
    }

    for (int i=0; i<loops; i++) {
        // reset file descriptor
        fseek(fd, 0, SEEK_SET);
        
        // parse message
        GMimeMessage *message = parse_message(fileno(fd));
        
        // parser all headers:
        char *headers = g_mime_object_get_headers((GMimeObject *) message);
        //printf("Header: %s\n", headers);
        
        if (!headers) {
            printf("Can't parse headers");
            return -1;
        }
        free(headers);
        
        // get Content-Type header:
        const char *content_type = g_mime_object_get_header((GMimeObject *) message, "Content-Type");
        // printf("Content-Type: %s\n", content_type);
        
        if (!content_type) {
            printf("Can't parse Content-Type");
            return -1;
        }
        
        // parse parts
        int part_count = 0;
        
        g_mime_message_foreach (message, part_callback, &part_count);
        
//        printf ("There are %d parts in the message\n", part_count);
        
//        g_object_unref (message);
        printf(".");
    }
    
    printf("\n\nTotal Time: %ld seconds\n\n", time(NULL) - start_time);
    
    return 0;

}
