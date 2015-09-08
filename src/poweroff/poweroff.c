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
#include "dbus-handler.h"
#include "show-ui.h"

#define POWEROFF_ID   200
#define POWEROFF_NAME "poweroff"
#define POWEROFF_ICON "micro-poweroff.png"
#define POWEROFF_TEXT "IDS_ST_BODY_POWER_OFF"

#define DEVICED_BUS_NAME    "org.tizen.system.deviced"
#define DEVICED_OBJECT_PATH "/Org/Tizen/System/DeviceD"
#define DEVICED_IFACE_NAME  DEVICED_BUS_NAME
#define DEVICED_PATH_POWER  DEVICED_OBJECT_PATH"/Power"
#define DEVICED_IFACE_POWER DEVICED_IFACE_NAME".power"
#define METHOD_POWEROFF     "reboot"
#define TYPE_POWEROFF       "poweroff"

static void poweroff_clicked(void *data, Evas_Object * obj, void *event_info)
{
	int ret;
	char *param[2];
	char opt[4];

	_I("Power off is clicked");

	snprintf(opt, sizeof(opt), "0");
	param[0] = TYPE_POWEROFF;
	param[1] = opt;

	ret = dbus_method_sync(
			DEVICED_BUS_NAME,
			DEVICED_PATH_POWER,
			DEVICED_IFACE_POWER,
			METHOD_POWEROFF,
			"si", param);
	if (ret < 0)
		_E("Failed to send dbus for poweroff (%d)", ret);

	popup_terminate();
}

static void cancel_clicked(void *data, Evas_Object * obj, void *event_info)
{
	_I("Cancel clicked");
	popup_terminate();
}

static struct poweroff_option poweroff_ops = {
	.name		= POWEROFF_NAME,
	.title		= NULL,
	.content	= POWEROFF_TEXT,
	.ltext		= "IDS_COM_SK_CANCEL",
	.licon		= "circle-cancel.png",
	.left		= cancel_clicked,
	.rtext		= "IDS_COM_SK_OK",
	.ricon		= "circle-ok.png",
	.right		= poweroff_clicked,
};

static __attribute__ ((constructor)) void register_poweroff_item(void)
{
	register_power_off_popup(&poweroff_ops);
}
