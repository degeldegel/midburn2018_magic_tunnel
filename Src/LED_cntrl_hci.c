#include "stm32f4xx_hal.h"
#include "ws2812b_multi_strip_driver.h"
#include "LED_shows.h"
#include "LED_cntrl_hci.h"
#include "stdlib.h"

uint8_t          uart_hci_buffer[SIZE_OF_HCI_BUFFER];
hci_uart_state_e uart_hci_state;

extern volatile show_db_t shows[NUM_OF_SHOWS];

uint8_t handle_new_hci_command()
{
    if (uart_hci_buffer[HCI_PCKT_SYNC_WORD] != MAGIC_TUNNEL_HCI_SYNC_WORD) return HCI_COMMAND_FAILED;
    switch (uart_hci_buffer[HCI_PCKT_OPCODE])
    {
        case HCI_OPCODE_SHOW_CONFIG:
        {
            hci_show_config_pckt_t *p_show_config_pckt = (hci_show_config_pckt_t*)uart_hci_buffer;
            if (uart_hci_buffer[HCI_PCKT_LENGTH] != SHOW_CONFIG_LENGTH_SIZE) return HCI_COMMAND_FAILED;
            shows[p_show_config_pckt->show_id].max_power = p_show_config_pckt->max_power;
            if (p_show_config_pckt->direction != 3 /* Don't configure direction */)
            {
                shows[p_show_config_pckt->show_id].direction = p_show_config_pckt->direction;
            }
        } break;
        case HCI_OPCODE_STORE_CONFIG:
        {
            hci_config_flash_pckt_t *p_config_flash_pckt = (hci_config_flash_pckt_t*)uart_hci_buffer;
            if (uart_hci_buffer[HCI_PCKT_LENGTH] != CONFIG_FLASH_LENGTH_SIZE) return HCI_COMMAND_FAILED;
            switch (p_config_flash_pckt->action)
            {
                case CONFIG_FLASH_STORE_2_FLASH:
                    store_config_to_flash();
                    break;
                case CONFIG_FLASH_LOAD_FROM_FLASH:
                    load_config_from_flash();
                    break;
                case CONFIG_FLASH_LOAD_DEFAULTS:
                    load_default_configuration();
                    break;
            }
        } break;
        case HCI_OPCODE_RESERVED_1:
        case HCI_OPCODE_RESERVED_2:
        case HCI_OPCODE_RESERVED_3:
            break;
        default:
            return HCI_COMMAND_FAILED;
    }
    return HCI_COMMAND_SUCCESS;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uint16_t size_left=0;
    uint8_t hci_packet_fully_received = FALSE;
    if (uart_hci_state == HCI_UART_STATE_WAIT_FOR_HEADER)
    {
        if (uart_hci_buffer[HCI_PCKT_SYNC_WORD] == MAGIC_TUNNEL_HCI_SYNC_WORD)
        {
           size_left = uart_hci_buffer[HCI_PCKT_LENGTH];
        }
        else
        {
            //TODO - perform clean of UART pipe.
        }
        hci_packet_fully_received = (size_left == 0) ? TRUE : FALSE;
    }
    else
    {
        hci_packet_fully_received = TRUE;
    }

    if (hci_packet_fully_received)
    {
        if (HCI_COMMAND_FAILED == handle_new_hci_command())
        {
            /* if handle command fails get stuck - something went wrong!*/
            while(1){};
        }
        uart_hci_state = HCI_UART_STATE_WAIT_FOR_HEADER;
        HAL_UART_Receive_IT(huart, &uart_hci_buffer[HCI_PCKT_SYNC_WORD], HCI_PCKT_HEADER_SIZE);
    }
    else
    {
        uart_hci_state = HCI_UART_STATE_WAIT_FOR_REST;
        HAL_UART_Receive_IT(huart, &uart_hci_buffer[HCI_PCKT_DATA0], size_left);
    }

}

void init_HCI_UART(UART_HandleTypeDef *huart)
{
    uart_hci_state = HCI_UART_STATE_WAIT_FOR_HEADER;
    HAL_UART_Receive_IT(huart, &uart_hci_buffer[HCI_PCKT_SYNC_WORD], HCI_PCKT_HEADER_SIZE);
}
