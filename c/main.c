#include <stdio.h>

// https://github.com/espressif/esp-idf/blob/master/components/esp_wifi/include/local/esp_wifi_types_native.h#L117
typedef struct wifi_csi_info_t {
    wifi_pkt_rx_ctrl_t rx_ctrl;/**< received packet radio metadata header of the CSI data */
    uint8_t mac[6];            /**< source MAC address of the CSI data */
    uint8_t dmac[6];           /**< destination MAC address of the CSI data */
    bool first_word_invalid;   /**< first four bytes of the CSI data is invalid or not, true indicates the first four bytes is invalid due to hardware limitation */
    int8_t *buf;               /**< valid buffer of CSI data */
    uint16_t len;              /**< valid length of CSI data */
    uint8_t *hdr;              /**< header of the wifi packet */
    uint8_t *payload;          /**< payload of the wifi packet */
    uint16_t payload_len;      /**< payload len of the wifi packet */
    uint16_t rx_seq;           /**< rx sequence number of the wifi packet */
} wifi_csi_info_t;

void wifi_csi_rx_cb(void *ctx, wifi_csi_info_t *data) {
    (void)ctx;

    if (data == NULL || data->buf == NULL || data->len < 2) {
        return;
    }

    size_t offset = data->first_word_invalid ? 4 : 0;
    if (offset >= data->len) {
        return;
    }

    uint16_t usable_bytes = (uint16_t)(data->len - offset);
    size_t sample_count = usable_bytes / 2;
    if (sample_count > CSI_MAX_SAMPLES) {
        sample_count = CSI_MAX_SAMPLES;
    }


    const int8_t *buf = data->buf + offset;
    for (size_t i = 0; i < sample_count; ++i) {
        int real = buf[i * 2];
        int imag = buf[i * 2 + 1];
        int magnitude = abs(real) + abs(imag);
        if (magnitude > 255) {
            magnitude = 255;
        }
        
        (uint16_t)magnitude;
    }



















    // 1. Point to the start of the CSI data buffer
    int8_t *csi_buf = data->buf;
    uint16_t csi_len = data->len;
    int start_index = 0;

    // 2. Check for the hardware limitation flag
    if (data->first_word_invalid) {
        // Skip the first 4 bytes (2 subcarriers worth of IQ data)
        start_index = 4; 
    }

    // 3. Process the remaining valid subcarrier data safely
    for (int i = start_index; i < csi_len; i += 2) {
        int8_t imag = csi_buf[i];
        int8_t real = csi_buf[i + 1];
        
        // Your algorithm logic here (e.g., amplitude calculation)
    }

    size_t offset = data->first_word_invalid ? 4 : 0;
    if (offset >= data->len) {
        return;
    }

    const int8_t *csi_buf = data->buf + offset;
    for (size_t i = 0; i < sample_count; ++i) {
        int real = buf[i * 2];
        int imag = buf[i * 2 + 1];
        int magnitude = abs(real) + abs(imag);
        if (magnitude > 255) {
            magnitude = 255;
        }
        snapshot.magnitudes[i] = (uint16_t)magnitude;
    }
}

int main() {
    return 0;
}
