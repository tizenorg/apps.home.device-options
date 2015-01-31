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

#define POWEROFF_ID   200
#define POWEROFF_NAME "poweroff"
#define POWEROFF_ICON "micro-poweroff.png"
#define POWEROFF_TEXT "IDS_ST_BODY_POWER_OFF"

#define SYSTEMD_STOP_POWER_OFF      4

static bool poweroff_terminate(void)
{
	return true;
}

static bool poweroff_enabled(void)
{
	return true;
}

static int poweroff_item_type(void)
{
	return FULL_ITEM_1TEXT_1ICON;
}

static int poweroff_id(void)
{
	return POWEROFF_ID;
}

static int poweroff_icon(char *icon, unsigned int len)
{
	if (!icon)
		return -EINVAL;
	snprintf(icon, len, "%s", POWEROFF_ICON);
	return 0;
}

static int poweroff_text(char *text, unsigned int len)
{
	if (!text)
		return -EINVAL;
	snprintf(text, len, "%s", POWEROFF_TEXT);
	return 0;
}

static int poweroff_clicked(void *data)
{
	_I("Power off is clicked");

	if (vconf_set_int(VCONFKEY_SYSMAN_POWER_OFF_STATUS,
				SYSTEMD_STOP_POWER_OFF) != 0)
		_E("Failed to request poweroff to deviced");

	return 0;
}

static int poweroff_reg_handlers(void *data)
{
	return 0;
}

static int poweroff_unreg_handlers(void *data)
{
	return 0;
}

static struct device_option poweroff_ops = {
	.name                = POWEROFF_NAME,
	.terminate           = poweroff_terminate,
	.is_enabled          = poweroff_enabled,
	.get_item_type       = poweroff_item_type,
	.get_id              = poweroff_id,
	.get_icon            = poweroff_icon,
	.get_text            = poweroff_text,
	.get_sub_text        = NULL,
	.response_clicked    = poweroff_clicked,
	.clicked_data        = NULL,
	.register_handlers   = poweroff_reg_handlers,
	.unregister_handlers = poweroff_unreg_handlers,
};

static __attribute__ ((constructor)) void register_poweroff_item(void)
{
	register_device_options_item(&poweroff_ops);
}
