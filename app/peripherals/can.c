#include "can.h"

#include <string.h>

LOG_MODULE_REGISTER(can);


void can_tx_callback(const struct device *dev, int error, void *user_data)
{
    char *sender = (char *)user_data;

    if (error != 0) {
        LOG_ERR("Sending failed [%d]\nSender: %s\n", error, sender);
    }
}
int can_send_(const struct device *can_dev, uint8_t id, uint8_t *data, uint8_t data_len)
{
    struct can_frame frame = {
        .flags = 0,
        .id = id,
        .dlc = data_len
};
    memset(&frame.data[0], data, data_len);

    return can_send(can_dev, &frame, K_FOREVER, can_tx_callback, "Sender 1");
}



void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data)
{

}



int can_init(const struct device *can_dev, uint16_t baudrate)
{
    struct can_timing timing;
    int ret, filter_id;
    const struct can_filter my_filter = {
        .flags = 0U,
        .id = 0x123,
        .mask = CAN_STD_ID_MASK
    };

    filter_id = can_add_rx_filter(can_dev, can_rx_callback, NULL, &my_filter);
    if (filter_id < 0) {
        LOG_ERR("Unable to add rx filter [%d]", filter_id);
    }
    ret = can_calc_timing(can_dev, &timing, baudrate, 875);
    if (ret > 0) {
        LOG_INF("Sample-Point error: %d", ret);
    }
    if (ret < 0) {
        LOG_ERR("Failed to calc a valid timing");
        return -1;
    }

    ret = can_set_timing(can_dev, &timing);
    if (ret != 0) {
        LOG_ERR("Failed to set timing");
    }

    ret = can_start(can_dev);
    if (ret != 0) {
        LOG_ERR("Failed to start CAN controller");
    }
}