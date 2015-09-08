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

static struct poweroff_option *ops;

void register_power_off_popup(struct poweroff_option *poweroff_ops)
{
	if (poweroff_ops)
		ops = poweroff_ops;
}

#ifdef SYSTEM_APPS_CIRCLE
static int create_normal_popup(struct appdata *ad)
{
	Evas_Object *lbtn;
	Evas_Object *licon;
	Evas_Object *rbtn;
	Evas_Object *ricon;
	Evas_Object *popup;
	Evas_Object *layout;

	if (!ad || !(ad->win_main) || !ops)
		return -EINVAL;

	evas_object_show(ad->win_main);
	popup = elm_popup_add(ad->win_main);
	if (!popup)
		return -ENOMEM;
	elm_object_style_set(popup, "circle");
	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, ELM_NOTIFY_ALIGN_FILL);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	layout = elm_layout_add(popup);
	if (ops->title)
		elm_object_part_text_set(layout, "elm.text.title", _(ops->title));

	if (ops->ltext && !(ops->rtext)) {
		/* one button */
		elm_layout_theme_set(layout, "layout", "popup", "content/circle/buttons1");
		elm_object_part_text_set(layout, "elm.text", _(ops->content));
		elm_object_content_set(popup, layout);

		if (ops->ricon) {
			lbtn = elm_button_add(popup);
			if (lbtn) {
				elm_object_text_set(lbtn, _(ops->ltext));
				elm_object_style_set(lbtn, "popup/circle");
				elm_object_part_content_set(popup, "button1", lbtn);
				evas_object_smart_callback_add(lbtn, "clicked", ops->left, ad);
			}
			licon = elm_image_add(lbtn);
			if (licon) {
				elm_image_file_set(licon, EDJ_NAME, ops->licon);
				evas_object_size_hint_weight_set(licon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
				elm_object_part_content_set(lbtn, "elm.swallow.content", licon);
				evas_object_show(licon);
			}
		}
	} else {
		/* Two button */
		elm_layout_theme_set(layout, "layout", "popup", "content/circle/buttons2");
		elm_object_part_text_set(layout, "elm.text", _(ops->content));
		elm_object_content_set(popup, layout);

		if (ops->ltext && ops->licon) {
			/* Left button */
			lbtn = elm_button_add(popup);
			if (lbtn) {
				elm_object_text_set(lbtn, _(ops->ltext));
				elm_object_style_set(lbtn, "popup/circle/left");
				elm_object_part_content_set(popup, "button1", lbtn);
				evas_object_smart_callback_add(lbtn, "clicked", ops->left, ad);
			}
			licon = elm_image_add(lbtn);
			if (licon) {
				elm_image_file_set(licon, EDJ_NAME, ops->licon);
				evas_object_size_hint_weight_set(licon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
				elm_object_part_content_set(lbtn, "elm.swallow.content", licon);
				evas_object_show(licon);
			}
		}

		if (ops->rtext && ops->ricon) {
			/* Right button */
			rbtn = elm_button_add(popup);
			if (rbtn) {
				elm_object_text_set(rbtn, _(ops->rtext));
				elm_object_style_set(rbtn, "popup/circle/right");
				elm_object_part_content_set(popup, "button2", rbtn);
				evas_object_smart_callback_add(rbtn, "clicked", ops->right, ad);
			}
			ricon = elm_image_add(rbtn);
			if (ricon) {
				elm_image_file_set(ricon, EDJ_NAME, ops->ricon);
				evas_object_size_hint_weight_set(ricon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
				elm_object_part_content_set(rbtn, "elm.swallow.content", ricon);
				evas_object_show(ricon);
			}
		}
	}

	evas_object_show(popup);

	ad->popup = popup;
	return 0;
}
#else
static int create_normal_popup(struct appdata *ad)
{
	Evas_Object *lbtn;
	Evas_Object *rbtn;
	Evas_Object *popup;

	if (!ad || !(ad->win_main) || !ops)
		return -EINVAL;

	evas_object_show(ad->win_main);
	popup = elm_popup_add(ad->win_main);
	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, ELM_NOTIFY_ALIGN_FILL);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_text_set(popup, _(ops->content));

	if (ops->title) {
		/* Popup title */
		elm_object_part_text_set(popup, "title,text", _(ops->title));
	}

	if (ops->ltext && ops->left) {
		/* Left button */
		lbtn = elm_button_add(popup);
		elm_object_text_set(lbtn, _(ops->ltext));
		elm_object_style_set(lbtn, "popup");
		elm_object_part_content_set(popup, "button1", lbtn);
		evas_object_smart_callback_add(lbtn, "clicked", ops->left, ad);
	}

	if (ops->rtext && ops->right) {
		/* Right button */
		rbtn = elm_button_add(popup);
		elm_object_text_set(rbtn, _(ops->rtext));
		elm_object_style_set(rbtn, "popup");
		elm_object_part_content_set(popup, "button2", rbtn);
		evas_object_smart_callback_add(rbtn, "clicked", ops->right, ad);
	}

	evas_object_show(popup);

	ad->popup = popup;

	return 0;
}
#endif

static __attribute__ ((constructor)) void register_normal_popup(void)
{
	register_device_options_popup(create_normal_popup);
}
