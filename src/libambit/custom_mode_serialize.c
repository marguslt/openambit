#include "custom_mode_serialize.h"
#include "libambit.h"

#include <stdlib.h>
#include <string.h>

const u_int8_t UNKNOWN_DISPLAYES[] =
        {0x06,0x01,0x3e,0x00,0x07,0x01,0x04,0x00,0x11,0x01,0x04,0x00,0x08,0x01,0x08,0x00,0x09,0x01,0x04,0x00,0x00,0x00,0x08,0x00,0x08,0x01,0x08,0x00,0x09,0x01,0x04,0x00,0x01,0x00,0x08,0x00,0x08,0x01,0x1a,0x00,0x09,0x01,0x04,0x00,0x02,0x00,0x00,0x00,0x0a,0x01,0x02,0x00,0x10,0x00,0x0a,0x01,0x02,0x00,0x01,0x00,0x0a,0x01,0x02,0x00,0xfe,0xff
        ,0x06,0x01,0x44,0x00,0x07,0x01,0x04,0x00,0x23,0x01,0x05,0x00,0x08,0x01,0x08,0x00,0x09,0x01,0x04,0x00,0x00,0x00,0x08,0x00,0x08,0x01,0x08,0x00,0x09,0x01,0x04,0x00,0x01,0x00,0x28,0x00,0x08,0x01,0x20,0x00,0x09,0x01,0x04,0x00,0x02,0x00,0x00,0x00,0x0a,0x01,0x02,0x00,0x10,0x00,0x0a,0x01,0x02,0x00,0x08,0x00,0x0a,0x01,0x02,0x00,0x01,0x00,0x0a,0x01,0x02,0x00,0xfe,0xff
        ,0x06,0x01,0x3e,0x00,0x07,0x01,0x04,0x00,0x22,0x01,0x06,0x00,0x08,0x01,0x08,0x00,0x09,0x01,0x04,0x00,0x00,0x00,0x18,0x00,0x08,0x01,0x08,0x00,0x09,0x01,0x04,0x00,0x01,0x00,0x19,0x00,0x08,0x01,0x1a,0x00,0x09,0x01,0x04,0x00,0x02,0x00,0x00,0x00,0x0a,0x01,0x02,0x00,0x32,0x00,0x0a,0x01,0x02,0x00,0x1a,0x00,0x0a,0x01,0x02,0x00,0x10,0x00
        ,0x06,0x01,0x08,0x00,0x07,0x01,0x04,0x00,0x50,0x01,0x07,0x00
        ,0x06,0x01,0x4a,0x00,0x07,0x01,0x04,0x00,0x04,0x01,0x32,0x00,0x08,0x01,0x08,0x00,0x09,0x01,0x04,0x00,0x00,0x00,0x3e,0x00,0x08,0x01,0x08,0x00,0x09,0x01,0x04,0x00,0x01,0x00,0x3d,0x00,0x08,0x01,0x26,0x00,0x09,0x01,0x04,0x00,0x02,0x00,0x00,0x00,0x0a,0x01,0x02,0x00,0x05,0x00,0x0a,0x01,0x02,0x00,0x0a,0x00,0x0a,0x01,0x02,0x00,0x15,0x00,0x0a,0x01,0x02,0x00,0x0b,0x00,0x0a,0x01,0x02,0x00,0x1c,0x00};


static void serialize_header(u_int16_t header_nbr, u_int16_t length, u_int8_t *dataWrite);
static int serialize_custom_modes(ambit_device_settings_t *ambit_settings, u_int8_t *data);
static int serialize_custom_mode(ambit_custom_mode_t *ambit_custom_mode, u_int8_t *data);
static u_int8_t serialize_settings(ambit_custom_mode_settings_t *settings, u_int8_t *data);
static int serialize_displays(ambit_custom_mode_t *ambit_custom_mode, u_int8_t *data);
static int serialize_display(ambit_custom_mode_display_t *display, u_int8_t *data);
static int serialize_display_layout(uint16_t displayType, u_int8_t *data);
static int serialize_rows(ambit_custom_mode_display_t *display, u_int8_t *data);
static int serialize_row_entry(u_int16_t row_nbr, u_int16_t row_item, u_int8_t *data);
static int serialize_views(ambit_custom_mode_display_t *display, u_int8_t *data);
static int serialize_view_entry(uint16_t view, u_int8_t *data);

static int serialize_custom_mode_groups(ambit_device_settings_t *ambit_settings, u_int8_t *data);
static int serialize_custom_mode_group(ambit_custom_mode_group_t *custom_mode_group, u_int8_t *data);
static int serialize_name(char *activity_name, u_int8_t *data);
static int serialize_activity_id(uint16_t activity_id, u_int8_t *data);
static int serialize_modes_id(u_int16_t index, u_int8_t *data);


int calculate_size_for_serialize_device_settings(ambit_device_settings_t *ambit_device_settings)
{
    int serialize_buffer_size = 0;
    int i, j;

    serialize_buffer_size += HEADER_SIZE * 2;
    serialize_buffer_size += 6; // For unknown data field;

    // Add size for custom modes
    for (i = 0; i < ambit_device_settings->custom_modes_count; i++) {
        serialize_buffer_size += HEADER_SIZE;
        serialize_buffer_size += HEADER_SIZE + sizeof(ambit_custom_mode_settings_t);

        serialize_buffer_size += HEADER_SIZE;
        // Add size for all displays
        for (j = 0; j < ambit_device_settings->custom_modes[i].displays_count; j++) {
            // Add size for type, row1 and row2 (including headers). (The needed space for some display types can be smaler).
            serialize_buffer_size += 48;
            // Add size for all views.
            serialize_buffer_size += 6 * ambit_device_settings->custom_modes[i].display[j].views_count;
        }
        serialize_buffer_size += sizeof(UNKNOWN_DISPLAYES);
    }

    // Add size for custom mode groups
    serialize_buffer_size += HEADER_SIZE;
    for (i=0; i<ambit_device_settings->custom_mode_groups_count; i++) {
        serialize_buffer_size += 38;
        serialize_buffer_size += 6 * ambit_device_settings->custom_mode_groups[i].custom_mode_index_count;
    }

    return serialize_buffer_size;
}

int serialize_device_settings(ambit_device_settings_t *ambit_settings, uint8_t *data)
{
    u_int8_t *writePosition;
    writePosition = data + HEADER_SIZE; //Save space for header.

    writePosition += serialize_custom_modes(ambit_settings, writePosition);

    writePosition += serialize_custom_mode_groups(ambit_settings, writePosition);

    serialize_header(0x0003, writePosition - data - 4, data);

    return writePosition - data;
}

static void serialize_header(u_int16_t header_nbr, u_int16_t length, u_int8_t *dataWrite)
{
    ambit_write_header_t *header = (ambit_write_header_t*)dataWrite;
    header->header = header_nbr;
    header->length = length;
}

static int serialize_unknown_data_field(u_int8_t *data)
{
    serialize_header(0x010b, 2, data);
    data += 4;
    *(u_int16_t *)data = 2;

    return 4 + 2;
}

static int serialize_custom_modes(ambit_device_settings_t *ambit_settings, u_int8_t *data)
{
    u_int8_t *writePosition = data + HEADER_SIZE; //Save space for header.

    writePosition += serialize_unknown_data_field(writePosition);
    int i;
    ambit_custom_mode_t *custom_mode = ambit_settings->custom_modes;
    for (i = 0; i < ambit_settings->custom_modes_count; i++) {
        writePosition += serialize_custom_mode(custom_mode, writePosition);
        custom_mode++;
    }

    // Write header at the end, when total data size is known.
    serialize_header(CUSTOM_MODE_START_HEADER, writePosition - data - 4, data);

    return writePosition - data;
}

static int serialize_custom_mode(ambit_custom_mode_t *ambit_custom_mode, u_int8_t *data)
{
    u_int8_t *dataWrite = data + HEADER_SIZE;

    dataWrite += serialize_settings(&(ambit_custom_mode->settings), dataWrite);
    dataWrite += serialize_displays(ambit_custom_mode, dataWrite);

    serialize_header(CUSTOM_MODE_HEADER, dataWrite - data - HEADER_SIZE, data);

    return dataWrite - data;
}

static u_int8_t serialize_settings(ambit_custom_mode_settings_t *settings, u_int8_t *data)
{
    serialize_header(SETTINGS_HEADER, SETTINGS_SIZE, data);

    memcpy(data + HEADER_SIZE, settings, sizeof(ambit_custom_mode_settings_t));

    return SETTINGS_SIZE + HEADER_SIZE;
}

static int serialize_displays(ambit_custom_mode_t *ambit_custom_mode, u_int8_t *data)
{
    u_int8_t *writePosition;
    writePosition = data + HEADER_SIZE; //Save space for header.

    int i;
    ambit_custom_mode_display_t *display = ambit_custom_mode->display;
    for (i = 0; i < ambit_custom_mode->displays_count; i++) {
        writePosition += serialize_display(display, writePosition);
        display++;
    }

    memcpy(writePosition, UNKNOWN_DISPLAYES, sizeof(UNKNOWN_DISPLAYES));
    writePosition += sizeof(UNKNOWN_DISPLAYES);

    serialize_header(DISPLAYS_HEADER, writePosition - data - HEADER_SIZE, data);

    return writePosition - data;
}

static int serialize_display(ambit_custom_mode_display_t *display, u_int8_t *data)
{
    u_int8_t *writePosition;
    writePosition = data + HEADER_SIZE; //Save space for header.

    writePosition += serialize_display_layout(display->type, writePosition);
    writePosition += serialize_rows(display, writePosition);

    serialize_header(DISPLAY_HEADER, writePosition - data - HEADER_SIZE, data);

    return writePosition - data;
}

static int serialize_display_layout(uint16_t displayType, u_int8_t *data)
{
    ambit_custom_mode_display_layout_t *layout = (ambit_custom_mode_display_layout_t *)data;
    layout->header = DISPLAY_LAYOUT_HEADER;
    layout->length = 4;
    layout->display_layout = displayType;
    layout->unknown[0] = 0x0a;
    layout->unknown[1] = 0;

    return sizeof(ambit_custom_mode_display_layout_t);
}

static int serialize_rows(ambit_custom_mode_display_t *display, u_int8_t *data)
{
    u_int8_t *writePosition = data + HEADER_SIZE; //Save space for header.

    switch (display->type) {
    case SINGLE_ROW_DISPLAY_TYPE:
    {
        writePosition += serialize_row_entry(0, display->row1, writePosition);
        serialize_header(ROWS_HEADER, writePosition - data - HEADER_SIZE, data);
        break;
    }
    case DOUBLE_ROWS_DISPLAY_TYPE:
    {
        writePosition += serialize_row_entry(0, display->row1, writePosition);
        serialize_header(ROWS_HEADER, writePosition - data - HEADER_SIZE, data);

        u_int8_t *headerPosition = writePosition;
        writePosition += HEADER_SIZE; //Save space for second line header.
        writePosition += serialize_row_entry(1, display->row2, writePosition);
        writePosition += serialize_views(display, writePosition);
        serialize_header(ROWS_HEADER, writePosition - headerPosition - HEADER_SIZE, headerPosition);
        break;
    }
    case TRIPLE_ROWS_DISPLAY_TYPE:
    case BAROGRAPH_DISPLAY_TYPE:
    {
        writePosition += serialize_row_entry(0, display->row1, writePosition);
        serialize_header(ROWS_HEADER, writePosition - data - HEADER_SIZE, data);

        u_int8_t *headerPosition = writePosition;
        writePosition += HEADER_SIZE; //Save space for second line header.
        writePosition += serialize_row_entry(1, display->row2, writePosition);
        serialize_header(ROWS_HEADER, writePosition - headerPosition - HEADER_SIZE, headerPosition);

        headerPosition = writePosition;
        writePosition += HEADER_SIZE; //Save space for third line header.
        writePosition += serialize_row_entry(2, display->row3, writePosition);
        writePosition += serialize_views(display, writePosition);
        serialize_header(ROWS_HEADER, writePosition - headerPosition - HEADER_SIZE, headerPosition);
        break;
    }
    default:
        break;
    }

    return writePosition - data;
}

static int serialize_row_entry(u_int16_t row_nbr, u_int16_t row_item, u_int8_t *data)
{
    ambit_custom_mode_row_t *row = (ambit_custom_mode_row_t *)data;
    row->header = ROW_HEADER;
    row->length = 4;
    row->row_nbr = row_nbr;
    row->item = row_item;

    return sizeof(ambit_custom_mode_row_t);
}

static int serialize_views(ambit_custom_mode_display_t *display, u_int8_t *data)
{
    u_int8_t *writePosition = data;
    uint16_t *view = display->view;

    int i;
    for (i = 0; i < display->views_count; i++) {
        writePosition += serialize_view_entry(*view, writePosition);
        view++;
    }

    return writePosition - data;
}

static int serialize_view_entry(uint16_t view, u_int8_t *data)
{
    ambit_custom_mode_view_t *ambitView = (ambit_custom_mode_view_t *)data;
    ambitView->header = VIEW_HEADER;
    ambitView->length = 2;
    ambitView->item = view;

    return sizeof(ambit_custom_mode_view_t);
}

static int serialize_custom_mode_groups(ambit_device_settings_t *ambit_settings, u_int8_t *data)
{
    u_int8_t *writePosition = data + HEADER_SIZE; //Save space for header.

    int i;
    ambit_custom_mode_group_t *custom_mode_group = ambit_settings->custom_mode_groups;
    for (i = 0; i < ambit_settings->custom_mode_groups_count; i++) {
        writePosition += serialize_custom_mode_group(custom_mode_group, writePosition);
        custom_mode_group++;
    }

    // Write header at the end, when total data size is known.
    serialize_header(CUSTOM_MODE_GROUP_START_HEADER, writePosition - data - 4, data);

    return writePosition - data;
}

static int serialize_custom_mode_group(ambit_custom_mode_group_t *custom_mode_group, u_int8_t *data)
{
    u_int8_t *dataWrite = data + HEADER_SIZE;

    dataWrite += serialize_name(custom_mode_group->activity_name, dataWrite);
    dataWrite += serialize_activity_id(custom_mode_group->activity_id, dataWrite);

    int i;
    uint16_t *group_index = custom_mode_group->custom_mode_index;
    for (i = 0; i < custom_mode_group->custom_mode_index_count; i++) {
        dataWrite += serialize_modes_id(*group_index, dataWrite);
        group_index++;
    }

    serialize_header(CUSTOM_MODE_GROUP_HEADER, dataWrite - data - HEADER_SIZE, data);

    return dataWrite - data;
}

static int serialize_name(char *activity_name, u_int8_t *data)
{
    u_int8_t *dataWrite = data + HEADER_SIZE;

    char *str = (char*)dataWrite;
    memcpy(str, activity_name, GROUP_NAME_SIZE);

    serialize_header(NAME_HEADER, GROUP_NAME_SIZE, data);

    return GROUP_NAME_SIZE + HEADER_SIZE;
}

static int serialize_activity_id(uint16_t activity_id, u_int8_t *data)
{
    u_int8_t *dataWrite = data + HEADER_SIZE;

    *(u_int16_t*)dataWrite = activity_id;
    serialize_header(ACTIVITY_ID_HEADER, sizeof(u_int16_t), data);

    return sizeof(u_int16_t) + HEADER_SIZE;
}

static int serialize_modes_id(u_int16_t index, u_int8_t *data)
{
    u_int8_t *dataWrite = data + HEADER_SIZE;

    // Write the IDs/position/index for all the custom mode that is in this group.
    *dataWrite = index;

    serialize_header(MODES_ID_HEADER, sizeof(u_int16_t), data);

    return sizeof(u_int16_t) + HEADER_SIZE;
}
