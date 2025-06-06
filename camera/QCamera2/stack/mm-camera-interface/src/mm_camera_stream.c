/* Copyright (c) 2012-2017, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

// System dependencies
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <media/msm_media_info.h>
#define TIME_H <SYSTEM_HEADER_PREFIX/time.h>
#include TIME_H
#define IOCTL_H <SYSTEM_HEADER_PREFIX/ioctl.h>
#include IOCTL_H
#include <cutils/properties.h>

// Camera dependencies
#include "cam_semaphore.h"
#include "mm_camera_dbg.h"
#include "mm_camera_interface.h"
#include "mm_camera.h"
#include "mm_camera_muxer.h"
/* internal function decalre */
int32_t mm_stream_qbuf(mm_stream_t *my_obj,
                       mm_camera_buf_def_t *buf);
int32_t mm_stream_set_ext_mode(mm_stream_t * my_obj);
int32_t mm_stream_set_fmt(mm_stream_t * my_obj);
int32_t mm_stream_cancel_buf(mm_stream_t * my_obj,
                           uint32_t buf_idx);
int32_t mm_stream_sync_info(mm_stream_t *my_obj);
int32_t mm_stream_init_bufs(mm_stream_t * my_obj);
int32_t mm_stream_deinit_bufs(mm_stream_t * my_obj);
int32_t mm_stream_request_buf(mm_stream_t * my_obj);
int32_t mm_stream_unreg_buf(mm_stream_t * my_obj);
int32_t mm_stream_release(mm_stream_t *my_obj);
int32_t mm_stream_set_parm(mm_stream_t *my_obj,
                           cam_stream_parm_buffer_t *value);
int32_t mm_stream_get_parm(mm_stream_t *my_obj,
                           cam_stream_parm_buffer_t *value);
int32_t mm_stream_do_action(mm_stream_t *my_obj,
                            void *in_value);
int32_t mm_stream_streamon(mm_stream_t *my_obj);
int32_t mm_stream_streamoff(mm_stream_t *my_obj);
int32_t mm_stream_read_msm_frame(mm_stream_t * my_obj,
                                 mm_camera_buf_info_t* buf_info,
                                 uint8_t num_planes);
int32_t mm_stream_read_user_buf(mm_stream_t * my_obj,
        mm_camera_buf_info_t* buf_info);
int32_t mm_stream_write_user_buf(mm_stream_t * my_obj,
        mm_camera_buf_def_t *buf);

int32_t mm_stream_init(mm_stream_t *my_obj);
int32_t mm_stream_deinit(mm_stream_t *my_obj);
int32_t mm_stream_config(mm_stream_t *my_obj,
                         mm_camera_stream_config_t *config);
int32_t mm_stream_reg_buf(mm_stream_t * my_obj);
int32_t mm_stream_buf_done(mm_stream_t * my_obj,
                           mm_camera_buf_def_t *frame);
int32_t mm_stream_get_queued_buf_count(mm_stream_t * my_obj);

int32_t mm_stream_calc_offset(mm_stream_t *my_obj);
int32_t mm_stream_calc_offset_preview(cam_stream_info_t *stream_info,
                                      cam_dimension_t *dim,
                                      cam_padding_info_t *padding,
                                      cam_stream_buf_plane_info_t *buf_planes);
int32_t mm_stream_calc_offset_post_view(cam_stream_info_t *stream_info,
                                      cam_dimension_t *dim,
                                      cam_padding_info_t *padding,
                                      cam_stream_buf_plane_info_t *buf_planes);

int32_t mm_stream_calc_offset_snapshot(cam_format_t fmt,
                                       cam_dimension_t *dim,
                                       cam_stream_type_t type,
                                       cam_padding_info_t *padding,
                                       cam_stream_buf_plane_info_t *buf_planes);
int32_t mm_stream_calc_offset_raw(cam_format_t fmt,
                                  cam_dimension_t *dim,
                                  cam_padding_info_t *padding,
                                  cam_stream_buf_plane_info_t *buf_planes);
int32_t mm_stream_calc_offset_video(cam_stream_info_t *stream_info,
        cam_padding_info_t *padding,
        cam_stream_buf_plane_info_t *buf_planes);
int32_t mm_stream_calc_offset_metadata(cam_dimension_t *dim,
                                       cam_padding_info_t *padding,
                                       cam_stream_buf_plane_info_t *buf_planes);
int32_t mm_stream_calc_offset_depth(cam_format_t fmt,
                                    cam_dimension_t *dim,
                                    cam_padding_info_t *padding,
                                    cam_stream_buf_plane_info_t *buf_planes);
int32_t mm_stream_calc_offset_postproc(cam_stream_info_t *stream_info,
                                       cam_padding_info_t *padding,
                                       cam_stream_buf_plane_info_t *plns);
uint32_t mm_stream_calc_lcm(int32_t num1, int32_t num2);


/* state machine function declare */
int32_t mm_stream_fsm_inited(mm_stream_t * my_obj,
                             mm_stream_evt_type_t evt,
                             void * in_val,
                             void * out_val);
int32_t mm_stream_fsm_acquired(mm_stream_t * my_obj,
                               mm_stream_evt_type_t evt,
                               void * in_val,
                               void * out_val);
int32_t mm_stream_fsm_cfg(mm_stream_t * my_obj,
                          mm_stream_evt_type_t evt,
                          void * in_val,
                          void * out_val);
int32_t mm_stream_fsm_buffed(mm_stream_t * my_obj,
                             mm_stream_evt_type_t evt,
                             void * in_val,
                             void * out_val);
int32_t mm_stream_fsm_reg(mm_stream_t * my_obj,
                          mm_stream_evt_type_t evt,
                          void * in_val,
                          void * out_val);
int32_t mm_stream_fsm_active(mm_stream_t * my_obj,
                             mm_stream_evt_type_t evt,
                             void * in_val,
                             void * out_val);
uint32_t mm_stream_get_v4l2_fmt(cam_format_t fmt);
int32_t mm_stream_reg_frame_sync(mm_stream_t *my_obj,
        mm_evt_paylod_reg_frame_sync *sync);
int32_t mm_stream_handle_cache_ops(mm_stream_t* my_obj,
        mm_camera_buf_def_t* buf, bool deque);
int32_t mm_stream_trigger_frame_sync(mm_stream_t *my_obj,
        mm_camera_cb_req_type type);
uint32_t mm_channel_link_stream(mm_channel_t *my_obj,
        mm_camera_stream_link_t *stream_link);


/*===========================================================================
 * FUNCTION   : mm_stream_notify_channel
 *
 * DESCRIPTION: function to notify channel object on received buffer
 *
 * PARAMETERS :
 *   @ch_obj  : channel object
 *   @buf_info: ptr to struct storing buffer information
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              0> -- failure
 *==========================================================================*/
int32_t mm_stream_notify_channel(struct mm_channel* ch_obj,
        mm_camera_buf_info_t *buf_info)
{
    int32_t rc = 0;
    mm_camera_cmdcb_t* node = NULL;

    if ((NULL == ch_obj) || (NULL == buf_info)) {
        LOGD("Invalid channel/buffer");
        return -ENODEV;
    }

    /* send cam_sem_post to wake up channel cmd thread to enqueue
     * to super buffer */
    node = (mm_camera_cmdcb_t *)malloc(sizeof(mm_camera_cmdcb_t));
    if (NULL != node) {
        memset(node, 0, sizeof(mm_camera_cmdcb_t));
        node->cmd_type = MM_CAMERA_CMD_TYPE_DATA_CB;
        node->u.buf = *buf_info;

        /* enqueue to cmd thread */
        cam_queue_enq(&(ch_obj->cmd_thread.cmd_queue), node);

        /* wake up cmd thread */
        cam_sem_post(&(ch_obj->cmd_thread.cmd_sem));
    } else {
        LOGE("No memory for mm_camera_node_t");
        rc = -ENOMEM;
    }

    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_handle_rcvd_buf
 *
 * DESCRIPTION: function to handle newly received stream buffer
 *
 * PARAMETERS :
 *   @cam_obj : stream object
 *   @buf_info: ptr to struct storing buffer information
 *
 * RETURN     : none
 *==========================================================================*/
void mm_stream_handle_rcvd_buf(mm_stream_t *my_obj,
                               mm_camera_buf_info_t *buf_info,
                               uint8_t has_cb)
{
    int32_t rc = 0;
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);

    /* enqueue to super buf thread */
    if (my_obj->is_bundled) {
        rc = mm_stream_notify_channel(my_obj->ch_obj, buf_info);
        if (rc < 0) {
            LOGE("Unable to notify channel");
        }
    }

    pthread_mutex_lock(&my_obj->buf_lock);
    if(my_obj->is_linked) {
        /* need to add into super buf for linking, add ref count */
        my_obj->buf_status[buf_info->buf->buf_idx].buf_refcnt++;

        rc = mm_stream_notify_channel(my_obj->linked_obj, buf_info);
        if (rc < 0) {
            LOGE("Unable to notify channel");
        }
    }
    pthread_mutex_unlock(&my_obj->buf_lock);

    pthread_mutex_lock(&my_obj->cmd_lock);
    if(has_cb && my_obj->cmd_thread.is_active) {
        mm_camera_cmdcb_t* node = NULL;

        /* send cam_sem_post to wake up cmd thread to dispatch dataCB */
        node = (mm_camera_cmdcb_t *)malloc(sizeof(mm_camera_cmdcb_t));
        if (NULL != node) {
            memset(node, 0, sizeof(mm_camera_cmdcb_t));
            node->cmd_type = MM_CAMERA_CMD_TYPE_DATA_CB;
            node->u.buf = *buf_info;

            /* enqueue to cmd thread */
            cam_queue_enq(&(my_obj->cmd_thread.cmd_queue), node);

            /* wake up cmd thread */
            cam_sem_post(&(my_obj->cmd_thread.cmd_sem));
        } else {
            LOGE("No memory for mm_camera_node_t");
        }
    }
    pthread_mutex_unlock(&my_obj->cmd_lock);
}

/*===========================================================================
 * FUNCTION   : mm_stream_dispatch_sync_data
 *
 * DESCRIPTION: dispatch stream buffer to registered users on poll thread
 *
 * PARAMETERS :
 *   @cmd_cb  : ptr storing stream buffer information
 *   @userdata: user data ptr (stream object)
 *
 * RETURN     : none
 *==========================================================================*/
static void mm_stream_dispatch_sync_data(mm_stream_t * my_obj,
         mm_stream_data_cb_t *buf_cb, mm_camera_buf_info_t *buf_info)
{
    mm_camera_super_buf_t super_buf;
    mm_stream_t *m_obj = my_obj;

    if (NULL == my_obj || buf_info == NULL ||
            buf_cb == NULL) {
        return;
    }

    if (m_obj->master_str_obj != NULL) {
        m_obj = m_obj->master_str_obj;
    }

    memset(&super_buf, 0, sizeof(mm_camera_super_buf_t));
    super_buf.num_bufs = 1;
    super_buf.bufs[0] = buf_info->buf;
    super_buf.camera_handle = my_obj->ch_obj->cam_obj->my_hdl;
    super_buf.ch_id = my_obj->ch_obj->my_hdl;
    if ((buf_cb != NULL) && (buf_cb->cb_type == MM_CAMERA_STREAM_CB_TYPE_SYNC)
            && (buf_cb->cb_count != 0)
            && my_obj->is_cb_active) {
        /* callback */
        buf_cb->cb(&super_buf, buf_cb->user_data);

        /* if >0, reduce count by 1 every time we called CB until reaches 0
             * when count reach 0, reset the buf_cb to have no CB */
        if (buf_cb->cb_count > 0) {
            buf_cb->cb_count--;
            if (0 == buf_cb->cb_count) {
                buf_cb->cb = NULL;
                buf_cb->user_data = NULL;
            }
        }
    }
}

/*===========================================================================
 * FUNCTION   : mm_stream_data_notify
 *
 * DESCRIPTION: callback to handle data notify from kernel
 *
 * PARAMETERS :
 *   @user_data : user data ptr (stream object)
 *
 * RETURN     : none
 *==========================================================================*/
static void mm_stream_data_notify(void* user_data)
{
    mm_stream_t *my_obj = (mm_stream_t*)user_data;
    int32_t i, rc;
    uint8_t has_cb = 0, length = 0;
    mm_camera_buf_info_t buf_info;

    if (NULL == my_obj) {
        return;
    }

    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);
    if (MM_STREAM_STATE_ACTIVE != my_obj->state) {
        /* this Cb will only received in active_stream_on state
         * if not so, return here */
        LOGE("ERROR!! Wrong state (%d) to receive data notify!",
                    my_obj->state);
        return;
    }

    if (my_obj->stream_info->streaming_mode == CAM_STREAMING_MODE_BATCH) {
        length = 1;
    } else {
        length = my_obj->frame_offset.num_planes;
    }

    memset(&buf_info, 0, sizeof(mm_camera_buf_info_t));
    rc = mm_stream_read_msm_frame(my_obj, &buf_info,
        (uint8_t)length);
    if (rc != 0) {
        return;
    }
    uint32_t idx = buf_info.buf->buf_idx;

    pthread_mutex_lock(&my_obj->cb_lock);
    for (i = 0; i < MM_CAMERA_STREAM_BUF_CB_MAX; i++) {
        if(NULL != my_obj->buf_cb[i].cb) {
            if (my_obj->buf_cb[i].cb_type == MM_CAMERA_STREAM_CB_TYPE_SYNC) {
                /*For every SYNC callback, send data*/
                mm_stream_dispatch_sync_data(my_obj,
                        &my_obj->buf_cb[i], &buf_info);
            } else {
                /* for every ASYNC CB, need ref count */
                has_cb = 1;
            }
        }
    }
    pthread_mutex_unlock(&my_obj->cb_lock);

    pthread_mutex_lock(&my_obj->buf_lock);
    /* update buffer location */
    my_obj->buf_status[idx].in_kernel = 0;

    /* update buf ref count */
    if (my_obj->is_bundled) {
        /* need to add into super buf since bundled, add ref count */
        my_obj->buf_status[idx].buf_refcnt++;
    }
    my_obj->buf_status[idx].buf_refcnt =
        (uint8_t)(my_obj->buf_status[idx].buf_refcnt + has_cb);
    pthread_mutex_unlock(&my_obj->buf_lock);

    mm_stream_handle_rcvd_buf(my_obj, &buf_info, has_cb);
}

/*===========================================================================
 * FUNCTION   : mm_stream_dispatch_app_data
 *
 * DESCRIPTION: dispatch stream buffer to registered users
 *
 * PARAMETERS :
 *   @cmd_cb  : ptr storing stream buffer information
 *   @userdata: user data ptr (stream object)
 *
 * RETURN     : none
 *==========================================================================*/
static void mm_stream_dispatch_app_data(mm_camera_cmdcb_t *cmd_cb,
                                        void* user_data)
{
    int i;
    mm_stream_t * my_obj = (mm_stream_t *)user_data;
    mm_camera_buf_info_t* buf_info = NULL;
    mm_camera_super_buf_t super_buf;
    mm_stream_t *m_obj = my_obj;

    if (NULL == my_obj) {
        return;
    }
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);

    if (MM_CAMERA_CMD_TYPE_DATA_CB != cmd_cb->cmd_type) {
        LOGE("Wrong cmd_type (%d) for dataCB",
                    cmd_cb->cmd_type);
        return;
    }

    buf_info = &cmd_cb->u.buf;
    memset(&super_buf, 0, sizeof(mm_camera_super_buf_t));
    super_buf.num_bufs = 1;
    super_buf.bufs[0] = buf_info->buf;
    super_buf.camera_handle = my_obj->ch_obj->cam_obj->my_hdl;
    super_buf.ch_id = my_obj->ch_obj->my_hdl;

    if (m_obj->master_str_obj != NULL) {
        m_obj = m_obj->master_str_obj;
    }

    pthread_mutex_lock(&m_obj->frame_sync.sync_lock);
    LOGD("frame_sync.is_active = %d, is_cb_active = %d",
            m_obj->frame_sync.is_active, my_obj->is_cb_active);
    if (m_obj->frame_sync.is_active) {
        pthread_mutex_lock(&my_obj->buf_lock);
        my_obj->buf_status[buf_info->buf->buf_idx].buf_refcnt++;
        pthread_mutex_unlock(&my_obj->buf_lock);
        mm_camera_muxer_stream_frame_sync(&super_buf, my_obj);
    } else if (my_obj->is_cb_active) {
        pthread_mutex_lock(&my_obj->cb_lock);
        for(i = 0; i < MM_CAMERA_STREAM_BUF_CB_MAX; i++) {
            if(NULL != my_obj->buf_cb[i].cb
                    && (my_obj->buf_cb[i].cb_type !=
                    MM_CAMERA_STREAM_CB_TYPE_SYNC)) {
                if (my_obj->buf_cb[i].cb_count != 0) {
                    /* if <0, means infinite CB
                     * if >0, means CB for certain times
                     * both case we need to call CB */

                    /* increase buf ref cnt */
                    pthread_mutex_lock(&my_obj->buf_lock);
                    my_obj->buf_status[buf_info->buf->buf_idx].buf_refcnt++;
                    pthread_mutex_unlock(&my_obj->buf_lock);

                    /* callback */
                    my_obj->buf_cb[i].cb(&super_buf,
                            my_obj->buf_cb[i].user_data);
                }

                /* if >0, reduce count by 1 every time we called CB until reaches 0
                 * when count reach 0, reset the buf_cb to have no CB */
                if (my_obj->buf_cb[i].cb_count > 0) {
                    my_obj->buf_cb[i].cb_count--;
                    if (0 == my_obj->buf_cb[i].cb_count) {
                        my_obj->buf_cb[i].cb = NULL;
                        my_obj->buf_cb[i].user_data = NULL;
                    }
                }
            }
        }
        pthread_mutex_unlock(&my_obj->cb_lock);
    }
    pthread_mutex_unlock(&m_obj->frame_sync.sync_lock);

    /* do buf_done since we increased refcnt by one when has_cb */
    mm_stream_buf_done(my_obj, buf_info->buf);
}

/*===========================================================================
 * FUNCTION   : mm_stream_fsm_fn
 *
 * DESCRIPTION: stream finite state machine entry function. Depends on stream
 *              state, incoming event will be handled differently.
 *
 * PARAMETERS :
 *   @my_obj   : ptr to a stream object
 *   @evt      : stream event to be processed
 *   @in_val   : input event payload. Can be NULL if not needed.
 *   @out_val  : output payload, Can be NULL if not needed.
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_fsm_fn(mm_stream_t *my_obj,
                         mm_stream_evt_type_t evt,
                         void * in_val,
                         void * out_val)
{
    int32_t rc = -1;

    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);
    switch (my_obj->state) {
    case MM_STREAM_STATE_NOTUSED:
        LOGD("Not handling evt in unused state");
        break;
    case MM_STREAM_STATE_INITED:
        rc = mm_stream_fsm_inited(my_obj, evt, in_val, out_val);
        break;
    case MM_STREAM_STATE_ACQUIRED:
        rc = mm_stream_fsm_acquired(my_obj, evt, in_val, out_val);
        break;
    case MM_STREAM_STATE_CFG:
        rc = mm_stream_fsm_cfg(my_obj, evt, in_val, out_val);
        break;
    case MM_STREAM_STATE_BUFFED:
        rc = mm_stream_fsm_buffed(my_obj, evt, in_val, out_val);
        break;
    case MM_STREAM_STATE_REG:
        rc = mm_stream_fsm_reg(my_obj, evt, in_val, out_val);
        break;
    case MM_STREAM_STATE_ACTIVE:
        rc = mm_stream_fsm_active(my_obj, evt, in_val, out_val);
        break;
    default:
        LOGD("Not a valid state (%d)", my_obj->state);
        break;
    }
    LOGD("X rc =%d",rc);
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_fsm_inited
 *
 * DESCRIPTION: stream finite state machine function to handle event in INITED
 *              state.
 *
 * PARAMETERS :
 *   @my_obj   : ptr to a stream object
 *   @evt      : stream event to be processed
 *   @in_val   : input event payload. Can be NULL if not needed.
 *   @out_val  : output payload, Can be NULL if not needed.
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_fsm_inited(mm_stream_t *my_obj,
                             mm_stream_evt_type_t evt,
                             void * in_val,
                             void * out_val)
{
    int32_t rc = 0;
    char dev_name[MM_CAMERA_DEV_NAME_LEN];
    const char *dev_name_value = NULL;
    if (NULL == my_obj) {
      LOGE("NULL camera object\n");
      return -1;
    }

    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);
    switch(evt) {
    case MM_STREAM_EVT_ACQUIRE:
        if ((NULL == my_obj->ch_obj) || (NULL == my_obj->ch_obj->cam_obj)) {
            LOGE("NULL channel or camera obj\n");
            rc = -1;
            break;
        }

        mm_stream_init(my_obj);
        uint32_t cam_handle = my_obj->ch_obj->cam_obj->my_hdl;
        dev_name_value = mm_camera_util_get_dev_name_by_num(
                my_obj->ch_obj->cam_obj->my_num, cam_handle);
        if (NULL == dev_name_value) {
            LOGE("NULL device name\n");
            rc = -1;
            mm_stream_deinit(my_obj);
            break;
        }

        snprintf(dev_name, sizeof(dev_name), "/dev/%s",
                 dev_name_value);

        my_obj->fd = open(dev_name, O_RDWR | O_NONBLOCK);
        if (my_obj->fd < 0) {
            LOGE("open dev returned %d\n", my_obj->fd);
            rc = -1;
            mm_stream_deinit(my_obj);
            break;
        }
        LOGD("open dev fd = %d\n", my_obj->fd);
        rc = mm_stream_set_ext_mode(my_obj);
        if (0 == rc) {
            my_obj->state = MM_STREAM_STATE_ACQUIRED;
        } else {
            /* failed setting ext_mode
             * close fd */
            close(my_obj->fd);
            my_obj->fd = -1;
            mm_stream_deinit(my_obj);
            break;
        }
        break;
    case MM_STREAM_EVT_RELEASE:
        mm_stream_deinit(my_obj);
        memset(my_obj, 0, sizeof(mm_stream_t));
        break;
    default:
        LOGE("invalid state (%d) for evt (%d), in(%p), out(%p)",
                    my_obj->state, evt, in_val, out_val);
        break;
    }
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_fsm_acquired
 *
 * DESCRIPTION: stream finite state machine function to handle event in AQUIRED
 *              state.
 *
 * PARAMETERS :
 *   @my_obj   : ptr to a stream object
 *   @evt      : stream event to be processed
 *   @in_val   : input event payload. Can be NULL if not needed.
 *   @out_val  : output payload, Can be NULL if not needed.
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_fsm_acquired(mm_stream_t *my_obj,
                               mm_stream_evt_type_t evt,
                               void * in_val,
                               void * out_val)
{
    int32_t rc = 0;

    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);
    switch(evt) {
    case MM_STREAM_EVT_SET_FMT:
        {
            mm_camera_stream_config_t *config =
                (mm_camera_stream_config_t *)in_val;

            rc = mm_stream_config(my_obj, config);

            /* change state to configed */
            my_obj->state = MM_STREAM_STATE_CFG;

            break;
        }
    case MM_STREAM_EVT_RELEASE:
        rc = mm_stream_release(my_obj);
        /* change state to not used */
         my_obj->state = MM_STREAM_STATE_NOTUSED;
        break;
    case MM_STREAM_EVT_SET_PARM:
        {
            mm_evt_paylod_set_get_stream_parms_t *payload =
                (mm_evt_paylod_set_get_stream_parms_t *)in_val;
            rc = mm_stream_set_parm(my_obj, payload->parms);
        }
        break;
    case MM_STREAM_EVT_GET_PARM:
        {
            mm_evt_paylod_set_get_stream_parms_t *payload =
                (mm_evt_paylod_set_get_stream_parms_t *)in_val;
            rc = mm_stream_get_parm(my_obj, payload->parms);
        }
        break;
    case MM_STREAM_EVT_REG_FRAME_SYNC:
        {
            mm_evt_paylod_reg_frame_sync *payload =
                (mm_evt_paylod_reg_frame_sync *)in_val;
            rc = mm_stream_reg_frame_sync(my_obj, payload);
        }
        break;
    case MM_STREAM_EVT_TRIGGER_FRAME_SYNC:
        {
            mm_camera_cb_req_type type =
                    *((mm_camera_cb_req_type *)in_val);
            rc = mm_stream_trigger_frame_sync(my_obj, type);
        }
        break;
    default:
        LOGE("invalid state (%d) for evt (%d), in(%p), out(%p)",
                    my_obj->state, evt, in_val, out_val);
    }
    LOGD("X rc = %d", rc);
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_fsm_cfg
 *
 * DESCRIPTION: stream finite state machine function to handle event in CONFIGURED
 *              state.
 *
 * PARAMETERS :
 *   @my_obj   : ptr to a stream object
 *   @evt      : stream event to be processed
 *   @in_val   : input event payload. Can be NULL if not needed.
 *   @out_val  : output payload, Can be NULL if not needed.
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_fsm_cfg(mm_stream_t * my_obj,
                          mm_stream_evt_type_t evt,
                          void * in_val,
                          void * out_val)
{
    int32_t rc = 0;
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);
    switch(evt) {
    case MM_STREAM_EVT_SET_FMT:
        {
            mm_camera_stream_config_t *config =
                (mm_camera_stream_config_t *)in_val;

            rc = mm_stream_config(my_obj, config);

            /* change state to configed */
            my_obj->state = MM_STREAM_STATE_CFG;

            break;
        }
    case MM_STREAM_EVT_RELEASE:
        rc = mm_stream_release(my_obj);
        my_obj->state = MM_STREAM_STATE_NOTUSED;
        break;
    case MM_STREAM_EVT_SET_PARM:
        {
            mm_evt_paylod_set_get_stream_parms_t *payload =
                (mm_evt_paylod_set_get_stream_parms_t *)in_val;
            rc = mm_stream_set_parm(my_obj, payload->parms);
        }
        break;
    case MM_STREAM_EVT_GET_PARM:
        {
            mm_evt_paylod_set_get_stream_parms_t *payload =
                (mm_evt_paylod_set_get_stream_parms_t *)in_val;
            rc = mm_stream_get_parm(my_obj, payload->parms);
        }
        break;
    case MM_STREAM_EVT_GET_BUF:
        rc = mm_stream_init_bufs(my_obj);
        /* change state to buff allocated */
        if(0 == rc) {
            my_obj->state = MM_STREAM_STATE_BUFFED;
        }
        break;
    case MM_STREAM_EVT_TRIGGER_FRAME_SYNC:
        {
            mm_camera_cb_req_type type =
                    *((mm_camera_cb_req_type *)in_val);
            rc = mm_stream_trigger_frame_sync(my_obj, type);
        }
        break;
    default:
        LOGE("invalid state (%d) for evt (%d), in(%p), out(%p)",
                    my_obj->state, evt, in_val, out_val);
    }
    LOGD("X rc = %d", rc);
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_fsm_buffed
 *
 * DESCRIPTION: stream finite state machine function to handle event in BUFFED
 *              state.
 *
 * PARAMETERS :
 *   @my_obj   : ptr to a stream object
 *   @evt      : stream event to be processed
 *   @in_val   : input event payload. Can be NULL if not needed.
 *   @out_val  : output payload, Can be NULL if not needed.
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_fsm_buffed(mm_stream_t * my_obj,
                             mm_stream_evt_type_t evt,
                             void * in_val,
                             void * out_val)
{
    int32_t rc = 0;
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);
    switch(evt) {
    case MM_STREAM_EVT_PUT_BUF:
        rc = mm_stream_deinit_bufs(my_obj);
        /* change state to configed */
        my_obj->state = MM_STREAM_STATE_CFG;
        break;
    case MM_STREAM_EVT_REG_BUF:
        rc = mm_stream_reg_buf(my_obj);
        /* change state to regged */
        if(0 == rc) {
            my_obj->state = MM_STREAM_STATE_REG;
        }
        break;
    case MM_STREAM_EVT_SET_PARM:
        {
            mm_evt_paylod_set_get_stream_parms_t *payload =
                (mm_evt_paylod_set_get_stream_parms_t *)in_val;
            rc = mm_stream_set_parm(my_obj, payload->parms);
        }
        break;
    case MM_STREAM_EVT_GET_PARM:
        {
            mm_evt_paylod_set_get_stream_parms_t *payload =
                (mm_evt_paylod_set_get_stream_parms_t *)in_val;
            rc = mm_stream_get_parm(my_obj, payload->parms);
        }
        break;
    default:
        LOGW("invalid state (%d) for evt (%d), in(%p), out(%p)",
                    my_obj->state, evt, in_val, out_val);
    }
    LOGD("X rc = %d", rc);
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_fsm_reg
 *
 * DESCRIPTION: stream finite state machine function to handle event in REGGED
 *              state.
 *
 * PARAMETERS :
 *   @my_obj   : ptr to a stream object
 *   @evt      : stream event to be processed
 *   @in_val   : input event payload. Can be NULL if not needed.
 *   @out_val  : output payload, Can be NULL if not needed.
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_fsm_reg(mm_stream_t * my_obj,
                          mm_stream_evt_type_t evt,
                          void * in_val,
                          void * out_val)
{
    int32_t rc = 0;
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);

    switch(evt) {
    case MM_STREAM_EVT_UNREG_BUF:
        rc = mm_stream_unreg_buf(my_obj);

        /* change state to buffed */
        my_obj->state = MM_STREAM_STATE_BUFFED;
        break;
    case MM_STREAM_EVT_START:
        {
            uint8_t has_cb = 0;
            uint8_t i;
            /* launch cmd thread if CB is not null */
            pthread_mutex_lock(&my_obj->cb_lock);
            for (i = 0; i < MM_CAMERA_STREAM_BUF_CB_MAX; i++) {
                if((NULL != my_obj->buf_cb[i].cb) &&
                        (my_obj->buf_cb[i].cb_type != MM_CAMERA_STREAM_CB_TYPE_SYNC)) {
                    has_cb = 1;
                    break;
                }
            }
            pthread_mutex_unlock(&my_obj->cb_lock);

            pthread_mutex_lock(&my_obj->cmd_lock);
            if (has_cb) {
                snprintf(my_obj->cmd_thread.threadName, THREAD_NAME_SIZE, "CAM_StrmAppDat");
                mm_camera_cmd_thread_launch(&my_obj->cmd_thread,
                                            mm_stream_dispatch_app_data,
                                            (void *)my_obj);
            }
            pthread_mutex_unlock(&my_obj->cmd_lock);

            my_obj->state = MM_STREAM_STATE_ACTIVE;
            rc = mm_stream_streamon(my_obj);
            if (0 != rc) {
                /* failed stream on, need to release cmd thread if it's launched */
                pthread_mutex_lock(&my_obj->cmd_lock);
                if (has_cb) {
                    mm_camera_cmd_thread_release(&my_obj->cmd_thread);
                }
                pthread_mutex_unlock(&my_obj->cmd_lock);
                my_obj->state = MM_STREAM_STATE_REG;
                break;
            }
        }
        break;
    case MM_STREAM_EVT_SET_PARM:
        {
            mm_evt_paylod_set_get_stream_parms_t *payload =
                (mm_evt_paylod_set_get_stream_parms_t *)in_val;
            rc = mm_stream_set_parm(my_obj, payload->parms);
        }
        break;
    case MM_STREAM_EVT_GET_PARM:
        {
            mm_evt_paylod_set_get_stream_parms_t *payload =
                (mm_evt_paylod_set_get_stream_parms_t *)in_val;
            rc = mm_stream_get_parm(my_obj, payload->parms);
        }
        break;
    default:
        LOGE("invalid state (%d) for evt (%d), in(%p), out(%p)",
                    my_obj->state, evt, in_val, out_val);
    }
    LOGD("X rc = %d", rc);
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_fsm_active
 *
 * DESCRIPTION: stream finite state machine function to handle event in ACTIVE
 *              state.
 *
 * PARAMETERS :
 *   @my_obj   : ptr to a stream object
 *   @evt      : stream event to be processed
 *   @in_val   : input event payload. Can be NULL if not needed.
 *   @out_val  : output payload, Can be NULL if not needed.
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_fsm_active(mm_stream_t * my_obj,
                             mm_stream_evt_type_t evt,
                             void * in_val,
                             void * out_val)
{
    int32_t rc = 0;
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);
    switch(evt) {
    case MM_STREAM_EVT_QBUF:
        rc = mm_stream_buf_done(my_obj, (mm_camera_buf_def_t *)in_val);
        break;
    case MM_STREAM_EVT_CANCEL_BUF:
        rc = mm_stream_cancel_buf(my_obj, *((uint32_t*)in_val));
        break;
    case MM_STREAM_EVT_GET_QUEUED_BUF_COUNT:
        rc = mm_stream_get_queued_buf_count(my_obj);
        break;
    case MM_STREAM_EVT_STOP:
        {
            uint8_t has_cb = 0;
            uint8_t i;
            rc = mm_stream_streamoff(my_obj);

            pthread_mutex_lock(&my_obj->cb_lock);
            for (i = 0; i < MM_CAMERA_STREAM_BUF_CB_MAX; i++) {
                if(NULL != my_obj->buf_cb[i].cb
                        && my_obj->buf_cb[i].cb_type != MM_CAMERA_STREAM_CB_TYPE_SYNC) {
                    has_cb = 1;
                    break;
                }
            }
            pthread_mutex_unlock(&my_obj->cb_lock);

            pthread_mutex_lock(&my_obj->cmd_lock);
            if (has_cb) {
                mm_camera_cmd_thread_release(&my_obj->cmd_thread);
            }
            pthread_mutex_unlock(&my_obj->cmd_lock);
            my_obj->state = MM_STREAM_STATE_REG;
        }
        break;
    case MM_STREAM_EVT_SET_PARM:
        {
            mm_evt_paylod_set_get_stream_parms_t *payload =
                (mm_evt_paylod_set_get_stream_parms_t *)in_val;
            rc = mm_stream_set_parm(my_obj, payload->parms);
        }
        break;
    case MM_STREAM_EVT_GET_PARM:
        {
            mm_evt_paylod_set_get_stream_parms_t *payload =
                (mm_evt_paylod_set_get_stream_parms_t *)in_val;
            rc = mm_stream_get_parm(my_obj, payload->parms);
        }
        break;
    case MM_STREAM_EVT_DO_ACTION:
        rc = mm_stream_do_action(my_obj, in_val);
        break;
    case MM_STREAM_EVT_TRIGGER_FRAME_SYNC:
        {
            mm_camera_cb_req_type type =
                    *((mm_camera_cb_req_type *)in_val);
            rc = mm_stream_trigger_frame_sync(my_obj, type);
        }
        break;
    default:
        LOGE("invalid state (%d) for evt (%d), in(%p), out(%p)",
                    my_obj->state, evt, in_val, out_val);
    }
    LOGD("X rc = %d", rc);
    return rc;
}

int32_t mm_stream_init(mm_stream_t *my_obj)
{
    int32_t rc = 0;

    pthread_condattr_t cond_attr;

    pthread_condattr_init(&cond_attr);
    pthread_condattr_setclock(&cond_attr, CLOCK_MONOTONIC);

    pthread_mutex_init(&my_obj->buf_lock, NULL);
    pthread_mutex_init(&my_obj->cb_lock, NULL);
    pthread_mutex_init(&my_obj->cmd_lock, NULL);
    pthread_cond_init(&my_obj->buf_cond, &cond_attr);
    pthread_condattr_destroy(&cond_attr);

    memset(my_obj->buf_status, 0,
            sizeof(my_obj->buf_status));
    memset(&my_obj->frame_sync, 0, sizeof(my_obj->frame_sync));
    pthread_mutex_init(&my_obj->frame_sync.sync_lock, NULL);
    mm_muxer_frame_sync_queue_init(&my_obj->frame_sync.superbuf_queue);
    my_obj->is_cb_active = 1;
    LOGD("my_obj->is_cb_active = %d, my_obj->ch_obj->cam_obj->my_num: %d",
            my_obj->is_cb_active, my_obj->ch_obj->cam_obj->my_num);

    my_obj->is_res_shared = 0;
    my_obj->map_ops.map_ops = mm_camera_map_stream_buf_ops;
    my_obj->map_ops.bundled_map_ops = mm_camera_bundled_map_stream_buf_ops;
    my_obj->map_ops.unmap_ops = mm_camera_unmap_stream_buf_ops;
    my_obj->map_ops.userdata = my_obj;
    my_obj->is_stream_inited = 1;
    return rc;
}

int32_t mm_stream_deinit(mm_stream_t *my_obj)
{
    int32_t rc = 0;
    /* destroy mutex */
    LOGH("stream inited %d",my_obj->is_stream_inited);
    if (my_obj->is_stream_inited) {
        mm_muxer_frame_sync_queue_deinit(&my_obj->frame_sync.superbuf_queue);
        pthread_mutex_destroy(&my_obj->frame_sync.sync_lock);
        pthread_cond_destroy(&my_obj->buf_cond);
        pthread_mutex_destroy(&my_obj->buf_lock);
        pthread_mutex_destroy(&my_obj->cb_lock);
        pthread_mutex_destroy(&my_obj->cmd_lock);
        my_obj->is_stream_inited = 0;
    }

    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_config
 *
 * DESCRIPTION: configure a stream
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *   @config       : stream configuration
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_config(mm_stream_t *my_obj,
                         mm_camera_stream_config_t *config)
{
    int32_t rc = 0;
    int32_t cb_index = 0;

    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);
    my_obj->stream_info = config->stream_info;

    if (config->stream_info->buf_cnt == 0) {
        my_obj->buf_num = (uint8_t)config->stream_info->num_bufs;
    } else {
        my_obj->buf_num = (uint8_t)config->stream_info->buf_cnt;
    }
    my_obj->total_buf_cnt = config->stream_info->num_bufs;
    my_obj->mem_vtbl = config->mem_vtbl;
    my_obj->padding_info = config->padding_info;

    if (config->stream_cb_sync != NULL) {
        /* SYNC callback is always placed at index 0*/
        my_obj->buf_cb[cb_index].cb = config->stream_cb_sync;
        my_obj->buf_cb[cb_index].user_data = config->userdata;
        my_obj->buf_cb[cb_index].cb_count = -1; /* infinite by default */
        my_obj->buf_cb[cb_index].cb_type = MM_CAMERA_STREAM_CB_TYPE_SYNC;
        cb_index++;
    }
    my_obj->buf_cb[cb_index].cb = config->stream_cb;
    my_obj->buf_cb[cb_index].user_data = config->userdata;
    my_obj->buf_cb[cb_index].cb_count = -1; /* infinite by default */
    my_obj->buf_cb[cb_index].cb_type = MM_CAMERA_STREAM_CB_TYPE_ASYNC;
    // For dual camera use case, make cb active only for main camera
    // For asymmetric streams, cb is always active and the streams are not linked
    if ((my_obj->ch_obj->cam_obj->my_num == 0) ||(my_obj->master_str_obj == NULL)) {
        my_obj->is_cb_active = 1;
    } else {
        // Disable CB only if CB is not requested on all streams
        my_obj->is_cb_active = 0;
    }
    LOGD("my_obj->is_cb_active = %d, my_obj->ch_obj->cam_obj->my_num: %d",
            my_obj->is_cb_active, my_obj->ch_obj->cam_obj->my_num);

    if ((my_obj->frame_sync.superbuf_queue.num_objs != 0)
            && (my_obj->frame_sync.super_buf_notify_cb == NULL)) {
        my_obj->frame_sync.super_buf_notify_cb = config->stream_cb;
    }
    if ((my_obj->frame_sync.superbuf_queue.num_objs != 0)
            && (my_obj->frame_sync.user_data == NULL)) {
        my_obj->frame_sync.user_data = config->userdata;
    }

    rc = mm_stream_sync_info(my_obj);
    if (rc == 0) {
        rc = mm_stream_set_fmt(my_obj);
        if (rc < 0) {
            LOGE("mm_stream_set_fmt failed %d", rc);
        }
    }

    if((my_obj->mem_vtbl.set_config_ops != NULL)
            && (!my_obj->is_res_shared)) {
        my_obj->mem_vtbl.set_config_ops(&my_obj->map_ops,
                my_obj->mem_vtbl.user_data);
    }
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_reg_frame_sync
 *
 * DESCRIPTION: reg stream frame sync
 *
 * PARAMETERS :
 *   @str_obj      : stream object
 *   @sync  : sync attribute
 *
 * RETURN     : uint32_t type of stream handle
 *              0  -- invalid stream handle, meaning the op failed
 *              >0 -- successfully added a stream with a valid handle
 *==========================================================================*/
int32_t mm_stream_reg_frame_sync(mm_stream_t *str_obj, mm_evt_paylod_reg_frame_sync *sync)
{
    int32_t rc = 0;
    mm_stream_t *my_obj = str_obj;

    if (NULL == sync || sync->a_str_obj == NULL) {
        LOGE("Invalid stream link");
        return -1;
    }

    if (str_obj->master_str_obj != NULL) {
        my_obj = str_obj->master_str_obj;
    }

    mm_frame_sync_t *frame_sync = &my_obj->frame_sync;
    pthread_mutex_lock(&frame_sync->sync_lock);
    mm_frame_sync_queue_t *queue = NULL;

    frame_sync->super_buf_notify_cb = sync->sync_attr->buf_cb;
    frame_sync->user_data = sync->sync_attr->userdata;
    queue = &frame_sync->superbuf_queue;
    queue->num_objs = 0;
    memset(&queue->bundled_objs, 0, sizeof(queue->bundled_objs));
    queue->bundled_objs[queue->num_objs] = my_obj->my_hdl;
    queue->num_objs++;
    queue->bundled_objs[queue->num_objs] = sync->a_str_obj->my_hdl;
    queue->num_objs++;
    queue->expected_frame_id = 0;
    queue->attr = sync->sync_attr->attr;

    sync->a_str_obj->is_res_shared = sync->sync_attr->is_res_shared;
    my_obj->aux_str_obj[my_obj->num_s_cnt++] = sync->a_str_obj;
    sync->a_str_obj->master_str_obj = my_obj;
    pthread_mutex_unlock(&frame_sync->sync_lock);
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_trigger_frame_sync
 *
 * DESCRIPTION: start/stop stream frame sync
 *
 * PARAMETERS :
 *   @my_obj  : stream object
 *   @type  : flag to start/stop frame sync.
 *
 * RETURN     : uint32_t type of stream handle
 *              0  -- invalid stream handle, meaning the op failed
 *              >0 -- successfully added a stream with a valid handle
 *==========================================================================*/
int32_t mm_stream_trigger_frame_sync(mm_stream_t *my_obj,
        mm_camera_cb_req_type type)
{
    int32_t rc = 0;
    mm_stream_t *m_obj = my_obj;
    mm_stream_t *s_obj = NULL;
    mm_frame_sync_t *frame_sync = NULL;

    if (m_obj->master_str_obj != NULL) {
        m_obj = m_obj->master_str_obj;
    }
    s_obj = m_obj->aux_str_obj[0];

    frame_sync = &m_obj->frame_sync;
    pthread_mutex_lock(&frame_sync->sync_lock);
    switch (type) {
        case MM_CAMERA_CB_REQ_TYPE_SWITCH:
            if (m_obj->frame_sync.is_active) {
                mm_camera_muxer_stream_frame_sync_flush(m_obj);
            }
            m_obj->frame_sync.is_active = 0;

            pthread_mutex_lock(&s_obj->cb_lock);
            s_obj->is_cb_active = !s_obj->is_cb_active;
            pthread_mutex_unlock(&s_obj->cb_lock);

            pthread_mutex_lock(&m_obj->cb_lock);
            m_obj->is_cb_active = !m_obj->is_cb_active;
            if (s_obj->is_cb_active == 0
                    && m_obj->is_cb_active == 0) {
                m_obj->is_cb_active = 1;
            }
            pthread_mutex_unlock(&m_obj->cb_lock);
            LOGD("FD: %d After switch s_obj->is_cb_active: %d, m_obj->is_cb_active: %d",
                    my_obj->fd, s_obj->is_cb_active, m_obj->is_cb_active);
        break;

        case MM_CAMERA_CB_REQ_TYPE_FRAME_SYNC:
            m_obj->frame_sync.is_active = 1;
        break;

        case MM_CAMERA_CB_REQ_TYPE_ALL_CB:
            pthread_mutex_lock(&m_obj->cb_lock);
            m_obj->is_cb_active = 1;
            pthread_mutex_unlock(&m_obj->cb_lock);

            pthread_mutex_lock(&s_obj->cb_lock);
            s_obj->is_cb_active = 1;
            pthread_mutex_unlock(&s_obj->cb_lock);
            LOGD("ALL_CB s_obj->is_cb_active: %d, m_obj->is_cb_active: %d",
                    s_obj->is_cb_active, m_obj->is_cb_active)
        break;

        case MM_CAMERA_CB_REQ_TYPE_DEFER:
            my_obj->is_deferred = 1;
        break;

        case MM_CAMERA_CB_REQ_TYPE_SHARE_FRAME: {
            mm_camera_stream_link_t stream_link;

            stream_link.ch = s_obj->ch_obj;
            stream_link.stream_id = s_obj->my_hdl;
            m_obj->is_frame_shared = 1;
            mm_channel_link_stream(m_obj->ch_obj,&stream_link);

            stream_link.ch = m_obj->ch_obj;
            stream_link.stream_id = m_obj->my_hdl;
            s_obj->is_frame_shared = 1;
            mm_channel_link_stream(s_obj->ch_obj,&stream_link);
        }
        break;

        default:
            //no-op
            break;
    }
    pthread_mutex_unlock(&frame_sync->sync_lock);
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_release
 *
 * DESCRIPTION: release a stream resource
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_release(mm_stream_t *my_obj)
{
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);

    pthread_mutex_lock(&my_obj->buf_lock);
    memset(my_obj->buf_status, 0, sizeof(my_obj->buf_status));
    pthread_mutex_unlock(&my_obj->buf_lock);

    /* close fd */
    if (my_obj->fd >= 0) {
#ifndef DAEMON_PRESENT
        int32_t rc = 0;
        cam_shim_packet_t *shim_cmd;
        cam_shim_cmd_data shim_cmd_data;
        mm_camera_obj_t *cam_obj = my_obj->ch_obj->cam_obj;

        memset(&shim_cmd_data, 0, sizeof(shim_cmd_data));
        shim_cmd_data.command = MSM_CAMERA_PRIV_DEL_STREAM;
        shim_cmd_data.stream_id = my_obj->server_stream_id;
        shim_cmd_data.value = NULL;
        shim_cmd = mm_camera_create_shim_cmd_packet(CAM_SHIM_SET_PARM,
                cam_obj->sessionid, &shim_cmd_data);
        rc = mm_camera_module_send_cmd(shim_cmd);
        if (rc < 0) {
            LOGE("failed to DELETE STREAM");
        }
        mm_camera_destroy_shim_cmd_packet(shim_cmd);
#endif /* DAEMON_PRESENT */
        close(my_obj->fd);
    }

    if (my_obj->master_str_obj != NULL) {
        //Assuming order of stream release is maintained
        my_obj->master_str_obj->num_s_cnt--;
        my_obj->master_str_obj->aux_str_obj[
                my_obj->master_str_obj->num_s_cnt] = NULL;
    }
    mm_stream_deinit(my_obj);

    /* reset stream obj */
    memset(my_obj, 0, sizeof(mm_stream_t));
    my_obj->fd = -1;

    return 0;
}

/*===========================================================================
 * FUNCTION   : mm_stream_streamon
 *
 * DESCRIPTION: stream on a stream. sending v4l2 request to kernel
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_streamon(mm_stream_t *my_obj)
{
    int32_t rc = 0;
    int8_t i;
    enum v4l2_buf_type buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    uint8_t idx = mm_camera_util_get_index_by_num(
            my_obj->ch_obj->cam_obj->my_num, my_obj->my_hdl);

    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);

    pthread_mutex_lock(&my_obj->buf_lock);
    for (i = my_obj->buf_idx; i < (my_obj->buf_idx + my_obj->buf_num); i++) {
        if ((my_obj->buf_status[i].map_status == 0) &&
                (my_obj->buf_status[i].in_kernel)) {
            LOGD("waiting for mapping to done: strm fd = %d",
                     my_obj->fd);
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC, &ts);
            ts.tv_sec += WAIT_TIMEOUT;
            rc = pthread_cond_timedwait(&my_obj->buf_cond, &my_obj->buf_lock, &ts);
            if (rc == ETIMEDOUT) {
                LOGE("Timed out. Abort stream-on \n");
                rc = -1;
            }
            break;
        } else if (my_obj->buf_status[i].map_status < 0) {
            LOGD("Buffer mapping failed. Abort Stream On");
            rc = -1;
            break;
        }
    }
    pthread_mutex_unlock(&my_obj->buf_lock);

    if (rc < 0) {
        /* remove fd from data poll thread in case of failure */
        mm_camera_poll_thread_del_poll_fd(&my_obj->ch_obj->poll_thread[0],
                idx, my_obj->my_hdl, mm_camera_sync_call);
        return rc;
    }
    mm_camera_obj_t *cam_obj = my_obj->ch_obj->cam_obj;
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d session_id:%d stream_id:%d",
            my_obj->my_hdl, my_obj->fd, my_obj->state, cam_obj->sessionid,
            my_obj->server_stream_id);

    rc = ioctl(my_obj->fd, VIDIOC_STREAMON, &buf_type);
    if (rc < 0 && my_obj->stream_info->num_bufs != 0) {
        LOGE("ioctl VIDIOC_STREAMON failed: rc=%d, errno %d",
                rc, errno);
        goto error_case;
    }

#ifndef DAEMON_PRESENT
    cam_shim_packet_t *shim_cmd;
    cam_shim_cmd_data shim_cmd_data;

    memset(&shim_cmd_data, 0, sizeof(shim_cmd_data));
    shim_cmd_data.command = MSM_CAMERA_PRIV_STREAM_ON;
    shim_cmd_data.stream_id = my_obj->server_stream_id;
    shim_cmd_data.value = NULL;
    shim_cmd = mm_camera_create_shim_cmd_packet(CAM_SHIM_SET_PARM,
            cam_obj->sessionid, &shim_cmd_data);
    rc = mm_camera_module_send_cmd(shim_cmd);
    mm_camera_destroy_shim_cmd_packet(shim_cmd);
    if (rc < 0) {
        LOGE("Module StreamON failed: rc=%d", rc);
        ioctl(my_obj->fd, VIDIOC_STREAMOFF, &buf_type);
        goto error_case;
    }
#endif
    LOGD("X rc = %d",rc);
    return rc;
error_case:
     /* remove fd from data poll thread in case of failure */
     mm_camera_poll_thread_del_poll_fd(&my_obj->ch_obj->poll_thread[0],
             idx, my_obj->my_hdl, mm_camera_sync_call);

    LOGD("X rc = %d",rc);
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_streamoff
 *
 * DESCRIPTION: stream off a stream. sending v4l2 request to kernel
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_streamoff(mm_stream_t *my_obj)
{
    int32_t rc = 0;
    enum v4l2_buf_type buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);

    uint8_t idx = mm_camera_util_get_index_by_num(
            my_obj->ch_obj->cam_obj->my_num, my_obj->my_hdl);
    /* step1: remove fd from data poll thread */
    mm_camera_poll_thread_del_poll_fd(&my_obj->ch_obj->poll_thread[0],
            idx, my_obj->my_hdl, mm_camera_sync_call);
    mm_camera_poll_thread_commit_updates(&my_obj->ch_obj->poll_thread[0]);

#ifndef DAEMON_PRESENT
    cam_shim_packet_t *shim_cmd;
    cam_shim_cmd_data shim_cmd_data;
    mm_camera_obj_t *cam_obj = my_obj->ch_obj->cam_obj;

    memset(&shim_cmd_data, 0, sizeof(shim_cmd_data));
    shim_cmd_data.command = MSM_CAMERA_PRIV_STREAM_OFF;
    shim_cmd_data.stream_id = my_obj->server_stream_id;
    shim_cmd_data.value = NULL;
    shim_cmd = mm_camera_create_shim_cmd_packet(CAM_SHIM_SET_PARM,
            cam_obj->sessionid, &shim_cmd_data);

    rc |= mm_camera_module_send_cmd(shim_cmd);
    mm_camera_destroy_shim_cmd_packet(shim_cmd);
    if (rc < 0) {
        LOGE("Module StreamOFF failed: rc=%d", rc)
    }
#endif

    /* step2: stream off */
    rc |= ioctl(my_obj->fd, VIDIOC_STREAMOFF, &buf_type);
    if (rc < 0) {
        LOGE("STREAMOFF ioctl failed: %s", strerror(errno));
    }
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_write_user_buf
 *
 * DESCRIPTION: dequeue a stream buffer from user buffer queue and fill internal structure
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *   @buf     : ptr to a struct storing buffer information
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_write_user_buf(mm_stream_t * my_obj,
        mm_camera_buf_def_t *buf)
{
    int32_t rc = 0, i;
    int32_t index = -1, count = 0;
    struct msm_camera_user_buf_cont_t *cont_buf = NULL;

    if (buf->buf_type == CAM_STREAM_BUF_TYPE_USERPTR) {
        pthread_mutex_lock(&my_obj->buf_lock);
        my_obj->buf_status[buf->buf_idx].buf_refcnt--;
        if (0 == my_obj->buf_status[buf->buf_idx].buf_refcnt) {
            pthread_mutex_unlock(&my_obj->buf_lock);
            cont_buf = (struct msm_camera_user_buf_cont_t *)my_obj->buf[buf->buf_idx].buffer;
            cont_buf->buf_cnt = my_obj->buf[buf->buf_idx].user_buf.bufs_used;
            for (i = 0; i < (int32_t)cont_buf->buf_cnt; i++) {
                cont_buf->buf_idx[i] = my_obj->buf[buf->buf_idx].user_buf.buf_idx[i];
            }
            rc = mm_stream_qbuf(my_obj, buf);
            if(rc < 0) {
                LOGE("mm_camera_stream_qbuf(idx=%d) err=%d\n",
                            buf->buf_idx, rc);
            } else {
                for (i = 0; i < (int32_t)cont_buf->buf_cnt; i++) {
                    my_obj->buf[buf->buf_idx].user_buf.buf_idx[i] = -1;
                }
                my_obj->buf_status[buf->buf_idx].in_kernel = 1;
                my_obj->buf[buf->buf_idx].user_buf.buf_in_use = 1;
            }
        } else {
            LOGD("<DEBUG> : ref count pending count :%d idx = %d",
                 my_obj->buf_status[buf->buf_idx].buf_refcnt, buf->buf_idx);
            pthread_mutex_unlock(&my_obj->buf_lock);
        }
        return rc;
    }

    if ((my_obj->cur_buf_idx < 0)
            || (my_obj->cur_buf_idx >=
            (my_obj->buf_idx + my_obj->buf_num))) {
        for (i = 0; i < my_obj->buf_num; i++) {
            if ((my_obj->buf_status[i].in_kernel)
                    || (my_obj->buf[i].user_buf.buf_in_use)) {
                continue;
            }

            my_obj->cur_buf_idx = index = i;
            break;
        }
    } else {
        index = my_obj->cur_buf_idx;
    }

    if (index == -1) {
        LOGE("No Free batch buffer");
        rc = -1;
        return rc;
    }

    //Insert Buffer to Batch structure.
    my_obj->buf[index].user_buf.buf_idx[count] = buf->buf_idx;
    my_obj->cur_bufs_staged++;

    LOGD("index = %d filled = %d used = %d",
            index,
            my_obj->cur_bufs_staged,
            my_obj->buf[index].user_buf.bufs_used);

    if (my_obj->cur_bufs_staged
            == my_obj->buf[index].user_buf.bufs_used){
        pthread_mutex_lock(&my_obj->buf_lock);
        my_obj->buf_status[index].buf_refcnt--;
        if (0 == my_obj->buf_status[index].buf_refcnt) {
            pthread_mutex_unlock(&my_obj->buf_lock);
            cont_buf = (struct msm_camera_user_buf_cont_t *)my_obj->buf[index].buffer;
            cont_buf->buf_cnt = my_obj->buf[index].user_buf.bufs_used;
            for (i = 0; i < (int32_t)cont_buf->buf_cnt; i++) {
                cont_buf->buf_idx[i] = my_obj->buf[index].user_buf.buf_idx[i];
            }
            rc = mm_stream_qbuf(my_obj, &my_obj->buf[index]);
            if(rc < 0) {
                LOGE("mm_camera_stream_qbuf(idx=%d) err=%d\n",
                            index, rc);
            } else {
                for (i = 0; i < (int32_t)cont_buf->buf_cnt; i++) {
                    my_obj->buf[index].user_buf.buf_idx[i] = -1;
                }
                my_obj->buf_status[index].in_kernel = 1;
                my_obj->buf[index].user_buf.buf_in_use = 1;
                my_obj->cur_bufs_staged = 0;
                my_obj->cur_buf_idx = -1;
            }
        }else{
            LOGD("<DEBUG> : ref count pending count :%d idx = %d",
                 my_obj->buf_status[index].buf_refcnt, index);
            pthread_mutex_unlock(&my_obj->buf_lock);
        }
    }

    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_read_user_buf
 *
 * DESCRIPTION: dequeue a stream buffer from user buffer queue and fill internal structure
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *   @buf_info     : ptr to a struct storing buffer information
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_read_user_buf(mm_stream_t * my_obj,
        mm_camera_buf_info_t* buf_info)
{
    int32_t rc = 0, i;
    mm_camera_buf_def_t *stream_buf  = NULL;
    struct msm_camera_user_buf_cont_t *user_buf = NULL;
    nsecs_t interval_nsec = 0, frame_ts = 0, timeStamp = 0;
    int ts_delta = 0;
    uint32_t frameID = 0;

    user_buf = (struct msm_camera_user_buf_cont_t *)buf_info->buf->buffer;

    if(user_buf != my_obj->buf[buf_info->buf->buf_idx].buffer) {
        LOGD("Buffer modified. ERROR");
        rc = -1;
        return rc;
    }

    if (buf_info->buf->frame_idx == 1) {
        frameID = buf_info->buf->frame_idx;
    }else {
        frameID = (buf_info->buf->frame_idx - 1) * user_buf->buf_cnt;
    }

    timeStamp = (nsecs_t)(buf_info->buf->ts.tv_sec) *
            1000000000LL + buf_info->buf->ts.tv_nsec;
    if(!IS_BUFFER_ERROR(buf_info->buf->flags)) {
        if (timeStamp <= my_obj->prev_timestamp) {
            LOGE("TimeStamp received less than expected");
            mm_stream_qbuf(my_obj, buf_info->buf);
            return rc;
        } else if (my_obj->prev_timestamp == 0
               || (my_obj->prev_frameID != buf_info->buf->frame_idx + 1)) {
            /* For first frame or incase batch is droped */
            interval_nsec = ((my_obj->stream_info->user_buf_info.frameInterval) * 1000000);
            my_obj->prev_timestamp = (timeStamp - (nsecs_t)(user_buf->buf_cnt * interval_nsec));
        } else {
             ts_delta = timeStamp - my_obj->prev_timestamp;
             interval_nsec = (nsecs_t)(ts_delta / user_buf->buf_cnt);
             LOGD("Timestamp delta = %d timestamp = %lld", ts_delta, timeStamp);
        }
    }

    for (i = 0; i < (int32_t)user_buf->buf_cnt; i++) {
        buf_info->buf->user_buf.buf_idx[i] = user_buf->buf_idx[i];
        stream_buf = &my_obj->plane_buf[user_buf->buf_idx[i]];
        stream_buf->frame_idx = frameID + i;

        frame_ts  = (i * interval_nsec) + my_obj->prev_timestamp;

        stream_buf->ts.tv_sec  = (frame_ts / 1000000000LL);
        stream_buf->ts.tv_nsec = (frame_ts - (stream_buf->ts.tv_sec * 1000000000LL));
        stream_buf->is_uv_subsampled = buf_info->buf->is_uv_subsampled;

        LOGD("buf_index %d, frame_idx %d, stream type %d, timestamp = %lld",
                 stream_buf->buf_idx, stream_buf->frame_idx,
                my_obj->stream_info->stream_type, frame_ts);
    }

    buf_info->buf->ts.tv_sec  = (my_obj->prev_timestamp / 1000000000LL);
    buf_info->buf->ts.tv_nsec = (my_obj->prev_timestamp -
            (buf_info->buf->ts.tv_sec * 1000000000LL));

    buf_info->buf->user_buf.bufs_used = user_buf->buf_cnt;
    buf_info->buf->user_buf.buf_in_use = 1;

    my_obj->prev_timestamp = timeStamp;
    my_obj->prev_frameID = buf_info->buf->frame_idx;

    LOGD("X rc = %d",rc);
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_read_msm_frame
 *
 * DESCRIPTION: dequeue a stream buffer from kernel queue
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *   @buf_info     : ptr to a struct storing buffer information
 *   @num_planes   : number of planes in the buffer
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_read_msm_frame(mm_stream_t * my_obj,
                                 mm_camera_buf_info_t* buf_info,
                                 uint8_t num_planes)
{
    int32_t rc = 0;
    uint32_t buffer_type;
    struct v4l2_buffer vb;
    struct v4l2_plane planes[VIDEO_MAX_PLANES];
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);

    memset(&vb,  0,  sizeof(vb));
    vb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    vb.memory = V4L2_MEMORY_USERPTR;
    vb.m.planes = &planes[0];
    vb.length = num_planes;

    rc = ioctl(my_obj->fd, VIDIOC_DQBUF, &vb);
    if (0 > rc) {
        LOGE("VIDIOC_DQBUF ioctl call failed on stream type %d (rc=%d): %s",
             my_obj->stream_info->stream_type, rc, strerror(errno));
    } else {
        pthread_mutex_lock(&my_obj->buf_lock);
        my_obj->queued_buffer_count--;
        if (0 == my_obj->queued_buffer_count) {
            uint8_t idx = mm_camera_util_get_index_by_num(
                    my_obj->ch_obj->cam_obj->my_num, my_obj->my_hdl);
            LOGH("Remove Poll stream %p type: %d FD = %d",
                    my_obj, my_obj->stream_info->stream_type, my_obj->fd);
            mm_camera_poll_thread_del_poll_fd(&my_obj->ch_obj->poll_thread[0],
                    idx, my_obj->my_hdl, mm_camera_async_call);
        }
        pthread_mutex_unlock(&my_obj->buf_lock);
        uint32_t idx = vb.index;
        buf_info->buf = &my_obj->buf[idx];
        buf_info->frame_idx = vb.sequence;
        buf_info->stream_id = my_obj->my_hdl;

        buf_info->buf->stream_id = my_obj->my_hdl;
        buf_info->buf->buf_idx = idx;
        buf_info->buf->frame_idx = vb.sequence;
        buf_info->buf->ts.tv_sec  = vb.timestamp.tv_sec;
        buf_info->buf->ts.tv_nsec = vb.timestamp.tv_usec * 1000;
        buf_info->buf->flags = vb.flags;
        // Buffers are cleaned/invalidated when received by HAL
        // Cache ops not required on DQBUF
        buf_info->buf->cache_flags = 0;

        LOGH("VIDIOC_DQBUF buf_index %d, frame_idx %d, stream type %d, rc %d,"
                "queued: %d, buf_type = %d flags = %d FD = %d my_num %d buf fd %d",
                vb.index, buf_info->buf->frame_idx,
                my_obj->stream_info->stream_type, rc,
                my_obj->queued_buffer_count, buf_info->buf->buf_type,
                buf_info->buf->flags,
                my_obj->fd,
                my_obj->ch_obj->cam_obj->my_num,
                buf_info->buf->fd);

        #ifdef USE_KERNEL_VERSION_GE_4_4_DEFS
        buffer_type = vb.timecode.type;
        #else
        buffer_type = vb.reserved;
        #endif
        buf_info->buf->is_uv_subsampled =
            (buffer_type == V4L2_PIX_FMT_NV14 || buffer_type == V4L2_PIX_FMT_NV41);

        if(buf_info->buf->buf_type == CAM_STREAM_BUF_TYPE_USERPTR) {
            mm_stream_read_user_buf(my_obj, buf_info);
        }

        rc = mm_stream_handle_cache_ops(my_obj, buf_info->buf, TRUE);
        if (rc != 0) {
            LOGE("Error cleaning/invalidating the buffer");
        }
    }

    LOGD("X rc = %d",rc);
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_set_parms
 *
 * DESCRIPTION: set parameters per stream
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *   @in_value     : ptr to a param struct to be set to server
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 * NOTE       : Assume the parms struct buf is already mapped to server via
 *              domain socket. Corresponding fields of parameters to be set
 *              are already filled in by upper layer caller.
 *==========================================================================*/
int32_t mm_stream_set_parm(mm_stream_t *my_obj,
                           cam_stream_parm_buffer_t *in_value)
{
    int32_t rc = -1;
    int32_t value = 0;
    if (in_value != NULL) {
      mm_camera_obj_t *cam_obj = my_obj->ch_obj->cam_obj;
      int stream_id = my_obj->server_stream_id;
      rc = mm_camera_util_s_ctrl(cam_obj, stream_id, my_obj->fd,
              CAM_PRIV_STREAM_PARM, &value);
      if (rc < 0) {
        LOGE("Failed to set stream parameter type = %d", in_value->type);
      }
    }
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_get_parms
 *
 * DESCRIPTION: get parameters per stream
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *   @in_value     : ptr to a param struct to be get from server
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 * NOTE       : Assume the parms struct buf is already mapped to server via
 *              domain socket. Corresponding fields of parameters to be get
 *              are already filled in by upper layer caller.
 *==========================================================================*/
int32_t mm_stream_get_parm(mm_stream_t *my_obj,
                           cam_stream_parm_buffer_t *in_value)
{
    int32_t rc = -1;
    int32_t value = 0;
    if (in_value != NULL) {
        mm_camera_obj_t *cam_obj = my_obj->ch_obj->cam_obj;
        int stream_id = my_obj->server_stream_id;
        rc = mm_camera_util_g_ctrl(cam_obj, stream_id, my_obj->fd,
              CAM_PRIV_STREAM_PARM, &value);
    }
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_do_actions
 *
 * DESCRIPTION: request server to perform stream based actions
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *   @in_value     : ptr to a struct of actions to be performed by the server
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 * NOTE       : Assume the action struct buf is already mapped to server via
 *              domain socket. Corresponding fields of actions to be performed
 *              are already filled in by upper layer caller.
 *==========================================================================*/
int32_t mm_stream_do_action(mm_stream_t *my_obj,
                            void *in_value)
{
    int32_t rc = -1;
    int32_t value = 0;
    if (in_value != NULL) {
        mm_camera_obj_t *cam_obj = my_obj->ch_obj->cam_obj;
        int stream_id = my_obj->server_stream_id;
        rc = mm_camera_util_s_ctrl(cam_obj, stream_id, my_obj->fd,
              CAM_PRIV_STREAM_PARM, &value);
    }
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_set_ext_mode
 *
 * DESCRIPTION: set stream extended mode to server via v4l2 ioctl
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 * NOTE       : Server will return a server stream id that uniquely identify
 *              this stream on server side. Later on communication to server
 *              per stream should use this server stream id.
 *==========================================================================*/
int32_t mm_stream_set_ext_mode(mm_stream_t * my_obj)
{
    int32_t rc = 0;
    struct v4l2_streamparm s_parm;
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);

    memset(&s_parm, 0, sizeof(s_parm));
    s_parm.type =  V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;

    rc = ioctl(my_obj->fd, VIDIOC_S_PARM, &s_parm);
    LOGD("stream fd=%d, rc=%d, extended_mode=%d",
         my_obj->fd, rc, s_parm.parm.capture.extendedmode);
    if (rc == 0) {
        my_obj->server_stream_id = s_parm.parm.capture.extendedmode;
#ifndef DAEMON_PRESENT
        cam_shim_packet_t *shim_cmd;
        cam_shim_cmd_data shim_cmd_data;
        mm_camera_obj_t *cam_obj = my_obj->ch_obj->cam_obj;

        memset(&shim_cmd_data, 0, sizeof(shim_cmd_data));
        shim_cmd_data.command = MSM_CAMERA_PRIV_NEW_STREAM;
        shim_cmd_data.stream_id = my_obj->server_stream_id;
        shim_cmd_data.value = NULL;
        shim_cmd = mm_camera_create_shim_cmd_packet(CAM_SHIM_SET_PARM,
                cam_obj->sessionid, &shim_cmd_data);
        rc = mm_camera_module_send_cmd(shim_cmd);
        mm_camera_destroy_shim_cmd_packet(shim_cmd);
#endif /* DAEMON_PRESENT */
    } else {
        LOGE("VIDIOC_S_PARM  extendedmode error");
    }
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_qbuf
 *
 * DESCRIPTION: enqueue buffer back to kernel queue for furture use
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *   @buf          : ptr to a struct storing buffer information
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_qbuf(mm_stream_t *my_obj, mm_camera_buf_def_t *buf)
{
    int32_t rc = 0;
    uint32_t length = 0;
    struct v4l2_buffer buffer;
    struct v4l2_plane planes[VIDEO_MAX_PLANES];
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d, stream type = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state,
         my_obj->stream_info->stream_type);

    if (buf->buf_type == CAM_STREAM_BUF_TYPE_USERPTR) {
        LOGD("USERPTR num_buf = %d, idx = %d",
                buf->user_buf.bufs_used, buf->buf_idx);
        memset(&planes, 0, sizeof(planes));
        planes[0].length = my_obj->stream_info->user_buf_info.size;
        planes[0].m.userptr = buf->fd;
        length = 1;
    } else {
        memcpy(planes, buf->planes_buf.planes, sizeof(planes));
        length = buf->planes_buf.num_planes;
    }

    memset(&buffer, 0, sizeof(buffer));
    buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    buffer.memory = V4L2_MEMORY_USERPTR;
    buffer.index = (__u32)buf->buf_idx;
    buffer.m.planes = &planes[0];
    buffer.length = (__u32)length;

    rc = mm_stream_handle_cache_ops(my_obj, buf, FALSE);
    if (rc != 0) {
        LOGE("Error cleaning/invalidating the buffer");
    }
    pthread_mutex_lock(&my_obj->buf_lock);
    my_obj->queued_buffer_count++;
    if (1 == my_obj->queued_buffer_count) {
        uint8_t idx = mm_camera_util_get_index_by_num(
                my_obj->ch_obj->cam_obj->my_num, my_obj->my_hdl);
        /* Add fd to data poll thread */
        LOGH("Add Poll FD %p type: %d idx = %d num = %d fd = %d",
                my_obj,my_obj->stream_info->stream_type, idx,
                my_obj->ch_obj->cam_obj->my_num, my_obj->fd);
        rc = mm_camera_poll_thread_add_poll_fd(&my_obj->ch_obj->poll_thread[0],
                idx, my_obj->my_hdl, my_obj->fd, mm_stream_data_notify,
                (void*)my_obj, mm_camera_async_call);
        if (0 > rc) {
            LOGE("Add poll on stream %p type: %d fd error (rc=%d)",
                 my_obj, my_obj->stream_info->stream_type, rc);
        } else {
            LOGH("Started poll on stream %p type: %d",
                my_obj, my_obj->stream_info->stream_type);
        }
    }
    pthread_mutex_unlock(&my_obj->buf_lock);

    rc = ioctl(my_obj->fd, VIDIOC_QBUF, &buffer);
    pthread_mutex_lock(&my_obj->buf_lock);
    if (0 > rc) {
        LOGE("VIDIOC_QBUF ioctl call failed on stream type %d (rc=%d): %s",
             my_obj->stream_info->stream_type, rc, strerror(errno));
        my_obj->queued_buffer_count--;
        if (0 == my_obj->queued_buffer_count) {
            uint8_t idx = mm_camera_util_get_index_by_num(
                    my_obj->ch_obj->cam_obj->my_num, my_obj->my_hdl);
            /* Remove fd from data poll in case of failing
             * first buffer queuing attempt */
            LOGH("Stoping poll on stream %p type: %d",
                my_obj, my_obj->stream_info->stream_type);
            mm_camera_poll_thread_del_poll_fd(&my_obj->ch_obj->poll_thread[0],
                    idx, my_obj->my_hdl, mm_camera_async_call);
            LOGH("Stopped poll on stream %p type: %d",
                my_obj, my_obj->stream_info->stream_type);
        }
    } else {
        LOGH("VIDIOC_QBUF buf_index %d, frame_idx %d stream type %d, rc %d,"
                " queued: %d, buf_type = %d stream-FD = %d my_num %d buf fd: %d",
                buffer.index, buf->frame_idx, my_obj->stream_info->stream_type, rc,
                my_obj->queued_buffer_count, buf->buf_type, my_obj->fd,
                my_obj->ch_obj->cam_obj->my_num, buf->fd);
    }
    pthread_mutex_unlock(&my_obj->buf_lock);

    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_request_buf
 *
 * DESCRIPTION: This function let kernel know the amount of buffers need to
 *              be registered via v4l2 ioctl.
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_request_buf(mm_stream_t * my_obj)
{
    int32_t rc = 0;
    struct v4l2_requestbuffers bufreq;
    uint8_t buf_num = my_obj->total_buf_cnt;
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d buf_num = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state, buf_num);

    if(buf_num > MM_CAMERA_MAX_NUM_FRAMES) {
        LOGE("buf num %d > max limit %d\n",
                    buf_num, MM_CAMERA_MAX_NUM_FRAMES);
        return -1;
    }

    memset(&bufreq, 0, sizeof(bufreq));
    bufreq.count = buf_num;
    bufreq.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    bufreq.memory = V4L2_MEMORY_USERPTR;
    rc = ioctl(my_obj->fd, VIDIOC_REQBUFS, &bufreq);
    if (rc < 0) {
      LOGE("fd=%d, ioctl VIDIOC_REQBUFS failed: rc=%d, errno %d",
            my_obj->fd, rc, errno);
    }

    LOGD("X rc = %d",rc);
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_need_wait_for_mapping
 *
 * DESCRIPTION: Utility function to determine whether to wait for mapping
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *
 * RETURN     : int8_t whether wait is necessary
 *              0  -- no wait
 *              1 -- wait
 *==========================================================================*/
int8_t mm_stream_need_wait_for_mapping(mm_stream_t * my_obj)
{
    uint32_t i;
    int8_t ret = 0;

    for (i = my_obj->buf_idx; i < (my_obj->buf_idx + my_obj->buf_num); i++) {
        if ((my_obj->buf_status[i].map_status == 0)
                && (my_obj->buf_status[i].in_kernel)) {
            /*do not signal in case if any buffer is not mapped
              but queued to kernel.*/
            ret = 1;
        } else if (my_obj->buf_status[i].map_status < 0) {
            return 0;
        }
    }

    return ret;
}

/*===========================================================================
 * FUNCTION   : mm_stream_map_buf
 *
 * DESCRIPTION: mapping stream buffer via domain socket to server
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *   @buf_type     : type of buffer to be mapped. could be following values:
 *                   CAM_MAPPING_BUF_TYPE_STREAM_BUF
 *                   CAM_MAPPING_BUF_TYPE_STREAM_INFO
 *                   CAM_MAPPING_BUF_TYPE_OFFLINE_INPUT_BUF
 *   @frame_idx    : index of buffer within the stream buffers, only valid if
 *                   buf_type is CAM_MAPPING_BUF_TYPE_STREAM_BUF or
 *                   CAM_MAPPING_BUF_TYPE_OFFLINE_INPUT_BUF
 *   @plane_idx    : plane index. If all planes share the same fd,
 *                   plane_idx = -1; otherwise, plean_idx is the
 *                   index to plane (0..num_of_planes)
 *   @fd           : file descriptor of the buffer
 *   @size         : size of the buffer
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_map_buf(mm_stream_t *my_obj,
        uint8_t buf_type, uint32_t frame_idx,
        int32_t plane_idx, int32_t fd,
        size_t size, void *buffer)
{
    int32_t rc = 0;
    if (NULL == my_obj || NULL == my_obj->ch_obj || NULL == my_obj->ch_obj->cam_obj) {
        LOGE("NULL obj of stream/channel/camera");
        return -1;
    }

    cam_sock_packet_t packet;
    memset(&packet, 0, sizeof(cam_sock_packet_t));
    packet.msg_type = CAM_MAPPING_TYPE_FD_MAPPING;
    packet.payload.buf_map.type = buf_type;
    packet.payload.buf_map.fd = fd;
    packet.payload.buf_map.size = size;
    packet.payload.buf_map.stream_id = my_obj->server_stream_id;
    packet.payload.buf_map.frame_idx = frame_idx;
    packet.payload.buf_map.plane_idx = plane_idx;
    packet.payload.buf_map.buffer = buffer;
    LOGD("mapping buf_type %d, stream_id %d, frame_idx %d, fd %d, size %d",
             buf_type, my_obj->server_stream_id, frame_idx, fd, size);

#ifdef DAEMON_PRESENT
    rc = mm_camera_util_sendmsg(my_obj->ch_obj->cam_obj,
                                &packet, sizeof(cam_sock_packet_t), fd);
#else
    cam_shim_packet_t *shim_cmd;
    shim_cmd = mm_camera_create_shim_cmd_packet(CAM_SHIM_REG_BUF,
            my_obj->ch_obj->cam_obj->sessionid, &packet);
    rc = mm_camera_module_send_cmd(shim_cmd);
    mm_camera_destroy_shim_cmd_packet(shim_cmd);
#endif
    if ((buf_type == CAM_MAPPING_BUF_TYPE_STREAM_BUF)
            || ((buf_type
            == CAM_MAPPING_BUF_TYPE_STREAM_USER_BUF)
            && (my_obj->stream_info != NULL)
            && (my_obj->stream_info->streaming_mode
            == CAM_STREAMING_MODE_BATCH))) {
        pthread_mutex_lock(&my_obj->buf_lock);
        if (rc < 0) {
            my_obj->buf_status[frame_idx].map_status = -1;
            LOGE("fail status =%d", my_obj->buf_status[frame_idx].map_status);
        } else {
            my_obj->buf_status[frame_idx].map_status = 1;
        }
        if (mm_stream_need_wait_for_mapping(my_obj) == 0) {
            LOGD("Buffer mapping Done: Signal strm fd = %d",
                     my_obj->fd);
            pthread_cond_signal(&my_obj->buf_cond);
        }
        pthread_mutex_unlock(&my_obj->buf_lock);
    }
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_map_bufs
 *
 * DESCRIPTION: mapping stream buffers via domain socket to server
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *   @buf_map_list : list of buffer objects to map
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/

int32_t mm_stream_map_bufs(mm_stream_t * my_obj,
                           const cam_buf_map_type_list *buf_map_list)
{
    if (NULL == my_obj || NULL == my_obj->ch_obj || NULL == my_obj->ch_obj->cam_obj) {
        LOGE("NULL obj of stream/channel/camera");
        return -1;
    }

    cam_sock_packet_t packet;
    memset(&packet, 0, sizeof(cam_sock_packet_t));
    packet.msg_type = CAM_MAPPING_TYPE_FD_BUNDLED_MAPPING;

    memcpy(&packet.payload.buf_map_list, buf_map_list,
           sizeof(packet.payload.buf_map_list));

    int sendfds[CAM_MAX_NUM_BUFS_PER_STREAM];
    uint32_t numbufs = packet.payload.buf_map_list.length;
    if (numbufs < 1) {
      LOGD("No buffers, suppressing the mapping command");
      return 0;
    }

    uint32_t i;
    for (i = 0; i < numbufs; i++) {
        packet.payload.buf_map_list.buf_maps[i].stream_id = my_obj->server_stream_id;
        sendfds[i] = packet.payload.buf_map_list.buf_maps[i].fd;
    }

    for (i = numbufs; i < CAM_MAX_NUM_BUFS_PER_STREAM; i++) {
        packet.payload.buf_map_list.buf_maps[i].fd = -1;
        sendfds[i] = -1;
    }

#ifdef DAEMON_PRESENT
    int32_t ret = mm_camera_util_bundled_sendmsg(my_obj->ch_obj->cam_obj,
            &packet, sizeof(cam_sock_packet_t), sendfds, numbufs);
#else
    cam_shim_packet_t *shim_cmd;
    shim_cmd = mm_camera_create_shim_cmd_packet(CAM_SHIM_REG_BUF,
            my_obj->ch_obj->cam_obj->sessionid, &packet);
    int32_t ret = mm_camera_module_send_cmd(shim_cmd);
    mm_camera_destroy_shim_cmd_packet(shim_cmd);
#endif
    if ((numbufs > 0) && ((buf_map_list->buf_maps[0].type
            == CAM_MAPPING_BUF_TYPE_STREAM_BUF)
            || ((buf_map_list->buf_maps[0].type ==
            CAM_MAPPING_BUF_TYPE_STREAM_USER_BUF)
            && (my_obj->stream_info != NULL)
            && (my_obj->stream_info->streaming_mode
            == CAM_STREAMING_MODE_BATCH)))) {
        pthread_mutex_lock(&my_obj->buf_lock);
        for (i = 0; i < numbufs; i++) {
           if (ret < 0) {
               my_obj->buf_status[i].map_status = -1;
           } else {
               my_obj->buf_status[i].map_status = 1;
           }
        }

        if (mm_stream_need_wait_for_mapping(my_obj) == 0) {
            LOGD("Buffer mapping Done: Signal strm fd = %d",
                     my_obj->fd);
            pthread_cond_signal(&my_obj->buf_cond);
        }
        pthread_mutex_unlock(&my_obj->buf_lock);
    }
    return ret;
}

/*===========================================================================
 * FUNCTION   : mm_stream_unmap_buf
 *
 * DESCRIPTION: unmapping stream buffer via domain socket to server
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *   @buf_type     : type of buffer to be unmapped. could be following values:
 *                   CAM_MAPPING_BUF_TYPE_STREAM_BUF
 *                   CAM_MAPPING_BUF_TYPE_STREAM_INFO
 *                   CAM_MAPPING_BUF_TYPE_OFFLINE_INPUT_BUF
 *   @frame_idx    : index of buffer within the stream buffers, only valid if
 *                   buf_type is CAM_MAPPING_BUF_TYPE_STREAM_BUF or
 *                   CAM_MAPPING_BUF_TYPE_OFFLINE_INPUT_BUF
 *   @plane_idx    : plane index. If all planes share the same fd,
 *                   plane_idx = -1; otherwise, plean_idx is the
 *                   index to plane (0..num_of_planes)
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_unmap_buf(mm_stream_t * my_obj,
                            uint8_t buf_type,
                            uint32_t frame_idx,
                            int32_t plane_idx)
{
    int32_t ret;
    if (NULL == my_obj || NULL == my_obj->ch_obj || NULL == my_obj->ch_obj->cam_obj) {
        LOGE("NULL obj of stream/channel/camera");
        return -1;
    }
    cam_sock_packet_t packet;
    memset(&packet, 0, sizeof(cam_sock_packet_t));
    packet.msg_type = CAM_MAPPING_TYPE_FD_UNMAPPING;
    packet.payload.buf_unmap.type = buf_type;
    packet.payload.buf_unmap.stream_id = my_obj->server_stream_id;
    packet.payload.buf_unmap.frame_idx = frame_idx;
    packet.payload.buf_unmap.plane_idx = plane_idx;
#ifdef DAEMON_PRESENT
    ret = mm_camera_util_sendmsg(my_obj->ch_obj->cam_obj,
            &packet, sizeof(cam_sock_packet_t), -1);
#else
    cam_shim_packet_t *shim_cmd;
    shim_cmd = mm_camera_create_shim_cmd_packet(CAM_SHIM_REG_BUF,
            my_obj->ch_obj->cam_obj->sessionid, &packet);
    ret = mm_camera_module_send_cmd(shim_cmd);
    mm_camera_destroy_shim_cmd_packet(shim_cmd);
#endif
    if ((buf_type == CAM_MAPPING_BUF_TYPE_STREAM_BUF) ||
            (buf_type == CAM_MAPPING_BUF_TYPE_STREAM_USER_BUF)) {
        pthread_mutex_lock(&my_obj->buf_lock);
        my_obj->buf_status[frame_idx].map_status = 0;
        pthread_mutex_unlock(&my_obj->buf_lock);
    }
    return ret;
}

/*===========================================================================
 * FUNCTION   : mm_stream_init_bufs
 *
 * DESCRIPTION: initialize stream buffers needed. This function will request
 *              buffers needed from upper layer through the mem ops table passed
 *              during configuration stage.
 *
 * PARAMETERS :
 *   @my_obj  : stream object
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_init_bufs(mm_stream_t * my_obj)
{
    int32_t i, rc = 0;
    uint8_t *reg_flags = NULL;
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);

    /* deinit buf if it's not NULL*/
    if ((NULL != my_obj->buf) && (!my_obj->is_res_shared)) {
        mm_stream_deinit_bufs(my_obj);
    }

    if (!my_obj->is_res_shared) {
        rc = my_obj->mem_vtbl.get_bufs(&my_obj->frame_offset,
                &my_obj->total_buf_cnt, &reg_flags, &my_obj->buf,
                &my_obj->map_ops, my_obj->mem_vtbl.user_data);
        if (rc == 0) {
            for (i = 0; i < my_obj->total_buf_cnt; i++) {
                my_obj->buf_status[i].initial_reg_flag = reg_flags[i];
            }
            if ((my_obj->num_s_cnt != 0) && (my_obj->total_buf_cnt != 0)) {
                rc = mm_camera_muxer_get_stream_bufs(my_obj);
            }
        }
    }
    if (0 != rc || ((my_obj->buf_num > 0) && (NULL == my_obj->buf))) {
        LOGE("Error get buf, rc = %d\n", rc);
        rc =-1;
        return rc;
    }

    LOGH("Buffer count = %d buf id = %d",my_obj->buf_num, my_obj->buf_idx);
    for (i = my_obj->buf_idx; i < (my_obj->buf_idx + my_obj->buf_num); i++) {
        my_obj->buf[i].stream_id = my_obj->my_hdl;
        my_obj->buf[i].stream_type = my_obj->stream_info->stream_type;

        if (my_obj->buf[i].buf_type == CAM_STREAM_BUF_TYPE_USERPTR) {
            my_obj->buf[i].user_buf.bufs_used =
                    (int8_t)my_obj->stream_info->user_buf_info.frame_buf_cnt;
            if (reg_flags) {
                my_obj->buf[i].user_buf.buf_in_use = reg_flags[i];
            }
        }
    }

    if (my_obj->stream_info->streaming_mode == CAM_STREAMING_MODE_BATCH) {
        my_obj->plane_buf = my_obj->buf[0].user_buf.plane_buf;
        if (my_obj->plane_buf != NULL) {
            my_obj->plane_buf_num =
                    my_obj->buf_num *
                    my_obj->stream_info->user_buf_info.frame_buf_cnt;
            for (i = 0; i < my_obj->plane_buf_num; i++) {
                my_obj->plane_buf[i].stream_id = my_obj->my_hdl;
                my_obj->plane_buf[i].stream_type = my_obj->stream_info->stream_type;
            }
        }
        my_obj->cur_bufs_staged = 0;
        my_obj->cur_buf_idx = -1;
    }

    free(reg_flags);
    reg_flags = NULL;

    /* update in stream info about number of stream buffers */
    my_obj->stream_info->num_bufs = my_obj->total_buf_cnt;

    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_deinit_bufs
 *
 * DESCRIPTION: return stream buffers to upper layer through the mem ops table
 *              passed during configuration stage.
 *
 * PARAMETERS :
 *   @my_obj  : stream object
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_deinit_bufs(mm_stream_t * my_obj)
{
    int32_t rc = 0;
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);

    if (NULL == my_obj->buf) {
        LOGD("Buf is NULL, no need to deinit");
        return rc;
    }

    if ((!my_obj->is_res_shared) &&
            (my_obj->mem_vtbl.put_bufs != NULL)) {
        rc = my_obj->mem_vtbl.put_bufs(&my_obj->map_ops,
                my_obj->mem_vtbl.user_data);
        if (my_obj->plane_buf != NULL) {
            free(my_obj->plane_buf);
            my_obj->plane_buf = NULL;
        }

        free(my_obj->buf);
        my_obj->buf = NULL;
    } else {
        rc = mm_camera_muxer_put_stream_bufs(my_obj);
    }

    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_reg_buf
 *
 * DESCRIPTION: register buffers with kernel by calling v4l2 ioctl QBUF for
 *              each buffer in the stream
 *
 * PARAMETERS :
 *   @my_obj  : stream object
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_reg_buf(mm_stream_t * my_obj)
{
    int32_t rc = 0;
    uint8_t i;
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);

    rc = mm_stream_request_buf(my_obj);
    if (rc != 0) {
        return rc;
    }

    my_obj->queued_buffer_count = 0;
    for(i = my_obj->buf_idx; i < (my_obj->buf_idx + my_obj->buf_num); i++){
        /* check if need to qbuf initially */
        if (my_obj->buf_status[i].initial_reg_flag) {
            rc = mm_stream_qbuf(my_obj, &my_obj->buf[i]);
            if (rc != 0) {
                LOGE("VIDIOC_QBUF idx = %d rc = %d\n", i, rc);
                break;
            }
            my_obj->buf_status[i].buf_refcnt = 0;
            my_obj->buf_status[i].in_kernel = 1;
        } else {
            /* the buf is held by upper layer, will not queue into kernel.
             * add buf reference count */
            my_obj->buf_status[i].buf_refcnt = 1;
            my_obj->buf_status[i].in_kernel = 0;
        }
    }

    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_unreg buf
 *
 * DESCRIPTION: unregister all stream buffers from kernel
 *
 * PARAMETERS :
 *   @my_obj  : stream object
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_unreg_buf(mm_stream_t * my_obj)
{
    struct v4l2_requestbuffers bufreq;
    int32_t i, rc = 0;
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);

    /* unreg buf to kernel */
    bufreq.count = 0;
    bufreq.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    bufreq.memory = V4L2_MEMORY_USERPTR;
    rc = ioctl(my_obj->fd, VIDIOC_REQBUFS, &bufreq);
    if (rc < 0) {
        LOGE("fd=%d, VIDIOC_REQBUFS failed, rc=%d, errno %d",
               my_obj->fd, rc, errno);
    }

    /* reset buf reference count */
    pthread_mutex_lock(&my_obj->buf_lock);
    for(i = my_obj->buf_idx; i < (my_obj->buf_idx + my_obj->buf_num); i++){
        my_obj->buf_status[i].buf_refcnt = 0;
        my_obj->buf_status[i].in_kernel = 0;
    }
    pthread_mutex_unlock(&my_obj->buf_lock);

    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_get_v4l2_fmt
 *
 * DESCRIPTION: translate camera image format into FOURCC code
 *
 * PARAMETERS :
 *   @fmt     : camera image format
 *
 * RETURN     : FOURCC code for image format
 *==========================================================================*/
uint32_t mm_stream_get_v4l2_fmt(cam_format_t fmt)
{
    uint32_t val = 0;
    switch(fmt) {
    case CAM_FORMAT_YUV_420_NV12:
    case CAM_FORMAT_YUV_420_NV12_VENUS:
    case CAM_FORMAT_YUV_420_NV12_UBWC:
        val = V4L2_PIX_FMT_NV12;
        break;
    case CAM_FORMAT_YUV_420_NV21:
    case CAM_FORMAT_YUV_420_NV21_VENUS:
        val = V4L2_PIX_FMT_NV21;
        break;
    case CAM_FORMAT_BAYER_QCOM_RAW_10BPP_GBRG:
    case CAM_FORMAT_BAYER_MIPI_RAW_10BPP_GBRG:
        val= V4L2_PIX_FMT_SGBRG10;
        break;
    case CAM_FORMAT_BAYER_QCOM_RAW_10BPP_GRBG:
    case CAM_FORMAT_BAYER_MIPI_RAW_10BPP_GRBG:
        val= V4L2_PIX_FMT_SGRBG10;
        break;
    case CAM_FORMAT_BAYER_QCOM_RAW_10BPP_RGGB:
    case CAM_FORMAT_BAYER_MIPI_RAW_10BPP_RGGB:
        val= V4L2_PIX_FMT_SRGGB10;
        break;
    case CAM_FORMAT_BAYER_QCOM_RAW_10BPP_BGGR:
    case CAM_FORMAT_BAYER_MIPI_RAW_10BPP_BGGR:
        val= V4L2_PIX_FMT_SBGGR10;
        break;
    case CAM_FORMAT_BAYER_QCOM_RAW_12BPP_GBRG:
    case CAM_FORMAT_BAYER_MIPI_RAW_12BPP_GBRG:
        val= V4L2_PIX_FMT_SGBRG12;
        break;
    case CAM_FORMAT_BAYER_QCOM_RAW_12BPP_GRBG:
    case CAM_FORMAT_BAYER_MIPI_RAW_12BPP_GRBG:
        val= V4L2_PIX_FMT_SGRBG12;
        break;
    case CAM_FORMAT_BAYER_QCOM_RAW_12BPP_RGGB:
    case CAM_FORMAT_BAYER_MIPI_RAW_12BPP_RGGB:
        val= V4L2_PIX_FMT_SRGGB12;
        break;
    case CAM_FORMAT_BAYER_QCOM_RAW_12BPP_BGGR:
    case CAM_FORMAT_BAYER_MIPI_RAW_12BPP_BGGR:
        val = V4L2_PIX_FMT_SBGGR12;
        break;
    case CAM_FORMAT_BAYER_QCOM_RAW_14BPP_GBRG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_14BPP_GBRG:
        val= V4L2_PIX_FMT_SGBRG14;
        break;
    case CAM_FORMAT_BAYER_QCOM_RAW_14BPP_GRBG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_14BPP_GRBG:
        val= V4L2_PIX_FMT_SGRBG14;
        break;
    case CAM_FORMAT_BAYER_QCOM_RAW_14BPP_RGGB:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_14BPP_RGGB:
        val= V4L2_PIX_FMT_SRGGB14;
        break;
    case CAM_FORMAT_BAYER_QCOM_RAW_14BPP_BGGR:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_14BPP_BGGR:
        val = V4L2_PIX_FMT_SBGGR14;
        break;
    case CAM_FORMAT_YUV_422_NV61:
        val= V4L2_PIX_FMT_NV61;
        break;
    case CAM_FORMAT_YUV_RAW_8BIT_YUYV:
        val= V4L2_PIX_FMT_YUYV;
        break;
    case CAM_FORMAT_YUV_RAW_8BIT_YVYU:
        val= V4L2_PIX_FMT_YVYU;
        break;
    case CAM_FORMAT_YUV_RAW_8BIT_UYVY:
        val= V4L2_PIX_FMT_UYVY;
        break;
    case CAM_FORMAT_YUV_RAW_8BIT_VYUY:
        val= V4L2_PIX_FMT_VYUY;
        break;
    case CAM_FORMAT_YUV_420_YV12:
        val= V4L2_PIX_FMT_NV12;
        break;
    case CAM_FORMAT_YUV_422_NV16:
        val= V4L2_PIX_FMT_NV16;
        break;
    case CAM_FORMAT_Y_ONLY:
        val= V4L2_PIX_FMT_GREY;
        break;
    case CAM_FORMAT_Y_ONLY_10_BPP:
        val= V4L2_PIX_FMT_Y10;
        break;
    case CAM_FORMAT_Y_ONLY_12_BPP:
        val= V4L2_PIX_FMT_Y12;
        break;
    case CAM_FORMAT_Y_ONLY_14_BPP:
        /* No v4l2 format is defined yet for CAM_FORMAT_Y_ONLY_14_BPP */
        /* val= V4L2_PIX_FMT_Y14; */
        val = 0;
        LOGE("Unknown fmt=%d", fmt);
        break;
    case CAM_FORMAT_DEPTH16:
    case CAM_FORMAT_DEPTH8:
    case CAM_FORMAT_DEPTH_POINT_CLOUD:
    case CAM_FORMAT_MAX:
        /* CAM_STREAM_TYPE_DEFAULT,
         * CAM_STREAM_TYPE_OFFLINE_PROC,
         * and CAM_STREAM_TYPE_METADATA
         * set fmt to CAM_FORMAT_MAX*/
        val = 0;
        break;
    default:
        val = 0;
        LOGE("Unknown fmt=%d", fmt);
        break;
    }
    LOGD("fmt=%d, val =%d", fmt, val);
    return val;
}

/*===========================================================================
 * FUNCTION   : mm_stream_calc_offset_preview
 *
 * DESCRIPTION: calculate preview frame offset based on format and
 *              padding information
 *
 * PARAMETERS :
 *   @fmt     : image format
 *   @dim     : image dimension
 *   @buf_planes : [out] buffer plane information
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_calc_offset_preview(cam_stream_info_t *stream_info,
                                      cam_dimension_t *dim,
                                      cam_padding_info_t *padding,
                                      cam_stream_buf_plane_info_t *buf_planes)
{
    int32_t rc = 0;
    int stride = 0, scanline = 0;

    uint32_t width_padding = 0;
    uint32_t height_padding = 0;

    switch (stream_info->fmt) {
    case CAM_FORMAT_YUV_420_NV12:
    case CAM_FORMAT_YUV_420_NV21:
    case CAM_FORMAT_Y_ONLY:
    case CAM_FORMAT_Y_ONLY_10_BPP:
    case CAM_FORMAT_Y_ONLY_12_BPP:
    case CAM_FORMAT_Y_ONLY_14_BPP:
        /* 2 planes: Y + CbCr */
        buf_planes->plane_info.num_planes = 2;

        if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
            width_padding =  padding->width_padding;
            height_padding = CAM_PAD_TO_2;
        } else {
            width_padding =  padding->width_padding;
            height_padding = padding->height_padding;
        }

        stride = PAD_TO_SIZE(dim->width, width_padding);
        scanline = PAD_TO_SIZE(dim->height, height_padding);

        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].len = (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[0].offset_x = 0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;

        stride = PAD_TO_SIZE(dim->width, width_padding);
        scanline = PAD_TO_SIZE(dim->height / 2, height_padding);
        buf_planes->plane_info.mp[1].offset = 0;
        buf_planes->plane_info.mp[1].len =
            (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[1].offset_x = 0;
        buf_planes->plane_info.mp[1].offset_y = 0;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width;
        buf_planes->plane_info.mp[1].height = dim->height / 2;

        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len +
                        buf_planes->plane_info.mp[1].len,
                        CAM_PAD_TO_4K);
        break;
    case CAM_FORMAT_YUV_420_NV21_ADRENO:
        /* 2 planes: Y + CbCr */
        buf_planes->plane_info.num_planes = 2;

        if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
            stride = PAD_TO_SIZE(dim->width, CAM_PAD_TO_32);
            scanline = PAD_TO_SIZE(dim->height, CAM_PAD_TO_32);
        } else {
            stride = PAD_TO_SIZE(dim->width, padding->width_padding);
            scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
        }
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline), CAM_PAD_TO_4K);
        buf_planes->plane_info.mp[0].offset_x = 0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;

        stride = PAD_TO_SIZE(dim->width / 2, CAM_PAD_TO_32) * 2;
        scanline = PAD_TO_SIZE(dim->height / 2, CAM_PAD_TO_32);
        buf_planes->plane_info.mp[1].offset = 0;
        buf_planes->plane_info.mp[1].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline), CAM_PAD_TO_4K);
        buf_planes->plane_info.mp[1].offset_x = 0;
        buf_planes->plane_info.mp[1].offset_y = 0;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width;
        buf_planes->plane_info.mp[1].height = dim->height / 2;

        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len +
                        buf_planes->plane_info.mp[1].len,
                        CAM_PAD_TO_4K);
        break;
    case CAM_FORMAT_YUV_420_YV12:
        /* 3 planes: Y + Cr + Cb */
        buf_planes->plane_info.num_planes = 3;

        if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
            stride = PAD_TO_SIZE(dim->width, CAM_PAD_TO_16);
            scanline = PAD_TO_SIZE(dim->height, CAM_PAD_TO_2);
        } else {
            stride = PAD_TO_SIZE(dim->width, padding->width_padding);
            scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
        }
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].len = (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[0].offset_x = 0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;

        stride = PAD_TO_SIZE(stride / 2, CAM_PAD_TO_16);
        scanline = scanline / 2;
        buf_planes->plane_info.mp[1].offset = 0;
        buf_planes->plane_info.mp[1].len =
            (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[1].offset_x = 0;
        buf_planes->plane_info.mp[1].offset_y = 0;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width / 2;
        buf_planes->plane_info.mp[1].height = dim->height / 2;

        buf_planes->plane_info.mp[2].offset = 0;
        buf_planes->plane_info.mp[2].len =
            (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[2].offset_x = 0;
        buf_planes->plane_info.mp[2].offset_y = 0;
        buf_planes->plane_info.mp[2].stride = stride;
        buf_planes->plane_info.mp[2].scanline = scanline;
        buf_planes->plane_info.mp[2].width = dim->width / 2;
        buf_planes->plane_info.mp[2].height = dim->height / 2;

        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len +
                        buf_planes->plane_info.mp[1].len +
                        buf_planes->plane_info.mp[2].len,
                        CAM_PAD_TO_4K);
        break;
    case CAM_FORMAT_YUV_422_NV16:
    case CAM_FORMAT_YUV_422_NV61:
        /* 2 planes: Y + CbCr */
        buf_planes->plane_info.num_planes = 2;

        if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
            stride = PAD_TO_SIZE(dim->width, CAM_PAD_TO_16);
            scanline = dim->height;
        } else {
            stride = PAD_TO_SIZE(dim->width, padding->width_padding);
            scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
        }
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].len = (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[0].offset_x = 0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;

        buf_planes->plane_info.mp[1].offset = 0;
        buf_planes->plane_info.mp[1].len = (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[1].offset_x = 0;
        buf_planes->plane_info.mp[1].offset_y = 0;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width;
        buf_planes->plane_info.mp[1].height = dim->height;

        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len +
                        buf_planes->plane_info.mp[1].len,
                        CAM_PAD_TO_4K);
        break;
    case CAM_FORMAT_YUV_420_NV12_VENUS:
#ifdef VENUS_PRESENT
        // using Venus
        if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
            stride = VENUS_Y_STRIDE(COLOR_FMT_NV12, dim->width);
            scanline = VENUS_Y_SCANLINES(COLOR_FMT_NV12, dim->height);
        } else {
            stride = PAD_TO_SIZE(dim->width, padding->width_padding);
            scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
        }
        buf_planes->plane_info.frame_len =
                VENUS_BUFFER_SIZE(COLOR_FMT_NV12, stride, scanline);
        buf_planes->plane_info.num_planes = 2;
        buf_planes->plane_info.mp[0].len = (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].offset_x =0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;
        if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
            stride = VENUS_UV_STRIDE(COLOR_FMT_NV12, dim->width);
            scanline = VENUS_UV_SCANLINES(COLOR_FMT_NV12, dim->height);
        } else {
            stride = PAD_TO_SIZE(dim->width, padding->width_padding);
            scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
        }
        buf_planes->plane_info.mp[1].len =
                buf_planes->plane_info.frame_len - buf_planes->plane_info.mp[0].len;
        buf_planes->plane_info.mp[1].offset = 0;
        buf_planes->plane_info.mp[1].offset_x =0;
        buf_planes->plane_info.mp[1].offset_y = 0;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width;
        buf_planes->plane_info.mp[1].height = dim->height / 2;
#else
        LOGE("Venus hardware not avail, cannot use this format");
        rc = -1;
#endif
        break;
    case CAM_FORMAT_YUV_420_NV21_VENUS:
#ifdef VENUS_PRESENT
        // using Venus
        if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
            stride = VENUS_Y_STRIDE(COLOR_FMT_NV21, dim->width);
            scanline = VENUS_Y_SCANLINES(COLOR_FMT_NV21, dim->height);
        } else {
            stride = PAD_TO_SIZE(dim->width, padding->width_padding);
            scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
        }
        buf_planes->plane_info.frame_len =
                VENUS_BUFFER_SIZE(COLOR_FMT_NV21, stride, scanline);
        buf_planes->plane_info.num_planes = 2;
        buf_planes->plane_info.mp[0].len = (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].offset_x =0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;
        if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
            stride = VENUS_UV_STRIDE(COLOR_FMT_NV21, dim->width);
            scanline = VENUS_UV_SCANLINES(COLOR_FMT_NV21, dim->height);
        } else {
            stride = PAD_TO_SIZE(dim->width, padding->width_padding);
            scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
        }
        buf_planes->plane_info.mp[1].len =
                buf_planes->plane_info.frame_len - buf_planes->plane_info.mp[0].len;
        buf_planes->plane_info.mp[1].offset = 0;
        buf_planes->plane_info.mp[1].offset_x =0;
        buf_planes->plane_info.mp[1].offset_y = 0;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width;
        buf_planes->plane_info.mp[1].height = dim->height / 2;
#else
        LOGE("Venus hardware not avail, cannot use this format");
        rc = -1;
#endif
        break;
    case CAM_FORMAT_YUV_420_NV12_UBWC:
#ifdef UBWC_PRESENT
        {
            int meta_stride = 0,meta_scanline = 0;
            // using UBWC
            if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
                stride = VENUS_Y_STRIDE(COLOR_FMT_NV12_UBWC, dim->width);
                scanline = VENUS_Y_SCANLINES(COLOR_FMT_NV12_UBWC, dim->height);
            } else {
                stride = PAD_TO_SIZE(dim->width, padding->width_padding);
                scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
            }
            meta_stride = VENUS_Y_META_STRIDE(COLOR_FMT_NV12_UBWC, dim->width);
            meta_scanline = VENUS_Y_META_SCANLINES(COLOR_FMT_NV12_UBWC, dim->height);

            buf_planes->plane_info.frame_len =
                    VENUS_BUFFER_SIZE(COLOR_FMT_NV12_UBWC, stride, scanline);
            buf_planes->plane_info.num_planes = 2;
            buf_planes->plane_info.mp[0].offset = 0;
            buf_planes->plane_info.mp[0].offset_x =0;
            buf_planes->plane_info.mp[0].offset_y = 0;
            buf_planes->plane_info.mp[0].stride = stride;
            buf_planes->plane_info.mp[0].scanline = scanline;
            buf_planes->plane_info.mp[0].width = dim->width;
            buf_planes->plane_info.mp[0].height = dim->height;
            buf_planes->plane_info.mp[0].meta_stride = meta_stride;
            buf_planes->plane_info.mp[0].meta_scanline = meta_scanline;
            buf_planes->plane_info.mp[0].meta_len =
                    MSM_MEDIA_ALIGN(meta_stride * meta_scanline, 4096);
            buf_planes->plane_info.mp[0].len =
                    (uint32_t)(MSM_MEDIA_ALIGN((stride * scanline), 4096) +
                    (buf_planes->plane_info.mp[0].meta_len));

            stride = VENUS_UV_STRIDE(COLOR_FMT_NV12_UBWC, dim->width);
            scanline = VENUS_UV_SCANLINES(COLOR_FMT_NV12_UBWC, dim->height);
            meta_stride = VENUS_UV_META_STRIDE(COLOR_FMT_NV12_UBWC, dim->width);
            meta_scanline = VENUS_UV_META_SCANLINES(COLOR_FMT_NV12_UBWC, dim->height);
            buf_planes->plane_info.mp[1].offset = 0;
            buf_planes->plane_info.mp[1].offset_x =0;
            buf_planes->plane_info.mp[1].offset_y = 0;
            buf_planes->plane_info.mp[1].stride = stride;
            buf_planes->plane_info.mp[1].scanline = scanline;
            buf_planes->plane_info.mp[1].width = dim->width;
            buf_planes->plane_info.mp[1].height = dim->height/2;
            buf_planes->plane_info.mp[1].meta_stride = meta_stride;
            buf_planes->plane_info.mp[1].meta_scanline = meta_scanline;
            buf_planes->plane_info.mp[1].meta_len =
                    MSM_MEDIA_ALIGN(meta_stride * meta_scanline, 4096);
            buf_planes->plane_info.mp[1].len =
                    buf_planes->plane_info.frame_len - buf_planes->plane_info.mp[0].len;
        }
#else
        LOGE("UBWC hardware not avail, cannot use this format");
        rc = -1;
#endif
        break;

    default:
        LOGE("Invalid cam_format for preview %d",
                    stream_info->fmt);
        rc = -1;
        break;
    }

    return rc;
}
/*===========================================================================
 * FUNCTION   : mm_stream_calc_offset_post_view
 *
 * DESCRIPTION: calculate postview frame offset based on format and
 *              padding information
 *
 * PARAMETERS :
 *   @stream_info  : Stream information
 *   @dim     : image dimension
 *   @padding : Padding info
 *   @buf_planes : [out] buffer plane information
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_calc_offset_post_view(cam_stream_info_t *stream_info,
                                      cam_dimension_t *dim,
                                      cam_padding_info_t *padding,
                                      cam_stream_buf_plane_info_t *buf_planes)
{
    int32_t rc = 0;
    int stride = 0, scanline = 0;

    uint32_t width_padding = 0;
    uint32_t height_padding = 0;

    switch (stream_info->fmt) {
    case CAM_FORMAT_YUV_420_NV12:
    case CAM_FORMAT_YUV_420_NV21:
    case CAM_FORMAT_Y_ONLY:
    case CAM_FORMAT_Y_ONLY_10_BPP:
    case CAM_FORMAT_Y_ONLY_12_BPP:
    case CAM_FORMAT_Y_ONLY_14_BPP:
        /* 2 planes: Y + CbCr */
        buf_planes->plane_info.num_planes = 2;

        if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
            width_padding =  padding->width_padding;
            height_padding = CAM_PAD_TO_2;
        } else {
            width_padding =  padding->width_padding;
            height_padding = padding->height_padding;
        }

        stride = PAD_TO_SIZE(dim->width, width_padding);
        scanline = PAD_TO_SIZE(dim->height, height_padding);
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].len = (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[0].offset_x = 0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;

        stride = PAD_TO_SIZE(dim->width, width_padding);
        scanline = PAD_TO_SIZE(dim->height / 2, height_padding);
        buf_planes->plane_info.mp[1].offset = 0;
        buf_planes->plane_info.mp[1].len =
            (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[1].offset_x = 0;
        buf_planes->plane_info.mp[1].offset_y = 0;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width;
        buf_planes->plane_info.mp[1].height = dim->height / 2;

        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len +
                        buf_planes->plane_info.mp[1].len,
                        CAM_PAD_TO_4K);
        break;
    case CAM_FORMAT_YUV_420_NV21_ADRENO:
        /* 2 planes: Y + CbCr */
        buf_planes->plane_info.num_planes = 2;

        if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
            width_padding =  CAM_PAD_TO_32;
            height_padding = CAM_PAD_TO_32;
        } else {
            width_padding =  padding->width_padding;
            height_padding = padding->height_padding;
        }

        stride = PAD_TO_SIZE(dim->width, width_padding);
        scanline = PAD_TO_SIZE(dim->height, height_padding);
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline), CAM_PAD_TO_4K);
        buf_planes->plane_info.mp[0].offset_x = 0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;

        stride = PAD_TO_SIZE(dim->width / 2, width_padding) * 2;
        scanline = PAD_TO_SIZE(dim->height / 2, height_padding);
        buf_planes->plane_info.mp[1].offset = 0;
        buf_planes->plane_info.mp[1].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline), CAM_PAD_TO_4K);
        buf_planes->plane_info.mp[1].offset_x = 0;
        buf_planes->plane_info.mp[1].offset_y = 0;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width;
        buf_planes->plane_info.mp[1].height = dim->height / 2;

        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len +
                        buf_planes->plane_info.mp[1].len,
                        CAM_PAD_TO_4K);
        break;
    case CAM_FORMAT_YUV_420_YV12:
        /* 3 planes: Y + Cr + Cb */
        buf_planes->plane_info.num_planes = 3;

        if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
            width_padding =  CAM_PAD_TO_16;
            height_padding = CAM_PAD_TO_2;
        } else {
            width_padding =  padding->width_padding;
            height_padding = padding->height_padding;
        }

        stride = PAD_TO_SIZE(dim->width, width_padding);
        scanline = PAD_TO_SIZE(dim->height, height_padding);
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].len = (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[0].offset_x = 0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;

        stride = PAD_TO_SIZE(stride / 2, width_padding);
        scanline = PAD_TO_SIZE(stride / 2, height_padding);
        buf_planes->plane_info.mp[1].offset = 0;
        buf_planes->plane_info.mp[1].len =
            (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[1].offset_x = 0;
        buf_planes->plane_info.mp[1].offset_y = 0;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width / 2;
        buf_planes->plane_info.mp[1].height = dim->height / 2;

        buf_planes->plane_info.mp[2].offset = 0;
        buf_planes->plane_info.mp[2].len =
            (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[2].offset_x = 0;
        buf_planes->plane_info.mp[2].offset_y = 0;
        buf_planes->plane_info.mp[2].stride = stride;
        buf_planes->plane_info.mp[2].scanline = scanline;
        buf_planes->plane_info.mp[2].width = dim->width / 2;
        buf_planes->plane_info.mp[2].height = dim->height / 2;

        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len +
                        buf_planes->plane_info.mp[1].len +
                        buf_planes->plane_info.mp[2].len,
                        CAM_PAD_TO_4K);
        break;
    case CAM_FORMAT_YUV_422_NV16:
    case CAM_FORMAT_YUV_422_NV61:
        /* 2 planes: Y + CbCr */
        buf_planes->plane_info.num_planes = 2;

        if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
            stride = PAD_TO_SIZE(dim->width, CAM_PAD_TO_16);
            scanline = dim->height;
        } else {
            stride = PAD_TO_SIZE(dim->width, padding->width_padding);
            scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
        }
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].len = (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[0].offset_x = 0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;

        buf_planes->plane_info.mp[1].offset = 0;
        buf_planes->plane_info.mp[1].len = (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[1].offset_x = 0;
        buf_planes->plane_info.mp[1].offset_y = 0;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width;
        buf_planes->plane_info.mp[1].height = dim->height;

        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len +
                        buf_planes->plane_info.mp[1].len,
                        CAM_PAD_TO_4K);
        break;
    case CAM_FORMAT_YUV_420_NV12_VENUS:
#ifdef VENUS_PRESENT
        // using Venus
        if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
            stride = VENUS_Y_STRIDE(COLOR_FMT_NV12, dim->width);
            scanline = VENUS_Y_SCANLINES(COLOR_FMT_NV12, dim->height);
        } else {
            stride = PAD_TO_SIZE(dim->width, padding->width_padding);
            scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
        }

        buf_planes->plane_info.frame_len =
            VENUS_BUFFER_SIZE(COLOR_FMT_NV12, stride, scanline);
        buf_planes->plane_info.num_planes = 2;
        buf_planes->plane_info.mp[0].len = (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].offset_x =0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;
        if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
            stride = VENUS_UV_STRIDE(COLOR_FMT_NV12, dim->width);
            scanline = VENUS_UV_SCANLINES(COLOR_FMT_NV12, dim->height);
        } else {
            stride = PAD_TO_SIZE(dim->width, padding->width_padding);
            scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
        }
        buf_planes->plane_info.mp[1].len =
            buf_planes->plane_info.frame_len - buf_planes->plane_info.mp[0].len;
        buf_planes->plane_info.mp[1].offset = 0;
        buf_planes->plane_info.mp[1].offset_x =0;
        buf_planes->plane_info.mp[1].offset_y = 0;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width;
        buf_planes->plane_info.mp[1].height = dim->height / 2;
#else
        LOGE("Venus hardware not avail, cannot use this format");
        rc = -1;
#endif
        break;
    case CAM_FORMAT_YUV_420_NV21_VENUS:
#ifdef VENUS_PRESENT
        // using Venus
        if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
            stride = VENUS_Y_STRIDE(COLOR_FMT_NV21, dim->width);
            scanline = VENUS_Y_SCANLINES(COLOR_FMT_NV21, dim->height);
        } else {
            stride = PAD_TO_SIZE(dim->width, padding->width_padding);
            scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
        }
        buf_planes->plane_info.frame_len =
                VENUS_BUFFER_SIZE(COLOR_FMT_NV21, dim->width, dim->height);
        buf_planes->plane_info.num_planes = 2;
        buf_planes->plane_info.mp[0].len = (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].offset_x =0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;
        if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
            stride = VENUS_UV_STRIDE(COLOR_FMT_NV21, dim->width);
            scanline = VENUS_UV_SCANLINES(COLOR_FMT_NV21, dim->height);
        } else {
            stride = PAD_TO_SIZE(dim->width, padding->width_padding);
            scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
        }
        buf_planes->plane_info.mp[1].len =
                buf_planes->plane_info.frame_len - buf_planes->plane_info.mp[0].len;
        buf_planes->plane_info.mp[1].offset = 0;
        buf_planes->plane_info.mp[1].offset_x =0;
        buf_planes->plane_info.mp[1].offset_y = 0;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width;
        buf_planes->plane_info.mp[1].height = dim->height / 2;
#else
        LOGE("Venus hardware not avail, cannot use this format");
        rc = -1;
#endif
        break;
    case CAM_FORMAT_YUV_420_NV12_UBWC:
#ifdef UBWC_PRESENT
        {
            int meta_stride = 0,meta_scanline = 0;
            // using UBWC
            if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
                stride = VENUS_Y_STRIDE(COLOR_FMT_NV12_UBWC, dim->width);
                scanline = VENUS_Y_SCANLINES(COLOR_FMT_NV12_UBWC, dim->height);
            } else {
                stride = PAD_TO_SIZE(dim->width, padding->width_padding);
                scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
            }
            meta_stride = VENUS_Y_META_STRIDE(COLOR_FMT_NV12_UBWC, dim->width);
            meta_scanline = VENUS_Y_META_SCANLINES(COLOR_FMT_NV12_UBWC, dim->height);

            buf_planes->plane_info.frame_len =
                    VENUS_BUFFER_SIZE(COLOR_FMT_NV12_UBWC, dim->width, dim->height);
            buf_planes->plane_info.num_planes = 2;
            buf_planes->plane_info.mp[0].offset = 0;
            buf_planes->plane_info.mp[0].offset_x =0;
            buf_planes->plane_info.mp[0].offset_y = 0;
            buf_planes->plane_info.mp[0].stride = stride;
            buf_planes->plane_info.mp[0].scanline = scanline;
            buf_planes->plane_info.mp[0].width = dim->width;
            buf_planes->plane_info.mp[0].height = dim->height;
            buf_planes->plane_info.mp[0].meta_stride = meta_stride;
            buf_planes->plane_info.mp[0].meta_scanline = meta_scanline;
            buf_planes->plane_info.mp[0].meta_len =
                    MSM_MEDIA_ALIGN(meta_stride * meta_scanline, 4096);
            buf_planes->plane_info.mp[0].len =
                    (uint32_t)(MSM_MEDIA_ALIGN((stride * scanline), 4096) +
                    (buf_planes->plane_info.mp[0].meta_len));

            stride = VENUS_UV_STRIDE(COLOR_FMT_NV12_UBWC, dim->width);
            scanline = VENUS_UV_SCANLINES(COLOR_FMT_NV12_UBWC, dim->height);
            meta_stride = VENUS_UV_META_STRIDE(COLOR_FMT_NV12_UBWC, dim->width);
            meta_scanline = VENUS_UV_META_SCANLINES(COLOR_FMT_NV12_UBWC, dim->height);
            buf_planes->plane_info.mp[1].offset = 0;
            buf_planes->plane_info.mp[1].offset_x =0;
            buf_planes->plane_info.mp[1].offset_y = 0;
            buf_planes->plane_info.mp[1].stride = stride;
            buf_planes->plane_info.mp[1].scanline = scanline;
            buf_planes->plane_info.mp[1].width = dim->width;
            buf_planes->plane_info.mp[1].height = dim->height/2;
            buf_planes->plane_info.mp[1].meta_stride = meta_stride;
            buf_planes->plane_info.mp[1].meta_scanline = meta_scanline;
            buf_planes->plane_info.mp[1].meta_len =
                    MSM_MEDIA_ALIGN(meta_stride * meta_scanline, 4096);
            buf_planes->plane_info.mp[1].len =
                    buf_planes->plane_info.frame_len - buf_planes->plane_info.mp[0].len;
        }
#else
        LOGE("UBWC hardware not avail, cannot use this format");
        rc = -1;
#endif
        break;
    default:
        LOGE("Invalid cam_format for preview %d",
                    stream_info->fmt);
        rc = -1;
        break;
    }

    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_calc_offset_snapshot
 *
 * DESCRIPTION: calculate snapshot/postproc frame offset based on format and
 *              padding information
 *
 * PARAMETERS :
 *   @fmt     : image format
 *   @dim     : image dimension
 *   @padding : padding information
 *   @buf_planes : [out] buffer plane information
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_calc_offset_snapshot(cam_format_t fmt,
                                       cam_dimension_t *dim,
                                       __unused cam_stream_type_t type,
                                       cam_padding_info_t *padding,
                                       cam_stream_buf_plane_info_t *buf_planes)
{
    int32_t rc = 0;
    uint8_t isAFamily = mm_camera_util_chip_is_a_family();
    int offset_x = 0, offset_y = 0;
    int stride = 0, scanline = 0;

    if (isAFamily) {
        stride = dim->width;
        scanline = PAD_TO_SIZE(dim->height, CAM_PAD_TO_16);
        offset_x = 0;
        offset_y = scanline - dim->height;
        scanline += offset_y; /* double padding */
    } else {
        offset_x = PAD_TO_SIZE(padding->offset_info.offset_x,
                padding->plane_padding);
        offset_y = PAD_TO_SIZE(padding->offset_info.offset_y,
                padding->plane_padding);
        stride = PAD_TO_SIZE((dim->width +
                (2 * offset_x)), padding->width_padding);
        scanline = PAD_TO_SIZE((dim->height +
                (2 * offset_y)), padding->height_padding);
    }

    switch (fmt) {
    case CAM_FORMAT_YUV_420_NV12:
    case CAM_FORMAT_YUV_420_NV21:
    case CAM_FORMAT_Y_ONLY:
    case CAM_FORMAT_Y_ONLY_10_BPP:
    case CAM_FORMAT_Y_ONLY_12_BPP:
    case CAM_FORMAT_Y_ONLY_14_BPP:
        /* 2 planes: Y + CbCr */
        buf_planes->plane_info.num_planes = 2;
        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline),
                padding->plane_padding);
        buf_planes->plane_info.mp[0].offset =
                PAD_TO_SIZE((uint32_t)(offset_x + stride * offset_y),
                padding->plane_padding);
        buf_planes->plane_info.mp[0].offset_x = offset_x;
        buf_planes->plane_info.mp[0].offset_y = offset_y;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;

        scanline = scanline/2;
        buf_planes->plane_info.mp[1].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline),
                padding->plane_padding);
        buf_planes->plane_info.mp[1].offset =
                PAD_TO_SIZE((uint32_t)(offset_x + stride * offset_y),
                padding->plane_padding);
        buf_planes->plane_info.mp[1].offset_x = offset_x;
        buf_planes->plane_info.mp[1].offset_y = offset_y;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width;
        buf_planes->plane_info.mp[1].height = dim->height / 2;

        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len +
                buf_planes->plane_info.mp[1].len,
                CAM_PAD_TO_4K);
        break;
    case CAM_FORMAT_YUV_420_YV12:
        /* 3 planes: Y + Cr + Cb */
        buf_planes->plane_info.num_planes = 3;

        buf_planes->plane_info.mp[0].offset =
                PAD_TO_SIZE((uint32_t)(offset_x + stride * offset_y),
                        padding->plane_padding);
        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline),
                        padding->plane_padding);
        buf_planes->plane_info.mp[0].offset_x = offset_x;
        buf_planes->plane_info.mp[0].offset_y = offset_y;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;

        stride = PAD_TO_SIZE(stride / 2, CAM_PAD_TO_16);
        scanline = scanline / 2;
        buf_planes->plane_info.mp[1].offset =
                PAD_TO_SIZE((uint32_t)(offset_x + stride * offset_y),
                        padding->plane_padding);
        buf_planes->plane_info.mp[1].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline),
                        padding->plane_padding);
        buf_planes->plane_info.mp[1].offset_x = offset_x;
        buf_planes->plane_info.mp[1].offset_y = offset_y;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width / 2;
        buf_planes->plane_info.mp[1].height = dim->height / 2;

        buf_planes->plane_info.mp[2].offset =
                PAD_TO_SIZE((uint32_t)(offset_x + stride * offset_y),
                        padding->plane_padding);
        buf_planes->plane_info.mp[2].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline),
                        padding->plane_padding);
        buf_planes->plane_info.mp[2].offset_x = offset_x;
        buf_planes->plane_info.mp[2].offset_y = offset_y;
        buf_planes->plane_info.mp[2].stride = stride;
        buf_planes->plane_info.mp[2].scanline = scanline;
        buf_planes->plane_info.mp[2].width = dim->width / 2;
        buf_planes->plane_info.mp[2].height = dim->height / 2;

        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len +
                        buf_planes->plane_info.mp[1].len +
                        buf_planes->plane_info.mp[2].len,
                        CAM_PAD_TO_4K);
        break;
    case CAM_FORMAT_YUV_422_NV16:
    case CAM_FORMAT_YUV_422_NV61:
        /* 2 planes: Y + CbCr */
        buf_planes->plane_info.num_planes = 2;
        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline),
                        padding->plane_padding);
        buf_planes->plane_info.mp[0].offset =
                PAD_TO_SIZE((uint32_t)(offset_x + stride * offset_y),
                        padding->plane_padding);
        buf_planes->plane_info.mp[0].offset_x = offset_x;
        buf_planes->plane_info.mp[0].offset_y = offset_y;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;

        buf_planes->plane_info.mp[1].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline),
                        padding->plane_padding);
        buf_planes->plane_info.mp[1].offset =
                PAD_TO_SIZE((uint32_t)(offset_x + stride * offset_y),
                        padding->plane_padding);
        buf_planes->plane_info.mp[1].offset_x = offset_x;
        buf_planes->plane_info.mp[1].offset_y = offset_y;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width;
        buf_planes->plane_info.mp[1].height = dim->height;

        buf_planes->plane_info.frame_len = PAD_TO_SIZE(
            buf_planes->plane_info.mp[0].len + buf_planes->plane_info.mp[1].len,
            CAM_PAD_TO_4K);
        break;
    case CAM_FORMAT_YUV_420_NV12_UBWC:

#ifdef UBWC_PRESENT
        {
            int meta_stride = 0,meta_scanline = 0;
            // using UBWC
            stride = VENUS_Y_STRIDE(COLOR_FMT_NV12_UBWC, dim->width);
            scanline = VENUS_Y_SCANLINES(COLOR_FMT_NV12_UBWC, dim->height);
            meta_stride = VENUS_Y_META_STRIDE(COLOR_FMT_NV12_UBWC, dim->width);
            meta_scanline = VENUS_Y_META_SCANLINES(COLOR_FMT_NV12_UBWC, dim->height);

            buf_planes->plane_info.frame_len =
                    VENUS_BUFFER_SIZE(COLOR_FMT_NV12_UBWC, dim->width, dim->height);
            buf_planes->plane_info.num_planes = 2;
            buf_planes->plane_info.mp[0].offset = 0;
            buf_planes->plane_info.mp[0].offset_x = 0;
            buf_planes->plane_info.mp[0].offset_y = 0;
            buf_planes->plane_info.mp[0].stride = stride;
            buf_planes->plane_info.mp[0].scanline = scanline;
            buf_planes->plane_info.mp[0].width = dim->width;
            buf_planes->plane_info.mp[0].height = dim->height;
            buf_planes->plane_info.mp[0].meta_stride = meta_stride;
            buf_planes->plane_info.mp[0].meta_scanline = meta_scanline;
            buf_planes->plane_info.mp[0].meta_len =
                    MSM_MEDIA_ALIGN(meta_stride * meta_scanline, 4096);
            buf_planes->plane_info.mp[0].len =
                    (uint32_t)(MSM_MEDIA_ALIGN((stride * scanline), 4096) +
                    (buf_planes->plane_info.mp[0].meta_len));

            stride = VENUS_UV_STRIDE(COLOR_FMT_NV12_UBWC, dim->width);
            scanline = VENUS_UV_SCANLINES(COLOR_FMT_NV12_UBWC, dim->height);
            meta_stride = VENUS_UV_META_STRIDE(COLOR_FMT_NV12_UBWC, dim->width);
            meta_scanline = VENUS_UV_META_SCANLINES(COLOR_FMT_NV12_UBWC, dim->height);
            buf_planes->plane_info.mp[1].offset = 0;
            buf_planes->plane_info.mp[1].offset_x =0;
            buf_planes->plane_info.mp[1].offset_y = 0;
            buf_planes->plane_info.mp[1].stride = stride;
            buf_planes->plane_info.mp[1].scanline = scanline;
            buf_planes->plane_info.mp[1].width = dim->width;
            buf_planes->plane_info.mp[1].height = dim->height/2;
            buf_planes->plane_info.mp[1].meta_stride = meta_stride;
            buf_planes->plane_info.mp[1].meta_scanline = meta_scanline;
            buf_planes->plane_info.mp[1].meta_len =
                    MSM_MEDIA_ALIGN(meta_stride * meta_scanline, 4096);
            buf_planes->plane_info.mp[1].len =
                    buf_planes->plane_info.frame_len - buf_planes->plane_info.mp[0].len;
        }
#else
        LOGE("UBWC hardware not avail, cannot use this format");
        rc = -1;
#endif
        break;
    case CAM_FORMAT_YUV_420_NV12_VENUS:
#ifdef VENUS_PRESENT
        // using Venus
        if(type != CAM_STREAM_TYPE_OFFLINE_PROC)
        {
            if(IS_USAGE_HEIF(padding->usage))
            {
#ifdef COLOR_FMT_NV12_512
                stride = VENUS_Y_STRIDE(COLOR_FMT_NV12_512, dim->width);
                scanline = VENUS_Y_SCANLINES(COLOR_FMT_NV12_512, dim->height);
#else
                stride = PAD_TO_SIZE(dim->width, padding->width_padding);
                scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
#endif //COLOR_FMT_NV12_512
            }
            else {
                stride = VENUS_Y_STRIDE(COLOR_FMT_NV12, dim->width);
                scanline = VENUS_Y_SCANLINES(COLOR_FMT_NV12, dim->height);
            }
        } else {
                stride = PAD_TO_SIZE(dim->width, padding->width_padding);
                scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
        }

        if(IS_USAGE_HEIF(padding->usage))
        {
#ifdef COLOR_FMT_NV12_512
            buf_planes->plane_info.frame_len =
                VENUS_BUFFER_SIZE(COLOR_FMT_NV12_512, stride, scanline);
#else
            buf_planes->plane_info.frame_len =
                PAD_TO_SIZE((uint32_t)(scanline*scanline), CAM_PAD_TO_512);
#endif //COLOR_FMT_NV12_512
        }else {
            buf_planes->plane_info.frame_len =
                VENUS_BUFFER_SIZE(COLOR_FMT_NV12, dim->width, dim->height);
        }

        buf_planes->plane_info.num_planes = 2;
        buf_planes->plane_info.mp[0].len = (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].offset_x =0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;
        if(type != CAM_STREAM_TYPE_OFFLINE_PROC)
        {
            if(IS_USAGE_HEIF(padding->usage))
            {
#ifdef COLOR_FMT_NV12_512
                stride = VENUS_UV_STRIDE(COLOR_FMT_NV12_512, dim->width);
                scanline = VENUS_UV_SCANLINES(COLOR_FMT_NV12_512, dim->height);
#else
                stride = PAD_TO_SIZE(dim->width, padding->width_padding);
                scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
#endif //COLOR_FMT_NV12_512
            }else {
                stride = VENUS_UV_STRIDE(COLOR_FMT_NV12, dim->width);
                scanline = VENUS_UV_SCANLINES(COLOR_FMT_NV12, dim->height);
            }
        } else {
                stride = PAD_TO_SIZE(dim->width, padding->width_padding);
                scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
        }

        buf_planes->plane_info.mp[1].len =
                buf_planes->plane_info.frame_len -
                buf_planes->plane_info.mp[0].len;
        buf_planes->plane_info.mp[1].offset = 0;
        buf_planes->plane_info.mp[1].offset_x =0;
        buf_planes->plane_info.mp[1].offset_y = 0;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width;
        buf_planes->plane_info.mp[1].height = dim->height / 2;
#else
        LOGD("Video format VENUS is not supported = %d",
                 fmt);
#endif
        break;
    case CAM_FORMAT_YUV_420_NV21_VENUS:

#ifdef VENUS_PRESENT
        // using Venus
        stride = VENUS_Y_STRIDE(COLOR_FMT_NV21, dim->width);
        scanline = VENUS_Y_SCANLINES(COLOR_FMT_NV21, dim->height);
        buf_planes->plane_info.frame_len =
                VENUS_BUFFER_SIZE(COLOR_FMT_NV21, dim->width, dim->height);
        buf_planes->plane_info.num_planes = 2;
        buf_planes->plane_info.mp[0].len = (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].offset_x =0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;
        stride = VENUS_UV_STRIDE(COLOR_FMT_NV21, dim->width);
        scanline = VENUS_UV_SCANLINES(COLOR_FMT_NV21, dim->height);
        buf_planes->plane_info.mp[1].len =
                buf_planes->plane_info.frame_len - buf_planes->plane_info.mp[0].len;
        buf_planes->plane_info.mp[1].offset = 0;
        buf_planes->plane_info.mp[1].offset_x =0;
        buf_planes->plane_info.mp[1].offset_y = 0;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width;
        buf_planes->plane_info.mp[1].height = dim->height / 2;
#else
        LOGE("Venus hardware not avail, cannot use this format");
        rc = -1;
#endif
        break;
    default:
        LOGE("Invalid cam_format for snapshot %d",
                    fmt);
        rc = -1;
        break;
    }

    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_calc_offset_raw
 *
 * DESCRIPTION: calculate raw frame offset based on format and padding information
 *
 * PARAMETERS :
 *   @fmt     : image format
 *   @dim     : image dimension
 *   @padding : padding information
 *   @buf_planes : [out] buffer plane information
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_calc_offset_raw(cam_format_t fmt,
                                  cam_dimension_t *dim,
                                  cam_padding_info_t *padding,
                                  cam_stream_buf_plane_info_t *buf_planes)
{
    int32_t rc = 0;

    if ((NULL == dim) || (NULL == padding) || (NULL == buf_planes)) {
        return -1;
    }

    int32_t stride = PAD_TO_SIZE(dim->width, (int32_t)padding->width_padding);
    int32_t stride_in_bytes = stride;
    int32_t scanline = PAD_TO_SIZE(dim->height, (int32_t)padding->height_padding);

    switch (fmt) {
    case CAM_FORMAT_YUV_420_NV21:
        /* 2 planes: Y + CbCr */
        buf_planes->plane_info.num_planes = 2;

        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline),
                padding->plane_padding);
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].offset_x = 0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].stride_in_bytes = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;

        scanline = scanline / 2;
        buf_planes->plane_info.mp[1].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline),
                padding->plane_padding);
        buf_planes->plane_info.mp[1].offset = 0;
        buf_planes->plane_info.mp[1].offset_x = 0;
        buf_planes->plane_info.mp[1].offset_y = 0;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].stride_in_bytes = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width;
        buf_planes->plane_info.mp[1].height = dim->height / 2;

        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len +
                buf_planes->plane_info.mp[1].len,
                CAM_PAD_TO_4K);
        break;
    case CAM_FORMAT_YUV_RAW_8BIT_YUYV:
    case CAM_FORMAT_YUV_RAW_8BIT_YVYU:
    case CAM_FORMAT_YUV_RAW_8BIT_UYVY:
    case CAM_FORMAT_YUV_RAW_8BIT_VYUY:
    case CAM_FORMAT_JPEG_RAW_8BIT:
        /* 1 plane */
        /* Every 16 pixels occupy 16 bytes */
        stride = PAD_TO_SIZE(dim->width, CAM_PAD_TO_16);
        stride_in_bytes = stride * 2;
        buf_planes->plane_info.num_planes = 1;
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(stride_in_bytes * scanline),
                        padding->plane_padding);
        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len, CAM_PAD_TO_4K);
        buf_planes->plane_info.mp[0].offset_x =0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].stride_in_bytes = stride_in_bytes;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width =
                (int32_t)buf_planes->plane_info.mp[0].len;
        buf_planes->plane_info.mp[0].height = 1;
        break;
    case CAM_FORMAT_META_RAW_8BIT:
        // Every 16 pixels occupy 16 bytes
        stride = PAD_TO_SIZE(dim->width, CAM_PAD_TO_16);
        stride_in_bytes = stride * 2;
        buf_planes->plane_info.num_planes = 1;
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(stride_in_bytes * scanline),
                        padding->plane_padding);
        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len, CAM_PAD_TO_4K);
        buf_planes->plane_info.mp[0].offset_x =0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].stride_in_bytes = stride_in_bytes;
        buf_planes->plane_info.mp[0].scanline = scanline;
        break;

    case CAM_FORMAT_BAYER_QCOM_RAW_8BPP_GBRG:
    case CAM_FORMAT_BAYER_QCOM_RAW_8BPP_GRBG:
    case CAM_FORMAT_BAYER_QCOM_RAW_8BPP_RGGB:
    case CAM_FORMAT_BAYER_QCOM_RAW_8BPP_BGGR:
    case CAM_FORMAT_BAYER_QCOM_RAW_8BPP_GREY:
    case CAM_FORMAT_BAYER_MIPI_RAW_8BPP_GBRG:
    case CAM_FORMAT_BAYER_MIPI_RAW_8BPP_GRBG:
    case CAM_FORMAT_BAYER_MIPI_RAW_8BPP_RGGB:
    case CAM_FORMAT_BAYER_MIPI_RAW_8BPP_BGGR:
    case CAM_FORMAT_BAYER_MIPI_RAW_8BPP_GREY:
    case CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_8BPP_GBRG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_8BPP_GRBG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_8BPP_RGGB:
    case CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_8BPP_BGGR:
    case CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_8BPP_GREY:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_8BPP_GBRG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_8BPP_GRBG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_8BPP_RGGB:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_8BPP_BGGR:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_8BPP_GREY:
    case CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN8_8BPP_GBRG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN8_8BPP_GRBG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN8_8BPP_RGGB:
    case CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN8_8BPP_BGGR:
        /* 1 plane */
        /* Every 16 pixels occupy 16 bytes */
        stride = PAD_TO_SIZE(dim->width, CAM_PAD_TO_16);
        stride_in_bytes = stride;
        buf_planes->plane_info.num_planes = 1;
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(stride_in_bytes * scanline),
                        padding->plane_padding);
        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len, CAM_PAD_TO_4K);
        buf_planes->plane_info.mp[0].offset_x =0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].stride_in_bytes = stride_in_bytes;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = (int32_t)buf_planes->plane_info.mp[0].len;
        buf_planes->plane_info.mp[0].height = 1;
        break;
    case CAM_FORMAT_BAYER_QCOM_RAW_10BPP_GBRG:
    case CAM_FORMAT_BAYER_QCOM_RAW_10BPP_GRBG:
    case CAM_FORMAT_BAYER_QCOM_RAW_10BPP_RGGB:
    case CAM_FORMAT_BAYER_QCOM_RAW_10BPP_BGGR:
    case CAM_FORMAT_BAYER_QCOM_RAW_10BPP_GREY:
    case CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_10BPP_GBRG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_10BPP_GRBG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_10BPP_RGGB:
    case CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_10BPP_BGGR:
    case CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_10BPP_GREY:
        /* Every 12 pixels occupy 16 bytes */
        stride = (dim->width + 11)/12 * 12;
        stride_in_bytes = stride * 8 / 6;
        buf_planes->plane_info.num_planes = 1;
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(stride_in_bytes * scanline),
                        padding->plane_padding);
        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len, CAM_PAD_TO_4K);
        buf_planes->plane_info.mp[0].offset_x =0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].stride_in_bytes = stride_in_bytes;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = (int32_t)buf_planes->plane_info.mp[0].len;
        buf_planes->plane_info.mp[0].height = 1;
        break;
    case CAM_FORMAT_BAYER_QCOM_RAW_12BPP_GBRG:
    case CAM_FORMAT_BAYER_QCOM_RAW_12BPP_GRBG:
    case CAM_FORMAT_BAYER_QCOM_RAW_12BPP_RGGB:
    case CAM_FORMAT_BAYER_QCOM_RAW_12BPP_BGGR:
    case CAM_FORMAT_BAYER_QCOM_RAW_12BPP_GREY:
    case CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_12BPP_GBRG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_12BPP_GRBG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_12BPP_RGGB:
    case CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_12BPP_BGGR:
    case CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_12BPP_GREY:
        /* Every 10 pixels occupy 16 bytes */
        stride = (dim->width + 9)/10 * 10;
        stride_in_bytes = stride * 8 / 5;
        buf_planes->plane_info.num_planes = 1;
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(stride_in_bytes * scanline),
                        padding->plane_padding);
        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len, CAM_PAD_TO_4K);
        buf_planes->plane_info.mp[0].offset_x =0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].stride_in_bytes = stride_in_bytes;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = (int32_t)buf_planes->plane_info.mp[0].len;
        buf_planes->plane_info.mp[0].height = 1;
        break;
    case CAM_FORMAT_BAYER_MIPI_RAW_10BPP_GBRG:
    case CAM_FORMAT_BAYER_MIPI_RAW_10BPP_GRBG:
    case CAM_FORMAT_BAYER_MIPI_RAW_10BPP_RGGB:
    case CAM_FORMAT_BAYER_MIPI_RAW_10BPP_BGGR:
    case CAM_FORMAT_BAYER_MIPI_RAW_10BPP_GREY:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_10BPP_GBRG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_10BPP_GRBG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_10BPP_RGGB:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_10BPP_BGGR:
    case CAM_FORMAT_META_RAW_10BIT:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_10BPP_GREY:
        /* Every 64 pixels occupy 80 bytes */
        stride = PAD_TO_SIZE(dim->width, CAM_PAD_TO_4);
        stride_in_bytes = PAD_TO_SIZE(stride * 5 / 4, CAM_PAD_TO_8);
        buf_planes->plane_info.num_planes = 1;
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(stride_in_bytes * scanline),
                        padding->plane_padding);
        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len, CAM_PAD_TO_4K);
        buf_planes->plane_info.mp[0].offset_x =0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].stride_in_bytes = stride_in_bytes;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = (int32_t)buf_planes->plane_info.mp[0].len;
        buf_planes->plane_info.mp[0].height = 1;
        break;
    case CAM_FORMAT_BAYER_MIPI_RAW_12BPP_GBRG:
    case CAM_FORMAT_BAYER_MIPI_RAW_12BPP_GRBG:
    case CAM_FORMAT_BAYER_MIPI_RAW_12BPP_RGGB:
    case CAM_FORMAT_BAYER_MIPI_RAW_12BPP_BGGR:
    case CAM_FORMAT_BAYER_MIPI_RAW_12BPP_GREY:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_12BPP_GBRG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_12BPP_GRBG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_12BPP_RGGB:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_12BPP_BGGR:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_12BPP_GREY:
        /* Every 32 pixels occupy 48 bytes */
        stride = PAD_TO_SIZE(dim->width, CAM_PAD_TO_32);
        stride_in_bytes = stride * 3 / 2;
        buf_planes->plane_info.num_planes = 1;
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(stride_in_bytes * scanline),
                        padding->plane_padding);
        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len, CAM_PAD_TO_4K);
        buf_planes->plane_info.mp[0].offset_x =0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].stride_in_bytes = stride_in_bytes;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = (int32_t)buf_planes->plane_info.mp[0].len;
        buf_planes->plane_info.mp[0].height = 1;
        break;
    case CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_8BPP_GBRG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_8BPP_GRBG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_8BPP_RGGB:
    case CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_8BPP_BGGR:
    case CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_10BPP_GBRG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_10BPP_GRBG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_10BPP_RGGB:
    case CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_10BPP_BGGR:
    case CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_12BPP_GBRG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_12BPP_GRBG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_12BPP_RGGB:
    case CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_12BPP_BGGR:
    case CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_14BPP_GBRG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_14BPP_GRBG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_14BPP_RGGB:
    case CAM_FORMAT_BAYER_IDEAL_RAW_PLAIN16_14BPP_BGGR:
    case CAM_FORMAT_BAYER_RAW_PLAIN16_10BPP_GBRG:
    case CAM_FORMAT_BAYER_RAW_PLAIN16_10BPP_GRBG:
    case CAM_FORMAT_BAYER_RAW_PLAIN16_10BPP_RGGB:
    case CAM_FORMAT_BAYER_RAW_PLAIN16_10BPP_BGGR:
        /* Every 16 pixels occupy 32 bytes */
        stride = PAD_TO_SIZE(dim->width, CAM_PAD_TO_16);
        stride_in_bytes = stride * 2;
        buf_planes->plane_info.num_planes = 1;
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(stride_in_bytes * scanline),
                        padding->plane_padding);
        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len, CAM_PAD_TO_4K);
        buf_planes->plane_info.mp[0].offset_x =0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].stride_in_bytes = stride_in_bytes;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = (int32_t)buf_planes->plane_info.mp[0].len;
        buf_planes->plane_info.mp[0].height = 1;
        break;
    case CAM_FORMAT_BAYER_MIPI_RAW_14BPP_GBRG:
    case CAM_FORMAT_BAYER_MIPI_RAW_14BPP_GRBG:
    case CAM_FORMAT_BAYER_MIPI_RAW_14BPP_RGGB:
    case CAM_FORMAT_BAYER_MIPI_RAW_14BPP_BGGR:
    case CAM_FORMAT_BAYER_MIPI_RAW_14BPP_GREY:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_14BPP_GBRG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_14BPP_GRBG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_14BPP_RGGB:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_14BPP_BGGR:
    case CAM_FORMAT_BAYER_IDEAL_RAW_MIPI_14BPP_GREY:
        /* Every 64 pixels occupy 112 bytes */
        stride = PAD_TO_SIZE(dim->width, CAM_PAD_TO_64);
        stride_in_bytes = stride * 7 / 4;
        buf_planes->plane_info.num_planes = 1;
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(stride_in_bytes * scanline),
                padding->plane_padding);
        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len, CAM_PAD_TO_4K);
        buf_planes->plane_info.mp[0].offset_x =0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].stride_in_bytes = stride_in_bytes;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = (int32_t)buf_planes->plane_info.mp[0].len;
        buf_planes->plane_info.mp[0].height = 1;
        break;
    case CAM_FORMAT_BAYER_QCOM_RAW_14BPP_GBRG:
    case CAM_FORMAT_BAYER_QCOM_RAW_14BPP_GRBG:
    case CAM_FORMAT_BAYER_QCOM_RAW_14BPP_RGGB:
    case CAM_FORMAT_BAYER_QCOM_RAW_14BPP_BGGR:
    case CAM_FORMAT_BAYER_QCOM_RAW_14BPP_GREY:
    case CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_14BPP_GBRG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_14BPP_GRBG:
    case CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_14BPP_RGGB:
    case CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_14BPP_BGGR:
    case CAM_FORMAT_BAYER_IDEAL_RAW_QCOM_14BPP_GREY:
        /* Every 16 pixels occupy 32 bytes */
        stride = PAD_TO_SIZE(dim->width, CAM_PAD_TO_16);
        stride_in_bytes = stride * 2;
        buf_planes->plane_info.num_planes = 1;
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(stride_in_bytes * scanline),
                padding->plane_padding);
        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len, CAM_PAD_TO_4K);
        buf_planes->plane_info.mp[0].offset_x =0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].stride_in_bytes = stride_in_bytes;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = (int32_t)buf_planes->plane_info.mp[0].len;
        buf_planes->plane_info.mp[0].height = 1;
        break;
    default:
        LOGE("Invalid cam_format %d for raw stream",
                    fmt);
        rc = -1;
        break;
    }

    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_calc_offset_video
 *
 * DESCRIPTION: calculate video frame offset based on format and
 *              padding information
 *
 * PARAMETERS :
  *   @stream_info  : Stream information
 *   @padding : Padding info
 *   @buf_planes : [out] buffer plane information
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_calc_offset_video(cam_stream_info_t *stream_info,
        cam_padding_info_t *padding, cam_stream_buf_plane_info_t *buf_planes)
{
    int32_t rc = 0;
    int stride = 0, scanline = 0;
    cam_dimension_t *dim = &stream_info->dim ;

    #ifdef UBWC_PRESENT
    int meta_stride = 0,meta_scanline = 0;
    #endif


    switch (stream_info->fmt) {
        case CAM_FORMAT_YUV_420_NV12:
        case CAM_FORMAT_Y_ONLY:
        case CAM_FORMAT_Y_ONLY_10_BPP:
        case CAM_FORMAT_Y_ONLY_12_BPP:
        case CAM_FORMAT_Y_ONLY_14_BPP:
            buf_planes->plane_info.num_planes = 2;

            stride = dim->width;
            scanline = dim->height;
            buf_planes->plane_info.mp[0].len =
                    PAD_TO_SIZE((uint32_t)(stride * scanline),
                    CAM_PAD_TO_2K);
            buf_planes->plane_info.mp[0].offset = 0;
            buf_planes->plane_info.mp[0].offset_x =0;
            buf_planes->plane_info.mp[0].offset_y = 0;
            buf_planes->plane_info.mp[0].stride = stride;
            buf_planes->plane_info.mp[0].scanline = scanline;
            buf_planes->plane_info.mp[0].width = dim->width;
            buf_planes->plane_info.mp[0].height = dim->height;

            stride = dim->width;
            scanline = dim->height / 2;
            buf_planes->plane_info.mp[1].len =
                    PAD_TO_SIZE((uint32_t)(stride * scanline),
                    CAM_PAD_TO_2K);
            buf_planes->plane_info.mp[1].offset = 0;
            buf_planes->plane_info.mp[1].offset_x =0;
            buf_planes->plane_info.mp[1].offset_y = 0;
            buf_planes->plane_info.mp[1].stride = stride;
            buf_planes->plane_info.mp[1].scanline = scanline;
            buf_planes->plane_info.mp[1].width = dim->width;
            buf_planes->plane_info.mp[1].height = dim->height / 2;

            buf_planes->plane_info.frame_len =
                    PAD_TO_SIZE(buf_planes->plane_info.mp[0].len +
                    buf_planes->plane_info.mp[1].len,
                    CAM_PAD_TO_4K);
            break;
        case CAM_FORMAT_YUV_420_NV12_VENUS:
#ifdef VENUS_PRESENT
            // using Venus
            if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
                if(IS_USAGE_HEIF(padding->usage))
                {
#ifdef COLOR_FMT_NV12_512
                    stride = VENUS_Y_STRIDE(COLOR_FMT_NV12_512, dim->width);
                    scanline = VENUS_Y_SCANLINES(COLOR_FMT_NV12_512, dim->height);
#else
                    stride = PAD_TO_SIZE(dim->width, padding->width_padding);
                    scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
#endif //COLOR_FMT_NV12_512
                } else {
                    stride = VENUS_Y_STRIDE(COLOR_FMT_NV12, dim->width);
                    scanline = VENUS_Y_SCANLINES(COLOR_FMT_NV12, dim->height);
                }
            } else {
                stride = PAD_TO_SIZE(dim->width, padding->width_padding);
                scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
            }

            if(IS_USAGE_HEIF(padding->usage))
            {
#ifdef COLOR_FMT_NV12_512
                buf_planes->plane_info.frame_len =
                        VENUS_BUFFER_SIZE(COLOR_FMT_NV12_512, stride, scanline);
#else
                buf_planes->plane_info.frame_len =
                        PAD_TO_SIZE((uint32_t)(stride*scanline), CAM_PAD_TO_512);
#endif //COLOR_FMT_NV12_512
            }else {
                buf_planes->plane_info.frame_len =
                        VENUS_BUFFER_SIZE(COLOR_FMT_NV12, stride, scanline);
            }

            buf_planes->plane_info.num_planes = 2;
            buf_planes->plane_info.mp[0].len = (uint32_t)(stride * scanline);
            buf_planes->plane_info.mp[0].offset = 0;
            buf_planes->plane_info.mp[0].offset_x =0;
            buf_planes->plane_info.mp[0].offset_y = 0;
            buf_planes->plane_info.mp[0].stride = stride;
            buf_planes->plane_info.mp[0].scanline = scanline;
            buf_planes->plane_info.mp[0].width = dim->width;
            buf_planes->plane_info.mp[0].height = dim->height;
            if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
                if(IS_USAGE_HEIF(padding->usage))
                {
#ifdef COLOR_FMT_NV12_512
                    stride = VENUS_UV_STRIDE(COLOR_FMT_NV12_512, dim->width);
                    scanline = VENUS_UV_SCANLINES(COLOR_FMT_NV12_512, dim->height);
#else
                    stride = PAD_TO_SIZE(dim->width, padding->width_padding);
                    scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
#endif //COLOR_FMT_NV12_512
                }else {
                    stride = VENUS_UV_STRIDE(COLOR_FMT_NV12, dim->width);
                    scanline = VENUS_UV_SCANLINES(COLOR_FMT_NV12, dim->height);
                }
            } else {
                stride = PAD_TO_SIZE(dim->width, padding->width_padding);
                scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
            }
            buf_planes->plane_info.mp[1].len =
                    buf_planes->plane_info.frame_len -
                    buf_planes->plane_info.mp[0].len;
            buf_planes->plane_info.mp[1].offset = 0;
            buf_planes->plane_info.mp[1].offset_x =0;
            buf_planes->plane_info.mp[1].offset_y = 0;
            buf_planes->plane_info.mp[1].stride = stride;
            buf_planes->plane_info.mp[1].scanline = scanline;
            buf_planes->plane_info.mp[1].width = dim->width;
            buf_planes->plane_info.mp[1].height = dim->height/2;
#else
            LOGD("Video format VENUS is not supported = %d",
                     stream_info->fmt);
#endif
            break;
        case CAM_FORMAT_YUV_420_NV21_VENUS:
#ifdef VENUS_PRESENT
            // using Venus
            if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
                stride = VENUS_Y_STRIDE(COLOR_FMT_NV21, dim->width);
                scanline = VENUS_Y_SCANLINES(COLOR_FMT_NV21, dim->height);
            } else {
                stride = PAD_TO_SIZE(dim->width, padding->width_padding);
                scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
            }

            buf_planes->plane_info.frame_len =
                    VENUS_BUFFER_SIZE(COLOR_FMT_NV21, stride, scanline);
            buf_planes->plane_info.num_planes = 2;
            buf_planes->plane_info.mp[0].len = (uint32_t)(stride * scanline);
            buf_planes->plane_info.mp[0].offset = 0;
            buf_planes->plane_info.mp[0].offset_x =0;
            buf_planes->plane_info.mp[0].offset_y = 0;
            buf_planes->plane_info.mp[0].stride = stride;
            buf_planes->plane_info.mp[0].scanline = scanline;
            buf_planes->plane_info.mp[0].width = dim->width;
            buf_planes->plane_info.mp[0].height = dim->height;
            if (stream_info->stream_type != CAM_STREAM_TYPE_OFFLINE_PROC) {
               stride = VENUS_UV_STRIDE(COLOR_FMT_NV21, dim->width);
               scanline = VENUS_UV_SCANLINES(COLOR_FMT_NV21, dim->height);
            } else {
               stride = PAD_TO_SIZE(dim->width, padding->width_padding);
               scanline = PAD_TO_SIZE(dim->height, padding->height_padding);
            }
            buf_planes->plane_info.mp[1].len =
                    buf_planes->plane_info.frame_len -
                    buf_planes->plane_info.mp[0].len;
            buf_planes->plane_info.mp[1].offset = 0;
            buf_planes->plane_info.mp[1].offset_x =0;
            buf_planes->plane_info.mp[1].offset_y = 0;
            buf_planes->plane_info.mp[1].stride = stride;
            buf_planes->plane_info.mp[1].scanline = scanline;
            buf_planes->plane_info.mp[1].width = dim->width;
            buf_planes->plane_info.mp[1].height = dim->height / 2;
#else
            LOGD("Video format VENUS is not supported = %d",
                     stream_info->fmt);
#endif
            break;
        case CAM_FORMAT_YUV_420_NV12_UBWC:
#ifdef UBWC_PRESENT
            // using UBWC
            stride = VENUS_Y_STRIDE(COLOR_FMT_NV12_UBWC, dim->width);
            scanline = VENUS_Y_SCANLINES(COLOR_FMT_NV12_UBWC, dim->height);
            meta_stride = VENUS_Y_META_STRIDE(COLOR_FMT_NV12_UBWC, dim->width);
            meta_scanline = VENUS_Y_META_SCANLINES(COLOR_FMT_NV12_UBWC, dim->height);

            buf_planes->plane_info.frame_len =
                    VENUS_BUFFER_SIZE(COLOR_FMT_NV12_UBWC, dim->width, dim->height);
            buf_planes->plane_info.num_planes = 2;
            buf_planes->plane_info.mp[0].offset = 0;
            buf_planes->plane_info.mp[0].offset_x =0;
            buf_planes->plane_info.mp[0].offset_y = 0;
            buf_planes->plane_info.mp[0].stride = stride;
            buf_planes->plane_info.mp[0].scanline = scanline;
            buf_planes->plane_info.mp[0].width = dim->width;
            buf_planes->plane_info.mp[0].height = dim->height;
            buf_planes->plane_info.mp[0].meta_stride = meta_stride;
            buf_planes->plane_info.mp[0].meta_scanline = meta_scanline;
            buf_planes->plane_info.mp[0].meta_len =
                    MSM_MEDIA_ALIGN(meta_stride * meta_scanline, 4096);
            buf_planes->plane_info.mp[0].len =
                    (uint32_t)(MSM_MEDIA_ALIGN((stride * scanline), 4096) +
                    (buf_planes->plane_info.mp[0].meta_len));

            stride = VENUS_UV_STRIDE(COLOR_FMT_NV12_UBWC, dim->width);
            scanline = VENUS_UV_SCANLINES(COLOR_FMT_NV12_UBWC, dim->height);
            meta_stride = VENUS_UV_META_STRIDE(COLOR_FMT_NV12_UBWC, dim->width);
            meta_scanline = VENUS_UV_META_SCANLINES(COLOR_FMT_NV12_UBWC, dim->height);

            buf_planes->plane_info.mp[1].offset = 0;
            buf_planes->plane_info.mp[1].offset_x =0;
            buf_planes->plane_info.mp[1].offset_y = 0;
            buf_planes->plane_info.mp[1].stride = stride;
            buf_planes->plane_info.mp[1].scanline = scanline;
            buf_planes->plane_info.mp[1].width = dim->width;
            buf_planes->plane_info.mp[1].height = dim->height/2;
            buf_planes->plane_info.mp[1].meta_stride = meta_stride;
            buf_planes->plane_info.mp[1].meta_scanline = meta_scanline;
            buf_planes->plane_info.mp[1].meta_len =
                    MSM_MEDIA_ALIGN(meta_stride * meta_scanline, 4096);
            buf_planes->plane_info.mp[1].len =
                    buf_planes->plane_info.frame_len - buf_planes->plane_info.mp[0].len;

#else
            LOGD("Video format UBWC is not supported = %d",
                     stream_info->fmt);
            rc = -1;
#endif
            break;
        default:
            LOGD("Invalid Video Format = %d", stream_info->fmt);
            rc = -1;
            break;
    }
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_calc_offset_depth
 *
 * DESCRIPTION: calculate depth frame offset based on format and
 *              padding information
 *
 * PARAMETERS :
 *   @fmt     : stream format
 *   @dim     : image dimension
 *   @padding : padding information
 *   @buf_planes : [out] buffer plane information
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_calc_offset_depth(cam_format_t fmt,
                                       cam_dimension_t *dim,
                                       cam_padding_info_t *padding,
                                       cam_stream_buf_plane_info_t *buf_planes)
{
    int32_t rc = 0;
    int32_t offset_x = 0, offset_y = 0;
    int32_t stride, scanline;

    /* Clip to minimum supported bytes per line */
    if ((uint32_t)dim->width < padding->min_stride) {
        stride = (int32_t)padding->min_stride;
    } else {
        stride = dim->width;
    }

    if ((uint32_t)dim->height < padding->min_scanline) {
      scanline = (int32_t)padding->min_scanline;
    } else {
      scanline = dim->height;
    }

    stride = PAD_TO_SIZE(stride, padding->width_padding);
    scanline = PAD_TO_SIZE(scanline, padding->height_padding);
    switch (fmt) {
    case CAM_FORMAT_DEPTH_POINT_CLOUD:
        buf_planes->plane_info.num_planes = 1;
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(dim->width * dim->height),
                padding->plane_padding);
        buf_planes->plane_info.frame_len =
                buf_planes->plane_info.mp[0].len;

        buf_planes->plane_info.mp[0].offset_x =0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = dim->width;
        buf_planes->plane_info.mp[0].scanline = dim->height;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;
        break;
    case CAM_FORMAT_DEPTH16:
    case CAM_FORMAT_DEPTH8:
        buf_planes->plane_info.num_planes = 1;

        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline),
                padding->plane_padding);
        buf_planes->plane_info.mp[0].offset =
                PAD_TO_SIZE((uint32_t)(offset_x + stride * offset_y),
                padding->plane_padding);
        buf_planes->plane_info.mp[0].offset_x = offset_x;
        buf_planes->plane_info.mp[0].offset_y = offset_y;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;
        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len, CAM_PAD_TO_4K);
        break;
    default:
        LOGE("Unsupported depth format %d",fmt);
    }
    return rc;
}
/*===========================================================================
 * FUNCTION   : mm_stream_calc_offset_metadata
 *
 * DESCRIPTION: calculate metadata frame offset based on format and
 *              padding information
 *
 * PARAMETERS :
 *   @dim     : image dimension
 *   @padding : padding information
 *   @buf_planes : [out] buffer plane information
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_calc_offset_metadata(cam_dimension_t *dim,
                                       cam_padding_info_t *padding,
                                       cam_stream_buf_plane_info_t *buf_planes)
{
    int32_t rc = 0;
    buf_planes->plane_info.num_planes = 1;
    buf_planes->plane_info.mp[0].offset = 0;
    buf_planes->plane_info.mp[0].len =
            PAD_TO_SIZE((uint32_t)(dim->width * dim->height),
                    padding->plane_padding);
    buf_planes->plane_info.frame_len =
        buf_planes->plane_info.mp[0].len;

    buf_planes->plane_info.mp[0].offset_x =0;
    buf_planes->plane_info.mp[0].offset_y = 0;
    buf_planes->plane_info.mp[0].stride = dim->width;
    buf_planes->plane_info.mp[0].scanline = dim->height;
    buf_planes->plane_info.mp[0].width = dim->width;
    buf_planes->plane_info.mp[0].height = dim->height;
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_calc_offset_analysis
 *
 * DESCRIPTION: calculate analysis frame offset based on format and
 *              padding information
 *
 * PARAMETERS :
 *   @fmt     : image format
 *   @dim     : image dimension
 *   @padding : padding information
 *   @buf_planes : [out] buffer plane information
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_calc_offset_analysis(cam_format_t fmt,
                                       cam_dimension_t *dim,
                                       cam_padding_info_t *padding,
                                       cam_stream_buf_plane_info_t *buf_planes)
{
    int32_t rc = 0;
    int32_t offset_x = 0, offset_y = 0;
    int32_t stride, scanline;

    /* Clip to minimum supported bytes per line */
    if ((uint32_t)dim->width < padding->min_stride) {
        stride = (int32_t)padding->min_stride;
    } else {
        stride = dim->width;
    }

    if ((uint32_t)dim->height < padding->min_scanline) {
      scanline = (int32_t)padding->min_scanline;
    } else {
      scanline = dim->height;
    }

    stride = PAD_TO_SIZE(stride, padding->width_padding);
    scanline = PAD_TO_SIZE(scanline, padding->height_padding);

    switch (fmt) {
    case CAM_FORMAT_YUV_420_NV12:
    case CAM_FORMAT_YUV_420_NV21:
        /* 2 planes: Y + CbCr */
        buf_planes->plane_info.num_planes = 2;

        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline),
                        padding->plane_padding);
        buf_planes->plane_info.mp[0].offset =
                PAD_TO_SIZE((uint32_t)(offset_x + stride * offset_y),
                        padding->plane_padding);
        buf_planes->plane_info.mp[0].offset_x = offset_x;
        buf_planes->plane_info.mp[0].offset_y = offset_y;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;

        scanline = scanline / 2;
        buf_planes->plane_info.mp[1].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline),
                        padding->plane_padding);
        buf_planes->plane_info.mp[1].offset =
                PAD_TO_SIZE((uint32_t)(offset_x + stride * offset_y),
                        padding->plane_padding);
        buf_planes->plane_info.mp[1].offset_x = offset_x;
        buf_planes->plane_info.mp[1].offset_y = offset_y;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width;
        buf_planes->plane_info.mp[1].height = dim->height / 2;

        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len +
                        buf_planes->plane_info.mp[1].len,
                        CAM_PAD_TO_4K);
        break;
    case CAM_FORMAT_YUV_420_YV12:
        /* 3 planes: Y + Cr + Cb */
        buf_planes->plane_info.num_planes = 3;

        buf_planes->plane_info.mp[0].offset =
                PAD_TO_SIZE((uint32_t)(offset_x + stride * offset_y),
                        padding->plane_padding);
        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline),
                        padding->plane_padding);
        buf_planes->plane_info.mp[0].offset_x = offset_x;
        buf_planes->plane_info.mp[0].offset_y = offset_y;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;

        stride = PAD_TO_SIZE(stride / 2, CAM_PAD_TO_16);
        scanline = scanline / 2;
        buf_planes->plane_info.mp[1].offset =
                PAD_TO_SIZE((uint32_t)(offset_x + stride * offset_y),
                        padding->plane_padding);
        buf_planes->plane_info.mp[1].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline),
                        padding->plane_padding);
        buf_planes->plane_info.mp[1].offset_x = offset_x;
        buf_planes->plane_info.mp[1].offset_y = offset_y;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width / 2;
        buf_planes->plane_info.mp[1].height = dim->height / 2;

        buf_planes->plane_info.mp[2].offset =
                PAD_TO_SIZE((uint32_t)(offset_x + stride * offset_y),
                        padding->plane_padding);
        buf_planes->plane_info.mp[2].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline),
                        padding->plane_padding);
        buf_planes->plane_info.mp[2].offset_x = offset_x;
        buf_planes->plane_info.mp[2].offset_y = offset_y;
        buf_planes->plane_info.mp[2].stride = stride;
        buf_planes->plane_info.mp[2].scanline = scanline;
        buf_planes->plane_info.mp[2].width = dim->width / 2;
        buf_planes->plane_info.mp[2].height = dim->height / 2;

        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len +
                        buf_planes->plane_info.mp[1].len +
                        buf_planes->plane_info.mp[2].len,
                        CAM_PAD_TO_4K);
        break;
    case CAM_FORMAT_YUV_422_NV16:
    case CAM_FORMAT_YUV_422_NV61:
        /* 2 planes: Y + CbCr */
        buf_planes->plane_info.num_planes = 2;
        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline),
                        padding->plane_padding);
        buf_planes->plane_info.mp[0].offset =
                PAD_TO_SIZE((uint32_t)(offset_x + stride * offset_y),
                        padding->plane_padding);
        buf_planes->plane_info.mp[0].offset_x = offset_x;
        buf_planes->plane_info.mp[0].offset_y = offset_y;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;

        buf_planes->plane_info.mp[1].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline),
                        padding->plane_padding);
        buf_planes->plane_info.mp[1].offset =
                PAD_TO_SIZE((uint32_t)(offset_x + stride * offset_y),
                        padding->plane_padding);
        buf_planes->plane_info.mp[1].offset_x = offset_x;
        buf_planes->plane_info.mp[1].offset_y = offset_y;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width;
        buf_planes->plane_info.mp[1].height = dim->height;

        buf_planes->plane_info.frame_len = PAD_TO_SIZE(
            buf_planes->plane_info.mp[0].len + buf_planes->plane_info.mp[1].len,
            CAM_PAD_TO_4K);
        break;
    case CAM_FORMAT_Y_ONLY:
    case CAM_FORMAT_Y_ONLY_10_BPP:
    case CAM_FORMAT_Y_ONLY_12_BPP:
    case CAM_FORMAT_Y_ONLY_14_BPP:
        buf_planes->plane_info.num_planes = 1;

        buf_planes->plane_info.mp[0].len =
                PAD_TO_SIZE((uint32_t)(stride * scanline),
                padding->plane_padding);
        buf_planes->plane_info.mp[0].offset =
                PAD_TO_SIZE((uint32_t)(offset_x + stride * offset_y),
                padding->plane_padding);
        buf_planes->plane_info.mp[0].offset_x = offset_x;
        buf_planes->plane_info.mp[0].offset_y = offset_y;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;
        buf_planes->plane_info.frame_len =
                PAD_TO_SIZE(buf_planes->plane_info.mp[0].len, CAM_PAD_TO_4K);
        break;
    case CAM_FORMAT_YUV_420_NV12_VENUS:
#ifdef VENUS_PRESENT
        // using Venus
        stride = VENUS_Y_STRIDE(COLOR_FMT_NV12, dim->width);
        scanline = VENUS_Y_SCANLINES(COLOR_FMT_NV12, dim->height);

        buf_planes->plane_info.frame_len =
                VENUS_BUFFER_SIZE(COLOR_FMT_NV12, stride, scanline);
        buf_planes->plane_info.num_planes = 2;
        buf_planes->plane_info.mp[0].len = (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].offset_x =0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;
        stride = VENUS_UV_STRIDE(COLOR_FMT_NV12, dim->width);
        scanline = VENUS_UV_SCANLINES(COLOR_FMT_NV12, dim->height);
        buf_planes->plane_info.mp[1].len =
                buf_planes->plane_info.frame_len - buf_planes->plane_info.mp[0].len;
        buf_planes->plane_info.mp[1].offset = 0;
        buf_planes->plane_info.mp[1].offset_x =0;
        buf_planes->plane_info.mp[1].offset_y = 0;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width;
        buf_planes->plane_info.mp[1].height = dim->height / 2;
#else
        LOGE("Venus hardware not avail, cannot use this format");
        rc = -1;
#endif
        break;
    case CAM_FORMAT_YUV_420_NV21_VENUS:
#ifdef VENUS_PRESENT
        // using Venus
        stride = VENUS_Y_STRIDE(COLOR_FMT_NV21, dim->width);
        scanline = VENUS_Y_SCANLINES(COLOR_FMT_NV21, dim->height);

        buf_planes->plane_info.frame_len =
                VENUS_BUFFER_SIZE(COLOR_FMT_NV21, stride, scanline);
        buf_planes->plane_info.num_planes = 2;
        buf_planes->plane_info.mp[0].len = (uint32_t)(stride * scanline);
        buf_planes->plane_info.mp[0].offset = 0;
        buf_planes->plane_info.mp[0].offset_x =0;
        buf_planes->plane_info.mp[0].offset_y = 0;
        buf_planes->plane_info.mp[0].stride = stride;
        buf_planes->plane_info.mp[0].scanline = scanline;
        buf_planes->plane_info.mp[0].width = dim->width;
        buf_planes->plane_info.mp[0].height = dim->height;
        stride = VENUS_UV_STRIDE(COLOR_FMT_NV21, dim->width);
        scanline = VENUS_UV_SCANLINES(COLOR_FMT_NV21, dim->height);
        buf_planes->plane_info.mp[1].len =
                buf_planes->plane_info.frame_len - buf_planes->plane_info.mp[0].len;
        buf_planes->plane_info.mp[1].offset = 0;
        buf_planes->plane_info.mp[1].offset_x =0;
        buf_planes->plane_info.mp[1].offset_y = 0;
        buf_planes->plane_info.mp[1].stride = stride;
        buf_planes->plane_info.mp[1].scanline = scanline;
        buf_planes->plane_info.mp[1].width = dim->width;
        buf_planes->plane_info.mp[1].height = dim->height / 2;
#else
        LOGE("Venus hardware not avail, cannot use this format");
        rc = -1;
#endif
        break;
    case CAM_FORMAT_YUV_420_NV12_UBWC:
#ifdef UBWC_PRESENT
        {
            int meta_stride = 0,meta_scanline = 0;
            // using UBWC
            stride = VENUS_Y_STRIDE(COLOR_FMT_NV12_UBWC, dim->width);
            scanline = VENUS_Y_SCANLINES(COLOR_FMT_NV12_UBWC, dim->height);
            meta_stride = VENUS_Y_META_STRIDE(COLOR_FMT_NV12_UBWC, dim->width);
            meta_scanline = VENUS_Y_META_SCANLINES(COLOR_FMT_NV12_UBWC, dim->height);

            buf_planes->plane_info.frame_len =
                    VENUS_BUFFER_SIZE(COLOR_FMT_NV12_UBWC, stride, scanline);
            buf_planes->plane_info.num_planes = 2;
            buf_planes->plane_info.mp[0].offset = 0;
            buf_planes->plane_info.mp[0].offset_x =0;
            buf_planes->plane_info.mp[0].offset_y = 0;
            buf_planes->plane_info.mp[0].stride = stride;
            buf_planes->plane_info.mp[0].scanline = scanline;
            buf_planes->plane_info.mp[0].width = dim->width;
            buf_planes->plane_info.mp[0].height = dim->height;
            buf_planes->plane_info.mp[0].meta_stride = meta_stride;
            buf_planes->plane_info.mp[0].meta_scanline = meta_scanline;
            buf_planes->plane_info.mp[0].meta_len =
                    MSM_MEDIA_ALIGN(meta_stride * meta_scanline, 4096);
            buf_planes->plane_info.mp[0].len =
                    (uint32_t)(MSM_MEDIA_ALIGN((stride * scanline), 4096) +
                    (buf_planes->plane_info.mp[0].meta_len));

            stride = VENUS_UV_STRIDE(COLOR_FMT_NV12_UBWC, dim->width);
            scanline = VENUS_UV_SCANLINES(COLOR_FMT_NV12_UBWC, dim->height);
            meta_stride = VENUS_UV_META_STRIDE(COLOR_FMT_NV12_UBWC, dim->width);
            meta_scanline = VENUS_UV_META_SCANLINES(COLOR_FMT_NV12_UBWC, dim->height);
            buf_planes->plane_info.mp[1].offset = 0;
            buf_planes->plane_info.mp[1].offset_x =0;
            buf_planes->plane_info.mp[1].offset_y = 0;
            buf_planes->plane_info.mp[1].stride = stride;
            buf_planes->plane_info.mp[1].scanline = scanline;
            buf_planes->plane_info.mp[1].width = dim->width;
            buf_planes->plane_info.mp[1].height = dim->height/2;
            buf_planes->plane_info.mp[1].meta_stride = meta_stride;
            buf_planes->plane_info.mp[1].meta_scanline = meta_scanline;
            buf_planes->plane_info.mp[1].meta_len =
                    MSM_MEDIA_ALIGN(meta_stride * meta_scanline, 4096);
            buf_planes->plane_info.mp[1].len =
                    buf_planes->plane_info.frame_len - buf_planes->plane_info.mp[0].len;
        }
#else
        LOGE("UBWC hardware not avail, cannot use this format");
        rc = -1;
#endif
        break;
    default:
        LOGE("Invalid cam_format for anlysis %d",
                    fmt);
        rc = -1;
        break;
    }

    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_calc_offset_postproc
 *
 * DESCRIPTION: calculate postprocess frame offset
 *
 * PARAMETERS :
 *   @stream_info: ptr to stream info
 *   @padding : padding information
 *   @plns : [out] buffer plane information
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_calc_offset_postproc(cam_stream_info_t *stream_info,
                                       cam_padding_info_t *padding,
                                       cam_stream_buf_plane_info_t *plns)
{
    int32_t rc = 0;
    cam_stream_type_t type = CAM_STREAM_TYPE_DEFAULT;
    if (stream_info->reprocess_config.pp_type == CAM_OFFLINE_REPROCESS_TYPE) {
        type = stream_info->reprocess_config.offline.input_type;
        if (CAM_STREAM_TYPE_DEFAULT == type) {
            if (plns->plane_info.frame_len == 0) {
                // take offset from input source
                *plns = stream_info->reprocess_config.offline.input_buf_planes;
                return rc;
            }
        } else {
            type = stream_info->reprocess_config.offline.input_type;
        }
    } else {
        type = stream_info->reprocess_config.online.input_stream_type;
    }

    switch (type) {
    case CAM_STREAM_TYPE_PREVIEW:
        rc = mm_stream_calc_offset_preview(stream_info,
                                           &stream_info->dim,
                                           padding,
                                           plns);
        break;
    case CAM_STREAM_TYPE_POSTVIEW:
        rc = mm_stream_calc_offset_post_view(stream_info,
                                           &stream_info->dim,
                                           padding,
                                           plns);
        break;
    case CAM_STREAM_TYPE_SNAPSHOT:
    case CAM_STREAM_TYPE_CALLBACK:
        rc = mm_stream_calc_offset_snapshot(stream_info->fmt,
                                            &stream_info->dim,
                                            stream_info->stream_type,
                                            padding,
                                            plns);
        break;
    case CAM_STREAM_TYPE_VIDEO:
        rc = mm_stream_calc_offset_video(stream_info,
                padding, plns);
        break;
    case CAM_STREAM_TYPE_RAW:
        rc = mm_stream_calc_offset_raw(stream_info->fmt,
                                       &stream_info->dim,
                                       padding,
                                       plns);
        break;
    case CAM_STREAM_TYPE_ANALYSIS:
        rc = mm_stream_calc_offset_analysis(stream_info->fmt,
                                            &stream_info->dim,
                                            padding,
                                            plns);
        break;
    case CAM_STREAM_TYPE_METADATA:
        rc = mm_stream_calc_offset_metadata(&stream_info->dim,
                                            padding,
                                            plns);
        break;
    case CAM_STREAM_TYPE_OFFLINE_PROC:
        rc = mm_stream_calc_offset_snapshot(stream_info->fmt,
                                            &stream_info->dim,
                                            stream_info->stream_type,
                                            padding, plns);
        break;
    default:
        LOGE("not supported for stream type %d",
                    type);
        rc = -1;
        break;
    }
    return rc;
}

/*===========================================================================
* FUNCTION    : mm_stream_calc_lcm
*
* DESCRIPTION: calculate LCM of two numbers
*
* PARAMETERS :
*   @num1  : number 1
*   @num2  : number 2
*
* RETURN     : uint32_t type
*
*===========================================================================*/
uint32_t mm_stream_calc_lcm(int32_t num1, int32_t num2)
{
    uint32_t lcm = 0;
    uint32_t temp = 0;

    if ((num1 < 1) && (num2 < 1)) {
        return 0;
    } else if (num1 < 1) {
        return num2;
    } else if (num2 < 1) {
        return num1;
    }

    if (num1 > num2) {
        lcm = num1;
    } else {
        lcm = num2;
    }
    temp = lcm;

    while (1) {
        if (((lcm%num1) == 0) && ((lcm%num2) == 0)) {
            break;
        }
        lcm += temp;
    }
    return lcm;
}

/*===========================================================================
 * FUNCTION   : mm_stream_calc_offset
 *
 * DESCRIPTION: calculate frame offset based on format and padding information
 *
 * PARAMETERS :
 *   @my_obj  : stream object
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_calc_offset(mm_stream_t *my_obj)
{
    int32_t rc = 0;
    uint32_t i;

    cam_dimension_t dim = my_obj->stream_info->dim;
    if (my_obj->stream_info->pp_config.feature_mask & CAM_QCOM_FEATURE_ROTATION &&
        my_obj->stream_info->stream_type != CAM_STREAM_TYPE_VIDEO) {
        if (my_obj->stream_info->pp_config.rotation == ROTATE_90 ||
            my_obj->stream_info->pp_config.rotation == ROTATE_270) {
            // rotated by 90 or 270, need to switch width and height
            dim.width = my_obj->stream_info->dim.height;
            dim.height = my_obj->stream_info->dim.width;
        }
    }

    switch (my_obj->stream_info->stream_type) {
    case CAM_STREAM_TYPE_PREVIEW:
        rc = mm_stream_calc_offset_preview(my_obj->stream_info,
                                           &dim,
                                           &my_obj->padding_info,
                                           &my_obj->stream_info->buf_planes);
        break;
    case CAM_STREAM_TYPE_POSTVIEW:
      rc = mm_stream_calc_offset_post_view(my_obj->stream_info,
                                         &dim,
                                         &my_obj->padding_info,
                                         &my_obj->stream_info->buf_planes);
      break;
    case CAM_STREAM_TYPE_SNAPSHOT:
    case CAM_STREAM_TYPE_CALLBACK:
        rc = mm_stream_calc_offset_snapshot(my_obj->stream_info->fmt,
                                            &dim,
                                            my_obj->stream_info->stream_type,
                                            &my_obj->padding_info,
                                            &my_obj->stream_info->buf_planes);
        break;
    case CAM_STREAM_TYPE_OFFLINE_PROC:
        rc = mm_stream_calc_offset_postproc(my_obj->stream_info,
                                            &my_obj->padding_info,
                                            &my_obj->stream_info->buf_planes);
        break;
    case CAM_STREAM_TYPE_VIDEO:
        rc = mm_stream_calc_offset_video(my_obj->stream_info,
                &my_obj->padding_info, &my_obj->stream_info->buf_planes);
        break;
    case CAM_STREAM_TYPE_RAW:
        rc = mm_stream_calc_offset_raw(my_obj->stream_info->fmt,
                                       &dim,
                                       &my_obj->padding_info,
                                       &my_obj->stream_info->buf_planes);
        break;
    case CAM_STREAM_TYPE_ANALYSIS:
        rc = mm_stream_calc_offset_analysis(my_obj->stream_info->fmt,
                                            &dim,
                                            &my_obj->padding_info,
                                            &my_obj->stream_info->buf_planes);
        break;
    case CAM_STREAM_TYPE_METADATA:
        rc = mm_stream_calc_offset_metadata(&dim,
                                            &my_obj->padding_info,
                                            &my_obj->stream_info->buf_planes);
        break;
    case CAM_STREAM_TYPE_DEPTH:
        rc = mm_stream_calc_offset_depth(my_obj->stream_info->fmt,
                                         &dim,
                                         &my_obj->padding_info,
                                         &my_obj->stream_info->buf_planes);
        break;
    default:
        LOGE("not supported for stream type %d",
                    my_obj->stream_info->stream_type);
        rc = -1;
        break;
    }

    my_obj->frame_offset = my_obj->stream_info->buf_planes.plane_info;
    LOGH("Stream type %d num_planes %d", my_obj->stream_info->stream_type,
            my_obj->frame_offset.num_planes);
    for (i = 0; i < my_obj->frame_offset.num_planes; i++) {
        LOGH("Plane %d, stride %d, scanline %d, width %d, height %d, \
                length %d", i, my_obj->frame_offset.mp[i].stride,
                my_obj->frame_offset.mp[i].scanline,
                my_obj->frame_offset.mp[i].width,
                my_obj->frame_offset.mp[i].height,
                my_obj->frame_offset.mp[i].len);
    }

    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_sync_info
 *
 * DESCRIPTION: synchronize stream information with server
 *
 * PARAMETERS :
 *   @my_obj  : stream object
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 * NOTE       : assume stream info buffer is mapped to server and filled in with
 *              stream information by upper layer. This call will let server to
 *              synchornize the stream information with HAL. If server find any
 *              fields that need to be changed accroding to hardware configuration,
 *              server will modify corresponding fields so that HAL could know
 *              about it.
 *==========================================================================*/
int32_t mm_stream_sync_info(mm_stream_t *my_obj)
{
    int32_t rc = 0;
    int32_t value = 0;
    my_obj->stream_info->stream_svr_id = my_obj->server_stream_id;
    rc = mm_stream_calc_offset(my_obj);

    if (rc == 0) {
        mm_camera_obj_t *cam_obj = my_obj->ch_obj->cam_obj;
        int stream_id  =  my_obj->server_stream_id;
        if (my_obj->ch_obj->match_meta &&
                !my_obj->stream_info->noFrameExpected &&
                (my_obj->stream_info->stream_type != CAM_STREAM_TYPE_METADATA)) {
            my_obj->ch_obj->zsl_stream_id = stream_id;
        }
        rc = mm_camera_util_s_ctrl(cam_obj, stream_id, my_obj->fd,
                CAM_PRIV_STREAM_INFO_SYNC, &value);
    }
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_set_fmt
 *
 * DESCRIPTION: set stream format to kernel via v4l2 ioctl
 *
 * PARAMETERS :
 *   @my_obj  : stream object
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_set_fmt(mm_stream_t *my_obj)
{
    int32_t rc = 0;
    struct v4l2_format fmt;
    struct msm_v4l2_format_data msm_fmt;
    int i;
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);

    if (my_obj->stream_info->dim.width == 0 ||
        my_obj->stream_info->dim.height == 0) {
        LOGE("invalid input[w=%d,h=%d,fmt=%d]\n",
                   my_obj->stream_info->dim.width,
                   my_obj->stream_info->dim.height,
                   my_obj->stream_info->fmt);
        return -1;
    }

    memset(&fmt, 0, sizeof(fmt));
    memset(&msm_fmt, 0, sizeof(msm_fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    msm_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;

    msm_fmt.width = (unsigned int)my_obj->stream_info->dim.width;
    msm_fmt.height = (unsigned int)my_obj->stream_info->dim.height;
    msm_fmt.pixelformat = mm_stream_get_v4l2_fmt(my_obj->stream_info->fmt);

    if (my_obj->stream_info->streaming_mode != CAM_STREAMING_MODE_BATCH) {
        msm_fmt.num_planes = (unsigned char)my_obj->frame_offset.num_planes;
        for (i = 0; i < msm_fmt.num_planes; i++) {
            msm_fmt.plane_sizes[i] = my_obj->frame_offset.mp[i].len;
        }
    } else {
        msm_fmt.num_planes = 1;
        msm_fmt.plane_sizes[0] = my_obj->stream_info->user_buf_info.size;
    }

    memcpy(fmt.fmt.raw_data, &msm_fmt, sizeof(msm_fmt));
    rc = ioctl(my_obj->fd, VIDIOC_S_FMT, &fmt);
    if (rc < 0) {
        LOGE("ioctl VIDIOC_S_FMT failed: rc=%d errno %d\n", rc, errno);
    } else {
#ifndef DAEMON_PRESENT
        mm_camera_obj_t *cam_obj = my_obj->ch_obj->cam_obj;
        cam_shim_packet_t *shim_cmd;
        cam_shim_cmd_data shim_cmd_data;

        memset(&shim_cmd_data, 0, sizeof(shim_cmd_data));
        shim_cmd_data.command = MSM_CAMERA_PRIV_S_FMT;
        shim_cmd_data.stream_id = my_obj->server_stream_id;
        shim_cmd_data.value = NULL;
        shim_cmd = mm_camera_create_shim_cmd_packet(CAM_SHIM_SET_PARM,
                cam_obj->sessionid, &shim_cmd_data);
        rc = mm_camera_module_send_cmd(shim_cmd);
        mm_camera_destroy_shim_cmd_packet(shim_cmd);
#endif /* DAEMON_PRESENT */
    }
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_cancel_buf
 *
 * DESCRIPTION: Get buffer back from kernel
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *   @buf_idx        : frame index to be de-queued back from kernel
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_cancel_buf(mm_stream_t * my_obj,
                           uint32_t buf_idx)
{
    int32_t rc = 0;
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);

    pthread_mutex_lock(&my_obj->buf_lock);
    if(my_obj->buf_status[buf_idx].buf_refcnt != 0) {
        LOGE("Error Trying to extract a frame already sent to HAL(idx=%d) count=%d\n",
                    buf_idx,
                   my_obj->buf_status[buf_idx].buf_refcnt);
        pthread_mutex_unlock(&my_obj->buf_lock);
        rc = -1;
        return rc;
    }
    pthread_mutex_unlock(&my_obj->buf_lock);
    if (my_obj->stream_info->streaming_mode == CAM_STREAMING_MODE_BATCH) {
        /*rc = mm_stream_write_user_buf(my_obj, frame);*/
        // TODO handling batch buffers
    } else {
        pthread_mutex_lock(&my_obj->buf_lock);
        //my_obj->buf_status[buf_idx].buf_refcnt++;
        {
            pthread_mutex_unlock(&my_obj->buf_lock);
            LOGD("<DEBUG> : Cancel Buffer done for buffer:%d, stream type:%d", buf_idx, my_obj->stream_info->stream_type);

            struct msm_camera_return_buf bufid;
            memset(&bufid, 0, sizeof(struct msm_camera_return_buf));
            bufid.index = buf_idx;

            struct msm_camera_private_ioctl_arg arg;
            memset(&arg, 0, sizeof(struct msm_camera_private_ioctl_arg));
            arg.id = MSM_CAMERA_PRIV_IOCTL_ID_RETURN_BUF;
            arg.size = sizeof(struct msm_camera_return_buf);
            arg.ioctl_ptr = (uint32_t) &bufid;


            rc = ioctl(my_obj->fd, VIDIOC_MSM_CAMERA_PRIVATE_IOCTL_CMD, &arg);

            if(rc < 0) {
                LOGE("mm_stream_cancel_buf(idx=%d) err=%d\n",
                            buf_idx, rc);
            } else {
                //my_obj->buf_status[frame->buf_idx].in_kernel = 0;
            }
        }
    }
    return rc;
}


/*===========================================================================
 * FUNCTION   : mm_stream_buf_done
 *
 * DESCRIPTION: enqueue buffer back to kernel
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *   @frame        : frame to be enqueued back to kernel
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_buf_done(mm_stream_t * my_obj,
                           mm_camera_buf_def_t *frame)
{
    int32_t rc = 0;
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);

    pthread_mutex_lock(&my_obj->buf_lock);
    if(my_obj->buf_status[frame->buf_idx].buf_refcnt == 0) {
        LOGW("Warning: trying to free buffer for the second time?(idx=%d)\n",
                    frame->buf_idx);
        pthread_mutex_unlock(&my_obj->buf_lock);
        rc = -1;
        return rc;
    }
    pthread_mutex_unlock(&my_obj->buf_lock);
    if (my_obj->stream_info->streaming_mode == CAM_STREAMING_MODE_BATCH) {
        rc = mm_stream_write_user_buf(my_obj, frame);
    } else {
        pthread_mutex_lock(&my_obj->buf_lock);
        my_obj->buf_status[frame->buf_idx].buf_refcnt--;
        if (0 == my_obj->buf_status[frame->buf_idx].buf_refcnt) {
            pthread_mutex_unlock(&my_obj->buf_lock);
            LOGD("<DEBUG> : Buf done for buffer:%d, stream:%d", frame->buf_idx, frame->stream_type);
            rc = mm_stream_qbuf(my_obj, frame);
            if(rc < 0) {
                LOGE("mm_camera_stream_qbuf(idx=%d) err=%d\n",
                            frame->buf_idx, rc);
            } else {
                my_obj->buf_status[frame->buf_idx].in_kernel = 1;
            }
        }else{
            LOGD("<DEBUG> : Still ref count pending count :%d",
                 my_obj->buf_status[frame->buf_idx].buf_refcnt);
            LOGD("<DEBUG> : for buffer:%p:%d",
                 my_obj, frame->buf_idx);
            pthread_mutex_unlock(&my_obj->buf_lock);
        }
    }
    return rc;
}


/*===========================================================================
 * FUNCTION   : mm_stream_get_queued_buf_count
 *
 * DESCRIPTION: return queued buffer count
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *
 * RETURN     : queued buffer count
 *==========================================================================*/
int32_t mm_stream_get_queued_buf_count(mm_stream_t *my_obj)
{
    int32_t rc = 0;
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
             my_obj->my_hdl, my_obj->fd, my_obj->state);
    pthread_mutex_lock(&my_obj->buf_lock);
    rc = my_obj->queued_buffer_count;
    pthread_mutex_unlock(&my_obj->buf_lock);
    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_reg_buf_cb
 *
 * DESCRIPTION: Allow other stream to register dataCB at this stream.
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *   @val          : callback function to be registered
 *
 * RETURN     : int32_t type of status
 *              0  -- success
 *              -1 -- failure
 *==========================================================================*/
int32_t mm_stream_reg_buf_cb(mm_stream_t *my_obj,
        mm_stream_data_cb_t val)
{
    int32_t rc = -1;
    uint8_t i;
    LOGD("E, my_handle = 0x%x, fd = %d, state = %d",
          my_obj->my_hdl, my_obj->fd, my_obj->state);

    pthread_mutex_lock(&my_obj->cb_lock);
    for (i=0 ;i < MM_CAMERA_STREAM_BUF_CB_MAX; i++) {
        if(NULL == my_obj->buf_cb[i].cb) {
            my_obj->buf_cb[i] = val;
            rc = 0;
            break;
        }
    }
    pthread_mutex_unlock(&my_obj->cb_lock);

    return rc;
}

/*===========================================================================
 * FUNCTION   : mm_stream_handle_cache_ops
 *
 * DESCRIPTION: handles cache ops of a stream buffer
 *
 * PARAMETERS :
 *   @my_obj       : stream object
 *   @buf     : ptr to a stream buffer
 *   @deque  : specifies if enqueue or dequeue
 *
 * RETURN     : zero for success
 *                  non-zero error value
 *==========================================================================*/
int32_t mm_stream_handle_cache_ops(mm_stream_t* my_obj,
        mm_camera_buf_def_t* buf, bool deque)
{
    int32_t rc = 0;
    if(!my_obj || !buf) {
        LOGE("Error!! my_obj: %p, buf_info: %p", my_obj, buf);
        rc = -1;
        return rc;
    }
    if ((my_obj->mem_vtbl.clean_invalidate_buf  == NULL) ||
            (my_obj->mem_vtbl.invalidate_buf  == NULL) ||
            (my_obj->mem_vtbl.clean_buf  == NULL)) {
        LOGI("Clean/Invalidate cache ops not supported %p %p %p",
            my_obj->mem_vtbl.clean_invalidate_buf,
            my_obj->mem_vtbl.invalidate_buf,
            my_obj->mem_vtbl.clean_buf);
        // Not a fatal error
        rc = 0;
        return rc;
    }

    // Modify clean and invalidate flags depending on cache ops for stream
    switch (my_obj->stream_info->cache_ops) {
        case CAM_STREAM_CACHE_OPS_CLEAR_FLAGS:
            buf->cache_flags = 0;
            break;
        case CAM_STREAM_CACHE_OPS_DISABLED:
            if (deque) {
                buf->cache_flags = CPU_HAS_READ_WRITTEN;
            }
            else {
                buf->cache_flags = CPU_HAS_READ;
            }
        case CAM_STREAM_CACHE_OPS_HONOUR_FLAGS:
        default:
            // Do not change flags
            break;
    }

    if ((buf->cache_flags & CPU_HAS_READ_WRITTEN) ==
        CPU_HAS_READ_WRITTEN) {
        rc = my_obj->mem_vtbl.clean_invalidate_buf(
                buf->buf_idx, my_obj->mem_vtbl.user_data);
    } else if ((buf->cache_flags & CPU_HAS_READ) ==
        CPU_HAS_READ) {
        rc = my_obj->mem_vtbl.invalidate_buf(
                buf->buf_idx, my_obj->mem_vtbl.user_data);
    } else if ((buf->cache_flags & CPU_HAS_WRITTEN) ==
        CPU_HAS_WRITTEN) {
        rc = my_obj->mem_vtbl.clean_buf(
                buf->buf_idx, my_obj->mem_vtbl.user_data);
    }

    LOGD("[CACHE_OPS] Stream type: %d buf index: %d cache ops flags: 0x%x",
            buf->stream_type, buf->buf_idx, buf->cache_flags);

    if (rc != 0) {
        LOGE("Clean/Invalidate cache failed on buffer index: %d",
                buf->buf_idx);
    } else {
       // Reset buffer cache flags after cache ops
        buf->cache_flags = 0;
    }

    return rc;
}

