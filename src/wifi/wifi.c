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
#include "device-options.h"
#include "show-ui.h"

#define WIFI_ID   1400
#define WIFI_NAME "wifi"
#define WIFI_ICON "micro-wifi.png"
#define WIFI_TEXT "IDS_ST_OPT_WI_FI"

#define VCONFKEY_WIFI_USE "db/private/wifi/wearable_wifi_use"

enum wifi_state {
	WIFI_DEACTIVATED,
	WIFI_ACTIVATED,
};

enum wifi_error {
	WIFI_ERROR_DEACTIVATING = WIFI_DEACTIVATED,
	WIFI_ERROR_ACTIVATING = WIFI_ACTIVATED,
	WIFI_NOT_SUPPORTED,
};

static char *wifi_content = WIFI_TEXT;
static int wifi_state = -1;

void wifi_register_text(char *text)
{
	if (text)
		wifi_content = text;
}

static void wifi_changed(keynode_t *in_key, void *data)
{
	struct device_option *opt = data;
	update_item(opt);
}

static void wifi_error_toast(struct appdata *ad, int err)
{
	char *text;

	switch(err) {
	case WIFI_NOT_SUPPORTED:
		text = "IDS_ST_POP_NOT_SUPPORTED";
		break;

	case WIFI_ERROR_ACTIVATING:
		text = "Wi-Fi Activation error.";
		break;
	case WIFI_ERROR_DEACTIVATING:
		text = "Wi-Fi Deactivating error.";
		break;
	default:
		_E("Invalid error (%d)", err);
		return;
	}

	if (load_popup_toast(ad, _(text)) < 0)
		_E("Falied to load error toast popup for (%d)", err);
}

static bool wifi_terminate(void)
{
	return false;
}

static bool wifi_enabled(void)
{
	return true;
}

static int wifi_item_type(void)
{
	return HALF_ITEM;
}

static int wifi_id(void)
{
	return WIFI_ID;
}

static int wifi_icon(char *icon, unsigned int len)
{
	if (!icon)
		return -EINVAL;
	snprintf(icon, len, "%s", WIFI_ICON);
	return 0;
}

static bool wifi_icon_disabled(void)
{
	int state;

	if (vconf_get_int(VCONFKEY_WIFI_USE, &state) != 0)
		return false;

	wifi_state = state;
	if (wifi_state == WIFI_DEACTIVATED)
		return true;

	return false;
}

static int wifi_text(char *text, unsigned int len)
{
	if (!text)
		return -EINVAL;
	snprintf(text, len, "%s", wifi_content);
	return 0;
}

static int wifi_clicked(void *data)
{
	int state, ret;
	struct device_option *opt = data;

	if (!opt)
		return -EINVAL;

	_I("WiFi is clicked");

	switch (wifi_state) {
	case WIFI_DEACTIVATED:
		state = WIFI_ACTIVATED;
		break;
	case WIFI_ACTIVATED:
		state = WIFI_DEACTIVATED;
		break;
	default:
		_E("WiFi not supported");
		wifi_error_toast(opt->ad, WIFI_NOT_SUPPORTED);
		return -EINVAL;
	}

	ret = vconf_set_int(VCONFKEY_WIFI_USE, state);
	if (ret != 0) {
		_E("Failed to set wifi use");
		wifi_error_toast(opt->ad, state);
		return ret;
	}

	return 0;
}

static int wifi_reg_handlers(void *data)
{
	return vconf_notify_key_changed(VCONFKEY_WIFI_USE, wifi_changed, data);
}

static int wifi_unreg_handlers(void *data)
{
	vconf_ignore_key_changed(VCONFKEY_WIFI_USE, wifi_changed);
	return 0;
}

static struct device_option wifi_ops = {
	.name                = WIFI_NAME,
	.terminate           = wifi_terminate,
	.is_enabled          = wifi_enabled,
	.get_item_type       = wifi_item_type,
	.get_id              = wifi_id,
	.get_icon            = wifi_icon,
	.icon_disabled       = wifi_icon_disabled,
	.get_text            = wifi_text,
	.response_clicked    = wifi_clicked,
	.clicked_data        = NULL,
	.register_handlers   = wifi_reg_handlers,
	.unregister_handlers = wifi_unreg_handlers,
};

static __attribute__ ((constructor)) void register_wifi_item(void)
{
	int state;

	if (enhanced_power_saving_mode())
		return;

	if (vconf_get_int(VCONFKEY_WIFI_USE, &state) == 0)
		wifi_state = state;

	register_device_options_item(&wifi_ops);
}
