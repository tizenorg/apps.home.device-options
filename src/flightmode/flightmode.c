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
#include <dd-deviced.h>
#include <syspopup_caller.h>
#include "device-options.h"
#include "show-ui.h"

#define FLIGHTMODE_ID   1200
#define FLIGHTMODE_NAME "flightmode"
#define FLIGHTMODE_TEXT "IDS_ST_HEADER_FLIGHT_MODE_ABB"

#define SYSTEM_SYSPOPUP    "system-syspopup"
#define POPUP_KEY_TYPE     "_SYSPOPUP_CONTENT_"
#define FLIGHTMODE_ENABLE  "flightmode_enable"
#define FLIGHTMODE_DISABLE "flightmode_disable"

static const char *item_icon[] = {
	"micro-flightmodeoff.png",
	"micro-flightmodeon.png",
};

static void launch_flightmode_popup(const char *enable)
{
	bundle *b;
	int ret;

	if (!enable)
		return;

	b = bundle_create();
	if (!b) {
		_E("Failed to create bundle");
		return;
	}

	ret = bundle_add(b, POPUP_KEY_TYPE, enable);
	if (ret < 0) {
		_E("Failed to add value to bundle");
		bundle_free(b);
		return;
	}

	ret = syspopup_launch(SYSTEM_SYSPOPUP, b);
	bundle_free(b);
	if (ret < 0)
		_E("Failed to launch flight mode popup (%d)", ret);
}

static int get_flightmode(void)
{
	static int flightmode = -1;
	int ret;

	ret = vconf_get_bool(VCONFKEY_TELEPHONY_FLIGHT_MODE, &flightmode);
	if (ret != 0)
		return ret;
	_I("Flight mode: (%d)", flightmode);

	return flightmode;
}

static void flightmode_changed(keynode_t *in_key, void *data)
{
	struct device_option *opt = data;
	update_item(opt);
}

static bool flightmode_terminate(void)
{
	return true;
}

static bool flightmode_enabled(void)
{
	return true;
}

static int flightmode_item_type(void)
{
	return HALF_ITEM;
}

static int flightmode_id(void)
{
	return FLIGHTMODE_ID;
}

static int flightmode_icon(char *icon, unsigned int len)
{
	if (!icon)
		return -EINVAL;
	snprintf(icon, len, "%s", item_icon[get_flightmode()]);
	return 0;
}

static int flightmode_text(char *text, unsigned int len)
{
	if (!text)
		return -EINVAL;
	snprintf(text, len, "%s", FLIGHTMODE_TEXT);
	return 0;
}

static int flightmode_clicked(void *data)
{
	int mode;
	char *enable;

	_I("Flight mode is clicked");

	mode = get_flightmode();
	switch (mode) {
	case 0: /* flightmode off */
		enable = FLIGHTMODE_ENABLE;
		break;
	case 1: /* flightmode on */
		enable = FLIGHTMODE_DISABLE;
		break;
	default:
		return -EINVAL;
	}

	launch_flightmode_popup(enable);

	return 0;
}

static int flightmode_reg_handlers(void *data)
{
	return vconf_notify_key_changed(VCONFKEY_TELEPHONY_FLIGHT_MODE, flightmode_changed, data);
}

static int flightmode_unreg_handlers(void *data)
{
	vconf_ignore_key_changed(VCONFKEY_TELEPHONY_FLIGHT_MODE, flightmode_changed);
	return 0;
}

static struct device_option flightmode_ops = {
	.name                = FLIGHTMODE_NAME,
	.terminate           = flightmode_terminate,
	.is_enabled          = flightmode_enabled,
	.get_item_type       = flightmode_item_type,
	.get_id              = flightmode_id,
	.get_icon            = flightmode_icon,
	.get_text            = flightmode_text,
	.response_clicked    = flightmode_clicked,
	.clicked_data        = NULL,
	.register_handlers   = flightmode_reg_handlers,
	.unregister_handlers = flightmode_unreg_handlers,
};

static __attribute__ ((constructor)) void register_flightmode_item(void)
{
	if (enhanced_power_saving_mode())
		return;
	register_device_options_item(&flightmode_ops);
}
