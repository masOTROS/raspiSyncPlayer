raspiSyncPlayer
===============

Software controlling the Raspis for a network synchronized video player.

Communication with server:

SETUP & KEEP ALIVE
First the server must register to each client sending to it a message with osc address "/registerServer" and add a int64 arg with port number.
From that moment on, server should receive every 500ms a message from the client to osc address "/keepAlive" and an int64 arg with current frame number of the video being played on that raspi.

VIDEO CONTROL
So far clients are capable of pausing its video with "/play" and restarting it with "/restart"