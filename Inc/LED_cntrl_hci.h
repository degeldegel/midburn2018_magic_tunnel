#ifndef _LED_CNTRL_HCI_H
#define _LED_CNTRL_HCI_H

/* =========================================================================================== */
/* ==  DEFINES == */
/* =========================================================================================== */

#define HCI_COMMAND_SUCCESS (0)
#define HCI_COMMAND_FAILED  (1)
#define SIZE_OF_HCI_BUFFER (40)
#define MAGIC_TUNNEL_HCI_SYNC_WORD (0x55)
#define HCI_PCKT_HEADER_SIZE (3)

/* HCI commands lengths */
#define SHOW_CONFIG_LENGTH_SIZE (3)

/* =========================================================================================== */
/* ==  ENUMS == */
/* =========================================================================================== */
typedef enum _hci_uart_state
{
    HCI_UART_STATE_WAIT_FOR_HEADER = 0,
    HCI_UART_STATE_WAIT_FOR_REST   = 1
} hci_uart_state_e;

typedef enum _hci_packet_bytes
{
    HCI_PCKT_SYNC_WORD = 0,
    HCI_PCKT_OPCODE    = 1,
    HCI_PCKT_LENGTH    = 2,
    HCI_PCKT_DATA0     = 3
} hci_packet_bytes_e;

typedef enum _hci_opcodes
{
    HCI_OPCODE_NO_OPCODE        = 0,
    HCI_OPCODE_SHOW_CONFIG      = 1,
    HCI_OPCODE_RESERVED_0       = 2,
    HCI_OPCODE_RESERVED_1       = 3,
    HCI_OPCODE_RESERVED_2       = 4,
    HCI_OPCODE_RESERVED_3       = 5
} hci_opcodes_e;

/* =========================================================================================== */
/* ==  STRUCTS == */
/* =========================================================================================== */

typedef struct _hci_show_config_pckt
{
    uint8_t sync_word;
    uint8_t opcode;
    uint8_t length;
    uint8_t show_id;
    uint8_t max_power;
    uint8_t direction;
} hci_show_config_pckt_t;

/* =========================================================================================== */
/* ==  MACROS == */
/* =========================================================================================== */


/* =========================================================================================== */
/* ==  EXTERNAL FUNCTIONS== */
/* =========================================================================================== */

void init_HCI_UART(UART_HandleTypeDef *huart);

#endif  /* _LED_CNTRL_HCI_H */
