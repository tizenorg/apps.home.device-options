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

#define RESTART_ID   300
#define RESTART_NAME "restart"
#define RESTART_ICON "micro-restart.png"
#define RESTART_TEXT "IDS_ST_BUTTON_RESTART"

#define SYSTEMD_STOP_RESTART      5

static bool restart_terminate(void)
{
	return true;
}

static bool restart_enabled(void)
{
	return true;
}

static int restart_item_type(void)
{
	return FULL_ITEM_1TEXT_1ICON;
}

static int restart_id(void)
{
	return RESTART_ID;
}

static int restart_icon(char *icon, unsigned int len)
{
	if (!icon)
		return -EINVAL;
	snprintf(icon, len, "%s", RESTART_ICON);
	return 0;
}

static int restart_text(char *text, unsigned int len)
{
	if (!text)
		return -EINVAL;
	snprintf(text, len, "%s", RESTART_TEXT);
	return 0;
}

static int restart_clicked(void *data)
{
	_I("Power off is clicked");

	if (vconf_set_int(VCONFKEY_SYSMAN_POWER_OFF_STATUS,
				SYSTEMD_STOP_RESTART) != 0)
		_E("Failed to request restart to deviced");

	return 0;
}

static int restart_reg_handlers(void *data)
{
	return 0;
}

static int restart_unreg_handlers(void *data)
{
	return 0;
}

static struct device_option restart_ops = {
	.name                = RESTART_NAME,
	.terminate           = restart_terminate,
	.is_enabled          = restart_enabled,
	.get_item_type       = restart_item_type,
	.get_id              = restart_id,
	.get_icon            = restart_icon,
	.get_text            = restart_text,
	.get_sub_text        = NULL,
	.response_clicked    = restart_clicked,
	.clicked_data        = NULL,
	.register_handlers   = restart_reg_handlers,
	.unregister_handlers = restart_unreg_handlers,
};

static __attribute__ ((constructor)) void register_restart_item(void)
{
	register_device_options_item(&restart_ops);
}
