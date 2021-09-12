module("luci.controller.mosquitto_sub", package.seeall)

function index()
    entry( { "admin", "services", "mqtt", "subscriber"}, firstchild(), _("Subscriber"),10)
    entry( { "admin", "services", "mqtt", "subscriber", "settings"}, cbi("mqtt_settings"), _("Settings"), 1).dependent = true
    entry( { "admin", "services", "mqtt", "subscriber", "topics"}, cbi("mqtt_topics"), _("Topics"), 2).dependent = true
    entry( { "admin", "services", "mqtt", "subscriber", "messages"}, cbi("mqtt_messages"), _("Messages"), 3).dependent = true
    entry( { "admin", "services", "mqtt", "subscriber", "events"}, arcombine(cbi("events/events_add"),
                                                                    cbi("events/events_edit")), _("Events"), 4).leaf = true
end
