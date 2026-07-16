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



/*
    40MHz Wi-Fi splits a symbol into 128 orthogonal subcarrier
    frequencies. Each of these frequencies carries an independent
    data stream, so the receiver must measure CSI separately
    for each subcarrier frequency. In practice, the Wi-Fi standard
    does not use all 128 subcarriers. 5 carriers on the high and
    low ends of the spectrum are unused, to reduce the effect
    of interference between adjacent channels. 3 carriers around
    the center frequency are also left unused to simplify the
    receiver circuitry, and 6 carriers called 'pilots' transmit dummy
    data.
*/
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
    for (size_t i = 0; i < sample_count; i++) {
        int real = buf[i * 2];
        int imag = buf[i * 2 + 1];
        int magnitude = abs(real) + abs(imag);
        if (magnitude > 255) {
            magnitude = 255;
        }
        
        (uint16_t)magnitude;

        // load_h_40
    }
}

int main() {
    uint8_t buffer[] = {0,0,0,0,0,0,0,0,0,0,0,0,250,5,249,5,249,5,248,5,248,5,247,5,246,4,246,4,245,3,245,1,245,1,246,255,247,255,249,255,251,255,252,0,252,1,253,2,253,3,253,4,253,4,253,4,253,5,253,4,254,4,255,4,0,0,1,5,2,6,3,7,4,9,3,9,3,10,3,10,3,10,4,10,5,11,5,11,6,11,6,11,6,11,8,12,7,12,7,13,8,13,7,13,7,13,7,12,6,11,6,11,7,10,7,9,7,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,168,1,171,6,168,4,171,5,174,3,173,2,173,3,174,2,178,5,178,7,186,5,193,3,201,251,205,246,208,241,209,235,209,233,209,232,210,234,212,230,213,229,216,223,214,224,216,226,212,230,210,236,215,239,217,244,223,246,230,250,237,252,248,249,255,245,7,243,13,238,18,231,24,224,24,219,23,216,21,214,18,213,16,210,12,214,8,215,7,219,5,223,4,227,5,229,8,230,13,230,17,225,20,220,19,219,17,214,13,214,8,214,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,233,35,230,37,227,34,225,35,223,37,220,36,215,33,211,32,209,28,208,23,208,18,209,11,216,7,221,2,231,4,238,5,241,11,244,14,246,20,249,24,248,27,249,27,249,27,250,27,254,23,3,23,10,23,14,27,21,28,26,33,30,39,33,43,33,43,33,47,33,48,36,48,39,45,41,47,45,47,48,49,51,50,51,51,52,55,54,55,56,54,56,55,53,54,52,54,50,50,49,45,48,41,47,37,47,31,45,22,50,14,56,9,61,3,0,0};
    wifi_csi_info_t csi_info;

    // Copy raw bytes directly into the structural layout
    memcpy(&csi_info, buffer, sizeof(csi_infodata));

    wifi_csi_rx_cb(NULL, csi_info);

    return 0;
}






/*******************/
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

// Minimal CsiData layout assumed from Rust usage
typedef struct {
    const uint8_t *buf;
    size_t len;
} CsiData;

typedef struct {
    float re;
    float im;
} ComplexF32;

// Returns pointer to an array of 128 ComplexF32 on success, or NULL on failure.
// Caller is responsible for free()ing the returned pointer.
ComplexF32 *load_h_40(const CsiData *entry) {
    const size_t N = 128;
    const size_t EXPECTED_LEN = 384; // as checked in Rust

    if (!entry || entry->len != EXPECTED_LEN) {
        return NULL;
    }

    ComplexF32 *h = (ComplexF32 *)calloc(N, sizeof(ComplexF32));
    if (!h) {
        return NULL;
    }

    // Fill h: bytes at offsets 128 + 2*ii  (imag) and 128 + 2*ii + 1 (real)
    for (size_t ii = 0; ii < N; ++ii) {
        // Interpret the u8 as i8 (signed) like Rust's (entry.buf[...] as i8)
        int8_t imag_i8 = (int8_t)entry->buf[128 + 2 * ii];
        int8_t real_i8 = (int8_t)entry->buf[128 + 2 * ii + 1];
        h[ii].re = (float)real_i8;
        h[ii].im = (float)imag_i8;
    }

    // Compute sum = sum of norms (magnitudes)
    float sum = 0.0f;
    for (size_t ii = 0; ii < N; ++ii) {
        float re = h[ii].re;
        float im = h[ii].im;
        sum += sqrtf(re * re + im * im);
    }

    // Avoid division by zero: if sum is zero, treat as failure (or skip scaling).
    if (sum == 0.0f) {
        free(h);
        return NULL;
    }

    // Divide each complex sample by the real scalar 'sum' (Rust divides by Complex(sum,0))
    for (size_t ii = 0; ii < N; ++ii) {
        h[ii].re /= sum;
        h[ii].im /= sum;
    }

    // Multiply indices 64..127 by j (0 + 1i): (a + ib) * i = -b + i*a
    for (size_t ii = 64; ii < N; ++ii) {
        float old_re = h[ii].re;
        float old_im = h[ii].im;
        h[ii].re = -old_im;
        h[ii].im =  old_re;
    }

    return h;
}