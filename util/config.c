/******************************************************************************
 * Copyright (C) 2014-2018 Zhifeng Gong <gozfree@163.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libraries; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/types.h>
#include <unistd.h>
#include <libmacro.h>
#include <liblog.h>
#include "common.h"
#include "config.h"

#define DEFAULT_CONFIG_FILE	"./aquila.lua"
struct ikey_cvalue conf_map_table[] = {
    {ALSA,     "alsa"},
    {MJPEG,    "mjpeg"},
    {PLAYBACK, "playback"},
    {SDL,      "sdl"},
    {SNKFAKE,  "snkfake"},
    {UPSTREAM, "upstream"},
    {USBCAM,   "usbcam"},
    {VDEVFAKE, "vdevfake"},
    {VENCODE,  "vencode"},
    {VDECODE,  "vdecode"},
    {VIDEOCAP, "videocap"},
    {RECORD,   "record"},
    {REMOTECTRL,"remotectrl"},
    {X264,     "x264"},
    {H264ENC,  "h264enc"},
    {YUV420,   "yuv420"},
    {YUV422P,  "yuv422p"},
    {UNKNOWN,  "unknown"},
};


static int string_to_enum(char *str)
{
    if (!str) {
        return -1;
    }
    for (int i = 0; i < ARRAY_SIZE(conf_map_table); i++) {
        if (!strcasecmp(str, conf_map_table[i].str)) {
            return conf_map_table[i].val;
        }
    }
    return -1;
}

#if 0
static char *enum_to_string(int key)
{
    if (key > SIZEOF(conf_map_table)) {
        return NULL;
    }
    return conf_map_table[key].str;
}
#endif

static void load_videocap(struct aq_config *c)
{
    strcpy(c->videocap.type.str, conf_get_string(c->conf, "videocap", "type"));
    c->videocap.type.val = string_to_enum(c->videocap.type.str);
    strcpy(c->videocap.device, conf_get_string(c->conf, "videocap", "device"));
    c->videocap.url = CALLOC(strlen(c->videocap.type.str)+ strlen(c->videocap.device) + strlen("://"), char);
    strcat(c->videocap.url, c->videocap.type.str);
    strcat(c->videocap.url, "://");
    strcat(c->videocap.url, c->videocap.device);
    c->videocap.param.video.width = conf_get_int(c->conf, "videocap", "width");
    c->videocap.param.video.height = conf_get_int(c->conf, "videocap", "height");
    strcpy(c->videocap.format, conf_get_string(c->conf, "videocap", "format"));
    c->videocap.param.video.format = string_to_enum(c->videocap.format);
    logi("[videocap][type] = %s\n", c->videocap.type.str);
    logi("[videocap][device] = %s\n", c->videocap.device);
    logi("[videocap][url] = %s\n", c->videocap.url);
    logi("[videocap][format] = %s\n", c->videocap.format);
    logi("[videocap][w*h] = %d*%d\n", c->videocap.param.video.width, c->videocap.param.video.height);
}

static void load_vencode(struct aq_config *c)
{
    strcpy(c->vencode.type.str, conf_get_string(c->conf, "vencode", "type"));
    c->vencode.type.val = string_to_enum(c->vencode.type.str);
    c->vencode.url = CALLOC(strlen(c->vencode.type.str) + strlen("://"), char);
    strcat(c->vencode.url, c->vencode.type.str);
    strcat(c->vencode.url, "://");

    logi("[vencode][type] = %s\n", c->vencode.type.str);
}

static void load_vdecode(struct aq_config *c)
{
    strcpy(c->vdecode.type.str, conf_get_string(c->conf, "vdecode", "type"));
    c->vdecode.type.val = string_to_enum(c->vdecode.type.str);
    c->vdecode.url = CALLOC(strlen(c->vdecode.type.str) + strlen("://"), char);
    strcat(c->vdecode.url, c->vdecode.type.str);
    strcat(c->vdecode.url, "://");

    logi("[vdecode][type] = %s\n", c->vdecode.type.str);
}

static void load_upstream(struct aq_config *c)
{
    strcpy(c->upstream.type.str, conf_get_string(c->conf, "upstream", "type"));
    c->upstream.url = strdup(conf_get_string(c->conf, "upstream", "url"));
    c->upstream.type.val = string_to_enum(c->upstream.type.str);
    c->upstream.port = conf_get_int(c->conf, "upstream", "port");
    logi("[upstream][type] = %s\n", c->upstream.type.str);
    logi("[upstream][port] = %d\n", c->upstream.port);
    logi("[upstream][url] = %s\n", c->upstream.url);
}

static void load_record(struct aq_config *c)
{
    strcpy(c->record.type.str, conf_get_string(c->conf, "record", "type"));
    c->record.type.val = string_to_enum(c->record.type.str);
    strcat(c->record.url, c->record.type.str);
    strcat(c->record.url, "://");
    strcat(c->record.url, conf_get_string(c->conf, "record", "file"));
    logi("[record][type] = %s\n", c->record.type.str);
    logi("[record][url] = %s\n", c->record.url);
}


static void load_playback(struct aq_config *c)
{
    strcpy(c->playback.type.str, conf_get_string(c->conf, "playback", "type"));
    c->playback.type.val = string_to_enum(c->playback.type.str);
    strcpy(c->playback.device, conf_get_string(c->conf, "playback", "device"));
    strcpy(c->playback.format, conf_get_string(c->conf, "playback", "format"));
    c->playback.url = CALLOC(strlen(c->playback.type.str) + strlen(c->playback.format) + strlen("://"), char);
    strcat(c->playback.url, c->playback.type.str);
    strcat(c->playback.url, "://");
    strcat(c->playback.url, c->playback.format);
    c->playback.param.video.width = conf_get_int(c->conf, "playback", "width");
    c->playback.param.video.height = conf_get_int(c->conf, "playback", "height");

    logi("[playback][type] = %s\n", c->playback.type.str);
    logi("[playback][url] = %s\n", c->playback.url);
    logi("[playback][w*h] = %d*%d\n", c->playback.param.video.width, c->playback.param.video.height);
}

static void load_remotectrl(struct aq_config *c)
{
    c->remotectrl.port = conf_get_int(c->conf, "remotectrl", "port");
    logi("[remotectrl][port] = %d\n", c->remotectrl.port);
}


static void load_filter(struct aq_config *c)
{
    int i;
    c->filter_num = conf_get_length(c->conf, "filters");
    c->filter = CALLOC(c->filter_num, struct filter_conf);

    for (i = 0; i < c->filter_num; i++) {
        strcpy(c->filter[i].type.str, conf_get_string(c->conf, "filters", i+1));
        c->filter[i].type.val = string_to_enum(c->filter[i].type.str);
        logi("[filter][%d][type] = %s\n", i, c->filter[i].type.str);
        switch (c->filter[i].type.val) {
        case VIDEOCAP:
            c->filter[i].url = c->videocap.url;
            memcpy(&c->filter[i].conf.videocap, &c->videocap, sizeof(struct videocap_conf));
            break;
        case PLAYBACK:
            c->filter[i].url = c->playback.url;
            memcpy(&c->filter[i].conf.playback, &c->playback, sizeof(struct playback_conf));
            break;
        case VENCODE:
            c->filter[i].url = c->vencode.url;
            break;
        case VDECODE:
            c->filter[i].url = c->vdecode.url;
            break;
        case UPSTREAM:
            c->filter[i].url = c->upstream.url;
            break;
        case RECORD:
            c->filter[i].url = c->record.url;
            break;
        case REMOTECTRL:
            c->filter[i].url = "rpcd://xxx";
            break;
        default :
            loge("unsupport filter type %s\n", c->filter[i].type.str);
            break;
        }
    }
}

static void load_graph(struct aq_config *c)
{
    int i;
    c->graph_num = conf_get_length(c->conf, "graphs");
    c->graph = CALLOC(c->graph_num, struct graph_conf);

    logi("[graph][num] = %d\n", c->graph_num);
    for (i = 0; i < c->graph_num; i++) {
        strcpy(c->graph[i].source, conf_get_string(c->conf, "graphs", i+1, "source"));
        strcpy(c->graph[i].sink, conf_get_string(c->conf, "graphs", i+1, "sink"));
        logi("[graph][%d][source] = %s\n", i, c->graph[i].source);
        logi("[graph][%d][sink] = %s\n", i, c->graph[i].sink);
    }
}

int load_conf(struct aq_config *c)
{
    c->conf = conf_load(DEFAULT_CONFIG_FILE);
    if (!c->conf) {
        loge("conf_load failed!\n");
        return -1;
    }
    load_videocap(c);
    load_vencode(c);
    load_vdecode(c);
    load_record(c);
    load_upstream(c);
    load_playback(c);
    load_remotectrl(c);
    load_filter(c);
    load_graph(c);
    return 0;
}

void unload_conf(struct aq_config *c)
{
    conf_unload(c->conf);
}
