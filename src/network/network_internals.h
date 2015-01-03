/** @file network/network_internals.h
 *  @brief Details internal structures for the network library.
 *
 *  @author Bram Wasti
 *  @bug Incomplete
 */
#ifndef _NETWORK_INTERNALS_H
#define _NETWORK_INTERNALS_H

#include <pthread.h>
#include <netdb.h>
#include <sys/socket.h>
#include "../util/inc/llist.h"

/** @define The maximum size of a packet.
  */
#define MAX_PACKET_SIZE     512

/** @define Number of bytes used to determine namespace of
  *         the packet.
  */
#define NAME_SIZE           12

/** @define Number of bits in size.
  */
#define SIZE_MASK           0x1ff

/** @brief A header structure to be taken from the top of each
  *        packet.  This must fit into 2 machine words and currently
  *        will not work with windows.
  */
typedef struct {
  /** @brief A checksum to ensure a non-corrupted packet header. */
  unsigned int hcrc : 4;
  /** @brief A checksum to ensure a non-corrupted packet. */
  unsigned int crc : 16;
  /** @brief The size of the packet (up to 512) */
  unsigned int size : 9;
  /** @brief If the packet is a reliable packet. */
  unsigned char reliable : 1;
  /** @brief The type of packet being sent. */
  unsigned char type : 2;
  /** @brief The current count of the packet being sent. */
  unsigned int count : 32;
} packet_header_t;

/** @brief Type of packets.
  */
enum packet_type {
  /** @brief For ACK'ing reliable type packets.
    */
  ACK = 0,

  /** @brief For requesting a resource again.
    */
  RESEND = 1,

  /** @brief Used for pinging on initial connection.
    *        Can be used to determine latencies.
    */
  PING = 2,

  /** @brief Data transport packets.
    */
  DATA = 3
};

/** @brief A packet queue structure.
 */
typedef struct _packet_t {
  /** @brief Points to the next packet in the queue.
   */
  struct _packet_t *next;

  /** @brief A lock to allow fine grained parallel traversal of the queue.
    */
  pthread_mutex_t mutex;

  /** @brief Packet's associated name space.
    */
  char name[NAME_SIZE];

  /** @brief Packet contents.
    */
  char *data;

} packet_t;

/** @brief Information regarding the connection.
  */
typedef struct {
  /** @brief The socket associated with the connection.
    */
  int socket;

  /** @brief The address of the remote machine.
    */
  struct sockaddr_in addr;

  /** @brief The length of the address.
    */
  socklen_t addr_len;

  /** @brief Pending ACKs. 
   */
  llist_t out_queue;

  /** @brief Received but unprocessed. 
   */
  llist_t in_queue;

  /** @brief Current count of sent packets.
    */
  unsigned int out_count;

  /** @brief Current count of received packets.
    */
  unsigned int in_count;

} connection_t;

/** @brief A server struct.
  */
typedef struct {
  /** @brief The socket associated with the connection.
    */
  int socket;

  /** @brief The connections associated with the server.
    */
  connection_t *clients;
  
  /** @brief The number of connections open.
    */
  unsigned int client_count;

} server_t;

void rrecv_loop(connection_t *con);
connection_t *create_client(char *server_name, char *server_port);
server_t *create_server(unsigned int max_connections);

int con_sendto(connection_t *con, char *buf, int len, int flags);

#endif /* _NETWORK_INTERNALS_H */

