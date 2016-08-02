/*
 * Generated by gdbus-codegen 2.40.2. DO NOT EDIT.
 *
 * The license of this code is the same as for the source it was derived from.
 */

#ifndef __SHINCODBUS_H__
#define __SHINCODBUS_H__

#include <gio/gio.h>

G_BEGIN_DECLS


/* ------------------------------------------------------------------------ */
/* Declarations for com.shinco.shincoservice.SerialPortInterface */

#define TYPE_SERIAL_PORT_INTERFACE (serial_port_interface_get_type ())
#define SERIAL_PORT_INTERFACE(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), TYPE_SERIAL_PORT_INTERFACE, SerialPortInterface))
#define IS_SERIAL_PORT_INTERFACE(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), TYPE_SERIAL_PORT_INTERFACE))
#define SERIAL_PORT_INTERFACE_GET_IFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE ((o), TYPE_SERIAL_PORT_INTERFACE, SerialPortInterfaceIface))

struct _SerialPortInterface;
typedef struct _SerialPortInterface SerialPortInterface;
typedef struct _SerialPortInterfaceIface SerialPortInterfaceIface;

struct _SerialPortInterfaceIface
{
  GTypeInterface parent_iface;


  gboolean (*handle_send_data_to_audio) (
    SerialPortInterface *object,
    GDBusMethodInvocation *invocation,
    gboolean arg_ack,
    guchar arg_cmd,
    GVariant *arg_data);

  void (*get_data_from_audio) (
    SerialPortInterface *object,
    guchar arg_cmd,
    GVariant *arg_data);

};

GType serial_port_interface_get_type (void) G_GNUC_CONST;

GDBusInterfaceInfo *serial_port_interface_interface_info (void);
guint serial_port_interface_override_properties (GObjectClass *klass, guint property_id_begin);


/* D-Bus method call completion functions: */
void serial_port_interface_complete_send_data_to_audio (
    SerialPortInterface *object,
    GDBusMethodInvocation *invocation);



/* D-Bus signal emissions functions: */
void serial_port_interface_emit_get_data_from_audio (
    SerialPortInterface *object,
    guchar arg_cmd,
    GVariant *arg_data);



/* D-Bus method calls: */
void serial_port_interface_call_send_data_to_audio (
    SerialPortInterface *proxy,
    gboolean arg_ack,
    guchar arg_cmd,
    GVariant *arg_data,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean serial_port_interface_call_send_data_to_audio_finish (
    SerialPortInterface *proxy,
    GAsyncResult *res,
    GError **error);

gboolean serial_port_interface_call_send_data_to_audio_sync (
    SerialPortInterface *proxy,
    gboolean arg_ack,
    guchar arg_cmd,
    GVariant *arg_data,
    GCancellable *cancellable,
    GError **error);



/* ---- */

#define TYPE_SERIAL_PORT_INTERFACE_PROXY (serial_port_interface_proxy_get_type ())
#define SERIAL_PORT_INTERFACE_PROXY(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), TYPE_SERIAL_PORT_INTERFACE_PROXY, SerialPortInterfaceProxy))
#define SERIAL_PORT_INTERFACE_PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), TYPE_SERIAL_PORT_INTERFACE_PROXY, SerialPortInterfaceProxyClass))
#define SERIAL_PORT_INTERFACE_PROXY_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), TYPE_SERIAL_PORT_INTERFACE_PROXY, SerialPortInterfaceProxyClass))
#define IS_SERIAL_PORT_INTERFACE_PROXY(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), TYPE_SERIAL_PORT_INTERFACE_PROXY))
#define IS_SERIAL_PORT_INTERFACE_PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), TYPE_SERIAL_PORT_INTERFACE_PROXY))

typedef struct _SerialPortInterfaceProxy SerialPortInterfaceProxy;
typedef struct _SerialPortInterfaceProxyClass SerialPortInterfaceProxyClass;
typedef struct _SerialPortInterfaceProxyPrivate SerialPortInterfaceProxyPrivate;

struct _SerialPortInterfaceProxy
{
  /*< private >*/
  GDBusProxy parent_instance;
  SerialPortInterfaceProxyPrivate *priv;
};

struct _SerialPortInterfaceProxyClass
{
  GDBusProxyClass parent_class;
};

GType serial_port_interface_proxy_get_type (void) G_GNUC_CONST;

void serial_port_interface_proxy_new (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
SerialPortInterface *serial_port_interface_proxy_new_finish (
    GAsyncResult        *res,
    GError             **error);
SerialPortInterface *serial_port_interface_proxy_new_sync (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);

void serial_port_interface_proxy_new_for_bus (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
SerialPortInterface *serial_port_interface_proxy_new_for_bus_finish (
    GAsyncResult        *res,
    GError             **error);
SerialPortInterface *serial_port_interface_proxy_new_for_bus_sync (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);


/* ---- */

#define TYPE_SERIAL_PORT_INTERFACE_SKELETON (serial_port_interface_skeleton_get_type ())
#define SERIAL_PORT_INTERFACE_SKELETON(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), TYPE_SERIAL_PORT_INTERFACE_SKELETON, SerialPortInterfaceSkeleton))
#define SERIAL_PORT_INTERFACE_SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), TYPE_SERIAL_PORT_INTERFACE_SKELETON, SerialPortInterfaceSkeletonClass))
#define SERIAL_PORT_INTERFACE_SKELETON_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), TYPE_SERIAL_PORT_INTERFACE_SKELETON, SerialPortInterfaceSkeletonClass))
#define IS_SERIAL_PORT_INTERFACE_SKELETON(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), TYPE_SERIAL_PORT_INTERFACE_SKELETON))
#define IS_SERIAL_PORT_INTERFACE_SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), TYPE_SERIAL_PORT_INTERFACE_SKELETON))

typedef struct _SerialPortInterfaceSkeleton SerialPortInterfaceSkeleton;
typedef struct _SerialPortInterfaceSkeletonClass SerialPortInterfaceSkeletonClass;
typedef struct _SerialPortInterfaceSkeletonPrivate SerialPortInterfaceSkeletonPrivate;

struct _SerialPortInterfaceSkeleton
{
  /*< private >*/
  GDBusInterfaceSkeleton parent_instance;
  SerialPortInterfaceSkeletonPrivate *priv;
};

struct _SerialPortInterfaceSkeletonClass
{
  GDBusInterfaceSkeletonClass parent_class;
};

GType serial_port_interface_skeleton_get_type (void) G_GNUC_CONST;

SerialPortInterface *serial_port_interface_skeleton_new (void);


G_END_DECLS

#endif /* __SHINCODBUS_H__ */
