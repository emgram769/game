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
#include "../util/inc/trace.h"

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
  * @param len The length of the buffer or 0 to skip the check.
  * @param[out] header The header extracted from the buffer (NULL on failure).
  * @param[out] data The data extracted from the buffer (NULL on failue).
  * @returns -1 on failure and 0 on success.
  */
static inline int get_packet(char *buf, int len, packet_header_t **header, char **data) {
  /* If len is 0, we skip this check. */
  if (len < sizeof(packet_header_t) && len != 0) {
    *header = NULL;
    *data = NULL;
    return -1;
  }

  if (header != NULL) {
    *header = (packet_header_t *)buf;
  }

  if (data != NULL) {
    *data = buf + sizeof(packet_header_t);
  }
  return 0;
}

/** @brief Creates a packet buffer out of given data.
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

  if (len > SIZE_MASK) {
    return -1;
  } else {
    ((packet_header_t *)buf)->size = len & SIZE_MASK;
  }

  memcpy(buf + sizeof(packet_header_t), data, len);

  ((packet_header_t *)buf)->crc = create_packet_crc(data, len);
  ((packet_header_t *)buf)->hcrc = create_packet_header_crc((packet_header_t *)data);

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
  create_packet(buf, (char *)&count, sizeof(buf), 0, RESEND, con->out_count++);

  /* Send the request. */
  return con_sendto(con, buf, sizeof(buf), 0);
}

/** @brief Posts an ACK for an incoming packet.
  *
  * @param con The connection to send the ACK to.
  * @param count The packet that is being ACK'd.
  *
  * @return 0 on success -1 on failure.
  */
static inline int ack_packet(connection_t *con, unsigned count) {
  /* Stack allocate a buffer. */
  char buf[sizeof(unsigned int) + sizeof(packet_header_t)];

  /* Create a RESEND packet. */
  create_packet(buf, (char *)&count, sizeof(buf), 0, ACK, con->out_count++);

  /* Send the request. */
  return con_sendto(con, buf, sizeof(buf), 0);
}

/** @brief Find the related outgoing packet and remove it from
  *        the outgoing queue.
  *
  * @param con The connection the packet came in on.
  * @param header The header of the packet.
  * @param data The data of the packet.
  */
static inline void process_ack(connection_t *con, packet_header_t *header, char *data) {
  /* Process data. */
}

/** @brief Find the related outgoing packet and resend it.
  *
  * @param con The connection the packet came in on.
  * @param header The header of the packet.
  * @param data The data of the packet.
  */
static inline void process_resend(connection_t *con, packet_header_t *header, char *data) {
  /* Process data. */
}

/** @brief Ping simply ACKs if it needs to.
  *        Otherwise, nothing is done.
  *
  * @param con The connection the packet came in on.
  * @param header The header of the packet.
  * @param data The data of the packet.
  */
static inline void process_ping(connection_t *con, packet_header_t *header, char *data) {
  if (header->reliable) {
    ack_packet(con, header->count);
  }
}

/** @brief Returns 0 if the count of the two packets is
  *        equal, -1 if the count of the new packet is
  *        greater and 1 if the count of the new packet
  *        is less.
  *
  * @param in_queue The old packet to compare against.
  * @param new The new packet to compare against.
  */
int process_data_insert(void *old_packet, void *new_packet) {
  packet_header_t *old_packet_header;
  packet_header_t *new_packet_header;
  if (get_packet(old_packet, 0, &old_packet_header, NULL)) {
    TRACE(NETWORK_TRACE, TRACE_PRINT("Unable to get packet header from stored packet."));
  }
  if (get_packet(new_packet, 0, &new_packet_header, NULL)) {
    TRACE(NETWORK_TRACE, TRACE_PRINT("Unable to get packet header from recieved packet."));
  }

  if (new_packet_header->count < old_packet_header->count) {
    return 1;
  } else if (new_packet_header->count == old_packet_header->count) {
    return 0;
  } else {
    return -1;
  }

}

/** @brief Check if we are waiting on the data. If not, enque the data.
  *        Maintain order with respect to the packet's count.
  *
  * @param con The connection the packet came in on.
  * @param header The header of the packet.
  * @param data The data of the packet.
  */
static inline void process_data(connection_t *con, packet_header_t *header, char *data) {
  if (header->reliable) {
    ack_packet(con, header->count);
  }

  /* Process data. */
  
  /* Check if it's stale. */
  if (header->count < con->in_count) {
    return;
  }

  /* Insert it into the queue. */
  insert_when(&con->in_queue, header, &process_data_insert);
}

/* Always recieving. If no recieve is posted we can let it go. */
void rrecv_loop(connection_t *con) {

  packet_header_t *header;
  char *data;

  char buf[MAX_PACKET_SIZE];
  int recv_len;

  while (1) {
    ((packet_header_t *)buf)->reliable = 0;
    ((packet_header_t *)buf)->crc = 0;
    ((packet_header_t *)buf)->hcrc = 0;
    ((packet_header_t *)buf)->type = PING;
    con_sendto(con, buf, MAX_PACKET_SIZE, 0);
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
    
    if (get_packet(buf, recv_len, &header, &data)) {
      TRACE(NETWORK_TRACE, TRACE_PRINT("Unable to get packet header from recieved data."));
    }

    if (validate_packet(header, data)) {
      request_packet(con, header->count);      
      continue;
    }

    switch (header->type) {
      case ACK:
        process_ack(con, header, data);
        break;
      case RESEND:
        process_resend(con, header, data);
        break;
      case PING:
        process_ping(con, header, data);
        break;
      case DATA:
        process_data(con, header, data);
        break;
      default:
        continue;
    }

  }
}

