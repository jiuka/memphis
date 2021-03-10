/*
 * Tile-Server - Memphis-based OSM map tile server
 * Copyright (C) 2011 Brian J. Johnson <bjj4@charter.net>
 *
 * Based on:
 *
 * nweb: a tiny, safe Web server
 * http://www.ibm.com/developerworks/systems/library/es-nweb/index.html
 *
 * Also based on:
 *
 * Memphis - Cairo Rederer for OSM in C
 * Copyright (C) 2009  Simon Wenner <simon@wenner.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <memphis/memphis.h>
#include <stdlib.h>
#include <glib/gstdio.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define RESOLUTION 256
#define BUFSIZE 8096
#define ERROR 42
#define SORRY 43
#define LOG   44

static gboolean foreground = FALSE;
static gint     www_port   = 80;
static gchar*   map_file   = "map.osm";
static gchar*   rule_file  = "default-rules.xml";

static GOptionEntry entries[] =
{
  { "port", 'p', 0, G_OPTION_ARG_INT, &www_port, "TCP port number", "port" },
  { "no-daemon", 'd', 0, G_OPTION_ARG_NONE, &foreground, "Run in the foreground", NULL },
  { "map", 'm', 0, G_OPTION_ARG_FILENAME, &map_file, "Map file name", "mapfile" },
  { "rules", 'r', 0, G_OPTION_ARG_FILENAME, &rule_file, "XML rendering rule file name", "rulefile" },
  { NULL }
};


struct {
	gchar *ext;
	gchar *filetype;
} extensions [] = {
	{"gif", "image/gif" },  
	{"jpg", "image/jpeg"}, 
	{"jpeg","image/jpeg"},
	{"png", "image/png" },  
	{"zip", "image/zip" },  
	{"gz",  "image/gz"  },  
	{"tar", "image/tar" },  
	{"htm", "text/html" },  
	{"html","text/html" },  
	{"css", "text/css" },
	{"js",  "application/javascript" },
	{0,0} };

void nlog(gint type, gchar *s1, gchar *s2, gint num)
{
  gchar logbuffer[BUFSIZE*2];

  switch (type) {
  case ERROR:
    g_error ("%s:%s Errno=%d exiting",s1, s2, errno);
    break;
  case SORRY: 
    (void)sprintf(logbuffer, "<HTML><BODY><H1>tile-server Sorry: %s %s</H1></BODY></HTML>\r\n", s1, s2);
    (void)write(num,logbuffer,strlen(logbuffer));
    (void)sprintf(logbuffer,"SORRY: %s:%s",s1, s2); 
    break;
  case LOG:
    g_message("%s:%s:%d",s1, s2, num);
    break;
  }	
  if(type == ERROR || type == SORRY) exit(3);
}

MemphisRenderer *renderer;

/* callback to write PNG data to a file descriptor */
cairo_status_t write_func(void *closure, const unsigned char *data, 
			   unsigned int length)
{
  int fd = *(int *)closure;
  int count;

  while(length > 0) {
    count = write(fd,data,length);
    if (count < 0) {
      return CAIRO_STATUS_WRITE_ERROR;
    }
    data += count;
    length -= count;
  }

  return CAIRO_STATUS_SUCCESS;
}

/* generate and send a map tile using libmemphis */
void rendertile(gint zoom_level, gint x, gint y, gint fd)
{
  cairo_surface_t *surface;
  cairo_t *cr;
  char *msg;

  if (!memphis_renderer_tile_has_data (renderer, x, y, zoom_level)) {
    nlog(SORRY,"tile out of map area","",fd);
  }
  
  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
				       RESOLUTION, RESOLUTION);
  cr =  cairo_create(surface);
  memphis_renderer_draw_tile (renderer, cr, x, y, zoom_level);

  msg = "HTTP/1.0 200 OK\r\nContent-Type: image/png\r\n\r\n";
  (void)write(fd,msg,strlen(msg));
  cairo_surface_write_to_png_stream(surface, write_func, (void *)&fd);
          
  cairo_destroy(cr);
  cairo_surface_destroy(surface);
}

/* this is a child web server process, so we can exit on errors */
void web(gint fd, gint hit)
{
  gint j, file_fd, buflen, len;
  glong i, ret;
  gchar * fstr;
  gint zoom, x, y;
  static gchar buffer[BUFSIZE+1]; /* static so zero filled */

  ret =read(fd,buffer,BUFSIZE); /* read Web request in one go */
  if(ret == 0 || ret == -1) {	/* read failure stop now */
    nlog(SORRY,"failed to read browser request","",fd);
  }
  if(ret > 0 && ret < BUFSIZE)	/* return code is valid chars */
    buffer[ret]=0;		/* terminate the buffer */
  else buffer[0]=0;

  for(i=0;i<ret;i++)	/* remove CF and LF characters */
    if(buffer[i] == '\r' || buffer[i] == '\n')
      buffer[i]='*';
  nlog(LOG,"request",buffer,hit);

  if( strncmp(buffer,"GET ",4) && strncmp(buffer,"get ",4) )
    nlog(SORRY,"Only simple GET operation supported",buffer,fd);

  for(i=4;i<BUFSIZE;i++) { /* null terminate after the second space to ignore extra stuff */
    if(buffer[i] == ' ') { /* string is "GET URL " +lots of other stuff */
      buffer[i] = 0;
      break;
    }
  }

  for(j=0;j<i-1;j++) 	/* check for illegal parent directory use .. */
    if(buffer[j] == '.' && buffer[j+1] == '.')
      nlog(SORRY,"Parent directory (..) path names not supported",buffer,fd);

  if( !strncmp(&buffer[0],"GET /\0",6) || !strncmp(&buffer[0],"get /\0",6) ) /* convert no filename to index file */
    (void)strcpy(buffer,"GET /index.html");

  /* work out the file type and check we support it */
  buflen=strlen(buffer);
  fstr = (gchar *)0;
  for(i=0;extensions[i].ext != 0;i++) {
    len = strlen(extensions[i].ext);
    if( !strncmp(&buffer[buflen-len], extensions[i].ext, len)) {
      fstr =extensions[i].filetype;
      break;
    }
  }
  if(fstr == 0) nlog(SORRY,"file extension type not supported",buffer,fd);

  /* Open the file for reading.  Files will override tile requests. */
  if(( file_fd = open(&buffer[5],O_RDONLY)) == -1) {
    /* handle tile requests.  Eg. http://localhost/9/120/196.png */
    if (sscanf(&buffer[5], "%d/%d/%d.png", &zoom, &x, &y) == 3) {
      nlog(LOG,"MAP",&buffer[5],hit);
      rendertile(zoom, x, y, fd);
      goto done;
    }
    nlog(SORRY, "failed to open file",&buffer[5],fd);
  }

  nlog(LOG,"SEND",&buffer[5],hit);

  (void)sprintf(buffer,"HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", fstr);
  (void)write(fd,buffer,strlen(buffer));

  /* send file in 8KB block - last block may be smaller */
  while (	(ret = read(file_fd, buffer, BUFSIZE)) > 0 ) {
    (void)write(fd,buffer,ret);
  }

 done:
#ifdef LINUX
  sleep(1);	/* to allow socket to drain */
#endif
  exit(1);
}


int main (int argc, char **argv) {
  gint pid, listenfd, socketfd, hit;
  gchar *port;
  socklen_t length;
  static struct sockaddr_in cli_addr; /* static = initialised to zeros */
  static struct sockaddr_in serv_addr; /* static = initialised to zeros */

  g_autoptr(MemphisRuleSet) rules = NULL;
  g_autoptr(MemphisMap) map = NULL;

  GError *err = NULL;
  GOptionContext *context;

  context = g_option_context_new ("- render and serve OpenStreetMap map tiles");
  g_option_context_add_main_entries (context, entries, NULL);
  g_option_context_set_summary(context,
            "tile-server [-d] [-p <port>] [-m <mapfile>] [-r <rulefile>]");
  if (!g_option_context_parse (context, &argc, &argv, &err))
    {
      g_print ("option parsing failed: %s\n", err->message);
      exit (1);
    }

  g_print("Serving on port %d%s\nMap file: %s\nRule file: %s\n",
	  www_port, foreground ? " (in foreground)" : "",
	  map_file, rule_file);

  if (foreground) {
    memphis_debug_set_print_progress (TRUE);
  }

  rules = memphis_rule_set_new ();
  memphis_rule_set_load_from_file (rules, rule_file, NULL);

  map = memphis_map_new ();
  memphis_map_load_from_file (map, map_file, &err);
  if (err != NULL) {
    g_print ("Error: %s\n", err->message);
    return -1;
  }

  renderer = memphis_renderer_new_full (rules, map);
  memphis_renderer_set_resolution (renderer, RESOLUTION);
  g_print ("Tile resolution: %u\n", memphis_renderer_get_resolution (renderer));

  /* Become deamon + unstopable and no zombies children (= no wait()) */
  if (! foreground) {
    gint i;

    if(fork() != 0)
      return 0; /* parent returns OK to shell */

    (void)signal(SIGCLD, SIG_IGN); /* ignore child death */
    (void)signal(SIGHUP, SIG_IGN); /* ignore terminal hangups */

    for(i=0;i<32;i++)
      (void)close(i);		/* close open files */

    (void)setpgrp();		/* break away from process group */
  }

  port = g_strdup_printf("%d", www_port);
  nlog(LOG, "tile-server starting", port, getpid());
  /* setup the network socket */
  if((listenfd = socket(AF_INET, SOCK_STREAM,0)) <0)
    nlog(ERROR, "system call","socket",0);
  if(www_port < 0 || www_port >60000)
    nlog(ERROR, "Invalid port number (try 1->60000)", port, 0);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons((guint16)www_port);
  if(bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <0)
    nlog(ERROR,"system call","bind",0);
  if( listen(listenfd, 64) <0)
    nlog(ERROR,"system call","listen",0);
  g_free(port);

  /* Handle HTTP requests */

  for(hit=1; ;hit++) {
    length = sizeof(cli_addr);
    if((socketfd = accept(listenfd, (struct sockaddr *)&cli_addr, &length)) < 0)
      nlog(ERROR,"system call","accept",0);

    if((pid = fork()) < 0) {
      nlog(ERROR,"system call","fork",0);
    }
    else {
      if(pid == 0) { 	/* child */
	(void)close(listenfd);
	web(socketfd,hit); /* never returns */
      } else { 	/* parent */
	(void)close(socketfd);
      }
    }
  }

  // NOTREACHED
  g_object_unref (renderer);

  return 0;
}
