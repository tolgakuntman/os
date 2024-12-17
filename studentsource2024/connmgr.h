/**
 * \author Tolga Kuntman
 */

#ifndef CONNMGR_H
#define CONNMGR_H

/** listens the given TCP socket until the client disconnects or is timed out.
 * \param arg pointer to argumants structure
 * -arg1->client
 * -arg2->sbuffer
 * \return to NULL if it's terminated correctly
 */
void *listen_the_client(void *arg);

/** opens a tcp socket with a given port and waits for clients to connect. It creates a new thread for each client.
 * \param arg pointer to argumants structure
 * -arg1->port number
 * -arg2->max number of connections
 * -arg3->sbuf
 * \return to NULL if it's terminated correctly
 */
void *wait_for_clients(void *arg) ;

#endif