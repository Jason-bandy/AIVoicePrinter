/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _BT_A2DP_SRC_H_
#define _BT_A2DP_SRC_H_

/* ********************************************************************
 *  Bluetooth Advanced Audio Distribution Profile (A2DP) Sink Role API
 * ******************************************************************** */

#include <bt_a2dp_types.h>
#include <bt_avdtp.h>
#include <bt_api.h>
#include <bluetooth.h>

#if A2DP_ROLE_SOURCE_CODE
#include <bt_avdtp_proto.h>
#include "../../../bluetooth/profile/a2dp/bt_sbc.h"
#endif

#ifdef CONFIG_TWS

/* Handle to a Bluetooth session object */
typedef struct bt_a2dp_session_s *bt_a2dp_src_session_h;
/* Handle to a local Bluetooth server */
typedef struct bt_a2dp_session_s *bt_a2dp_src_server_h;

/**************************
 * Bluetooth Connectivity *
 *************************/

/**
 * Function type:  bt_a2dp_src_connected_cb_t
 * Description:    Notifies that a Bluetooth connection with a remote device
 *                 has been established.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *     @app_ctx_h: (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_a2dp_src_connected_cb_t)(bt_a2dp_src_session_h session_h,
    bt_app_ctx_h app_ctx_h);

/**
 * Function type:  bt_a2dp_src_disconnected_cb_t
 * Description:    Notifies that a Bluetooth connection with a remote device
 *                 has been disconnected, or that a previous connection attempt
 *                 has failed.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *     @status:    (IN) Disconnect status
 *     @app_ctx_h: (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_a2dp_src_disconnected_cb_t)(bt_a2dp_src_session_h session_h,
    result_t status, bt_app_ctx_h app_ctx_h);

/* Bluetooth connection-status application notification callback functions */
typedef struct {
    bt_a2dp_src_connected_cb_t    connected_cb;
    bt_a2dp_src_disconnected_cb_t disconnected_cb;
} bt_a2dp_src_conn_cbs_t;

/**
 * Function name:  bt_a2dp_src_conn_create
 * Description:    Creates a new Bluetooth session object, and registers the
 *                 application's Bluetooth connectivity callback functions for
 *                 the related session with the driver.
 *                 NOTE: The application must call this function before calling
 *                 bt_a2dp_sink_conn_connect() to initiate a new connection, or
 *                 bt_a2dp_sink_conn_accept() to accept a connection request.
 *                 The application is responsible for destroying the session
 *                 object, by calling bt_a2dp_sink_conn_destroy().
 * Parameters:
 *     @session_h: (OUT) Pointer to a handle to a Bluetooth session object, to
 *                       bet set by the function
 *     @conn_cbs:  (IN)  Pointer to a structure of Bluetooth connection-status
 *                       application notification callback functions
 *     @app_ctx_h: (IN)  Handle to a Bluetooth application context
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_a2dp_src_conn_create(bt_a2dp_src_session_h *session_h,
    const bt_a2dp_src_conn_cbs_t *conn_cbs, bt_app_ctx_h app_ctx_h);

/**
 * Function name:  bt_a2dp_src_conn_destroy
 * Description:    Destroys a Bluetooth session object.
 *                 NOTE: The application must call this function to destroy the
 *                 session handle for a disconnected or unestablished
 *                 connection.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *
 * Return value:   None
 * Scope:          Global
 **/
void bt_a2dp_src_conn_destroy(bt_a2dp_src_session_h *session_h);

/**
 * Function name:  bt_a2dp_sink_conn_disconnect
 * Description:    Initiates a disconnect of a Bluetooth connection with a
 *                 remote device.
 *                 NOTE: The driver will respond by calling the application's
 *                 bt_a2dp_sink_disconnected_cb_t callback.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_a2dp_src_conn_disconnect(bt_a2dp_src_session_h server_h);

/*--------------------------------
   Client-Specific Connection API
  --------------------------------*/
/**
 * Function name:  bt_a2dp_sink_conn_connect
 * Description:    Issues a Bluetooth connection request to a remote device.
 *                 NOTE: The driver will respond by calling the application's
 *                 bt_a2dp_sink_connected_cb_t callback if a connection is
 *                 established, or the bt_a2dp_sink_disconnected_cb_t callback
 *                 otherwise.
 * Parameters:
 *     @session_h:      (OUT) Pointer to a handle to a Bluetooth session object,
 *                            to be set by the function
 *     @laddr:          (IN)  Pointer to a local Bluetooth address
 *     @raddr:          (IN)  Pointer to a remote Bluetooth address
 *     @ep:             (IN)  Pointer to an audio stream endpoint descriptor
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_a2dp_src_conn_connect(bt_a2dp_src_session_h session_h,
    const btaddr_t *laddr, const btaddr_t *raddr, bt_a2dp_endpoint_desc_t *ep);

result_t bt_a2dp_src_conn_reconfig( bt_a2dp_src_session_h session,
    bt_a2dp_codec_t *codec );

/*--------------------------------
   Server-Specific Connection API
  --------------------------------*/

/****************************************
 * A2DP Sink Role-Specific Functionality
 ****************************************/

/**
 * Function name:  bt_a2dp_sink_register
 * Description:    Registers the application to use the A2dP sink driver's
 *                 role-specific APIs.
 * Parameters:
 *     @cbs: (IN)  Pointer to a structure of A2dP sink role-specific
 *                 application callback functions
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_a2dp_src_register(void);

/**
 * Function name:  bt_a2dp_sink_unregister
 * Description:    Unregisters the application from using the A2dP sink
 *                 driver's role-specific APIs.
 * Parameters:     None
 *
 * Return value:   None
 * Scope:          Global
 **/
void bt_a2dp_src_unregister(void);



#endif

#if A2DP_ROLE_SOURCE_CODE

/* Handle to a Bluetooth session object */
typedef struct bt_a2dp_session_s *bt_a2dp_src_session_h;
/* Handle to a local Bluetooth server */
typedef struct bt_a2dp_session_s *bt_a2dp_src_server_h;

/****************************************
 * A2DP Src Role-Specific Functionality
 ****************************************/

/**
 * Function type:  bt_a2dp_src_set_configuration_req_cb_t
 * Description:    Notifies that an AVDTP Set Configuration Command
 *                 (AVDTP_SET_CONFIGURATION_CMD) was received from the remote
 *                 device.
 *                 NOTE: The application must respond either by immediately
 *                 calling bt_a2dp_sink_set_configuration_rsp() to accept the
 *                 Set Configuration request, or by calling
 *                 bt_a2dp_sink_set_configuration_rej() to reject it.
 * Parameters:
 *     @session:      (IN) Handle to a Bluetooth session object
 *     @codec:        (IN) Pointer toa codec descriptor
 *     @local_ep_id:  (IN) Local endpoint ID
 *     @remote_ep_id: (IN) Remote endpoint ID
 *     @app_ctx_h:    (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_a2dp_src_set_configuration_req_cb_t)(
    bt_a2dp_src_session_h session_h, const bt_a2dp_codec_t *codec,
    int_t local_ep_id, int_t remote_ep_id, bt_app_ctx_h app_ctx_h);

/**
 * Function type:  bt_a2dp_sink_stream_start_cb_t
 * Description:    Notifies that an audio stream has been started.
 * Parameters:
 *     @session:     (IN) Handle to a Bluetooth session object
 *     @app_ctx_h:   (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_a2dp_src_stream_start_cb_t)(bt_a2dp_src_session_h session_h,
    bt_app_ctx_h app_ctx_h);
/* A2DP src role-specific application callback functions */
typedef struct {
    bt_a2dp_src_set_configuration_req_cb_t set_configuration_req_cb;
    bt_a2dp_src_stream_start_cb_t          stream_started_cb;
} bt_a2dp_src_cbs_t;

typedef void (*bt_a2dp_src_newconn_cb_t)(bt_conn_req_h conn_req_h,
    const btaddr_t *laddr, const btaddr_t *raddr, bt_link_modes_mask_t mode,
    bt_app_ctx_h app_ctx_h);


/**
 * Function type:  bt_a2dp_src_connected_cb_t
 * Description:    Notifies that a Bluetooth connection with a remote device
 *                 has been established.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *     @app_ctx_h: (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_a2dp_src_connected_cb_t)(bt_a2dp_src_session_h session_h,
    bt_app_ctx_h app_ctx_h);

/**
 * Function type:  bt_a2dp_src_disconnected_cb_t
 * Description:    Notifies that a Bluetooth connection with a remote device
 *                 has been disconnected, or that a previous connection attempt
 *                 has failed.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *     @status:    (IN) Disconnect status
 *     @app_ctx_h: (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_a2dp_src_disconnected_cb_t)(bt_a2dp_src_session_h session_h,
    result_t status, bt_app_ctx_h app_ctx_h);

/* Bluetooth connection-status application notification callback functions */
typedef struct {
    bt_a2dp_src_connected_cb_t    connected_cb;
    bt_a2dp_src_disconnected_cb_t disconnected_cb;
} bt_a2dp_src_conn_cbs_t;

/* Remote stream endpoint */
typedef struct {
    sockaddr_avdtp_stream_t       soaddr;

    /* Service capabilities supported by the remote endpoint */
    avdtp_cap_list_t              caps;
} sep_remote_t;

/* A2DP session state */
typedef enum {
    A2DP_SESSION_DETACHED = 0,
    A2DP_SESSION_ATTACHED
} a2dp_session_state_t;

typedef struct {
    struct bt_a2dp_session_s     *session;
    /* TODO: change name to include avdtp */
    avdtp_stream_h               lower;
    bt_sbc_h                     sbc_h;          /* SBC audio codec */
    uint8_t                      flags;

    bt_a2dp_endpoint_desc_t      ep;
    uint8_t                     method;
    void                         *app_stream_ctx;
} stream_softc_t;

typedef struct bt_a2dp_session_s {
    a2dp_session_state_t            state;

    bt_link_modes_mask_t            mode;        /* link mode */

    /* TODO: change name to include avdtp */
    avdtp_session_h                 session_h;
    bt_a2dp_src_newconn_cb_t       newconn_cb;

    bt_a2dp_src_conn_cbs_t         conn_cbs;
    bt_app_ctx_h                    app_ctx;

    stream_softc_t                  *stream;     /* stream */
    stream_softc_t                  *stream_l;   /* stream listen */

    /* An array of remote endpoints */
    sep_remote_t                    *rseps;
    uint8_t                         rseps_n;
    uint8_t                         get_caps_index;

    LIST_ENTRY(bt_a2dp_session_s)   next;
} a2dp_session_t;

#endif


#endif
