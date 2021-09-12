m = SimpleForm("system")
m.submit = false
m.reset = false

local s = m:section(Table, messages, translate("EVENTS LOG"), translate("Sent messages of subscribed topics"))
s.anonymous = true
s.template = "mqtt-sub/tblsection_messages"
s.addremove = false
s.refresh = true
s.table_config = {
    truncatePager = true,
    labels = {
        placeholder = "Search...",
        perPage = "Events per page {select}",
        noRows = "No entries found",
        info = ""
    },
    layout = {
        top = "<table><tr style='padding: 0 !important; border:none !important'><td style='display: flex !important; flex-direction: row'>{select}<span style='margin-left: auto; width:100px'>{search}</span></td></tr></table>",
        bottom = "{info}{pager}"
    }
}

o = s:option(DummyValue, "topic", translate("TOPIC"))
o = s:option(DummyValue, "payload", translate("MESSAGE"))
o = s:option(DummyValue, "date", translate("TIME"))
s:option(DummyValue, "", translate(""))

return m






