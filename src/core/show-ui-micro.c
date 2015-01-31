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

#ifdef DEVICE_OPTIONS_MICRO_3
#define LAYOUT_DEFAULT    "micro_3_default"
#else
#define LAYOUT_DEFAULT    NULL
#endif

#define LAYOUT_2ITEMS     "micro_2items"
#define LAYOUT_3ITEMS     "micro_3items"

#define FULL_1TEXT_1ICON_STYLE  "device_option.1text.1icon"
#define FULL_2TEXT_STYLE        "device_option.2text"
#define HALF_STYLE              "device_option.2icon.divider"

#define TOAST_TIMEOUT    3

static Evas_Object *toast_popup = NULL;
static Ecore_Timer *toast_timer = NULL;
static Ecore_Event_Handler *mouse_up_handler = NULL;

void unregister_toast_handlers(struct appdata *ad)
{
	if (toast_timer) {
		ecore_timer_del(toast_timer);
		toast_timer = NULL;
	}

	if (mouse_up_handler) {
		ecore_event_handler_del(mouse_up_handler);
		mouse_up_handler = NULL;
	}
}

static Eina_Bool mouse_up_response(void *data, int type, void *event)
{
	unregister_toast_handlers(data);
	release_evas_object(&toast_popup);
	return ECORE_CALLBACK_DONE;
}

static Eina_Bool toast_timeout(void *data)
{
	unregister_toast_handlers(data);
	release_evas_object(&toast_popup);
	return ECORE_CALLBACK_CANCEL;
}

static void register_toast_handlers(struct appdata *ad)
{
	unregister_toast_handlers(ad);
	toast_timer = ecore_timer_add(TOAST_TIMEOUT, toast_timeout, NULL);
	if (!toast_timer)
		_E("Failed to add timer");

	mouse_up_handler = ecore_event_handler_add(
			ECORE_EVENT_MOUSE_BUTTON_UP, mouse_up_response, ad);
	if (!mouse_up_handler)
		_E("Failed to register mouse up handler");
}

int load_popup_toast(struct appdata *ad, char *content)
{
	Evas_Object *popup;

	if (!ad || !(ad->win_main) || !content)
		return -EINVAL;

	popup = elm_popup_add(ad->win_main);
	elm_object_style_set(popup, "toast");
	elm_popup_orient_set(popup, ELM_POPUP_ORIENT_BOTTOM);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_text_set(popup, "elm.text", content);
	evas_object_show(popup);

	register_toast_handlers(ad);
	toast_popup = popup;

	return 0;
}

/* Common */
static void response_clicked(void *data, Evas_Object * obj, void *event_info)
{
	struct device_option *opt = data;
	int ret;
	bool terminate = false;

	if (opt->terminate)
		terminate = opt->terminate();

	if (opt->response_clicked) {
		ret = opt->response_clicked(opt);
		if (ret < 0)
			_E("Failed to response for clicking (%d)", ret);
	}

	if (terminate) /* terminate popup */
		popup_terminate();
	else           /* update popup */
		elm_genlist_item_update(opt->item);
}

/* Full item */
static char *get_full_item_text(void *data, Evas_Object *obj, const char *part)
{
	struct device_option *opt = data;
	char text[BUF_MAX];

	if (!strcmp(part, "elm.text") || !strcmp(part, "elm.text.1")) {
		if (opt->get_text(text, sizeof(text)) < 0) {
			_E("Failed to get text");
			return NULL;
		}
		return strdup(_(text));
	} else if (!strcmp(part, "elm.text.2")) {
		if (opt->get_sub_text(text, sizeof(text)) < 0) {
			_E("Failed to get sub text");
			return NULL;
		}
		return strdup(_(text));
	}

	return NULL;
}

static Evas_Object *get_full_item_content(void *data, Evas_Object *obj, const char *part)
{
	struct device_option *opt = data;
	char icon[BUF_MAX];
	Evas_Object *ic;

	if (!opt)
		return NULL;

	if (opt->get_item_type() != FULL_ITEM_1TEXT_1ICON)
		return NULL;

	if (strcmp(part, "elm.icon"))
		return NULL;

	if (opt->get_icon(icon, sizeof(icon)) < 0) {
		_E("Failed to get icon");
		return NULL;
	}

	ic = elm_icon_add(obj);
	if (!ic)
		return NULL;

	elm_image_file_set(ic, EDJ_NAME, icon);
	evas_object_size_hint_min_set(ic, IMAGE_LARGE, IMAGE_LARGE);
	evas_object_size_hint_max_set(ic, IMAGE_LARGE, IMAGE_LARGE);

	return ic;
}

static int get_full_itc(struct device_option *opt)
{
	if (!opt)
		return -EINVAL;

	if (opt->get_item_type() == FULL_ITEM_1TEXT_1ICON)
		opt->itc.item_style = FULL_1TEXT_1ICON_STYLE;
	else if (opt->get_item_type() == FULL_ITEM_2TEXT)
		opt->itc.item_style = FULL_2TEXT_STYLE;
	else
		return -EINVAL;

	opt->itc.func.text_get = get_full_item_text;
	opt->itc.func.content_get = get_full_item_content;

	return 0;
}

static void add_full_items_on_genlist(struct appdata *ad)
{
	GList *full_items = NULL, *l;
	struct device_option *opt = NULL;

	if (!ad)
		return;

	if (get_full_items_list(&full_items) < 0)
		return;

	if (!full_items)
		return;

	for (l = full_items ; l ; l = g_list_next(l)) {
		opt = (struct device_option *)(l->data);
		if (!opt)
			continue;

		if (get_full_itc(opt) < 0)
			continue;

		opt->ad = ad;

		opt->item = elm_genlist_item_append(
				ad->list,
				&(opt->itc),
				opt,
				NULL,
				ELM_GENLIST_ITEM_NONE,
				response_clicked,
				opt);

		if (opt->is_enabled() == false)
			elm_object_item_disabled_set(opt->item, EINA_TRUE);
	}
}

/* Half item */
static Evas_Object *make_button(Evas_Object *obj, char *text, char *icon,
		void (*btn_clicked)(void *data, Evas_Object * obj, void *event_info), void *data)
{
	Evas_Object *btn = NULL;
	Evas_Object *ic = NULL;
	struct device_option *opt = data;

	if (!obj || !text || !icon || !opt)
		return NULL;

	btn = elm_button_add(obj);
	if (!btn)
		goto out;
	elm_object_style_set(btn, "device_option");
	elm_object_text_set(btn, _(text));
	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);

	ic = elm_image_add(btn);
	if (!ic)
		goto out;
	elm_image_file_set(ic, EDJ_NAME,  icon);
	evas_object_size_hint_min_set(ic, IMAGE_SMALL_W, IMAGE_SMALL_H);
	evas_object_size_hint_max_set(ic, IMAGE_SMALL_W, IMAGE_SMALL_H);
	elm_object_content_set(btn, ic);
	evas_object_propagate_events_set(btn, EINA_FALSE);

	if (opt->icon_disabled && opt->icon_disabled())
		edje_object_signal_emit(elm_layout_edje_get(btn), "elm,state,disabled,icon", "elm");

	evas_object_smart_callback_add(btn, "clicked", btn_clicked, data);

	return btn;

out:
	if (btn)
		evas_object_del(btn);
	return NULL;
}

static Evas_Object *get_button(Evas_Object *obj, struct device_option *opt)
{
	char icon[BUF_MAX], text[BUF_MAX];

	if (opt && opt->get_text && opt->get_icon) {
		if (opt->get_text(text, sizeof(text)) < 0)
			return NULL;
		if (opt->get_icon(icon, sizeof(icon)) < 0)
			return NULL;
		opt->btn = make_button(obj, text, icon, response_clicked, opt);
		if (opt->is_enabled && opt->is_enabled() == false)
			elm_object_disabled_set(opt->btn, EINA_TRUE);
		return opt->btn;
	}
	return NULL;
}

static Evas_Object *get_half_item_content(void *data, Evas_Object *obj, const char *part)
{
	GList *l = data;
	struct device_option *opt;

	if (!l)
		return NULL;

	if (!strcmp(part, "elm.icon")) /* First button */
		;/* Do nothing */
	else if (!strcmp(part, "elm.icon.1")) { /* Second button */
		l = g_list_next(l);
		if (!l)
			return NULL;
	} else
		return NULL;

	/* Remove focus from genlist item */
	elm_object_focus_allow_set(obj, EINA_FALSE);

	opt = (struct device_option *)(l->data);
	if (!opt)
		return NULL;
	return get_button(obj, opt);
}

static int get_half_itc(struct device_option *opt)
{
	if (!opt)
		return -EINVAL;

	opt->itc.item_style = HALF_STYLE;
	opt->itc.func.text_get = NULL;
	opt->itc.func.content_get = get_half_item_content;

	return 0;
}

static void add_half_items_on_genlist(struct appdata *ad)
{
	GList *half_items = NULL, *l;
	Elm_Object_Item *item;
	struct device_option *opt;
	int cnt, i;

	if (!ad)
		return;

	if (get_half_items_list(&half_items) < 0)
		return;

	if (!half_items)
		return;

	cnt = nr_half_items_list();

	for (i = 0, l = half_items ; l && i <= cnt ; i += 2, l = g_list_next(l)) {
		opt = (struct device_option *)(l->data);
		if (!opt)
			continue;

		if (get_half_itc(opt) < 0)
			continue;

		item = elm_genlist_item_append(
				ad->list,
				&(opt->itc),
				l,
				NULL,
				ELM_GENLIST_ITEM_NONE,
				NULL,
				NULL);
		opt->item = item;
		opt->ad = ad;

		l = g_list_next(l);
		if (l) {
			opt = (struct device_option *)(l->data);
			if (opt) {
				opt->item = item;
				opt->ad = ad;
			}
		}
	}
}

/* Popup */
static void item_realized(void *data, Evas_Object *obj, void *event_info)
{
	int nr_items = (int)data;
	if (!event_info)
		return;

	if (elm_genlist_item_index_get(event_info) == (nr_items-1))
		elm_object_item_signal_emit(event_info, "elm,state,bottomline,hide", "");
}

static int get_layout_style(int nr, char *style, unsigned int len)
{
	if (!LAYOUT_DEFAULT)
		return -ENOENT;

	if (nr <= 2)
		snprintf(style, len, "%s", LAYOUT_2ITEMS);
	else if (nr == 3)
		snprintf(style, len, "%s", LAYOUT_3ITEMS);
	else
		snprintf(style, len, "%s", LAYOUT_DEFAULT);
	return 0;
}

static int create_micro_popup(struct appdata *ad)
{
	Evas_Object *layout;
	int nr_items, nr_full, nr_half;
	char style[64];
	int ret;

	if (!ad || !(ad->win_main))
		return -EINVAL;

	nr_full = nr_full_items_list();
	nr_half = nr_half_items_list();
	nr_items = nr_full + (nr_half + 1) / 2;

	ad->popup = elm_popup_add(ad->win_main);
	if (ad->popup == NULL) {
		_E("FAIL: elm_popup_add()");
		return -ENOMEM;
	}

	ret = get_layout_style(nr_items, style, sizeof(style));

	if (ret == 0) {
		layout = elm_layout_add(ad->popup);
		elm_layout_file_set(layout, EDJ_NAME, style);
		evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	} else {
		evas_object_size_hint_weight_set(ad->popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	}

	ad->list = elm_genlist_add(ad->popup);
	if (ad->list == NULL) {
		_E("Failed to create genlist");
		return -ENOMEM;
	}
	elm_object_style_set(ad->list, "popup");
	elm_genlist_mode_set(ad->list, ELM_LIST_COMPRESS);

	evas_object_smart_callback_add(ad->list, "realized", item_realized, (void *)nr_items);

	add_full_items_on_genlist(ad);
	add_half_items_on_genlist(ad);

	if (LAYOUT_DEFAULT) {
		elm_object_part_content_set(layout, "elm.swallow.content", ad->list);
		elm_object_content_set(ad->popup, layout);
	} else {
		elm_object_content_set(ad->popup, ad->list);
	}

	evas_object_show(ad->list);
	evas_object_show(ad->popup);

	return 0;
}

static __attribute__ ((constructor)) void register_micro_popup(void)
{
	register_device_options_popup(create_micro_popup);
}
