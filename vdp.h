#ifndef VDP_H
#define VDP_H

#include <stddef.h>

#include "clowncommon/clowncommon.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VDP_MAX_SCANLINE_WIDTH 320
#define VDP_MAX_SCANLINES (240 * 2) /* V30 in interlace mode 2 */

typedef struct VDP_Configuration
{
	cc_bool sprites_disabled;
	cc_bool window_disabled;
	cc_bool planes_disabled[2];
} VDP_Configuration;

typedef struct VDP_Constant
{
	cc_u8l blit_lookup[1 << (1 + 1 + 2 + 4)][1 << (1 + 2 + 4)];
	cc_u8l blit_lookup_shadow_highlight[1 << (1 + 1 + 2 + 4)][1 << (1 + 2 + 4)];
} VDP_Constant;

typedef enum VDP_Access
{
	VDP_ACCESS_VRAM,
	VDP_ACCESS_CRAM,
	VDP_ACCESS_VSRAM
} VDP_Access;

typedef enum VDP_DMAMode
{
	VDP_DMA_MODE_MEMORY_TO_VRAM,
	VDP_DMA_MODE_FILL,
	VDP_DMA_MODE_COPY
} VDP_DMAMode;

typedef enum VDP_HScrollMode
{
	VDP_HSCROLL_MODE_FULL,
	VDP_HSCROLL_MODE_1CELL,
	VDP_HSCROLL_MODE_1LINE
} VDP_HScrollMode;

typedef enum VDP_VScrollMode
{
	VDP_VSCROLL_MODE_FULL,
	VDP_VSCROLL_MODE_2CELL
} VDP_VScrollMode;

typedef struct VDP_SpriteRowCacheEntry
{
	cc_u8l table_index;
	cc_u8l y_in_sprite;
	cc_u8l width;
	cc_u8l height;
} VDP_SpriteRowCacheEntry;

typedef struct VDP_SpriteRowCacheRow
{
	cc_u8l total;
	VDP_SpriteRowCacheEntry sprites[20];
} VDP_SpriteRowCacheRow;

typedef struct VDP_State
{
	struct
	{
		cc_bool write_pending;
		cc_u16l cached_write;

		VDP_Access selected_buffer;

		cc_bool read_mode;
		cc_u16l index;
		cc_u16l increment;
	} access;

	struct
	{
		cc_bool enabled;
		cc_bool pending;
		VDP_DMAMode mode;
		cc_u8l source_address_high;
		cc_u16l source_address_low;
		cc_u16l length;
	} dma;

	cc_u16l plane_a_address;
	cc_u16l plane_b_address;
	cc_u16l window_address;
	cc_u16l sprite_table_address;
	cc_u16l hscroll_address;

	cc_bool window_aligned_right;
	cc_bool window_aligned_bottom;
	cc_u8l window_horizontal_boundary;
	cc_u8l window_vertical_boundary;

	cc_u16l plane_width;
	cc_u16l plane_height;
	cc_u16l plane_width_bitmask;
	cc_u16l plane_height_bitmask;

	cc_bool display_enabled;
	cc_bool v_int_enabled;
	cc_bool h_int_enabled;
	cc_bool h40_enabled;
	cc_bool v30_enabled;
	cc_bool shadow_highlight_enabled;
	cc_bool double_resolution_enabled;

	cc_u8l background_colour;
	cc_u8l h_int_interval;
	cc_bool currently_in_vblank;

	VDP_HScrollMode hscroll_mode;
	VDP_VScrollMode vscroll_mode;

	cc_u16l vram[0x8000];
	cc_u16l cram[4 * 16];
	/* http://gendev.spritesmind.net/forum/viewtopic.php?p=36727#p36727 */
	/* According to Mask of Destiny on SpritesMind, later models of Mega Drive (MD2 VA4 and later) have 64 words
	   of VSRAM, instead of the 40 words that earlier models have. */
	cc_u16l vsram[64];

	cc_u16l sprite_table_cache[80][2];

	struct
	{
		cc_bool needs_updating;
		VDP_SpriteRowCacheRow rows[VDP_MAX_SCANLINES];
	} sprite_row_cache;
} VDP_State;

typedef struct VDP
{
	const VDP_Configuration *configuration;
	const VDP_Constant *constant;
	VDP_State *state;
} VDP;

void VDP_Constant_Initialise(VDP_Constant *constant);
void VDP_State_Initialise(VDP_State *state);
void VDP_RenderScanline(const VDP *vdp, cc_u16f scanline, void (*scanline_rendered_callback)(const void *user_data, cc_u16f scanline, const cc_u8l *pixels, cc_u16f screen_width, cc_u16f screen_height), const void *scanline_rendered_callback_user_data);

cc_u16f VDP_ReadData(const VDP *vdp);
cc_u16f VDP_ReadControl(const VDP *vdp);
void VDP_WriteData(const VDP *vdp, cc_u16f value, void (*colour_updated_callback)(const void *user_data, cc_u16f index, cc_u16f colour), const void *colour_updated_callback_user_data);
void VDP_WriteControl(const VDP *vdp, cc_u16f value, void (*colour_updated_callback)(const void *user_data, cc_u16f index, cc_u16f colour), const void *colour_updated_callback_user_data, cc_u16f(*read_callback)(const void *user_data, cc_u32f address), const void *read_callback_user_data);

#ifdef __cplusplus
}
#endif

#endif /* VDP_H */
