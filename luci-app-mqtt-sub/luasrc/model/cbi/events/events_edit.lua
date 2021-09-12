
local http = require "luci.http"
local uci = require "luci.model.uci".cursor()

local section_name

if arg[1] then
	section_name = arg[1]
else
	luci.http.redirect(luci.dispatcher.build_url("admin", "services", "mqtt", "subscriber", "events"))
end

-----------------------------------------------------------------------------------------
------------------------------- Global EVENTS --------------------------------------------
-----------------------------------------------------------------------------------------
m = Map("mqtt_subs")

s = m:section(NamedSection, arg[1], "mqtt_event", translate("MQTT Events"))

topic_name = s:option(ListValue, "topic", translate("Topic"), translate("MQTT subscriber topic names"));
m.uci:foreach("mqtt_subs","mqtt_topic",
    function(i)
        topic_name:value(i.topic, i.topic)
    end)

value_type = s:option(ListValue, "type", translate("Event type"), translate("Choose event type"))
value_type:value("decimal", translate("Decimal"))
value_type:value("string", translate("String"))

value = s:option(Value, "value", translate("Value of event"), translate("Enter value of event in JSON format"))
value.rmempty = false

decval = s:option(ListValue, "dec_operator", translate("Decimal Operators"), translate("Select decimal operator"))
decval:depends("type","decimal")
decval:value("0", "<")
decval:value("1", ">")
decval:value("2", "<=")
decval:value("3", ">=")
decval:value("4", "==")
decval:value("5", "!=")

strval = s:option(ListValue, "str_operator", translate("String Operators"), translate("Select string operator"))
strval:depends("type","string")
strval:value("0", "Equal To")
strval:value("1", "Not Equal To")

o = s:option(Value, "user_email", translate("Sender's email address"), translate("An address that will be used to send your email from. Allowed characters (a-zA-Z0-9._%+-)"))
o.rmempty = false
o.datatype = "email"
o.placeholder = "email@domain.com"


sender = s:option(ListValue, "sender", translate("Sender"), translate("Mail sender config"))
m.uci:foreach("user_groups", "email", 
            function(i) 
                sender:value(i.name, i.name)
            end)

return m
