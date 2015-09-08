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
#include <efl_assist.h>
#include "device-options.h"
#include "dbus-handler.h"

#define DBUS_PATH_HOME_RAISE  "/Org/Tizen/Coreapps/home/raise"
#define DBUS_IFACE_HOME_RAISE "org.tizen.coreapps.home.raise"
#define HOME_RAISE_SIGNAL     "homeraise"

static E_DBus_Signal_Handler *powerkey_handler = NULL;

static void powerkey_pushed(void *data, DBusMessage *msg)
{
	if (dbus_message_is_signal(msg,
				DBUS_IFACE_HOME_RAISE,
				HOME_RAISE_SIGNAL) != 0)
		popup_terminate();
}

static void unregister_powerkey_handler(void)
{
	E_DBus_Connection *conn;

	conn = get_dbus_connection();
	if (powerkey_handler && conn) {
		e_dbus_signal_handler_del(conn, powerkey_handler);
		powerkey_handler = NULL;
	}
}

static int register_powerkey_handler(void)
{
	int ret;
	E_DBus_Connection *conn;

	conn = get_dbus_connection();
	if (!conn) {
		_E("Failed to get dbus connection");
		ret = -ENOMEM;
		goto out;
	}

	powerkey_handler = e_dbus_signal_handler_add(
			conn,
			NULL,
			DBUS_PATH_HOME_RAISE,
			DBUS_IFACE_HOME_RAISE,
			HOME_RAISE_SIGNAL,
			powerkey_pushed,
			NULL);
	if (!powerkey_handler) {
		_E("Failed to register handler");
		ret = -ENOMEM;
		goto out;
	}

	ret = 0;

out:
	if (ret < 0)
		unregister_powerkey_handler();
	return ret;
}

static void pm_state_changed(keynode_t *key, void *data)
{
	int state;
	struct appdata *ad = data;

	if (!key || !ad)
		return;

	state = vconf_keynode_get_int(key);
	if (state != VCONFKEY_PM_STATE_LCDOFF)
		return;

	if (vconf_ignore_key_changed(VCONFKEY_PM_STATE, pm_state_changed) != 0)
		_E("vconf key ignore failed");

	popup_terminate();
}

static void unregister_pm_state_handler(void)
{
	vconf_ignore_key_changed(VCONFKEY_PM_STATE, pm_state_changed);
}

static int register_pm_state_handler(struct appdata *ad)
{
	int ret;

	ret = vconf_notify_key_changed(VCONFKEY_PM_STATE, pm_state_changed, ad);
	if (ret != 0)
		_E("vconf key notify failed");

	return ret;
}

static void event_back_key_up(void *data, Evas_Object *obj, void *event_info)
{
	popup_terminate();
}

static void register_back_key_handler(struct appdata *ad)
{
	if (ad && ad->win_main)
		ea_object_event_callback_add(ad->win_main, EA_CALLBACK_BACK, event_back_key_up, ad);
}

static void unregister_back_key_handler(struct appdata *ad)
{
	if (ad && ad->win_main)
		ea_object_event_callback_del(ad->win_main, EA_CALLBACK_BACK, event_back_key_up);
}

void unregister_event_handlers(struct appdata *ad)
{
	if (!ad)
		return;

	unregister_powerkey_handler();
	unregister_pm_state_handler();
	unregister_back_key_handler(ad);
}

void register_event_handlers(struct appdata *ad)
{
	if (!ad)
		return;

	register_back_key_handler(ad);

	if (register_pm_state_handler(ad) != 0)
		_E("Failed to register pm state handler");

	if (register_powerkey_handler() < 0)
		_E("Failed to register power key handler");
}
