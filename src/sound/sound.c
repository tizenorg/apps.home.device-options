/*
 * device options
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/


#include <stdio.h>
#include <sound_manager.h>
#include "device-options.h"

#define SOUND_ID   1600
#define SOUND_NAME "sound"
#define SOUND_ICON_SOUND         "micro-sound.png"
#define SOUND_ICON_VIBRATION     "micro-vibration.png"
#define SOUND_ICON_MUTE          "micro-mute.png"
#define SOUND_TEXT_SOUND         "IDS_ST_MBODY_SOUND_ABB"
#define SOUND_TEXT_VIBRATION     "IDS_COM_POP_VIBRATION"
#define SOUND_TEXT_MUTE          "IDS_ST_BODY_MUTE"

#define DEFAULT_RINGTONE_LEVEL   1

enum sound_status {
	SOUND,
	VIBRATION,
	MUTE,
};

static int get_sound_status(void)
{
	int snd, vib;

	if (vconf_get_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL, &snd) != 0)
		return -ENOMEM;

	if (snd == 1)
		return SOUND;

	if (vconf_get_bool(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL, &vib) != 0)
		return -ENOMEM;

	if (vib == 1)
		return VIBRATION;

	return MUTE;
}

static int get_sound_text(int status, char *text, int len)
{
	char *title;

	if (!text || len <= 0)
		return -EINVAL;

	switch(status) {
	case SOUND:
		title = SOUND_TEXT_SOUND;
		break;
	case VIBRATION:
		title = SOUND_TEXT_VIBRATION;
		break;
	case MUTE:
		title = SOUND_TEXT_MUTE;
		break;
	default:
		return -EINVAL;
	}
	snprintf(text, len, "%s", title);

	return 0;
}

static int get_sound_icon(int status, char *icon, int len)
{
	char *image;

	if (!icon || len <= 0)
		return -EINVAL;

	switch(status) {
	case SOUND:
		image = SOUND_ICON_SOUND;
		break;
	case VIBRATION:
		image = SOUND_ICON_VIBRATION;
		break;
	case MUTE:
		image = SOUND_ICON_MUTE;
		break;
	default:
		return -EINVAL;
	}
	snprintf(icon, len, "%s", image);

	return 0;
}

static int change_sound_status(void)
{
	int current = get_sound_status();
	int ret, ringtone;

	switch (current) {
	case SOUND: /* Change to Vibration */
		vconf_set_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL, 0);
		vconf_set_bool(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL, 1);
		play_feedback(FEEDBACK_TYPE_VIBRATION, FEEDBACK_PATTERN_VIBRATION_ON);
		break;

	case VIBRATION: /* Change to Mute */
		vconf_set_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL, 0);
		vconf_set_bool(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL, 0);
		break;

	case MUTE: /* Change to Sound */
		vconf_set_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL, 1);
		vconf_set_bool(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL, 0);
		play_feedback(FEEDBACK_TYPE_SOUND, FEEDBACK_PATTERN_SILENT_OFF);
		ret = sound_manager_get_volume(SOUND_TYPE_RINGTONE, &ringtone);
		if (ret == SOUND_MANAGER_ERROR_NONE && ringtone == 0) {
			ret = sound_manager_set_volume(SOUND_TYPE_RINGTONE, DEFAULT_RINGTONE_LEVEL);
			if (ret != SOUND_MANAGER_ERROR_NONE)
				_E("Failed to set ringtone volume");
		}
		break;

	default:
		return -EINVAL;
	}

	return 0;
}



static bool sound_terminate(void)
{
	return false;
}

static bool sound_enabled(void)
{
	return true;
}

static int sound_item_type(void)
{
	return HALF_ITEM;
}

static int sound_id(void)
{
	return SOUND_ID;
}

static int sound_icon(char *icon, unsigned int len)
{
	char ic[BUF_MAX];
	int status;

	if (!icon)
		return -EINVAL;

	status = get_sound_status();
	if (get_sound_icon(status, ic, sizeof(ic)) < 0)
		return -ENOENT;

	snprintf(icon, len, "%s", ic);
	return 0;
}

static int sound_text(char *text, unsigned int len)
{
	char content[BUF_MAX];
	int status;

	if (!text)
		return -EINVAL;

	status = get_sound_status();
	if (get_sound_text(status, content, sizeof(content)) < 0)
		return -ENOENT;

	snprintf(text, len, "%s", content);
	return 0;
}

static int sound_clicked(void *data)
{
	_I("Sound is clicked");

	if (change_sound_status() < 0)
		_E("Failed to change sound state");

	return 0;
}

static int sound_reg_handlers(void *data)
{
	return 0;
}

static int sound_unreg_handlers(void *data)
{
	return 0;
}

static struct device_option sound_ops = {
	.name                = SOUND_NAME,
	.terminate           = sound_terminate,
	.is_enabled          = sound_enabled,
	.get_item_type       = sound_item_type,
	.get_id              = sound_id,
	.get_icon            = sound_icon,
	.get_text            = sound_text,
	.response_clicked    = sound_clicked,
	.clicked_data        = NULL,
	.register_handlers   = sound_reg_handlers,
	.unregister_handlers = sound_unreg_handlers,
};

static __attribute__ ((constructor)) void register_sound_item(void)
{
	if (enhanced_power_saving_mode())
		return;
	register_device_options_item(&sound_ops);
}
