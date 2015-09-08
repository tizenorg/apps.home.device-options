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
#include "dbus-handler.h"
#include "event-handler.h"

/* Terminate popup */
static Eina_Bool exit_idler_cb(void *data)
{
	elm_exit();
	return ECORE_CALLBACK_CANCEL;
}

void popup_terminate(void)
{
	if (ecore_idler_add(exit_idler_cb, NULL))
		return;
	exit_idler_cb(NULL);
}

static int app_create(void *data)
{
	Evas_Object *win;
	struct appdata *ad = data;
	int ret;

	win = create_win(APPNAME);
	if (win == NULL)
		return -1;

	ad->win_main = win;

	evas_object_show(ad->win_main);

	ret = appcore_set_i18n(LANG_DOMAIN, LOCALE_DIR);
	if (ret != 0)
		_E("FAIL: appcore_set_i18n()");

	elm_theme_overlay_add(NULL,EDJ_NAME);

	ret = set_dbus_connection();
	if (ret < 0)
		_E("Failed to set dbus connection(%d)", ret);

	register_event_handlers(ad);

	_I("Exit: app_create()");
	return 0;
}

static int app_terminate(void *data)
{
	struct appdata *ad = data;

	unregister_event_handlers(ad);
	unset_dbus_connection();

	if (ad->win_main)
		evas_object_del(ad->win_main);

	return 0;
}

static int app_pause(void *data)
{
	return 0;
}

static int app_resume(void *data)
{
	return 0;
}

static int app_reset(bundle *b, void *data)
{
	struct appdata *ad = data;
	int ret;

	if (ad->popup) {
		_E("Device options popup already exists");
		raise_window(ad->win_main);
		return 0;
	}

	ret = create_device_options_popup(ad);
	if (ret < 0) {
		_E("Failed to create popup(%d)", ret);
		popup_terminate();
	}

	_I("Exit: app_reset()");

	return ret;
}

int main(int argc, char *argv[])
{
	struct appdata ad;

	struct appcore_ops ops = {
		.create = app_create,
		.terminate = app_terminate,
		.pause = app_pause,
		.resume = app_resume,
		.reset = app_reset,
	};

	memset(&ad, 0x0, sizeof(struct appdata));
	ops.data = &ad;

	return appcore_efl_main(APPNAME, &argc, &argv, &ops);
}
