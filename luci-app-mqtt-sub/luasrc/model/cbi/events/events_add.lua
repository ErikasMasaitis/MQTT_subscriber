local utl = require "luci.util"
local sys = require "luci.sys"
local http = require "luci.http"
local uci = require "luci.model.uci".cursor()

local m, s, o

m = Map("mqtt_subs")

s = m:section(TypedSection, "mqtt_event", translate("Events Configuration"), translate("This section displays active events"))
	s.addremove = true
	s.noname = true
	s.template_addremove = "cbi/general_addremove"
	s.maxlength = 10
	s.add_title = translate("Add new event configuration")
	s.value_title = translate("New event Name")
	s.delete_alert = true
	s.novaluetext = translate("There are no events created yet")
	s.extedit = luci.dispatcher.build_url("admin", "services", "mqtt", "subscriber", "events", "%s")
    s.template = "cbi/tblsection"
	s.instance_description = translate("This section is used to create events for existing topics. To add a new instance, enter a custom name for it and click the 'Add'")


function s.remove(self, name)
        self.map.uci:foreach("mqtt_subs", "mqtt_event", function(s)
            self.map.uci:delete("mqtt_subs", s[".name"])
        end)

        return AbstractSection.remove(self, name)
end

local name = s:option(DummyValue, "name", translate("Tunnel name"), translate("Name of the tunnel. Used for easier tunnels management purpose only"))

function name.cfgvalue(self, section)
	return section or "Unknown"
end

o = s:option(DummyValue, "topic", translate("selected topic"), translate("event topic can be selected from list"))

function o.cfgvalue(self, section)
        local topic_name = self.map:get(section, self.option) or "Not selected"
        return topic_name
end

o = s:option(DummyValue, "type", translate("Event value type"), translate("Type of event value (string or decimal)"))

function o.cfgvalue(self, section)
	local value_type = self.map:get(section, self.option) or "Not selected"
	if value_type == "decimal" then
		return "decimal"
	elseif value_type == "string" then
		return "string"
	else
		return value_type
	end
end

return m
