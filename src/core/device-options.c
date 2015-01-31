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

/* List to store items on the popup list */
static GList *full_items = NULL;
static GList *half_items = NULL;

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

int get_full_items_list(GList **l)
{
	if (!l)
		return -EINVAL;
	*l = full_items;
	return 0;
}

int get_half_items_list(GList **l)
{
	if (!l)
		return -EINVAL;
	*l = half_items;
	return 0;
}

unsigned int nr_full_items_list(void)
{
	if (full_items)
		return g_list_length(full_items);
	return 0;
}

unsigned int nr_half_items_list(void)
{
	if (half_items)
		return g_list_length(half_items);
	return 0;
}

static gint compare_options(gconstpointer first, gconstpointer second)
{
	struct device_option *opt1 = (struct device_option *)first;
	struct device_option *opt2 = (struct device_option *)second;

	return (opt1->get_id() - opt2->get_id());
}

/* Register an item on the popup list */
void register_device_options_item(const struct device_option *opt)
{
	int style;

	if (!opt) {
		_E("Invalid parameter");
		return;
	}

	style = opt->get_item_type();
	switch (style) {
	case FULL_ITEM_1TEXT_1ICON:
	case FULL_ITEM_2TEXT:
		full_items = g_list_insert_sorted(full_items, (gpointer)opt, compare_options);
		break;

	case HALF_ITEM:
		half_items = g_list_insert_sorted(half_items, (gpointer)opt, compare_options);
		break;

	default:
		_E("Unknown item type (%d)", style);
	}
}

/* Unregister all items on the popup list */
static void unregister_option_full(void)
{
	if (full_items)
		g_list_free(full_items);
	if (half_items)
		g_list_free(half_items);
}

static void register_handlers_for_list (struct appdata *ad, GList *list)
{
	GList *l;
	struct device_option *opt;

	if (!ad || !list)
		return;

	for (l = list ; l ; l = g_list_next(l)) {
		opt = (struct device_option *)(l->data);
		if (!opt)
			continue;
		if (opt->register_handlers)
			opt->register_handlers(opt);
	}
}

static void register_main_list_handlers(struct appdata *ad)
{
	register_handlers_for_list (ad, full_items);
	register_handlers_for_list (ad, half_items);
}

static void unregister_handlers_for_list(struct appdata *ad, GList *list)
{
	GList *l;
	struct device_option *opt;

	if (!ad || !list)
		return;

	for (l = list ; l ; l = g_list_next(l)) {
		opt = (struct device_option *)(l->data);
		if (!opt)
			continue;
		if (opt->unregister_handlers)
			opt->unregister_handlers(opt);
	}
}

static void unregister_main_list_handlers(struct appdata *ad)
{
	unregister_handlers_for_list (ad, full_items);
	unregister_handlers_for_list (ad, half_items);
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

	elm_theme_overlay_add(NULL,EDJ_NAME);

	ret = appcore_set_i18n(LANG_DOMAIN, LOCALE_DIR);
	if (ret != 0)
		_E("FAIL: appcore_set_i18n()");

	ret = set_dbus_connection();
	if (ret < 0)
		_E("Failed to set dbus connection(%d)", ret);

	register_main_list_handlers(ad);
	register_event_handlers(ad);

	_I("Exit: app_create()");
	return 0;
}

static int app_terminate(void *data)
{
	struct appdata *ad = data;

	unregister_toast_handlers(ad);
	unregister_event_handlers(ad);
	unregister_main_list_handlers(ad);
	unregister_option_full();
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
