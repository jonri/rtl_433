/** @file
    Honeywell (Ademco) Door/Window Sensors (345.0Mhz).
*/
/**
Honeywell (Ademco) Door/Window Sensors (345.0Mhz).

Tested with the Honeywell 5811 Wireless Door/Window transmitters.

Also: 2Gig DW10 door sensors,
and Resolution Products RE208 (wire to air repeater).

Maybe: 5890PI?

64 bit packets, repeated multiple times per open/close event.

Protocol whitepaper: "DEFCON 22: Home Insecurity" by Logan Lamb.

Data layout:

    PP PP C IIIII EE SS SS

- P: 16bit Preamble and sync bit (always ff fe)
- C: 4bit Channel
- I: 20bit Device serial number / or counter value
- E: 8bit Event, where 0x80 = Open/Close, 0x04 = Heartbeat / or id
- S: 16bit CRC

*/

#include "decoder.h"

static int honeywell_callback(r_device *decoder, bitbuffer_t *bitbuffer)
{
    // full preamble is 0xFFFE
    uint8_t const preamble_pattern[2] = {0xff, 0xe0}; // 12 bits

    data_t *data;
    int row;
    int pos;
    uint8_t b[6] = {0};
    int channel;
    int device_id;
    int event;
    uint16_t crc_calculated;
    uint16_t crc;
    int state;
    int heartbeat;
    int alarm;
    int tamper;
    int battery_low;

    row = 0; // we expect a single row only. reduce collisions
    if (bitbuffer->num_rows != 1 || bitbuffer->bits_per_row[row] < 60)
        return 0; // Short buffer

    bitbuffer_invert(bitbuffer);

    pos = bitbuffer_search(bitbuffer, row, 0, preamble_pattern, 12) + 12;
    if (pos >= bitbuffer->bits_per_row[row])
        return 0;
    bitbuffer_extract_bytes(bitbuffer, row, pos, b, 48);

    channel   = b[0] >> 4;
    device_id = ((b[0] & 0xf) << 16) | (b[1] << 8) | b[2];
    crc       = (b[4] << 8) | b[5];

    if (device_id == 0 && crc == 0)
        return 0; // Reduce collisions

    if (channel != 0x8) {
        // 2GIG brand
        crc_calculated = crc16(b, 4, 0x8050, 0);
    } else { // channel == 0x8
        crc_calculated = crc16(b, 4, 0x8005, 0);
    }

    if (crc != crc_calculated)
        return 0; // Not a valid packet

    event = b[3];
    // decoded event bits: AATABHUU
    state       = (event & 0x80) >> 7;
    alarm       = (event & 0xb0) >> 4;
    tamper      = (event & 0x40) >> 6;
    battery_low = (event & 0x08) >> 3;
    heartbeat   = (event & 0x04) >> 2;

    /* clang-format off */
    data = data_make(
            "model",        "", DATA_STRING, _X("Honeywell-Security","Honeywell Door/Window Sensor"),
            "id",           "", DATA_FORMAT, "%05x", DATA_INT, device_id,
            "channel",      "", DATA_INT,    channel,
            "event",        "", DATA_FORMAT, "%02x", DATA_INT, event,
            "state",        "", DATA_STRING, state ? "open" : "closed",
            "alarm",        "", DATA_INT,    alarm,
            "tamper",       "", DATA_INT,    tamper,
            "battery_ok",   "", DATA_INT,    !battery_low,
            "heartbeat",    "", DATA_INT,    heartbeat,
            NULL);
    /* clang-format on */

    decoder_output_data(decoder, data);
    return 1;
}

static char *output_fields[] = {
        "model",
        "id",
        "channel",
        "event",
        "state",
        "alarm",
        "tamper",
        "battery_ok",
        "heartbeat",
        NULL,
};

r_device honeywell = {
        .name        = "Honeywell Door/Window Sensor",
        .modulation  = OOK_PULSE_MANCHESTER_ZEROBIT,
        .short_width = 156,
        .long_width  = 0,
        .reset_limit = 292,
        .decode_fn   = &honeywell_callback,
        .disabled    = 0,
        .fields      = output_fields,
};
