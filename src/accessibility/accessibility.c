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
#include <appsvc.h>
#include "device-options.h"

#define ACCESSIBILITY_ID   100
#define ACCESSIBILITY_NAME "accessibility"
#define ACCESSIBILITY_ICON "micro-accessibility.png"
#define ACCESSIBILITY_TEXT "IDS_ST_HEADER_ACCESSIBILITY"

#define ACCESSIBILITY_MENU "org.tizen.clocksetting.accessibility"

static int launch_accessibility_menu(void)
{
	int ret;
	bundle *b;

	b = bundle_create();
	if (!b) {
		_E("Failed to make bundle");
		return -ENOMEM;
	}

	appsvc_set_operation(b, APPSVC_OPERATION_VIEW);
	appsvc_set_pkgname(b, ACCESSIBILITY_MENU);

	ret = appsvc_run_service(b, 0, NULL, NULL);
	if (ret < 0)
		_E("Failed to launch accessibility menu(%d)", ret);
	bundle_free(b);

	return ret;
}

static bool accessibility_terminate(void)
{
	return true;
}

static bool accessibility_enabled(void)
{
	return true;
}

static int accessibility_item_type(void)
{
	return FULL_ITEM_1TEXT_1ICON;
}

static int accessibility_id(void)
{
	return ACCESSIBILITY_ID;
}

static int accessibility_icon(char *icon, unsigned int len)
{
	if (!icon)
		return -EINVAL;
	snprintf(icon, len, "%s", ACCESSIBILITY_ICON);
	return 0;
}

static int accessibility_text(char *text, unsigned int len)
{
	if (!text)
		return -EINVAL;
	snprintf(text, len, "%s", ACCESSIBILITY_TEXT);
	return 0;
}

static int accessibility_clicked(void *data)
{
	int ret;

	_I("Accessibility is clicked");

	ret = launch_accessibility_menu();
	if (ret < 0)
		_E("Failed to launch accessibility(%d)", ret);

	return 0;
}

static int accessibility_reg_handlers(void *data)
{
	return 0;
}

static int accessibility_unreg_handlers(void *data)
{
	return 0;
}

static struct device_option accessibility_ops = {
	.name                = ACCESSIBILITY_NAME,
	.terminate           = accessibility_terminate,
	.is_enabled          = accessibility_enabled,
	.get_item_type       = accessibility_item_type,
	.get_id              = accessibility_id,
	.get_icon            = accessibility_icon,
	.get_text            = accessibility_text,
	.get_sub_text        = NULL,
	.response_clicked    = accessibility_clicked,
	.clicked_data        = NULL,
	.register_handlers   = accessibility_reg_handlers,
	.unregister_handlers = accessibility_unreg_handlers,
};

static __attribute__ ((constructor)) void register_accessibility_item(void)
{
	int val;

	if (vconf_get_int(VCONFKEY_SETAPPL_ACCESSIBILITY_POWER_KEY_HOLD, &val) == 0
			&& val == SETTING_POWERKEY_SHORTCUT_ACCESSIBILITY)
		register_device_options_item(&accessibility_ops);
}
