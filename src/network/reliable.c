/** @file network/reliable.c
 *  @brief To reliably send packets over the network
 *         the protocol keeps track of send packets and
 *         recieved packets and puts them in an order.
 *
 *  @author Bram Wasti
 *  @bug Incomplete.
 *  @todo Async versions.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <sys/socket.h>
#include "network_internals.h"

/** @todo Implement rsend.
  */
int rsend(connection_t *con, char name[NAME_SIZE], char *data, int len);

/** @todo Add timeout code.
  *
  * struct timeval tv;
  * tv.tv_sec = 0;
  * tv.tv_usec = 100000;
  * if (setsockopt(rcv_sock, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
  *       perror("Error");
  * }
  */

/** @todo Implement rrecv.
  */
int rrecv(connection_t *con, char name[NAME_SIZE], char *data, int len);

/** @brief Generates a crc from the header of a packet.
  *
  * @param header The header to be included.
  * @return The crc generated.
  */
static inline unsigned char create_packet_header_crc(packet_header_t *header) {
  return 0;
}

/** @brief Generates a crc from the data of a packet.
  *
  * @param data The data included.
  * @param len The length of the buffer.
  * @return The crc generated.
  */
static inline unsigned int create_packet_crc(char *data, int len) {
  return 0;
}

/** @brief Validates the CRC's in a packet's header.
  *
  * @param header The header of the packet.
  * @param data The data within the packet.
  *
  * @return 0 if valid -1 otherwise.
  */
static inline int validate_packet(packet_header_t *header, char *data) {
  /* Perform CRC on header. */
  if (create_packet_header_crc(header) != header->hcrc) {
    return -1;
  }

  /* Perform CRC on data. */
  if (create_packet_crc(data, header->size) != header->crc) {
    return -1;
  }

  return 0;
}

/** @brief Gets a packet header and data out of a given buffer.
  *
  * @param buf The buffer to be converted into a packet header/data pair.
  * @param len The length of the buffer.
  * @param[out] header The header extracted from the buffer (NULL on failure).
  * @param[out] data The data extracted from the buffer (NULL on failue).
  * @returns -1 on failure and 0 on success.
  */
static inline int get_packet(char *buf, int len, packet_header_t **header, char **data) {
  if (len < sizeof(packet_header_t)) {
    *header = NULL;
    *data = NULL;
    return -1;
  }
  *header = (packet_header_t *)buf;
  *data = buf + sizeof(packet_header_t);
  return 0;
}

/** @brief Creates a packet buffer out of given data..
  *
  * @param[out] buf The buffer to be populated with a header and data.
  *             Undefined on failure.
  * @param data The data included..
  * @param len The length of the buffer.
  * @param reliable If the packet is to be reliable.
  * @param type Type of the packet.
  * @param count Count of the packet.
  * @returns -1 on failure and 0 on success.
  */
static inline int create_packet(char *buf, char *data, int len, int reliable, int type, int count) {
  ((packet_header_t *)buf)->reliable = reliable;
  ((packet_header_t *)buf)->type = type;
  ((packet_header_t *)buf)->count = count;

  if (size > SIZE_MASK) {
    return -1;
  } else {
    ((packet_header_t *)buf)->size = size & SIZE_MASK;
  }

  ((packet_header_t *)buf)->crc = create_packet_crc(data, len);
  ((packet_header_t *)buf)->hcrc = create_packet_header_crc(data, len);

  return 0;
}

/** @brief Posts a request for a new packet.  It was either corrupted or
  *        has timed out.
  * @param con The connection to send the request to.
  * @param count The packet that is missing.
  *
  * @return 0 on success -1 on failure.
  */
static inline int request_packet(connection_t *con, unsigned count) {
  /* Stack allocate a buffer. */
  char buf[sizeof(unsigned int) + sizeof(packet_header_t)];

  /* Create a RESEND packet. */
  create_packet(buf, (char *)&data, sizeof(buf), 0, RESEND, con->out_count++);

  /* Send the request. */
  return sendto(con->socket, buf, sizeof(buf), 0, &con->addr, con->addr_len);
}

/* Always recieving. If no recieve is posted we can let it go. */
void rrecv_loop(connection_t *con) {

  packet_header_t *header;
  char *data;

  char buf[MAX_PACKET_SIZE];
  int recv_len;

  while (1) {
    ((packet_header_t *)buf)->reliable = 1;
    ((packet_header_t *)buf)->crc = 12;
    ((packet_header_t *)buf)->type = 2;
    sendto(con->socket, buf, MAX_PACKET_SIZE, 0, (struct sockaddr *)&con->addr, con->addr_len);
    memset(buf, 0, MAX_PACKET_SIZE);
    recv_len = recvfrom(con->socket, buf, MAX_PACKET_SIZE, 0,
                        (struct sockaddr *)&con->addr, &con->addr_len);

    /* Handle errors. */
    if (recv_len < 0) {
      switch (errno) {
        case ETIMEDOUT:
          break;
        default:
          break;
      }
      /* Try again. */
      continue;
    }
    
    get_packet(buf, recv_len, &header, &data);

    if (validate_packet(con, header, data)) {
      
      continue;
    }

  }
}
